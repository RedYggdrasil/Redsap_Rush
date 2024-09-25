#include "App/Tools/Inputs/RSRGameInput.h"
#include "App/Tools/Inputs/RSRUserInput.h"
#include "App/Tools/Window.h"
#include "App/Libs/WinInclude.h"

#include "App/Tools/RSRLog.h"
//#include "GameCore.h"
#include <GameInput.h>
#include "Gen_App/Config/AppConfig.h"
#include "App/Tools/Inputs/RSRGameInput_XInput.h"
#include "App/Game/RSRProgramInstance.h"

#include <string>
#include <Tracy.hpp>

using Microsoft::WRL::ComPtr;
using namespace RSRush;
using namespace RSRush::NSGameInput;


//GameInput on Windows dose not handle Xbox Controller correctly if they are connect vua bluetooth
//https://github.com/microsoft/Xbox-GDK-Samples/issues/26


#define _R_FAILED_LOG_HR(returnValue, expr, log) do {HRESULT hr = expr; if (FAILED(hr)) { log; return returnValue; } } while (false)
#define _R_FAILED_LOG_HRS(returnValue, expr, logType, textType) do {HRESULT hr = expr; if (FAILED(hr)) { RSRLog::Log(logType, textType, hr); return returnValue; } } while (false)


// Callback function
void RSRGameInput::OnDeviceEnumerated(
    _In_ GameInputCallbackToken callbackToken,
    _In_ void* context,
    _In_ IGameInputDevice * device,
    _In_ uint64_t timestamp,
    _In_ GameInputDeviceStatus currentStatus,
    _In_ GameInputDeviceStatus previousStatus
) {
    RSRGameInput* _this = &RSRGameInput::Get();
    // Handle the device here
    // For example, you can get information about the device
    if (device)
    {
        const GameInputDeviceInfo* deviceInfo = device->GetDeviceInfo();
        bool bIsAvalable = currentStatus & GameInputDeviceStatus::GameInputDeviceConnected;
        bool bWasAccountedFor = _this->m_devices.contains(deviceInfo->deviceId);


        if (bIsAvalable)
        {
            if (!bWasAccountedFor)
            {
                _this->m_devices.emplace(deviceInfo->deviceId, RSRUserDeviceInfos{ device, USER_ID_PLAYER_0 });
            }
        }
        else if (bWasAccountedFor)
        {
            _this->m_devices.erase(deviceInfo->deviceId);
        }
    }
}

bool RSRGameInput::Init(IRSRUserInputProvider* InUserProvider)
{
    HRESULT hResult = GameInputCreate(&m_GameInput);
    if (FAILED(hResult))
    {
        RSRLog::Log(LOG_ERROR, TEXT("Failed to Initialize GameInput !"), hResult);
        m_bGameInputAvalable = false;
    }
    else
    {
        m_bGameInputAvalable = true;
    }
    
    if (m_bGameInputAvalable)
    {
        GameInputFocusPolicy gifp;
        gifp = GameInputFocusPolicy::GameInputExclusiveForegroundInput;
        m_GameInput->SetFocusPolicy(gifp);

        //GameInputKind::
        HRESULT _hr = m_GameInput->RegisterDeviceCallback
        (
            nullptr,
            GameInputKindKeyboard | GameInputKindMouse | GameInputKindGamepad /*| GameInputKindTouch*/,
            GameInputDeviceAnyStatus,
            GameInputBlockingEnumeration,
            nullptr,
            RSRGameInput::OnDeviceEnumerated,
            &m_inputCallbackToken
        );
    }

    SetUserProvider(InUserProvider ? InUserProvider : nullptr);
    return true;
}


void RSRGameInput::Shutdown()
{
    m_userProvider = nullptr;

    if (m_GameInput && m_inputCallbackToken)
    {
        m_GameInput->UnregisterCallback(m_inputCallbackToken, 5000Ui64);
    }
    m_GameInput.Reset();
}

