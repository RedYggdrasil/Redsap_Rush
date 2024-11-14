#include "App/Gameplay/RSRScene.h"

#include "App/D3D/DXContext.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;

RSRScene::RSRScene()
: m_bIsLoaded(false), m_gameManager(nullptr), m_instancedMeshes(this)
{
}

bool RSRScene::Load()
{

	m_bIsLoaded = true; 
	return true;
}

bool RSRScene::UnLoad()
{
	m_bIsLoaded = false;
	return true;
}

bool RSRScene::UpdateMeshInstanceBuffers(const double InGameTime, const double InDeltaTime, ID3D12GraphicsCommandList7* InFrameUploadCommandList)
{
	m_instancedMeshes.UpdateUploadBuffer();
	m_instancedMeshes.UploadResources(InFrameUploadCommandList);
	return true;
}

bool RSRScene::ClearUnusedMeshInstanceBuffers()
{
	m_instancedMeshes.ClearUnusedBuffers();
	return true;
}

void RSRScene::RegisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
	m_instancedMeshes.Register(InMeshHolder, InMesh);
}

void RSRScene::UnregisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
	m_instancedMeshes.Unregister(InMeshHolder, InMesh);
}

RSRScene::~RSRScene()
{
	if (m_bIsLoaded)
	{
		RSRLog::Log(LOG_EXCEPTION, TEXT("Didn't unloaded scene before deletion !"));
	}
}