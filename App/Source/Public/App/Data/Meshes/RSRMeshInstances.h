#pragma once

#include "App/D3D/DXContext.h"
#include "App/Libs/WinInclude.h"
#include <memory>
#include <wrl/client.h>

namespace RSRush
{
#define l_ComPtr	Microsoft::WRL::ComPtr
	class RSRMeshInstances
	{
	protected:
		const UINT m_InstanceUnitSize;
	protected:
		 size_t m_currentUploadCapacity;
		 size_t m_currentRenderCapacity;

		 size_t m_currentUploadInstanceCount;
		 size_t m_currentRenderInstanceCount;


		l_ComPtr<ID3D12Resource2> m_uploadBuffer;
		//GPU Resources
		l_ComPtr<ID3D12Resource2> m_instancesBuffer;

	public:
		inline UINT64 GetInstanceUnitSize() const { return m_InstanceUnitSize; };
		inline UINT64 GetInstanceBufferSize(const size_t InInstanceCount) const { return GetInstanceUnitSize() * InInstanceCount; };
		UINT64 GetAlignedInstanceBufferSize(const size_t InInstanceCount) const;

		inline size_t GetCurrentRenderInstanceCount() const { return m_currentRenderInstanceCount; }

		//virtual const void* GetInstanceAdress() const = 0;// { return m_vertexes; }

		inline const l_ComPtr<ID3D12Resource2>& GetInstancesBuffer() const { return m_instancesBuffer; }

	public:
		bool FreeUploadBuffer();
		//In this case Resource Buffer and Upload buffer are independants and desync, Resources can be freed and Upload kept
		bool FreeResourcesBuffer();
		bool FreeAllBuffers();
		void UpdateUploadBuffer(ID3D12Device10* InDevice, void* InInstanceDatas, const size_t InInstancesCount);

		void CopyUploadBufferToGPU(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList);
	
	public:
		RSRMeshInstances(const UINT InVertexUnitSize);

	public:
		 virtual ~RSRMeshInstances()/*override*/;
	public:
		bool EnsureUploadCapacity(ID3D12Device10* InDevice, const size_t InTargetCapacity, const size_t InNewMarginIfRecreate);
		bool EnsureResourcesCapacity(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, const size_t InTargetCapacity, const size_t InNewMarginIfRecreate);

		bool CreateUploadBuffer(ID3D12Device10* InDevice, const size_t InTargetCapacity);
		bool CreateResourceBuffer(ID3D12Device10* InDevice, const size_t InTargetCapacity);
		//Call in sync update
		//bool UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, const size_t InTargetCapacity);
	};
#undef l_ComPtr
}