#include "App/Tools/Inputs/RSRUserInput.h"

void RSRush::IRSRUserInputProvider::NotifyUserInputStart(const double InDeltaTime) const
{
	size_t count = this->GetUserInputCount();
	for (size_t i = 0; i < count; ++i)
	{
		RSRUserInput* ui = this->GetUserInput(i);
		if (ui) { ui->OnUserInputStart(InDeltaTime); }
	}
}

void RSRush::IRSRUserInputProvider::NotifyUserInputEnd() const
{
	size_t count = this->GetUserInputCount();
	for (size_t i = 0; i < count; ++i)
	{
		RSRUserInput* ui = this->GetUserInput(i);
		if (ui) { ui->OnUserInputEnd(); }
	}
}

void RSRush::RSRUserInput::OnUserInputStart(const double InDeltaTime)
{

    memcpy(m_keyState[1], m_keyState[0], sizeof(m_keyState[0]));
    memset(m_keyState[0], 0, sizeof(m_keyState[0]));

    memcpy(m_axis1D[1], m_axis1D[0], sizeof(m_axis1D[0]));
    memset(m_axis1D[0], 0, sizeof(m_axis1D[0]));

    memcpy(m_axis2D[1], m_axis2D[0], sizeof(m_axis2D[0]));
    memset(m_axis2D[0], 0, sizeof(m_axis2D[0]));
}

void RSRush::RSRUserInput::OnUserInputEnd()
{
}

