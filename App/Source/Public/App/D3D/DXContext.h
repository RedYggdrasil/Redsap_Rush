#pragma once

#include "App/Libs/WinInclude.h"
#include "MDS/Tools/System/IRProgramMemNode.h"


#include <wrl/client.h>

namespace RSRush
{
	class RSRProgramInstance;
}

class DXContext : public mds::IRProgramMemElem
{
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
	//For frame asynchronous upload, exemple is large texture and asset upload, to be ready at an unspecified time
	bool m_bHasUploadTask = false;
	HANDLE m_asyncUploadFenceEvent = nullptr;
	UINT64 m_asyncUploadFenceValue = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_asyncUploadFence;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_asyncUploadCmdQueue;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_asyncUploadCmdAllocator[2];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_asyncUploadCmdList[2];
	inline size_t GetRecordingUpldListIndex() const { return m_asyncUploadFenceValue % 2; }

private:
	//For frame sensitive upload, need to be completed to draw the next frame
	HANDLE m_frameUploadFenceEvent = nullptr;
	UINT64 m_frameUploadFenceValue = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_frameUploadFence;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_frameUploadCmdQueue;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_frameUploadCmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_frameUploadCmdList;

public:
	static DXContext* Get(mds::IRProgramMemElem* InProgramMemElem);
	static DXContext* Get(RSRush::RSRProgramInstance* InProgramInstance);
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

	inline ID3D12GraphicsCommandList7* GetAsyncUploadList()
	{
		return m_asyncUploadCmdList[GetRecordingUpldListIndex()].Get();
	}
	//Notify the Context than the uploadCmdList has tasks to execute and get the next Fence Value
	UINT64 ReportAddedAsyncUploadTask();
	UINT64 UpdateAsyncUploadCommandQueueState(double InDeltaTime);


	ID3D12GraphicsCommandList7* InitFrameUploadList();

	void SignalFrameUploadAndWait();
	void ExecuteFrameUploadCommandList();
private:
	bool InitRenderQueue();
	bool InitAsyncUploadQueue();
	bool InitFrameUploadQueue();

	void ShutdownRenderQueue();
	void ShutdownAsyncUploadQueue();
	void ShutdownFrameUploadQueue();

	bool StartAsyncUpload();
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

public:
	DXContext(RSRush::RSRProgramInstance* InProgramInstance);
};