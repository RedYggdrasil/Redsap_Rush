#include "App/Tools/Inputs/RSRGameInput_XInput.h"
#include "MDS/Tools/System/RInputKeys.h"
#include "App/Tools/Inputs/RSRUserInput.h"

///Code taken from DirectX_GraphicSamples\MiniEngine\input.cpp

#define ALLOW_XINPUT true

#if !ALLOW_XINPUT
bool RSRush::RSRGameInput_XInput::HandleGameInputGamePad_XInput(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime, bool& bOutHasInput)
{
    bOutHasInput = false;
    return false;
};
#else
#include "App/Tools/RSRLog.h"

///XInput version and libraires from DirectX_GraphicSamples\MiniEngine\input.cpp

#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")

//#define USE_KEYBOARD_MOUSE

#ifdef DIRECTINPUT_VERSION
#define DX_ALREADY_DEFINED  true
#else
#define DX_ALREADY_DEFINED  false
#define DIRECTINPUT_VERSION 0x0800
#endif // !DIRECTINPUT_VERSION

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

bool RSRush::RSRGameInput_XInput::HandleGameInputGamePad_XInput(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime, bool& bOutHasInput)
{
    bOutHasInput = false;
    
    XINPUT_STATE newInputState;
    if (ERROR_SUCCESS == XInputGetState(0, &newInputState))
    {
        bOutHasInput = bOutHasInput || ((UINT16)newInputState.Gamepad.wButtons != 0);

        if (newInputState.Gamepad.wButtons != 0)
        {
            RSRLog::Log(LOG_DISPLAY, TEXT("wButtons : {}"), (UINT16)newInputState.Gamepad.wButtons);
        }
#define SET_BUTTON_VALUE(InputEnum, GameInputMask) \
    InUserInput->m_keyState[0][mds::UT_cast(InputEnum)] = (bool)(newInputState.Gamepad.wButtons & GameInputMask);

        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_UP, XINPUT_GAMEPAD_DPAD_UP);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_DOWN);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_LEFT);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_MENU, XINPUT_GAMEPAD_START);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_VIEW, XINPUT_GAMEPAD_BACK);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_THUMBSTICK, XINPUT_GAMEPAD_LEFT_THUMB);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK, XINPUT_GAMEPAD_RIGHT_THUMB);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD, XINPUT_GAMEPAD_LEFT_SHOULDER);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD, XINPUT_GAMEPAD_RIGHT_SHOULDER);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_A, XINPUT_GAMEPAD_A);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_B, XINPUT_GAMEPAD_B);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_X, XINPUT_GAMEPAD_X);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_Y, XINPUT_GAMEPAD_Y);

#undef SET_BUTTON_VALUE

        InUserInput->m_axis1D[0][mds::UT_cast(EMDSAxis1DType::MDS_A1D_GMPD_LEFT_TRIGGER)]   = newInputState.Gamepad.bLeftTrigger / 255.0f;
        InUserInput->m_axis1D[0][mds::UT_cast(EMDSAxis1DType::MDS_A1D_GMPD_RIGHT_TRIGGER)]  = newInputState.Gamepad.bRightTrigger / 255.0f;

        bOutHasInput = bOutHasInput || (newInputState.Gamepad.bLeftTrigger > 0);
        bOutHasInput = bOutHasInput || (newInputState.Gamepad.bRightTrigger > 0);


        auto _analogFiler = [](const int InValue, const int InDeadZone, bool& bOutHasInput) -> float
            {
                bOutHasInput = bOutHasInput || (abs(InValue) > InDeadZone);
                if (InValue < 0)
                {
                    if (InValue > -InDeadZone)
                        return 0.0f;
                    else
                        return (InValue + InDeadZone) / (32768.0f - InDeadZone);
                }
                else
                {
                    if (InValue < InDeadZone)
                        return 0.0f;
                    else
                        return (InValue - InDeadZone) / (32767.0f - InDeadZone);
                }
                return 0.f;
            };
        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK)] =
        { _analogFiler(newInputState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, bOutHasInput) , _analogFiler(newInputState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, bOutHasInput) };

        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_GMPD_RIGHT_STICK)] =
        { _analogFiler(newInputState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, bOutHasInput) , _analogFiler(newInputState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, bOutHasInput) };
        
        return true;
    }
    return false;
}


#if !DX_ALREADY_DEFINED
#undef DIRECTINPUT_VERSION
#endif
#undef DX_ALREADY_DEFINED

#endif //ALLOW_XINPUT


//XInput Gamepads kind :

//GameInputKindUnknown = 0x00000000,
//GameInputKindRawDeviceReport = 0x00000001,
//GameInputKindControllerAxis = 0x00000002,
//GameInputKindControllerButton = 0x00000004,
//GameInputKindControllerSwitch = 0x00000008,
//GameInputKindController = 0x0000000E,
//GameInputKindKeyboard = 0x00000010,
//GameInputKindMouse = 0x00000020,
//GameInputKindTouch = 0x00000100,
//GameInputKindMotion = 0x00001000,
//GameInputKindArcadeStick = 0x00010000,
//GameInputKindFlightStick = 0x00020000,
//GameInputKindGamepad = 0x00040000,
//GameInputKindRacingWheel = 0x00080000,
//GameInputKindUiNavigation = 0x01000000
//Connected from cable
//17039367

//Connected from Bluetooth
//17039366