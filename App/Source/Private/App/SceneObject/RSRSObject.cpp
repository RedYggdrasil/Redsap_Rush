#include "App/SceneObject/RSRSObject.h"
#include "App/SceneObject/RSROScene.h"
#include <App/Tools/RSRLog.h>

RSRush::RSRSObject::RSRSObject(const mds::RAssetAuthority InMainMeshAuthority)
	: RSRSObject(InMainMeshAuthority, false, false)
{
}

RSRush::RSRSObject::RSRSObject(const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject)
	:RSRSObject(InMainMeshAuthority, bInIsHandledAsSObject, false)
{
}

RSRush::RSRSObject::RSRSObject(const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject, const bool bInIsDrawnAsInstance)
	:m_bIsHandledAsSObject(bInIsHandledAsSObject), m_bIsDrawnAsInstance(bInIsDrawnAsInstance), m_mainTransform(RSRush::RSRTransformMatrix(mds::TRS_IDENTITY)), m_mainMeshAuthority(InMainMeshAuthority), m_mainMesh(nullptr)
{
	assert(m_mainMeshAuthority != mds::RAssetAuthority::Shared && "AssetAuthority Shared need to be designed with asset manager and temp stack shared_ptr in mind !, unhandled for now !");
}


RSRush::RSRSObject::~RSRSObject()
{
	if (m_mainMesh)
	{
		m_mainMesh->FreeAllBuffers();
	}
	m_mainMesh.reset();
}


bool RSRush::RSRSObject::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	if (mds::HasAssetAssetAuthority(m_mainMeshAuthority))
	{
		if (m_mainMesh)
		{
			return m_mainMesh->UploadResources(InDevice, InUploadCommandList);
		}
	}
	return true;
}

bool RSRush::RSRSObject::FreeUploadBuffers()
{
	if (mds::HasAssetAssetAuthority(m_mainMeshAuthority))
	{
		if (m_mainMesh)
		{
			return m_mainMesh->FreeUploadBuffer();
		}
	}
	return true;
}

bool RSRush::RSRSObject::FreeResourceBuffers()
{
	if (mds::HasAssetAssetAuthority(m_mainMeshAuthority))
	{
		if (m_mainMesh && m_mainMeshAuthority == mds::RAssetAuthority::Absolute)
		{
			return m_mainMesh->FreeResourcesBuffer();
		}
	}
	return true;
}

bool RSRush::RSRSObject::DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const
{
	if (m_mainMesh)
	{
		return m_mainMesh->DrawMesh(InDraw3DCommandList, m_mainTransform);
	}
	return true;
}

void RSRush::RSRSObject::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
}

void RSRush::RSRSObject::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
}

mds::RResourceStateType RSRush::RSRSObject::GetResourceState() const
{
	if (m_mainMesh)
	{
		return m_mainMesh->GetResourceState();
	}
	return mds::RResourceStateType::Unknown;
}
