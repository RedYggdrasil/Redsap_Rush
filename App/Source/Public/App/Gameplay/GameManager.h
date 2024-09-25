#pragma once

#include "MDS/RTool.h"
#include "App/Tools/Inputs/RSRUserInput.h"
#include "App/Gameplay/PlayerController.h"

#include <vector>
#include <memory>

namespace RSRush
{
	class RSRScene;
	class GameManager : public IRSRUserInputProvider
	{
	protected:
		//Know
		std::weak_ptr<RSRScene> m_owningScene;
	protected:
		//Owner
		std::vector<std::shared_ptr<RSRush::PlayerController>> m_playerControllers;
		virtual std::shared_ptr<RSRush::PlayerController> MakeSharedPCInstance() { return std::make_shared<RSRush::PlayerController>(); }
		virtual std::shared_ptr<RSRush::PlayerController> AddPlayerController();
	public:
		virtual void InitializeGame(std::weak_ptr<RSRScene> InScene);
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