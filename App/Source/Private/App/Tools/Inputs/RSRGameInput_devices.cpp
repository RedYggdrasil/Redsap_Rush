#include "App/Tools/Inputs/RSRGameInput.h"

#include "MDS/Tools/RMath.h"
#include "MDS/Tools/System/RInputKeys.h"

#include "App/Tools/Inputs/RSRUserInput.h"
#include "App/Tools/Window.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;


bool RSRGameInput::HandleGameInputMouse(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime)
{
    GameInputMouseState mouseState;
    bool bGetSucess = InGameInputReading->GetMouseState(&mouseState);

    if (bGetSucess)
    {
#define SET_MOUSE_VALUE(InputEnum, GameInputMask) \
    InUserInput->m_keyState[0][mds::UT_cast(InputEnum)] |= (bool)(mouseState.buttons & GameInputMask);

        SET_MOUSE_VALUE(EMDSInputKeyType::MDS_IK_LBUTTON, GameInputMouseButtons::GameInputMouseLeftButton);
        SET_MOUSE_VALUE(EMDSInputKeyType::MDS_IK_RBUTTON, GameInputMouseButtons::GameInputMouseRightButton);
        SET_MOUSE_VALUE(EMDSInputKeyType::MDS_IK_MBUTTON, GameInputMouseButtons::GameInputMouseMiddleButton);
        SET_MOUSE_VALUE(EMDSInputKeyType::MDS_IK_XBUTTON1, GameInputMouseButtons::GameInputMouseButton4);
        SET_MOUSE_VALUE(EMDSInputKeyType::MDS_IK_XBUTTON2, GameInputMouseButtons::GameInputMouseButton5);
#undef SET_MOUSE_VALUE

        //To Check
        InUserInput->m_axis1D[0][mds::UT_cast(EMDSAxis1DType::MDS_A1D_MS_SCROLL)] = (float)mouseState.wheelX;

        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_ACC)] = { (float)mouseState.positionX, (float)mouseState.positionY };

        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_FRAME)] = mds::unoptimized_substract2(InUserInput->m_axis2D[1][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_ACC)], InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_ACC)]);
    }

    InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_MS_WND_NRML)] = DXWindow::Get().GetMouseRelativePosition();
    return bGetSucess;
};

bool RSRGameInput::HandleGameInputKeyboard(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime)
{
    uint32_t keyCount = InGameInputReading->GetKeyCount();
    //RSRLog::Log(LOG_DISPLAY, TEXT("Keyboard Keycount : {}"), keyCount);
    if (keyCount > 0)
    {
        //Dynamic instanciation there, probably need to store a pre allocated buffer somwhere and use that instead
        GameInputKeyState* keyStates = new GameInputKeyState[keyCount];
        uint32_t validKeyCount = InGameInputReading->GetKeyState(keyCount, keyStates);

        for (size_t i = 0; i < validKeyCount; ++i)
        {
            GameInputKeyState* key = &keyStates[i];
            EMDSInputKeyType keyType = ToMDSInputKeyType(key->codePoint);
            if (IsKeyBoardKey(keyType))
            {
                InUserInput->m_keyState[0][mds::UT_cast(keyType)] = true;
            }
        }
        delete[] keyStates;
    }
    return true;
};

bool RSRGameInput::HandleGameInputGamePad(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime, bool& bOutHasInput)
{
    GameInputGamepadState newInputState;
    bOutHasInput = false;
    bool bGetSucess =  InGameInputReading->GetGamepadState(&newInputState);
    if (bGetSucess)
    {
        bOutHasInput = bOutHasInput || (newInputState.buttons != GameInputGamepadButtons::GameInputGamepadNone);

#define SET_BUTTON_VALUE(InputEnum, GameInputMask) \
    InUserInput->m_keyState[0][mds::UT_cast(InputEnum)] |= (bool)(newInputState.buttons & GameInputMask);

        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_MENU, GameInputGamepadButtons::GameInputGamepadMenu);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_VIEW, GameInputGamepadButtons::GameInputGamepadView);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_A, GameInputGamepadButtons::GameInputGamepadA);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_B, GameInputGamepadButtons::GameInputGamepadB);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_X, GameInputGamepadButtons::GameInputGamepadX);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_Y, GameInputGamepadButtons::GameInputGamepadY);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_UP, GameInputGamepadButtons::GameInputGamepadDPadUp);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_DOWN, GameInputGamepadButtons::GameInputGamepadDPadDown);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT, GameInputGamepadButtons::GameInputGamepadDPadLeft);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT, GameInputGamepadButtons::GameInputGamepadDPadRight);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD, GameInputGamepadButtons::GameInputGamepadLeftShoulder);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD, GameInputGamepadButtons::GameInputGamepadRightShoulder);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_THUMBSTICK, GameInputGamepadButtons::GameInputGamepadLeftThumbstick);
        SET_BUTTON_VALUE(EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_THUMBSTICK, GameInputGamepadButtons::GameInputGamepadRightThumbstick);
#undef SET_BUTTON_VALUE


        InUserInput->m_axis1D[0][mds::UT_cast(EMDSAxis1DType::MDS_A1D_GMPD_LEFT_TRIGGER)] = newInputState.leftTrigger;
        InUserInput->m_axis1D[0][mds::UT_cast(EMDSAxis1DType::MDS_A1D_GMPD_RIGHT_TRIGGER)] = newInputState.rightTrigger;
        
        bOutHasInput = bOutHasInput || (abs(newInputState.leftTrigger) > FLT_EPSILON);
        bOutHasInput = bOutHasInput || (abs(newInputState.rightTrigger) > FLT_EPSILON);


        static constexpr float xInputGMPDLeftThumDeadZone = (7849.0f / 32768.0f);
        static constexpr float xInputGMPDRightThumDeadZone = (8689.0f / 32768.0f);
        auto _analogFiler = [](const float InValue, const float InDeadZone, bool& bOutHasInput) -> float
            {
                bOutHasInput = bOutHasInput || (abs(InValue) > InDeadZone);
                if (InValue < 0)
                {
                    if (InValue > -InDeadZone)
                        return 0.0f;
                    else
                        return (InValue + InDeadZone) / (1.f - InDeadZone);
                }
                else
                {
                    if (InValue < InDeadZone)
                        return 0.0f;
                    else
                        return (InValue - InDeadZone) / (1.f - InDeadZone);
                }
                return 0.f;
            };

        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK)] =
        { _analogFiler(newInputState.leftThumbstickX, xInputGMPDLeftThumDeadZone, bOutHasInput) , _analogFiler(newInputState.leftThumbstickY, xInputGMPDLeftThumDeadZone, bOutHasInput) };

        InUserInput->m_axis2D[0][mds::UT_cast(EMDSAxis2DType::MDS_A2D_GMPD_RIGHT_STICK)] =
        { _analogFiler(newInputState.rightThumbstickX, xInputGMPDRightThumDeadZone, bOutHasInput) , _analogFiler(newInputState.rightThumbstickY, xInputGMPDRightThumDeadZone, bOutHasInput) };

    }
    return bGetSucess;
};
