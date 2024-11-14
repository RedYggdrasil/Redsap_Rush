#include "App/TheTrench/Gameplay/RSRTTGameManager.h"
#include "App/Game/RSRPlayerPath.h"
#include "App/TheTrench/Gameplay/RSRTT404Pawn.h"
#include "App/TheTrench/RSRTTScene.h"
#include "App/Managers/RSRTrenchManager.h"
#include "App/TheTrench/Gameplay/RSRTTPlayerController.h"

using namespace RSRush;

double RSRush::RSRTTGameManager::GetPrePassProgression() const
{
	if (m_playerControllers.size() > 0 && m_playerControllers[0])
	{
		return GetTTPlayerController()->GetPrePassProgression();
	}
	return 0.0;
}

void RSRTTGameManager::InitializeGame(std::weak_ptr<RSRScene> InScene, std::weak_ptr<GameManager> InSelfWPtr)
{
	GameManager::InitializeGame(InScene, InSelfWPtr);

	m_playerControllers.clear();
	AddPlayerController();
#if _DEBUG
	std::shared_ptr<RSRTTScene> tTScene = std::dynamic_pointer_cast<RSRTTScene>(InScene.lock());
	assert((bool)tTScene && "Owning scene is a a RSRTTScene !");
#else
	std::shared_ptr<RSRTTScene> tTScene = std::static_pointer_cast<RSRTTScene>(InScene.lock());
#endif

	m_playerPath = std::make_shared<RSRush::RSRPlayerPath>(GetRoot<RSRProgramInstance>()->GetPhysicManager());
	GetTTPlayerController()->PocessPawn(tTScene->GetPlayerPawn().get());

	//Generate first Trench mesh
	tTScene->GetTrenchManager()->BeginNewTrench(0.0, m_playerPath);
}

void RSRTTGameManager::ShutdownGame()
{
	m_playerPath.reset();
	GameManager::ShutdownGame();
}

bool RSRush::RSRTTGameManager::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = GameManager::PrePassTick(InGameTime, InDeltaTime);
	AllSucessfull = GetTTPlayerController()->PrePassTick(InGameTime, InDeltaTime) && AllSucessfull;
	return AllSucessfull;
}

bool RSRush::RSRTTGameManager::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = GameManager::LateTickSync(InGameTime, InDeltaTime);
	double CurrentProgression = GetTTPlayerController()->GetPrePassProgression();
	m_playerPath.get()->TrimForProgression(CurrentProgression);

	AllSucessfull = GetTTPlayerController()->LateTickSync(InGameTime, InDeltaTime) && AllSucessfull;
	return AllSucessfull;
}
