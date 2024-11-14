#pragma once
#include "App/SceneObject/RSRSObject.h"
#include "App/Data/Shaders/ShaderStructures.h"
#include "App/Gameplay/Scene/RSRIInstancedMesheHolder.h"

namespace RSRush
{
	class RSRSObjectInstance : public RSRSObject, public RSRIInstancedMesheHolder
	{
	protected:
		InstanceDatas m_instanceData;
	public:
		//const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject, const bool bInIsDrawnAsInstance
		RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, const RSRTransform& InTransform, InstanceDatas&& InInstanceData, const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsDrawnAsInstance);
		RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, const RSRTransform& InTransform, InstanceDatas&& InInstanceData);
		RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, InstanceDatas&& InInstanceData);
		R_VIRTUAL_IMPLICIT ~RSRSObjectInstance() R_OVERRIDE_IMPLICIT;


		//Handled as instanced now
		virtual bool DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const override { return m_bIsDrawnAsInstance ? true : RSRSObject::DrawGeometry(InDraw3DCommandList); }

		virtual void OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual void OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
	};
};