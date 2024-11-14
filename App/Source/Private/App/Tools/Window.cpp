#include "App/Tools/Window.h"

#include "App/Tools/RSRLog.h"
#include "mds/Tools/System/RKeyCodes.h"
#include "App/D3D/DXContext.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Interfaces/Listeners/IInputListener.h"
#include <Tracy.hpp>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <WindowsX.h>

namespace DXwd = DXWindowDefaults;

const UINT DXwd::WIDTH      = 1280;
const UINT DXwd::HEIGHT     = 720;
const UINT DXwd::SWAP_CHAIN_FLAGS = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
const DXGI_FORMAT DXwd::SWAP_CHAIN_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
const DXGI_FORMAT DXwd::DEPTH_BUFFER_ENABLED = DXGI_FORMAT_D32_FLOAT;
const DXGI_FORMAT DXwd::DEPTH_BUFFER_DISABLED = DXGI_FORMAT_UNKNOWN;
const float DXwd::SWAP_CHAIN_BUFFER_BACKGROUND_COLOR[4] = {0.f, 0.f , 0.f, 1.f};
LPCWSTR DXwd::CLASS_NAME    = L"RSRWndCls";
LPCWSTR DXwd::INSTANCE_NAME = L"RedSap : Rush";


const DWORD DXwd::WINDOW_DEFAULT_STYLE      = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
const DWORD DXwd::WINDOW_DEFAULT_EXSTYLE    = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
const DWORD DXwd::WINDOW_FULLSCREEN_STYLE   = WS_POPUP | WS_VISIBLE;
const DWORD DXwd::WINDOW_FULLSCREEN_EXSTYLE = WS_EX_APPWINDOW;



std::unordered_map<HWND, DXWindow*> DXWindow::sm_dxWindowLookup;



DXWindow* DXWindow::Get(mds::IRProgramMemElem* InProgramMemElem)
{
    return InProgramMemElem->GetRoot<RSRush::RSRProgramInstance>()->GetDXWindow();
}

DXWindow* DXWindow::Get(RSRush::RSRProgramInstance* InProgramInstance)
{
    return InProgramInstance->GetDXWindow();
}

