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

uintptr_t RSRGameInput::ms_nextInputcallbackLookupKey = 0;
std::unordered_map<GameInputCallbackToken, RSRGameInput*> RSRGameInput::sm_dxGameInputLookup;

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
    if (device)
    {
        auto it = sm_dxGameInputLookup.find((uintptr_t)context);

        if (it != sm_dxGameInputLookup.end())
        {
            RSRGameInput* thisPtr = it->second;
            const GameInputDeviceInfo* deviceInfo = device->GetDeviceInfo();
            bool bIsAvalable = currentStatus & GameInputDeviceStatus::GameInputDeviceConnected;
            bool bWasAccountedFor = thisPtr->m_devices.contains(deviceInfo->deviceId);


            if (bIsAvalable)
            {
                if (!bWasAccountedFor)
                {
                    thisPtr->m_devices.emplace(deviceInfo->deviceId, RSRUserDeviceInfos{ device, USER_ID_PLAYER_0 });
                }
            }
            else if (bWasAccountedFor)
            {
                thisPtr->m_devices.erase(deviceInfo->deviceId);
            }
        }
    }
}

RSRush::RSRGameInput::RSRGameInput()
: mds::IRProgramMemElem()
{

}

bool RSRGameInput::Init(RSRProgramInstance* InProgramInstance, IRSRUserInputProvider* InUserProvider)
{
    this->InitMemElem(InProgramInstance);
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


        m_inputcallbackLookupKey = ms_nextInputcallbackLookupKey;
        ++ms_nextInputcallbackLookupKey;
        sm_dxGameInputLookup.emplace(m_inputcallbackLookupKey, this);

        HRESULT _hr = m_GameInput->RegisterDeviceCallback
        (
            nullptr,
            GameInputKindKeyboard | GameInputKindMouse | GameInputKindGamepad /*| GameInputKindTouch*/,
            GameInputDeviceAnyStatus,
            GameInputBlockingEnumeration,
            (void*)m_inputcallbackLookupKey,
            RSRGameInput::OnDeviceEnumerated,
            &m_inputCallbackToken
        );
        if (FAILED(_hr))
        {
            RSRLog::Log(LOG_ERROR, TEXT("Failed to Initialize GameInput !"), _hr);
            return false;
        }
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
    sm_dxGameInputLookup.erase(m_inputcallbackLookupKey);
    m_GameInput.Reset();
    this->ResetMemTreeData();
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
                        GetRoot<RSRProgramInstance>()->ReportSilentGameInputController();
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
    InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_WND_NRML)] = DXWindow::Get(this)->GetMouseRelativePosition();

    return true;
}