void RSRGameInput::Update(const double InDeltaTime)
{
    ZoneScoped;
    if (!m_userProvider)
    {
        return;
    }
    ComPtr<IGameInputReading> KeybordReader = nullptr;
    ComPtr<IGameInputReading> MouseReader = nullptr;
    ComPtr<IGameInputReading> ControllerReader = nullptr;
    
    size_t userCount = m_userProvider->GetUserInputCount();
    if (m_bGameInputAvalable && m_GameInput)
    {
        m_userProvider->NotifyUserInputStart(InDeltaTime);

        for (auto device : m_devices)
        {
            RSRUserDeviceInfos& userDeviceInfo = device.second;
            if 
                (
                    userDeviceInfo.DeviceOverride.IsGameInput() &&
                    userDeviceInfo.UserIndex != USER_ID_NONE &&  
                    userDeviceInfo.UserIndex < userCount &&
                    userDeviceInfo.Device
                )
            {
                RSRUserInput* userInput = m_userProvider->GetUserInput(userDeviceInfo.UserIndex);
                if (userInput)
                {                    
                    ComPtr<IGameInputReading> gameInputReading = nullptr;

                    HRESULT hr = m_GameInput->GetCurrentReading
                    (
                        userDeviceInfo.Device->GetDeviceInfo()->supportedInput,
                        userDeviceInfo.Device.Get(), &gameInputReading
                    );

                    if (SUCCEEDED(hr) && gameInputReading)
                    {
                        this->HandleGameInputReading(gameInputReading.Get(), m_userProvider->GetUserInput(userDeviceInfo.UserIndex), InDeltaTime);
                    }
                    //Release gameInputReading
                    gameInputReading.Reset();
                }
            }
        }

        m_userProvider->NotifyUserInputEnd();
    }
    else
    {
        HandleInputGameInputByWindowEvent(m_userProvider->GetUserInput(0));
    }
}

RSRGameInput::RSRGameInput()
:mds::Singleton<RSRGameInput>()
{

}

std::size_t RSRush::NSGameInput::APP_LOCAL_DEVICE_ID_Hash::operator()(const APP_LOCAL_DEVICE_ID& id) const
{
    // Compute the hash from the bytes in value
    std::size_t hash = 0;
    for (int i = 0; i < sizeof(APP_LOCAL_DEVICE_ID); ++i) {
        // Simple hash function.
        hash = hash * 31 + id.value[i];
    }
    return hash;
}



bool RSRGameInput::HandleGameInputReading(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime)
{
    if (InGameInputReading && InUserInput)
    {
        GameInputKind gik = InGameInputReading->GetInputKind();
        if (gik != GameInputKind::GameInputKindUnknown)
        {
            //certain devices, such as ROG Ally controller, are both KBM and GamePad
            if (gik & GameInputKind::GameInputKindMouse)
            {
                return this->HandleGameInputMouse(InGameInputReading, InUserInput, InDeltaTime);
            }
            if (gik & GameInputKind::GameInputKindKeyboard)
            {
                return this->HandleGameInputKeyboard(InGameInputReading, InUserInput, InDeltaTime);
            }
            if (gik & GameInputKind::GameInputKindGamepad)
            {
                bool bHasInputs = false;
                bool bHadSucess = this->HandleGameInputGamePad(InGameInputReading, InUserInput, InDeltaTime, /*Out*/bHasInputs);
                //Gameinput may be silent, this happend quite often, when controller is bluetooth or ROG Ally for exemple, default on old XInput in this case
                if (!bHasInputs && true)
                {
                    bHadSucess = RSRGameInput_XInput::HandleGameInputGamePad_XInput(InGameInputReading, InUserInput, InDeltaTime, /*Out*/bHasInputs) || bHadSucess;
                    if (bHasInputs)
                    {
                        RSRProgramInstance::Get().ReportSilentGameInputController();
                    }
                }
                return bHadSucess;
            }
        }
    }
    return false;
}


bool RSRush::RSRGameInput::HandleInputGameInputByWindowEvent(RSRUserInput* InUserInput)
{
    InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_WND_NRML)] = DXWindow::Get().GetMouseRelativePosition();

    return true;
}