bool DXWindow::Init(RSRush::RSRProgramInstance* InProgramInstance)
{
    this->InitMemElem(InProgramInstance);

    HRESULT hResult;
    WNDCLASSEXW wcex{};
    wcex.cbSize     = sizeof(wcex);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = &DXWindow::OnWindowMessage;
    wcex.cbClsExtra = 0; //Extra memory per Window Class, default = 0
    wcex.cbWndExtra = 0; //Extra memory per Window Instance, default = 0
    wcex.hInstance = GetModuleHandle(nullptr); //Instance ptr of owning app 
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = DXwd::CLASS_NAME;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    m_wndClass = RegisterClassExW(&wcex);

    if (m_wndClass == 0)
    {
        return false;
    }

    //Get monitor currently under the mouse
    POINT pos{ 0, 0 };
    GetCursorPos(&pos);
    HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);

    m_window = CreateWindowExW
    (
        DXwd::WINDOW_DEFAULT_EXSTYLE, 
        (LPCWSTR)m_wndClass,
        DXwd::INSTANCE_NAME, 
        DXwd::WINDOW_DEFAULT_STYLE,
        monitorInfo.rcWork.left + 100,
        monitorInfo.rcWork.top + 100,
        DXwd::WIDTH, DXwd::HEIGHT, nullptr, nullptr, wcex.hInstance, nullptr
    );
    if (m_window == nullptr)
    {
        return false;
    }

    sm_dxWindowLookup.emplace(m_window, this);

    DXContext* dxContext = DXContext::Get(this);

    const auto& factory = dxContext->GetFactory();

    DXGI_SWAP_CHAIN_DESC1 swd{};
    swd.Width       = DXwd::WIDTH;
    swd.Height      = DXwd::HEIGHT;
    swd.Format      = DXwd::SWAP_CHAIN_BUFFER_FORMAT;
    swd.Stereo      = false;
    //swd.SampleDesc  = { .Count = 1, .Quality = 0}; //Aggregate named init, Cpp 20
    swd.SampleDesc.Count = 1;
    swd.SampleDesc.Quality = 0;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.BufferCount = DXwd::SWAP_CHAIN_BUFFER_COUNT; //write to the back buffer, display the from buffer, when back buffer ready, swap
    //A value of 3 is for vsync (Display is a queue of 2 avalable buffer, and swap when the screen has finished rendering the current image, 
    //avoiding tearing
    swd.Scaling     = DXGI_SCALING_STRETCH; //on windows resize before redraw
    swd.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swd.AlphaMode   = DXGI_ALPHA_MODE_IGNORE;
    swd.Flags       = DXwd::SWAP_CHAIN_FLAGS;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};

    //sfd.RefreshRate         =; //Value ignored on Windowed mode
    //sfd.ScanlineOrdering    =; //Value ignored on Windowed mode
    //sfd.Scaling             =; //Value ignored on Windowed mode
    sfd.Windowed            = true;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> sc1;
    
    hResult = factory->CreateSwapChainForHwnd(dxContext->GetRenderCommandQueue().Get(), m_window, &swd, &sfd, nullptr, &sc1);
    if (FAILED(hResult))
    {
        RSRLog::Log(LOG_EXCEPTION, hResult, TEXT("Failed to create Swap Chain !"));
        return false;
    }
    hResult = sc1.CopyTo(IID_PPV_ARGS(&m_swapChain));
    if (FAILED(hResult))
    {
        RSRLog::Log(LOG_EXCEPTION, hResult, TEXT("Failed to copy Swap Chain !"));
        return false;
    }

    //Create RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descHeapDesc.NumDescriptors = DXWindowDefaults::SWAP_CHAIN_BUFFER_COUNT;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    descHeapDesc.NodeMask = 0;
    
    hResult = dxContext->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap));
    if (FAILED(hResult))
    {
        RSRLog::Log(LOG_EXCEPTION, hResult, TEXT("Failed to create DescriptorHeap !"));
        return false;
    }
    //Create CPU view handles
    D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();

    UINT handleSize = dxContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    for (size_t i = 0; i < DXWindowDefaults::SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        m_rtvHandles[i] = firstHandle;
        m_rtvHandles[i].ptr += (i * handleSize);
    }

    //Create DSV Heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    hResult = dxContext->GetDevice()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&m_dsvDescHeap));
    if (FAILED(hResult))
    {
        RSRLog::Log(LOG_EXCEPTION, hResult, TEXT("Failed to create DescriptorHeap m_dsvDescHeap !"));
        return false;
    }

    //Get Buffers
    if (!GetBuffers())
    {
        return false;
    }

    if (!CreateDepthBuffer(DXwd::WIDTH, DXwd::HEIGHT))
    {
        return false;
    }

    return true;
}

void DXWindow::Update()
{
    ZoneScoped;
    MSG msg;
    //PM_REMOVE get the message to &msg and then "consume" it  (remove it from message queue)
    while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE)) //wile there is still message relating to this windows
    {
        TranslateMessage(&msg);
        //This will send the message to WndProc function (OnWindowMessage)
        DispatchMessageW(&msg);
    }
}

