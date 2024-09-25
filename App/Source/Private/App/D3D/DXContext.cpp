#include "App/D3D/DXContext.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"
#include <cstdlib>
#include <Tracy.hpp>

DXContext DXContext::Instance = DXContext();

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
    RSRush::RSRProgramInstance::Get().SetTearingSupported(bTearingSupportedOnDevice);
    if (!bTearingSupportedOnDevice)
    {
        RSRLog::Log(LOG_WARNING, TEXT("System does not allow Tearing !"), AllowTearingResult);
    }
    //////////////End test for tearing support//////////////

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device))))
    {
        return false;
    }
    if (!InitUploadQueue())
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
    ShutdownUploadQueue();
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

UINT64 DXContext::ReportAddedUploadTask()
{
    m_bHasUploadTask = true; 
    return m_uploadFenceValue + 1;
}

UINT64 DXContext::UpdateUploadCommandQueueState(double InDeltaTime)
{
    ZoneScoped;
    UINT64 CurrentFence = 0;
    if (!m_uploadCmdList) { return CurrentFence; }

    CurrentFence = m_uploadFence->GetCompletedValue();
    if (CurrentFence < m_uploadFenceValue)
    {
        //Currently Uploading
        return CurrentFence;
    }
    //No upload running

    if (m_bHasUploadTask)
    {
        StartUpload();
    }
    return CurrentFence;
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

bool DXContext::InitUploadQueue()
{
    D3D12_COMMAND_QUEUE_DESC uploadCmdQueueDesc{};
    uploadCmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;// D3D12_COMMAND_LIST_TYPE_DIRECT;
    uploadCmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;// D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    uploadCmdQueueDesc.NodeMask = 0;
    uploadCmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT; //D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT ?
    if (FAILED(m_device->CreateCommandQueue(&uploadCmdQueueDesc, IID_PPV_ARGS(&m_uploadCmdQueue))))
    {
        return false;
    }
    if (FAILED(m_device->CreateFence(m_uploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_uploadFence))))
    {
        return false;
    }
    m_uploadFenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (m_uploadFenceEvent == nullptr)
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_uploadCmdAllocator[0]))))
    {
        return false;
    }
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_uploadCmdAllocator[1]))))
    {
        return false;
    }

    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_uploadCmdList[0]))))
    {
        return false;
    }
    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_uploadCmdList[1]))))
    {
        return false;
    }

    m_uploadCmdAllocator[0]->Reset();
    m_uploadCmdList[0]->Reset(m_uploadCmdAllocator[0].Get(), nullptr);
    //Allocator and CmdList[1] Are not Reset because they will be after the first StartUpload
    //So reseting them first there would then cause an issue then
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

void DXContext::ShutdownUploadQueue()
{
    m_uploadCmdList[1].Reset();
    m_uploadCmdList[0].Reset();
    m_uploadCmdAllocator[1].Reset();
    m_uploadCmdAllocator[0].Reset();
    if (m_uploadFenceEvent)
    {
        CloseHandle(m_uploadFenceEvent);
    }
    m_uploadFence.Reset();
    m_uploadCmdQueue.Reset();
}

bool DXContext::StartUpload()
{
    //Start Upload of currently recording list
    m_uploadCmdList[GetRecordingUpldListIndex()]->Close();
    ID3D12CommandList* uploadLists[] = { m_uploadCmdList[GetRecordingUpldListIndex()].Get() };
    m_uploadCmdQueue->ExecuteCommandLists(1, uploadLists);

    if (FAILED(m_uploadCmdQueue->Signal(m_uploadFence.Get(), ++m_uploadFenceValue)))
    {
        return false;
    }
    //Set previously uploading (or yet unitialized) list&allocator back to record state
    m_uploadCmdAllocator[GetRecordingUpldListIndex()]->Reset();
    m_uploadCmdList[GetRecordingUpldListIndex()]->Reset(m_uploadCmdAllocator[GetRecordingUpldListIndex()].Get(), nullptr);

    m_bHasUploadTask = false;

    return true;
}
