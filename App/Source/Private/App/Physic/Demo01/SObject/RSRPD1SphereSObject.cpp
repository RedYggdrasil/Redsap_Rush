#include "App/Physic/Demo01/SObject/RSRPD1SphereSObject.h"
#include "App/Managers/RSRPhysicManager.h"

#include <DirectXMath.h>
#include "MDS/Tools/RMath.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;

RSRush::RSRPD1SphereSObject::RSRPD1SphereSObject(const RSRTransform& InTransform, const RSRColBehavior& InColBehavior, const bool InbHasDynamics)
:RSRSObject(mds::RAssetAuthority::None, true), RSRIPhysicalEntity(), m_colBehavior(InColBehavior), m_hasDynamics(InbHasDynamics)
{
	m_mainTransform.SetTransform(InTransform);
	m_mainMesh = RSRBasicShapes::Get().GetDefSphere();
}

RSRush::RSRPD1SphereSObject::RSRPD1SphereSObject(const RSRTransform& InTransform)
:RSRPD1SphereSObject(InTransform, RSRPD1SphereSObject::DEFAULT_COL_BEHAVIOR, RSRPD1SphereSObject::bDEFAULT_HAS_DYNAMICS)
{
}

RSRPD1SphereSObject::RSRPD1SphereSObject()
:RSRPD1SphereSObject(mds::TRS_IDENTITY, RSRPD1SphereSObject::DEFAULT_COL_BEHAVIOR, RSRPD1SphereSObject::bDEFAULT_HAS_DYNAMICS)
{}

RSRPD1SphereSObject::~RSRPD1SphereSObject()
{
}

RSRPhysicBody RSRush::RSRPD1SphereSObject::GeneratePhysicBody(const RSRColBehavior& InColBehavior, bool InbHasDynamics) const
{
	RSRPhysicBody result =
	{
		.Transform = m_mainTransform,
		.Colliders = RSRCollidersBodyLWPair(InColBehavior),
		.Dynamics = InbHasDynamics ? Dynamics(1.f) : NoDynamics(1.f),
		.Entity = m_physicalBodyKey.SelfEntity
	};

	RSRCollidersBody& localBody = result.Colliders.DirtyGetLocal();
	localBody.Colliders.resize(1);
	RSRCollider* bodyCol = &localBody.Colliders[0];

	//XMMatrixTransformation
	bodyCol->Type = RSRColliderType::SphereCollider;
	bodyCol->SphereData =
	{
		/*.Center = */      {0.f, 0.f, 0.f},
		/*.Radius = */     0.5f
	};

	localBody.ComputeAABB();

	XMMATRIX worldTransform = XMLoadFloat4x4(&result.Transform.GetMatrix());
	result.Colliders.RecomputeWorld(worldTransform, true);
	return result;
}


void RSRush::RSRPD1SphereSObject::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1SphereSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1SphereSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			this->SetSelfReference(_thisPtr);
			RSRush::RSRPhysicManager::Get().AddPhysicalEntity(this->GeneratePhysicBody());
		}
	}
}

void RSRush::RSRPD1SphereSObject::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1SphereSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1SphereSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			RSRush::RSRPhysicManager::Get().RemovePhysicalEntity(this->GetEditKey());
		}
	}
}

bool RSRush::RSRPD1SphereSObject::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool bAllSucessfull = RSRSObject::LateTickSync(InGameTime, InDeltaTime);
	if (m_hasDynamics && GetKey().bHasBeenRegistered)
	{
		m_mainTransform = this->GetLastResolvedPhysicBody().Transform;
	}
	return bAllSucessfull;
}

void RSRush::RSRPD1SphereSObject::OnPhysicalPrePass(double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InDeltaTime);
}

void RSRush::RSRPD1SphereSObject::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	//RSRLog::Log(LOG_DISPLAY, TEXT("RSRPD1SphereSObject Overlap event received !"));
}

