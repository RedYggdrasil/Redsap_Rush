#include "App/Physic/Demo01/Gameplay/RSRPD1GameManager.h"
#include "App/Physic/Demo01/RSRPD1Scene.h"

using namespace RSRush;

void RSRush::RSRPD1GameManager::InitializeGame(std::weak_ptr<RSRScene> InScene)
{
	GameManager::InitializeGame(InScene);
	m_playerControllers.clear();
	AddPlayerController();
#if _DEBUG
	std::shared_ptr<RSRPD1Scene> pD1Scene = std::dynamic_pointer_cast<RSRPD1Scene>(m_owningScene.lock());
	assert((bool)pD1Scene && "Owning scene is not a RSRPD1Scene !");
#else
	std::shared_ptr<RSRPD1Scene> pD1Scene = std::static_pointer_cast<RSRPD1Scene>(m_owningScene.lock());
#endif
	GetPD1PlayerController()->Initialize();
}

void RSRush::RSRPD1GameManager::ShutdownGame()
{
	if (m_playerControllers.size() > 0)
	{
		GetPD1PlayerController()->Shutdown();
	}
	GameManager::ShutdownGame();
}

bool RSRush::RSRPD1GameManager::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = GameManager::PrePassTick(InGameTime, InDeltaTime);
	if (m_playerControllers.size() > 0)
	{
		AllSucessfull = GetPD1PlayerController()->PrePassTick(InGameTime, InDeltaTime) && AllSucessfull;
	}
	return AllSucessfull;
}

bool RSRush::RSRPD1GameManager::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = GameManager::LateTickSync(InGameTime, InDeltaTime);
	if (m_playerControllers.size() > 0)
	{
		AllSucessfull = GetPD1PlayerController()->LateTickSync(InGameTime, InDeltaTime) && AllSucessfull;
	}
	return AllSucessfull;
}
