#include "App/Data/Meshes/RSRMeshInstances.h"
#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"


UINT64 RSRush::RSRMeshInstances::GetAlignedInstanceBufferSize(const size_t InInstanceCount) const
{
	return mds::RMath::Align<UINT64>(this->GetInstanceBufferSize(InInstanceCount), 1024);
}

bool RSRush::RSRMeshInstances::FreeUploadBuffer()
{
    bool bSucess = true;
    if (m_instancesBuffer)
    {
        m_instancesBuffer.Reset();
    }
    m_currentUploadCapacity = m_currentUploadInstanceCount = 0;
    return bSucess;
}

bool RSRush::RSRMeshInstances::FreeResourcesBuffer()
{
    //In this case Resource Buffer and Upload buffer are independants and desync, Resources can be freed and Upload kept
    bool bSucess = true;

    if (m_instancesBuffer)
    {
        m_instancesBuffer.Reset();
    }
    m_currentRenderCapacity = m_currentRenderInstanceCount = 0;
    return true;
}

bool RSRush::RSRMeshInstances::FreeAllBuffers()
{
	return FreeResourcesBuffer() && FreeUploadBuffer();
}

void RSRush::RSRMeshInstances::UpdateUploadBuffer(ID3D12Device10* InDevice, void* InInstanceDatas, const size_t InInstancesCount)
{
    bool bAllSucessfull = EnsureUploadCapacity(InDevice, InInstancesCount, 10);
    if (bAllSucessfull)
    {
        void* mappedData;
        m_uploadBuffer->Map(0, nullptr, &mappedData);
        memcpy(mappedData, InInstanceDatas, GetInstanceBufferSize(InInstancesCount));
        m_uploadBuffer->Unmap(0, nullptr);

        m_currentUploadInstanceCount = InInstancesCount;
    }
}

void RSRush::RSRMeshInstances::CopyUploadBufferToGPU(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
    bool bAllSucessfull = EnsureResourcesCapacity(InDevice, InUploadCommandList, m_currentUploadInstanceCount, 10);

    if (bAllSucessfull)
    {
        const UINT64 AlignedInstancesBufferSize = this->GetAlignedInstanceBufferSize(m_currentUploadInstanceCount);
        InUploadCommandList->CopyBufferRegion(m_instancesBuffer.Get(), /*offset*/0, m_uploadBuffer.Get(), /*offset*/0, AlignedInstancesBufferSize);
        m_currentRenderInstanceCount = m_currentUploadInstanceCount;
    }
}


RSRush::RSRMeshInstances::RSRMeshInstances(const UINT InVertexUnitSize)
	: m_InstanceUnitSize(InVertexUnitSize), m_currentUploadCapacity(0), m_currentRenderCapacity(0), m_currentUploadInstanceCount(0), m_currentRenderInstanceCount(0)
{
}

RSRush::RSRMeshInstances::~RSRMeshInstances()
{
	bool bSucess = FreeAllBuffers();
}

bool RSRush::RSRMeshInstances::EnsureUploadCapacity(ID3D12Device10* InDevice, const size_t InTargetCapacity, const size_t InNewMarginIfRecreate)
{
    if (m_currentUploadCapacity < InTargetCapacity || (InTargetCapacity < 1 && m_currentUploadCapacity < InNewMarginIfRecreate))
    {
        return CreateUploadBuffer(InDevice, InTargetCapacity + InNewMarginIfRecreate);
    }
    return true;
}

bool RSRush::RSRMeshInstances::EnsureResourcesCapacity(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, const size_t InTargetCapacity, const size_t InNewMarginIfRecreate)
{
    if (m_currentRenderCapacity < InTargetCapacity || (InTargetCapacity < 1 && m_currentRenderCapacity < InNewMarginIfRecreate))
    {
        return CreateResourceBuffer(InDevice, InTargetCapacity + InNewMarginIfRecreate);
    }
    return true;
}

bool RSRush::RSRMeshInstances::CreateUploadBuffer(ID3D12Device10* InDevice, const size_t InTargetCapacity)
{
    bool bAllSucessfull = true;
    m_currentUploadCapacity = m_currentUploadInstanceCount = 0;
    if (m_uploadBuffer)
    {
        bAllSucessfull = FreeUploadBuffer() && bAllSucessfull;
        if (!bAllSucessfull)
        {
            return bAllSucessfull;
        }
    }

    const UINT64 AlignedInstancesBufferSize = this->GetAlignedInstanceBufferSize(InTargetCapacity);

    //Upload Heap Properties
    D3D12_HEAP_PROPERTIES hpUpload{};
    hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
    hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hpUpload.CreationNodeMask = 0;
    hpUpload.VisibleNodeMask = 0;


    D3D12_RESOURCE_DESC rdu{};
    rdu.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rdu.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rdu.Width = AlignedInstancesBufferSize;
    rdu.Height = 1; //Size for 2nd dimension of array
    rdu.DepthOrArraySize = 1; //Size for 3rd dimension of array
    rdu.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
    rdu.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
    rdu.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
    rdu.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
    rdu.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create the CPU side upload Buffer, can be done during render, but not during a GPU upload
    HRESULT hr = InDevice->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &rdu, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uploadBuffer));
    if (FAILED(hr))
    {
        return bAllSucessfull = false;
    }

    m_currentUploadCapacity = InTargetCapacity;
    return bAllSucessfull;
}

bool RSRush::RSRMeshInstances::CreateResourceBuffer(ID3D12Device10* InDevice, const size_t InTargetCapacity)
{
    bool bAllSucessfull = true;
    m_currentRenderCapacity =  m_currentRenderInstanceCount = 0;
    if (m_instancesBuffer)
    {
        bAllSucessfull = FreeResourcesBuffer() && bAllSucessfull;
        if (!bAllSucessfull)
        {
            return bAllSucessfull;
        }
    }

    const UINT64 AlignedInstancesBufferSize = this->GetAlignedInstanceBufferSize(InTargetCapacity);

    //Device Resource Heap Properties
    D3D12_HEAP_PROPERTIES hpDefault{};
    hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hpDefault.CreationNodeMask = 0;
    hpDefault.VisibleNodeMask = 0;


    //Vertice Buffer Description
    D3D12_RESOURCE_DESC rdi{};
    rdi.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rdi.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rdi.Width = AlignedInstancesBufferSize;
    rdi.Height = 1; //Size for 2nd dimension of array
    rdi.DepthOrArraySize = 1; //Size for 3rd dimension of array
    rdi.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
    rdi.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
    rdi.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
    rdi.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
    rdi.Flags = D3D12_RESOURCE_FLAG_NONE;

    //Create the GPU resource Buffer, can NOT be done during render or GPU upload
    HRESULT hr = InDevice->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rdi, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_instancesBuffer));
    if (FAILED(hr))
    {
        return bAllSucessfull = false;
    }

    m_currentRenderCapacity = InTargetCapacity;
    return bAllSucessfull;
}