void DXWindow::Present()
{
    ZoneScoped;
    if (m_bUseVerticalSync)
    {
        m_swapChain->Present(1, 0);
    }
    else
    {
        m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
}

void DXWindow::Shutdown()
{
    ReleaseBuffers();

    m_rtvDescHeap.Reset();

    m_swapChain.Reset();

    m_dsvDescHeap.Reset();
    m_depthStencil.Reset();
    if (m_window)
    {
        DestroyWindow(m_window);
    }

    if (m_wndClass)
    {
        UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
    }
    sm_dxWindowLookup.erase(m_window);
    if (m_initializedMemElem)
    {
        this->ResetMemTreeData();
    }
}

void DXWindow::Resize()
{
    ReleaseBuffers();
    RECT cr;
    if (GetClientRect(m_window, &cr))
    {
        m_width = cr.right - cr.left;
        m_height = cr.bottom - cr.top;

        if (FAILED(m_swapChain->ResizeBuffers(GetFrameCount(), m_width, m_height, DXGI_FORMAT_UNKNOWN, DXwd::SWAP_CHAIN_FLAGS)))
        {
            //Too bad
        }
        if (!CreateDepthBuffer(m_width, m_height))
        {
            //Too bad
        }
        m_shouldResize = false;
    }
    GetBuffers();
}
bool DXWindow::CreateDepthBuffer(UINT InWidth, UINT InHeight)
{
    //Create Depth Buffer : 
    DXContext* dxContext = DXContext::Get(this);
    D3D12_HEAP_PROPERTIES depthHeapProperties =
    {
        .Type = D3D12_HEAP_TYPE_DEFAULT,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1,
        .VisibleNodeMask = 1
    };

    D3D12_RESOURCE_DESC depthResourceDesc
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = 0,
        .Width = InWidth,
        .Height = InHeight,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXwd::DEPTH_BUFFER_ENABLED,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = (D3D12_RESOURCE_FLAG_NONE | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
    };
    D3D12_CLEAR_VALUE clearValue =
    {
        .Format = DXwd::DEPTH_BUFFER_ENABLED,
        .DepthStencil =
        {
            .Depth = 0.f,
            .Stencil = 0
        }
    };

    if (FAILED(dxContext->GetDevice()->CreateCommittedResource(
        &depthHeapProperties, D3D12_HEAP_FLAG_NONE, &depthResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_depthStencil)
    )))
    {
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc =
    {
        .Format = DXwd::DEPTH_BUFFER_ENABLED,
        .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
        .Flags = D3D12_DSV_FLAG_NONE
    };
    dxContext->GetDevice()->CreateDepthStencilView(
        m_depthStencil.Get(),
        &dsvDesc,
        m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart());
    return true;
}


void DXWindow::SetFullscreen(bool enabled)
{
    //Set stype
    SetWindowLongW(m_window, GWL_STYLE  , enabled ? DXwd::WINDOW_FULLSCREEN_STYLE   : DXwd::WINDOW_DEFAULT_STYLE    );
    SetWindowLongW(m_window, GWL_EXSTYLE, enabled ? DXwd::WINDOW_FULLSCREEN_EXSTYLE : DXwd::WINDOW_DEFAULT_EXSTYLE  );

    //Adjust window size
    if (enabled)
    {
        HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo))
        {
            SetWindowPos
            (
                m_window, nullptr,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOZORDER
            );
        }
    }
    else
    {
        ShowWindow(m_window, SW_MAXIMIZE);
    }

    m_isFullscreen = enabled;
}

void DXWindow::SetUsingVerticalSync(bool InbEnabled)
{
    m_bUseVerticalSync = InbEnabled;
}

void DXWindow::BeginFrame(ID3D12GraphicsCommandList7* cmdList)
{
    //The Buffer was just swapped from Prensent State to Render State, we need to set it to the
    //rendering state before writing to it
    m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex].Get();
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    cmdList->ResourceBarrier(1, &barr);
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart();
    cmdList->ClearRenderTargetView(m_rtvHandles[m_currentBufferIndex], DXwd::SWAP_CHAIN_BUFFER_BACKGROUND_COLOR, 0, nullptr);
    cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);

    cmdList->OMSetRenderTargets(1, &m_rtvHandles[m_currentBufferIndex], false, &depthStencilView);
}

void DXWindow::EndFrame(ID3D12GraphicsCommandList7* cmdList)
{
    ZoneScoped;
    //The Buffer is now completly drawn on, we need to set it back into Present State before presenting it
    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex].Get();
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    cmdList->ResourceBarrier(1, &barr);
}

void DXWindow::AddUniqueInputListener(std::shared_ptr<RSRush::IInputListener> InListener)
{
    auto it = std::find(m_InputListeners.begin(), m_InputListeners.end(), InListener);
    if (it == m_InputListeners.end()) {
        m_InputListeners.push_back(InListener);
    }
}

