#include "App/D3D/DXContext.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"
#include <cstdlib>
#include <Tracy.hpp>

DXContext* DXContext::Get(mds::IRProgramMemElem* InProgramMemElem)
{
    return InProgramMemElem->GetRoot<RSRush::RSRProgramInstance>()->GetDXContect();
}

DXContext* DXContext::Get(RSRush::RSRProgramInstance* InProgramInstance)
{
    return InProgramInstance->GetDXContect();
}

bool DXContext::Init()
{
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory))))
    {
        return false;
    }

    ////////////////Test for tearing support////////////////
    BOOL allowTearing = FALSE;
    HRESULT AllowTearingResult = m_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    
    bool bTearingSupportedOnDevice = (bool)(SUCCEEDED(AllowTearingResult) && allowTearing);
    GetRoot<RSRush::RSRProgramInstance>()->SetTearingSupported(bTearingSupportedOnDevice);
    if (!bTearingSupportedOnDevice)
    {
        RSRLog::Log(LOG_WARNING, TEXT("System does not allow Tearing !"), AllowTearingResult);
    }
    //////////////End test for tearing support//////////////

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device))))
    {
        return false;
    }
    if (!InitAsyncUploadQueue())
    {
        return false;
    }
    if (!InitFrameUploadQueue())
    {
        return false;
    }
    if (!InitRenderQueue())
    {
        return false;
    }
	return true;
}

void DXContext::Shutdown()
{
    ShutdownRenderQueue();
    ShutdownFrameUploadQueue();
    ShutdownAsyncUploadQueue();
    m_device.Reset();
    m_factory.Reset();
}

void DXContext::SignalRenderAndWait()
{
    m_renderCmdQueue->Signal(m_renderFence.Get(), ++m_renderFenceValue);
    if (SUCCEEDED(m_renderFence->SetEventOnCompletion(m_renderFenceValue, m_renderFenceEvent)))
    {
        if (WaitForSingleObject(m_renderFenceEvent, 20000) != WAIT_OBJECT_0)
        {
            std::exit(-1);
        }
    }
    else
    {
        std::exit(-1);
    }
}

ID3D12GraphicsCommandList7* DXContext::InitRenderCommandList()
{
    ZoneScoped;
    m_cmdAllocator->Reset();
    m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
    return m_cmdList.Get();
}

void DXContext::ExecuteRenderCommandList()
{
    ZoneScoped;
    m_cmdList->Close();
    ID3D12CommandList* lists[] = { m_cmdList.Get()};
    m_renderCmdQueue->ExecuteCommandLists(1, lists);
    SignalRenderAndWait();
}

UINT64 DXContext::ReportAddedAsyncUploadTask()
{
    m_bHasUploadTask = true; 
    return m_asyncUploadFenceValue + 1;
}

UINT64 DXContext::UpdateAsyncUploadCommandQueueState(double InDeltaTime)
{
    ZoneScoped;
    UINT64 CurrentFence = 0;
    if (!m_asyncUploadCmdList) { return CurrentFence; }

    CurrentFence = m_asyncUploadFence->GetCompletedValue();
    if (CurrentFence < m_asyncUploadFenceValue)
    {
        //Currently Uploading
        return CurrentFence;
    }
    //No upload running

    if (m_bHasUploadTask)
    {
        StartAsyncUpload();
    }
    return CurrentFence;
}

ID3D12GraphicsCommandList7* DXContext::InitFrameUploadList()
{
    m_frameUploadCmdAllocator->Reset();
    m_frameUploadCmdList->Reset(m_frameUploadCmdAllocator.Get(), nullptr);
    return m_frameUploadCmdList.Get();
}

void DXContext::SignalFrameUploadAndWait()
{
    m_frameUploadCmdQueue->Signal(m_frameUploadFence.Get(), ++m_frameUploadFenceValue);
    if (SUCCEEDED(m_frameUploadFence->SetEventOnCompletion(m_frameUploadFenceValue, m_frameUploadFenceEvent)))
    {
        if (WaitForSingleObject(m_frameUploadFenceEvent, 20000) != WAIT_OBJECT_0)
        {
            std::exit(-1);
        }
    }
    else
    {
        std::exit(-1);
    }
}

void DXContext::ExecuteFrameUploadCommandList()
{
    ZoneScoped;
    m_frameUploadCmdList->Close();
    ID3D12CommandList* lists[] = { m_frameUploadCmdList.Get() };
    m_frameUploadCmdQueue->ExecuteCommandLists(1, lists);
    SignalFrameUploadAndWait();
}

bool DXContext::InitRenderQueue()
{
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_renderCmdQueue))))
    {
        return false;
    }
    if (FAILED(m_device->CreateFence(m_renderFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_renderFence))))
    {
        return false;
    }
    m_renderFenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (m_renderFenceEvent == nullptr)
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator))))
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList))))
    {
        return false;
    }
    return true;
}

