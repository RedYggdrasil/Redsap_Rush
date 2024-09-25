#include "App/Tools/DebugLayer.h"

CADebugLayer CADebugLayer::Instance = CADebugLayer();

bool CADebugLayer::Init()
{
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))))
    {
        m_d3d12Debug->EnableDebugLayer();

        // Enable additional debug layers.
        //dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug))))
        {
            m_dxgiDebug->EnableLeakTrackingForThread();
            return true;
        }
    }
#endif // _DEBUG
    return false;
}

void CADebugLayer::Shutdown()
{
#ifdef _DEBUG
    if (m_dxgiDebug)
    {
        OutputDebugStringW(L"[DX Debug] DXGI Reports living device objects:\n");
        m_dxgiDebug->ReportLiveObjects(
            DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
        );
    }

    m_dxgiDebug.Reset();
    m_d3d12Debug.Reset();
#endif // _DEBUG
}
