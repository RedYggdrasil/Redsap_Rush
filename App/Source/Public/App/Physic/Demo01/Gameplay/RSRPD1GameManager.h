#pragma once
#include "App/Gameplay/GameManager.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1PlayerController.h"


namespace RSRush
{
	class RSRPD1GameManager : public GameManager
	{
	public:
		inline RSRPD1PlayerController* GetPD1PlayerController() { return static_cast<RSRPD1PlayerController*>(m_playerControllers[0].get()); }
		inline const RSRPD1PlayerController* GetPD1PlayerController() const { return static_cast<const RSRPD1PlayerController*>(m_playerControllers[0].get()); }

	protected:
		virtual std::shared_ptr<RSRush::PlayerController> MakeSharedPCInstance() override { return std::make_shared<RSRPD1PlayerController>(); }

	public:
		virtual void InitializeGame(std::weak_ptr<RSRScene> InScene) override;
		virtual void ShutdownGame() override;
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

	};
};