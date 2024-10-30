#pragma once
#include "Gen_App/Config/AppConfig.h"
#include "App/Libs/WinInclude.h"
#include <wrl/client.h>
#include <memory>


namespace RSRush
{
    class RSRComputeTextureBuffer
    {
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource2> m_buffer;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_UAVHeapTable;
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUHandle;
    public:
        uint32_t Width;
        uint32_t Height;
        DXGI_FORMAT Format;
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource2> GetBuffer() const { return m_buffer; }
        ID3D12DescriptorHeap* GetUAVHeap() const { return m_UAVHeapTable.Get(); }
        ID3D12DescriptorHeap* const* GetUAVHeapPtr() const { return m_UAVHeapTable.GetAddressOf(); }
        RSRComputeTextureBuffer(const uint32_t InWidth, const uint32_t InHeight, const DXGI_FORMAT InFormat);
        void AllocateDataOnGPU(ID3D12Device10* InDevice);
        void AllocateDescriptorTable(ID3D12Device10* InDevice);

        void TransitionToState(ID3D12GraphicsCommandList7* InCommandList, const D3D12_RESOURCE_STATES InNewState);
    };
};