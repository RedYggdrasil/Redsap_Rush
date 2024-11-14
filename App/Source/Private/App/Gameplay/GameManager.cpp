#include "App/Gameplay/GameManager.h"

#include "App/Gameplay/PlayerController.h"
#include "App/Gameplay/RSRScene.h"
#include "App/Tools/Window.h"
using namespace RSRush;

RSRUserInput* GameManager::GetUserInput(const size_t InIndex) const
{
	return m_playerControllers.size() > InIndex ? m_playerControllers[InIndex].get() : nullptr;
}

size_t GameManager::GetUserInputCount() const
{
	return m_playerControllers.size();
}

std::shared_ptr<RSRush::PlayerController> RSRush::GameManager::AddPlayerController()
{
	m_playerControllers.push_back(MakeSharedPCInstance());
	std::shared_ptr<RSRush::PlayerController> newPC = m_playerControllers.back();
	DXWindow::Get(this)->AddUniqueInputListener(newPC);
	return newPC;
}

void GameManager::InitializeGame(std::weak_ptr<RSRScene> InScene, std::weak_ptr<GameManager> InSelfWPtr)
{
	if (std::shared_ptr<RSRScene> scene = InScene.lock())
	{
		if (std::shared_ptr<GameManager> gameManager = InSelfWPtr.lock())
		{
			this->InitMemNode(scene, gameManager);
		}
	}
}

void GameManager::ShutdownGame()
{
	for (std::shared_ptr<PlayerController> pc : m_playerControllers)
	{
		if (pc)
		{
			DXWindow::Get(this)->RemoveInputListener(pc);
		}
	}
	m_playerControllers.clear();
}

std::shared_ptr<PlayerController> GameManager::GetPlayerController(const size_t InIndex)
{
	return m_playerControllers.size() > InIndex ? m_playerControllers[InIndex] : nullptr;
}

std::shared_ptr<const RSRush::PlayerController> RSRush::GameManager::GetPlayerController(const size_t InIndex) const
{
	return m_playerControllers.size() > InIndex ? m_playerControllers[InIndex] : nullptr;
}
