#pragma once
#include "App/Interfaces/Listeners/IInputListener.h"
#include "MDS/Tools/System/IRProgramMemNode.h"
#include "mds/Tools/System/RKeyCodes.h"

#include "Gen_App/Config/AppConfig.h"
#include "App/Data/RSRCameraData.h"

#include "App/Gameplay/Pawn.h"
#include "App/Tools/Inputs/RSRUserInput.h"
#include "MDS/Tools/System/RInputKeys.h"
namespace RSRush
{
	class GameManager;
	class PlayerController : public mds::IRProgramMemNode, public RSRUserInput, public IInputListener
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

		template <class TGameManager = GameManager> 
		std::shared_ptr<TGameManager> LockGameManager()
		{
			if (std::shared_ptr<TGameManager> gm = std::static_pointer_cast<TGameManager>(m_WPtrParentNode.lock()))
			{
				return gm;
			}
			return nullptr;
		}

	public:
		inline const RSRush::RSRCameraData* GetCameraData() const { return m_pawn ? m_pawn->GetCameraData() : nullptr; };
		inline RSRush::RSRCameraData* GetCameraData() 
		{ return m_pawn ? m_pawn->GetCameraData() : nullptr; };


		template<class TPlayerController = PlayerController, typename... Args>
		static std::shared_ptr<TPlayerController> CreatePlayerController(std::weak_ptr<GameManager> InOwningManager, Args&&... args)
		{
			if (std::shared_ptr<GameManager> gm = InOwningManager.lock())
			{
				std::shared_ptr<TPlayerController> pc = std::make_shared<TPlayerController>(std::forward<Args>(args)...);
				pc->InitMemNode(gm, pc);
				return pc;
			}
			else
			{
				return nullptr;
			}
		}

	public:
		PlayerController();
	public:
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