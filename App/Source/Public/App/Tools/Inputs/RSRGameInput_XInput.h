#pragma once
#include "MDS/Defines/MDS.h"
#include <GameInput.h>
namespace RSRush
{
	class RSRUserInput;
	class RSRGameInput_XInput
	{
	public:
		STATIC_STRUCT_R5(RSRGameInput_XInput);
	public:
		static bool HandleGameInputGamePad_XInput(IGameInputReading* InGameInputReading, RSRUserInput* InUserInput, const double InDeltaTime, bool& bOutHasInput);
	};
};