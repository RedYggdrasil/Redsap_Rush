#include "App/Gameplay/RSRScene.h"

#include "App/D3D/DXContext.h"
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

bool RSRush::RSRScene::UpdateMeshInstanceBuffers(const double InGameTime, const double InDeltaTime, ID3D12GraphicsCommandList7* InFrameUploadCommandList)
{
	m_instancedMeshes.UpdateUploadBuffer();
	m_instancedMeshes.UploadResources(InFrameUploadCommandList);
	return true;
}

bool RSRush::RSRScene::ClearUnusedMeshInstanceBuffers()
{
	m_instancedMeshes.ClearUnusedBuffers();
	return true;
}

void RSRush::RSRScene::RegisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
	m_instancedMeshes.Register(InMeshHolder, InMesh);
}

void RSRush::RSRScene::UnregisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
	m_instancedMeshes.Unregister(InMeshHolder, InMesh);
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
