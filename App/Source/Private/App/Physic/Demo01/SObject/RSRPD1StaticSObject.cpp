#include "App/Physic/Demo01/SObject/RSRPD1StaticSObject.h"
#include "App/Managers/RSRPhysicManager.h"
#include <DirectXMath.h>
#include "MDS/Tools/RMath.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;

bool RSRush::RSRPD1StaticSObject::DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const
{
	return RSRSObject::DrawGeometry(InDraw3DCommandList);
	if (m_mainMesh)
	{
		XMMATRIX trsMatCustom = m_mainTransform.GetTransform().ComputeTRSMatrix(XMLoadFloat4(&CustomRot));
		//MVPDLC::S32B_MOD_MAT
		InDraw3DCommandList->SetGraphicsRoot32BitConstants(0, MVPDLC::S32B_MOD_MAT, &trsMatCustom, MVPDLC::O32B_MOD_MAT);

		//Compute InverseTranspose matrix for normal computations
		RSRTransform NoTranslation = m_mainTransform.GetTransform();
		NoTranslation.Position = DirectX::XMFLOAT3(0.f, 0.f, 0.f);

		DirectX::XMMATRIX TransformMatrix = NoTranslation.ComputeTRSMatrix(XMLoadFloat4(&CustomRot));
		DirectX::XMMATRIX InverseTranspose = mds::RMath::InverseTranspose(TransformMatrix);

		DirectX::XMFLOAT4X4  StoredInverseTranspose;
		DirectX::XMStoreFloat4x4(&StoredInverseTranspose, InverseTranspose);

		InDraw3DCommandList->SetGraphicsRoot32BitConstants(0, MVPDLC::S32B_INV_MOD_MAT, &StoredInverseTranspose, MVPDLC::O32B_INV_MOD_MAT);


		return m_mainMesh->DrawMesh(InDraw3DCommandList);
	}
	return true;
}

RSRush::RSRPD1StaticSObject::RSRPD1StaticSObject(const RSRTransform& InTransform, const RSRColBehavior& InColBehavior, const bool InbHasDynamics)
:RSRSObject(mds::RAssetAuthority::None, true), RSRIPhysicalEntity(), m_colBehavior(InColBehavior), m_hasDynamics(InbHasDynamics)
{
	m_mainTransform.SetTransform(InTransform);
	m_mainMesh = RSRBasicShapes::Get().GetDefSquare();
}

RSRush::RSRPD1StaticSObject::RSRPD1StaticSObject(const RSRTransform& InTransform)
	:RSRPD1StaticSObject(InTransform, RSRPD1StaticSObject::DEFAULT_COL_BEHAVIOR, RSRPD1StaticSObject::bDEFAULT_HAS_DYNAMICS)
{
}

RSRPD1StaticSObject::RSRPD1StaticSObject()
	:RSRPD1StaticSObject(mds::TRS_IDENTITY, RSRPD1StaticSObject::DEFAULT_COL_BEHAVIOR, RSRPD1StaticSObject::bDEFAULT_HAS_DYNAMICS)
{
}

void RSRush::RSRPD1StaticSObject::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1StaticSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1StaticSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			this->SetSelfReference(_thisPtr);
			RSRush::RSRPhysicManager::Get().AddPhysicalEntity(this->GeneratePhysicBody());
		}
	}
}

void RSRush::RSRPD1StaticSObject::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1StaticSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1StaticSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			RSRush::RSRPhysicManager::Get().RemovePhysicalEntity(this->GetEditKey());
		}
	}
}

RSRPD1StaticSObject::~RSRPD1StaticSObject()
{
}

RSRPhysicBody RSRush::RSRPD1StaticSObject::GeneratePhysicBody(const RSRColBehavior& InColBehavior, bool InbHasDynamics) const
{
	RSRPhysicBody result =
	{
		.Transform = m_mainTransform,
		.Colliders = RSRCollidersBodyLWPair(InColBehavior),
		.Dynamics = InbHasDynamics  ? Dynamics(1.f) : NoDynamics(1.f),
		.Entity = m_physicalBodyKey.SelfEntity
	};

	RSRCollidersBody& localBody = result.Colliders.DirtyGetLocal();
	localBody.Colliders.resize(1);
	RSRCollider* bodyCol = &localBody.Colliders[0];

	XMFLOAT4 QuatIdentity = { 0.f, 0.f, 0.f, 1.f };
	//XMMatrixTransformation
	bodyCol->Type = RSRColliderType::RectCollider;
	bodyCol->RectData =
	{
		/*.Center = */      {0.f, 0.f, 0.f},
		/*.Extents = */     {0.5f, 0.5f, 0.5f},
		/*.Orientation = */ QuatIdentity
	};

	localBody.ComputeAABB();

	XMMATRIX worldTransform = XMLoadFloat4x4(&result.Transform.GetMatrix());
	result.Colliders.RecomputeWorld(worldTransform, true);
	return result;
}

bool RSRush::RSRPD1StaticSObject::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool bAllSucessfull = RSRSObject::LateTickSync(InGameTime, InDeltaTime);
	if (m_hasDynamics && GetKey().bHasBeenRegistered)
	{
		m_mainTransform = this->GetLastResolvedPhysicBody().Transform;
	}
	return bAllSucessfull;
}

void RSRush::RSRPD1StaticSObject::OnPhysicalPrePass(double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InDeltaTime);
}

void RSRush::RSRPD1StaticSObject::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	//RSRLog::Log(LOG_DISPLAY, TEXT("RSRPD1StaticSObject Overlap event received !"));
}

