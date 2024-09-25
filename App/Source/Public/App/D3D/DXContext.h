#pragma once

#include "App/Libs/WinInclude.h"
//#include "App/Tools/ComPointer.hpp"

#include <wrl/client.h>



class DXContext
{
private:
	static DXContext Instance;

private:
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_factory;
	Microsoft::WRL::ComPtr<ID3D12Device10> m_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_renderCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_renderFence;
	HANDLE m_renderFenceEvent = nullptr;
	UINT64 m_renderFenceValue = 0;


	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_cmdList;

private :
	bool m_bHasUploadTask = false;
	HANDLE m_uploadFenceEvent = nullptr;
	UINT64 m_uploadFenceValue = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_uploadFence;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_uploadCmdQueue;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_uploadCmdAllocator[2];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_uploadCmdList[2];
	inline size_t GetRecordingUpldListIndex() const { return m_uploadFenceValue % 2; }

public:
	bool Init();
	void Shutdown();

	void SignalRenderAndWait();
	ID3D12GraphicsCommandList7* InitRenderCommandList();
	void ExecuteRenderCommandList();
	
	inline void FlushRender(size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			SignalRenderAndWait();
		}
	}

	inline const Microsoft::WRL::ComPtr<IDXGIFactory7>& GetFactory()
	{
		return m_factory;
	}
	inline const Microsoft::WRL::ComPtr<ID3D12Device10>& GetDevice()
	{
		return m_device;
	}
	inline const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetRenderCommandQueue()
	{
		return m_renderCmdQueue;
	}

	inline ID3D12GraphicsCommandList7* GetUploadList()
	{
		return m_uploadCmdList[GetRecordingUpldListIndex()].Get();
	}
	//Notify the Context than the uploadCmdList has tasks to execute and get the next Fence Value
	UINT64 ReportAddedUploadTask();
	UINT64 UpdateUploadCommandQueueState(double InDeltaTime);
private:
	bool InitRenderQueue();
	bool InitUploadQueue();

	void ShutdownRenderQueue();
	void ShutdownUploadQueue();

	bool StartUpload();
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;
	inline static DXContext& Get()
	{
		return Instance;
	}

private:
	DXContext() = default;
};