void DXWindow::RemoveInputListener(std::shared_ptr<RSRush::IInputListener> InListener)
{
    auto it = std::find(m_InputListeners.begin(), m_InputListeners.end(), InListener);
    if (it != m_InputListeners.end()) {
        m_InputListeners.erase(std::remove(m_InputListeners.begin(), m_InputListeners.end(), InListener), m_InputListeners.end());
    }
}

DirectX::XMFLOAT2 DXWindow::Compute2DScale() const
{
    float ratioX = fminf((float)m_height / (float)m_width, 1.f);
    float ratioY = fminf((float)m_width / (float)m_height, 1.f);
    bool bIsWidescreen = m_width > m_height;
    return DirectX::XMFLOAT2
    (
        ratioX,
        ratioY
    );
}

bool DXWindow::GetBuffers()
{
    for (UINT i = 0; i < DXwd::SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]))))
        {
            return false;
        }
        D3D12_RENDER_TARGET_VIEW_DESC rtv{};
        rtv.Format = DXwd::SWAP_CHAIN_BUFFER_FORMAT;
        rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtv.Texture2D = { .MipSlice = 0, .PlaneSlice = 0 };
        DXContext::Get(this)->GetDevice()->CreateRenderTargetView(m_buffers[i].Get(), &rtv, m_rtvHandles[i]);
    }
    return true;
}

void DXWindow::ReleaseBuffers()
{
    for (INT32 i = DXwd::SWAP_CHAIN_BUFFER_COUNT - 1; i > -1; --i)
    {
        m_buffers[i].Reset();
    }
}

//lParam is windows parameters
LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto it = sm_dxWindowLookup.find(wnd);

    if (it != sm_dxWindowLookup.end()) 
    {
        DXWindow* thisPtr = it->second;

        switch (msg)
        {
        case WM_LBUTTONDOWN:
            //RSRLog::Log(LOG_DISPLAY, TEXT("Mouse LButton Down !"));
            break;
        case WM_LBUTTONUP:
            //RSRLog::Log(LOG_DISPLAY, TEXT("Mouse LButton Up !"));
            break;
        case WM_MOUSEMOVE:
        {
            int xAxis = GET_X_LPARAM(lparam);
            int yAxis = GET_Y_LPARAM(lparam);

            thisPtr->m_mouseRelativePosition =
            {
                ((float)xAxis / (float)thisPtr->m_width),
                ((float)yAxis / (float)thisPtr->m_height)
            };
            break;
        }
        case WM_KEYDOWN:
        {
            if (wparam == VK_F11)
            {
                thisPtr->SetFullscreen((!thisPtr->IsFullscreen()));
            }
        }
        case WM_KEYUP:
        {
        }
        case WM_CHAR:
        {
            //std::cout << "WM_CHAR : '" << wparam << "' (" << lparam << ")" << std::endl;
            break;
        }
        //Window resize event
        case WM_SIZE:
        {
            //In case of WM_SIZE lParam is a 64bit integer witch is actually 2 short UINT height and width bundled together on the low order (first 32 bytes)
            // lparam BYTE : [EMPTY] [EMPTY] [EMPTY] [EMPTY] [HEIGHT] [HEIGHT] [WIDTH] [WIDTH]
            //                                                 HIWORD(lparam)    LOWORD(lparam)
            //if lParam is not 0 (would be zero if window minimized), we check if width or height is different than buffer size)
            //HIWORD return hight order value from value (first 2 bytes) LOWORD return low order value

            if (lparam/*not 0*/ && (HIWORD(lparam) != thisPtr->m_height || LOWORD(lparam) != thisPtr->m_width))
            {
                //We are there if windows is drawn (size > 0) et size different than current swap chain
                //This avoid a swap chain resize on minimize and reopen, as size would have stayed the same
                thisPtr->m_shouldResize = true;
            }
            break;
        }
        //Window close event
        case WM_CLOSE:
            thisPtr->m_shouldClose = true;
            return 0;
            //Handle Specific cases
        default: break;
        }
    }


    //Default
    return DefWindowProcW(wnd, msg, wparam, lparam);
}

DXWindow::DXWindow()
: mds::IRProgramMemElem()
{
}

DXWindow::~DXWindow()
{
    if (m_initializedMemElem)
    {
        Shutdown();
    }
}