//void RSRush::RSRUserInput::HandleGameInputReading(_In_ IGameInputReading* gameInputReading, const double InDeltaTime)
//{
/*
#pragma region WIP

	//RSRUserInput* userInput = prov->GetUserInput(device.second.UserIndex);
	//
	//auto iter = prov->begin() + device.second.UserIndex;
	GameInputKind kind = userDeviceInfo.Device->GetDeviceInfo()->supportedInput;
	IGameInputReading* gameInputReading = nullptr;
	HRESULT hr = m_GameInput->GetCurrentReading(kind, userDeviceInfo.Device.Get(), &gameInputReading);

	if (SUCCEEDED(hr) && gameInputReading)
	{
		prov->GetUserInput(userDeviceInfo.UserIndex)->
	}
	std::vector<int>::iterator


        HRESULT hr = m_GameInput->GetCurrentReading(GameInputKindKeyboard, nullptr, &KeybordReader);
    hr = m_GameInput->GetCurrentReading(GameInputKindMouse, nullptr, &MouseReader);
    hr = m_GameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &ControllerReader);

    static int updateGPState = 100;
    updateGPState -= 1;
    if (updateGPState < 1)
    {
        updateGPState = 100;
        RSRLog::Log(LOG_DISPLAY, hr);
        RSRLog::Log(LOG_DISPLAY, TEXT("Controller State '{}' "), (bool)ControllerReader.Get());
    }

    if (KeybordReader)
    {
        uint32_t keyCount = KeybordReader->GetKeyCount();
        GameInputKeyState* keyStates = new GameInputKeyState[keyCount];
        uint32_t validKeyCount = KeybordReader->GetKeyState(keyCount, keyStates);

        for (size_t i = 0; i < validKeyCount; ++i)
        {
            GameInputKeyState* key = &keyStates[i];

            RSRLog::Log(LOG_DISPLAY, TEXT("Key {}"), key->codePoint);
        }
        delete[] keyStates;
    }

    //bool s_Buttons[2][GameInput::kNumDigitalInputs];

    if (MouseReader)
    {
        GameInputMouseState mouseState;
        bool bGetSucess = MouseReader->GetMouseState(&mouseState);

        bool LeftClick = mouseState.buttons & GameInputMouseButtons::GameInputMouseLeftButton;
        bool RightClick = mouseState.buttons & GameInputMouseButtons::GameInputMouseRightButton;
        bool MiddleClick = mouseState.buttons & GameInputMouseButtons::GameInputMouseMiddleButton;
        bool Button4 = mouseState.buttons & GameInputMouseButtons::GameInputMouseButton4;
        bool Button5 = mouseState.buttons & GameInputMouseButtons::GameInputMouseButton5;
        bool tiltLeft = mouseState.buttons & GameInputMouseButtons::GameInputMouseWheelTiltLeft;
        bool tiltRight = mouseState.buttons & GameInputMouseButtons::GameInputMouseWheelTiltRight;
        //RSRLog::Log(LOG_DISPLAY, TEXT("Mouse Get '{}', Mask : {}, LB {}, RB {}, MB {}, B4 {}, B5 {}, TL {}, TR {}. Px {}, Py {}.  Wx {}, Wy {}"), bGetSucess,
        //    (uint32_t)mouseState.buttons,
        //    LeftClick, RightClick, MiddleClick, Button4, Button5, tiltLeft, tiltRight,
        //    mouseState.positionX, mouseState.positionY,
        //    mouseState.wheelX, mouseState.wheelY
        //);
    }
    if (ControllerReader)
    {
        GameInputGamepadState gamepadState;
        GameInputKind kind = ControllerReader->GetInputKind();
        bool bGetSucess = ControllerReader->GetGamepadState(&gamepadState);
        bool aButton = gamepadState.buttons & GameInputGamepadA;
        bool bButton = gamepadState.buttons & GameInputGamepadB;
        bool xButton = gamepadState.buttons & GameInputGamepadX;
        bool yButton = gamepadState.buttons & GameInputGamepadY;

        uint32_t contCount = ControllerReader->GetControllerButtonCount();
        //ControllerReader->GetDevice()
        //RSRLog::Log(LOG_DISPLAY, TEXT("Key Get '{}', CCount : {}, Mask : {}, A {}, B {}, X {}, Y {}. LTx {}, LTy {}"), bGetSucess,
        //    contCount,
        //    (uint32_t)gamepadState.buttons,
        //    aButton, bButton, xButton, yButton,
        //    gamepadState.leftThumbstickX, gamepadState.leftThumbstickY
        //);

        bool* buttonStates = new bool[contCount];
        uint32_t validButtonCount = ControllerReader->GetControllerButtonState(contCount, buttonStates);

        for (size_t i = 0; i < validButtonCount; ++i)
        {
            bool* button = &buttonStates[i];

            //RSRLog::Log(LOG_DISPLAY, TEXT("Key [{}] : {}"), i, *button);
        }
        delete[] buttonStates;
        //RSRLog::Log(LOG_DISPLAY, TEXT("Mask : '{}'"), (uint32_t) gamepadState.buttons);
        //gamepadState.
        //gamepadState.buttons.
        //gamepadState.
        //uint32_t buttonCount = ControllerReader->GetControllerButtonCount();
        //GameInputKeyState* buttonStates = new GameInputKeyState[buttonCount];
        //ControllerReader->GetButton(keyCount, keyStates);
        //
        //for (size_t i = 0; i < keyCount; ++i)
        //{
        //    GameInputKeyState* key = &keyStates[i];
        //
        //    RSRLog::Log(LOG_DISPLAY, TEXT("Key {}"), key->codePoint);
        //}
        //delete[] keyStates;
    }
    //RSRLog::Log(LOG_DISPLAY, TEXT("Inputs : KB : '{}', MOUSE : '{}', GP : '{}'"), (bool)KeybordReader, (bool)MouseReader, (bool)ControllerReader);

    //if (FAILED(hr))
    //{
    //    RSRLog::Log(LOG_ERROR, hr);
    //}
    //else
    //{
    //    RSRLog::Log(LOG_DISPLAY, TEXT("YAY !"));
    //}
    //_R_FAILED_LOG_HRS(,m_GameInput->GetCurrentReading(GameInputKindUnknown, nullptr, &pGIR), LOG_ERROR, TEXT("Failed to IGameInputReading !"));

#pragma endregion WIP
*/
//}
