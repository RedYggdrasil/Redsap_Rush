#pragma once

#include "MDS/RTool.h"
#include "MDS/Tools/System/IRProgramMemNode.h"
#include "App/Tools/Inputs/RSRUserInput.h"
#include "App/Gameplay/PlayerController.h"

#include <vector>
#include <memory>

namespace RSRush
{
	class RSRScene;
	class GameManager : public mds::IRProgramMemNode, public IRSRUserInputProvider
	{
	protected:
		//Owner
		std::vector<std::shared_ptr<RSRush::PlayerController>> m_playerControllers;
		virtual std::shared_ptr<RSRush::PlayerController> MakeSharedPCInstance() { return PlayerController::CreatePlayerController<RSRush::PlayerController>(LockSelf<GameManager>()); }
		virtual std::shared_ptr<RSRush::PlayerController> AddPlayerController();
	public:

		template <class TRSRScene = RSRScene>
		std::shared_ptr<TRSRScene> LockScene()
		{
			if (std::shared_ptr<TRSRScene> scene = std::static_pointer_cast<TRSRScene>(m_WPtrParentNode.lock()))
			{
				return scene;
			}
			return nullptr;
		}

	public:
		virtual void InitializeGame(std::weak_ptr<RSRScene> InScene, std::weak_ptr<GameManager> InSelfWPtr);
		virtual void ShutdownGame();
		/// <summary>
		/// Not executed on main tread, Compute new input and request new position
		/// new application state is to be validated by Physic Pass
		/// </summary>
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) { return true; };
		/// <summary>
		/// Read the result of PrepassTick and Physic Pass, and apply them to object before rendering
		/// </summary>
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) { return true; };

		std::shared_ptr<RSRush::PlayerController> GetPlayerController(const size_t InIndex);
		std::shared_ptr<const RSRush::PlayerController> GetPlayerController(const size_t InIndex) const;
	public:
		//Implemented via IRSRUserInputProvider
		virtual RSRUserInput* GetUserInput(const size_t InIndex) const R_PURE_OVERRIDE;
		virtual size_t GetUserInputCount() const R_PURE_OVERRIDE;
		//End implemented via IRSRUserInputProvider
	};
};