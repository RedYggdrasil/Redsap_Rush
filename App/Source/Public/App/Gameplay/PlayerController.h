#pragma once
#include "App/Interfaces/Listeners/IInputListener.h"
#include "mds/Tools/System/RKeyCodes.h"

#include "Gen_App/Config/AppConfig.h"
#include "App/Data/RSRCameraData.h"

#include "App/Gameplay/Pawn.h"
#include "App/Tools/Inputs/RSRUserInput.h"
#include "MDS/Tools/System/RInputKeys.h"
namespace RSRush
{
	class PlayerController : public RSRUserInput, public IInputListener
	{
	public:
		enum class EMainInputDevice : uint8_t
		{
			MouseKeyboard = 0,
			GamePad = 1,
			RENUM_COUNT
		};
	protected:
		Pawn* m_pawn = nullptr;
		EMainInputDevice m_mainInputDevice = USE_START_INPUT;

	public:
		inline const RSRush::RSRCameraData* GetCameraData() const { return m_pawn ? m_pawn->GetCameraData() : nullptr; };
		inline RSRush::RSRCameraData* GetCameraData() 
		{ return m_pawn ? m_pawn->GetCameraData() : nullptr; };

		PlayerController();
		virtual ~PlayerController();

		/// <summary>
		/// Not executed on main tread, Compute new input and request new position
		/// new application state is to be validated by Physic Pass
		/// </summary>
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) { return true; };
		/// <summary>
		/// Read the result of PrepassTick and Physic Pass, and apply them to object before rendering
		/// </summary>
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) { return true; };


	public:
		//Interfaces IInputListener
		virtual bool ReceiveKey(bool bIsDown, WPARAM InKey) override;
		//End Interfaces IInputListener

		virtual void PocessPawn(Pawn* InPawn);
		virtual void UnpocessPawn();
	};
}