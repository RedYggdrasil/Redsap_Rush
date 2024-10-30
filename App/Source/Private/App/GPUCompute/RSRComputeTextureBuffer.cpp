#include "App/GPUCompute/RSRComputeTextureBuffer.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"
#include "App/Tools/Shader.h"
#include "App/Tools/ImageLoader.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include <string>


using namespace RSRush;
using namespace DirectX;


RSRComputeTextureBuffer::RSRComputeTextureBuffer(const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat)
    : Width(InWidth), Height(InHeight), Format(InFormat)
{

}

void RSRComputeTextureBuffer::AllocateDataOnGPU(ID3D12Device10* InDevice)
{

    // 1) Write a description of our resource //
    D3D12_RESOURCE_DESC bufferDescription;
    bufferDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    bufferDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    bufferDescription.Width = Width;
    bufferDescription.Height = Height;
    bufferDescription.DepthOrArraySize = 1;
    bufferDescription.MipLevels = 1;
    bufferDescription.Format = Format;
    bufferDescription.SampleDesc = { .Count = 1, .Quality = 0 };
    bufferDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    bufferDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;



    //// 2) Allocate our buffer with the device using the resource description //
    //CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_HEAP_PROPERTIES heapPropertyDefault{};
    heapPropertyDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapPropertyDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapPropertyDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapPropertyDefault.CreationNodeMask = 0;
    heapPropertyDefault.VisibleNodeMask = 0;

    InDevice->CreateCommittedResource(&heapPropertyDefault, D3D12_HEAP_FLAG_NONE,
        &bufferDescription, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_buffer));
}


void RSRComputeTextureBuffer::AllocateDescriptorTable(ID3D12Device10* InDevice)
{

    UINT nbTexture = 1;
    // -- Descriptor Heap for Texture(s) --
    D3D12_DESCRIPTOR_HEAP_DESC dhd =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = nbTexture,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0
    };
    //Create Table for Texture UAVs
    HRESULT result = InDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&m_UAVHeapTable));
    if (FAILED(result))
    {
        RSRLog::Log(LOG_ERROR, TEXT("Cannot create texture Descriptor Heaps !"), result); return;
    }

    //Create CPU view handles
    D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = m_UAVHeapTable->GetCPUDescriptorHandleForHeapStart();
    UINT handleSize = InDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    UINT UAVIndex = 0;

    m_CPUHandle = firstHandle;
    m_CPUHandle.ptr += (UAVIndex * handleSize);

    // --UAV for texture --
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav;
    uav.Format = Format;
    uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uav.Texture2D = D3D12_TEX2D_UAV
    {
        .MipSlice = 0,
        .PlaneSlice = 0
    };

    InDevice->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uav, m_CPUHandle);

}

void RSRComputeTextureBuffer::TransitionToState(ID3D12GraphicsCommandList7* InCommandList, const D3D12_RESOURCE_STATES InNewState)
{
    // Describe the transition barrier
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_buffer.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS; // Current state
    barrier.Transition.StateAfter = InNewState; // New state
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // Execute the transition
    InCommandList->ResourceBarrier(1, &barrier);
}