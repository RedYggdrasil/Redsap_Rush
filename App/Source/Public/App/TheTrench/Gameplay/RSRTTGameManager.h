#pragma once
#include "App/Gameplay/GameManager.h"
#include "App/TheTrench/Gameplay/RSRTTPlayerController.h"


namespace RSRush
{
	class RSRPlayerPath;
	class RSRTTGameManager : public GameManager
	{
	private:
		inline RSRTTPlayerController* GetTTPlayerController() const { return static_cast<RSRTTPlayerController*>(m_playerControllers[0].get()); }
	protected:
		std::shared_ptr<RSRush::RSRPlayerPath> m_playerPath;
	protected:
		virtual std::shared_ptr<RSRush::PlayerController> MakeSharedPCInstance() override { return PlayerController::CreatePlayerController<RSRush::RSRTTPlayerController>(LockSelf<RSRTTGameManager>()); }

	public:
		double GetPrePassProgression() const;
	public:
		virtual void InitializeGame(std::weak_ptr<RSRScene> InScene, std::weak_ptr<GameManager> InSelfWPtr) override;
		virtual void ShutdownGame() override;
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

	};
};