bool DXContext::InitAsyncUploadQueue()
{
    D3D12_COMMAND_QUEUE_DESC asyncUploadCmdQueueDesc{};
    asyncUploadCmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;// D3D12_COMMAND_LIST_TYPE_DIRECT;
    asyncUploadCmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;// D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    asyncUploadCmdQueueDesc.NodeMask = 0;
    asyncUploadCmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT; //D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT ?
    if (FAILED(m_device->CreateCommandQueue(&asyncUploadCmdQueueDesc, IID_PPV_ARGS(&m_asyncUploadCmdQueue))))
    {
        return false;
    }
    if (FAILED(m_device->CreateFence(m_asyncUploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_asyncUploadFence))))
    {
        return false;
    }
    m_asyncUploadFenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (m_asyncUploadFenceEvent == nullptr)
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_asyncUploadCmdAllocator[0]))))
    {
        return false;
    }
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_asyncUploadCmdAllocator[1]))))
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_asyncUploadCmdList[0]))))
    {
        return false;
    }
    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_asyncUploadCmdList[1]))))
    {
        return false;
    }

    m_asyncUploadCmdAllocator[0]->Reset();
    m_asyncUploadCmdList[0]->Reset(m_asyncUploadCmdAllocator[0].Get(), nullptr);
    //Allocator and CmdList[1] Are not Reset because they will be after the first StartUpload
    //So reseting them first there would then cause an issue then
    return true;
}

bool DXContext::InitFrameUploadQueue()
{
    D3D12_COMMAND_QUEUE_DESC frameUploadCmdQueueDesc{};
    frameUploadCmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;// D3D12_COMMAND_LIST_TYPE_DIRECT;
    frameUploadCmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;// D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    frameUploadCmdQueueDesc.NodeMask = 0;
    frameUploadCmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT; //D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT ?
    if (FAILED(m_device->CreateCommandQueue(&frameUploadCmdQueueDesc, IID_PPV_ARGS(&m_frameUploadCmdQueue))))
    {
        return false;
    }
    if (FAILED(m_device->CreateFence(m_frameUploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameUploadFence))))
    {
        return false;
    }
    m_frameUploadFenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (m_frameUploadFenceEvent == nullptr)
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_frameUploadCmdAllocator))))
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_frameUploadCmdList))))
    {
        return false;
    }

    return true;
}

void DXContext::ShutdownRenderQueue()
{
    m_cmdList.Reset();
    m_cmdAllocator.Reset();
    if (m_renderFenceEvent)
    {
        CloseHandle(m_renderFenceEvent);
    }
    m_renderFence.Reset();
    m_renderCmdQueue.Reset();
}

void DXContext::ShutdownAsyncUploadQueue()
{
    m_asyncUploadCmdList[1].Reset();
    m_asyncUploadCmdList[0].Reset();
    m_asyncUploadCmdAllocator[1].Reset();
    m_asyncUploadCmdAllocator[0].Reset();
    if (m_asyncUploadFenceEvent)
    {
        CloseHandle(m_asyncUploadFenceEvent);
    }
    m_asyncUploadFence.Reset();
    m_asyncUploadCmdQueue.Reset();
}

void DXContext::ShutdownFrameUploadQueue()
{
    m_frameUploadCmdList.Reset();
    m_frameUploadCmdAllocator.Reset();
    if (m_frameUploadFenceEvent)
    {
        CloseHandle(m_frameUploadFenceEvent);
    }
    m_frameUploadFence.Reset();
    m_frameUploadCmdQueue.Reset();
}

bool DXContext::StartAsyncUpload()
{
    //Start Upload of currently recording list
    m_asyncUploadCmdList[GetRecordingUpldListIndex()]->Close();
    ID3D12CommandList* uploadLists[] = { m_asyncUploadCmdList[GetRecordingUpldListIndex()].Get() };
    m_asyncUploadCmdQueue->ExecuteCommandLists(1, uploadLists);

    if (FAILED(m_asyncUploadCmdQueue->Signal(m_asyncUploadFence.Get(), ++m_asyncUploadFenceValue)))
    {
        return false;
    }
    //Set previously uploading (or yet unitialized) list&allocator back to record state
    m_asyncUploadCmdAllocator[GetRecordingUpldListIndex()]->Reset();
    m_asyncUploadCmdList[GetRecordingUpldListIndex()]->Reset(m_asyncUploadCmdAllocator[GetRecordingUpldListIndex()].Get(), nullptr);

    m_bHasUploadTask = false;

    return true;
}

DXContext::DXContext(RSRush::RSRProgramInstance* InProgramInstance)
: mds::IRProgramMemElem(InProgramInstance)
{
}
