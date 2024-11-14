#pragma once
#include "App/SceneObject/RSRSObject.h"
#include "App/Data/Shaders/ShaderStructures.h"
#include "App/Data/Physic/RSRPhysicBody.h"
#include "App/Data/Physic/RSRIPhysicalEntity.h"
#include "App/Gameplay/Scene/RSRIInstancedMesheHolder.h"

namespace RSRush
{
	class RSRPD1SphereSObject : public RSRSObject, public RSRIPhysicalEntity, public RSRIInstancedMesheHolder
	{
	protected:
		static constexpr bool bDEFAULT_HAS_DYNAMICS = true;
		static constexpr RSRColBehavior DEFAULT_COL_BEHAVIOR = WORLD_DYNAMIC;
	protected:
		RSRColBehavior m_colBehavior;
		bool m_hasDynamics;
		InstanceDatas m_instanceData;
	public:
		RSRPD1SphereSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform, const RSRColBehavior& InColBehavior, const bool InbHasDynamics);
		RSRPD1SphereSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform);
		RSRPD1SphereSObject(InstanceDatas&& InInstanceData);
		R_VIRTUAL_IMPLICIT ~RSRPD1SphereSObject() R_OVERRIDE_IMPLICIT;

		//Handled as instanced now
		virtual bool DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const override { return true; }

		virtual void OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual void OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;
	private:
		inline RSRPhysicBody GeneratePhysicBody() const { return GeneratePhysicBody(m_colBehavior, m_hasDynamics); }
		RSRPhysicBody GeneratePhysicBody(const RSRColBehavior& InColBehavior, bool InbHasDynamics) const;
	public:
		//Implemented via RSRIPhysicalEntity
		virtual void OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime) override;
		virtual void OnOverlapWith(RSRIPhysicalEntity* InOther) override;
	};
};