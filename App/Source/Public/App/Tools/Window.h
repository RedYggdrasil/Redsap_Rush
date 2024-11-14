#pragma once

#include "App/Libs/WinInclude.h"
#include "MDS/Tools/System/IRProgramMemNode.h"
//#include "App/Tools/ComPointer.hpp"
#include "directxmath.h"

#include <vector>
#include <memory>
#include <wrl/client.h>
#include <unordered_map>

namespace DXWindowDefaults
{
	extern const UINT WIDTH;
	extern const UINT HEIGHT;
	constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;
	extern const UINT SWAP_CHAIN_FLAGS;
	extern const DXGI_FORMAT SWAP_CHAIN_BUFFER_FORMAT;
	extern const DXGI_FORMAT DEPTH_BUFFER_ENABLED;
	extern const DXGI_FORMAT DEPTH_BUFFER_DISABLED;
	extern const float SWAP_CHAIN_BUFFER_BACKGROUND_COLOR[4];
	extern LPCWSTR CLASS_NAME;
	extern LPCWSTR INSTANCE_NAME;
	extern const DWORD WINDOW_DEFAULT_STYLE;
	extern const DWORD WINDOW_DEFAULT_EXSTYLE;
	extern const DWORD WINDOW_FULLSCREEN_STYLE;
	extern const DWORD WINDOW_FULLSCREEN_EXSTYLE;
}

namespace RSRush
{
	class IInputListener;
	class RSRUserInput;
	class RSRProgramInstance;
}
class DXWindow : public mds::IRProgramMemElem
{
private:
	static std::unordered_map<HWND, DXWindow*> sm_dxWindowLookup;
protected:
	bool m_bUseVerticalSync = true;
	std::vector<std::shared_ptr<RSRush::IInputListener>> m_InputListeners;
	std::weak_ptr<RSRush::RSRUserInput> m_mouseRelativePosUser;
	DirectX::XMFLOAT2 m_mouseRelativePosition;

public:
	static DXWindow* Get(mds::IRProgramMemElem* InProgramMemElem);
	static DXWindow* Get(RSRush::RSRProgramInstance* InProgramInstance);
public:
	bool Init(RSRush::RSRProgramInstance* InProgramInstance);
	void Update();
	void Present();
	void Shutdown();
	void Resize();
	bool CreateDepthBuffer(UINT InWidth, UINT InHeight);
	void SetFullscreen(bool InbEnabled);
	void SetUsingVerticalSync(bool InbEnabled);

	void BeginFrame(ID3D12GraphicsCommandList7* cmdList);
	void EndFrame(ID3D12GraphicsCommandList7* cmdList);

	void AddUniqueInputListener(std::shared_ptr<RSRush::IInputListener> InListener);
	void RemoveInputListener(std::shared_ptr<RSRush::IInputListener> InListener);

	inline const DirectX::XMFLOAT2& GetMouseRelativePosition() const { return m_mouseRelativePosition; }

	inline bool ShouldClose() const
	{
		return m_shouldClose;
	}
	inline bool ShouldResize() const
	{
		return m_shouldResize;
	}
	inline bool IsFullscreen() const
	{
		return m_isFullscreen;
	}

	inline UINT GetWidth() const { return m_width; }
	inline UINT GetHeight() const { return m_height; }
	inline float GetRelativeHeight() const { return ((float)m_height / (float)DXWindowDefaults::HEIGHT); }
	inline float GetRelativeWidth() const { return ((float)m_width / (float)DXWindowDefaults::WIDTH); }

	float ComputeAspectRatio() const { return ((float)m_height / (float)m_width); }
	
	DirectX::XMFLOAT2 Compute2DScale() const;

	static constexpr UINT GetFrameCount()
	{
		return DXWindowDefaults::SWAP_CHAIN_BUFFER_COUNT;
	}
private:
	ATOM m_wndClass = 0;
	HWND m_window = nullptr;
	bool m_shouldClose = false;
	bool m_shouldResize = false;
	// swapChain width, not windows width
	UINT m_width = DXWindowDefaults::WIDTH;
	// swapChain height, not windows height
	UINT m_height = DXWindowDefaults::HEIGHT;

	bool m_isFullscreen = false;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource2> m_buffers[DXWindowDefaults::SWAP_CHAIN_BUFFER_COUNT];
	UINT8 m_currentBufferIndex = 0;

	//Render Target View Descriptor Heap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[DXWindowDefaults::SWAP_CHAIN_BUFFER_COUNT];

	//Depth stencil buffer
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
	{
		return m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart();
	}


	bool GetBuffers();
	void ReleaseBuffers();
	static LRESULT OnWindowMessage(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;
	DXWindow();
	virtual ~DXWindow();
private:
};