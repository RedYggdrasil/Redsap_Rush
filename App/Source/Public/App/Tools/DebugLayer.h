#pragma once

#include "App/Libs/WinInclude.h"
//#include "App/Tools/ComPointer.hpp"

#include <wrl/client.h>



class CADebugLayer
{
private:
	static CADebugLayer Instance;

public:
	bool Init();
	void Shutdown();
private:
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug6> m_d3d12Debug;
	Microsoft::WRL::ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif // _DEBUG

public:
	CADebugLayer(const CADebugLayer&) = delete;
	CADebugLayer& operator=(const CADebugLayer&) = delete;
	inline static CADebugLayer& Get()
	{
		return Instance;
	}
private:
	CADebugLayer() = default;
};