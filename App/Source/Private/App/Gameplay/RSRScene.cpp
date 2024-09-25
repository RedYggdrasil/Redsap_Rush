#include "App/Gameplay/RSRScene.h"

#include "App/Tools/RSRLog.h"

RSRush::RSRScene::RSRScene()
	: m_thisWPtr(), m_programInstance(nullptr), m_bIsLoaded(false), m_gameManager(nullptr)
{
}

bool RSRush::RSRScene::Load()
{

	m_bIsLoaded = true; 
	return true;
}

bool RSRush::RSRScene::UnLoad()
{
	m_bIsLoaded = false;
	return true;
}

RSRush::RSRScene::~RSRScene()
{
	if (m_bIsLoaded)
	{
		RSRLog::Log(LOG_EXCEPTION, TEXT("Didn't unloaded scene before deletion !"));
	}
	m_programInstance = nullptr;
	m_thisWPtr.reset();
}
