#include "App/Physic/Demo01/Gameplay/RSRPD1Pawn.h"
#include "App/Managers/RSRPhysicManager.h"
#include <App/Tools/RSRLog.h>
#include "MDS/Tools/RMath.h"
#include <DirectXMath.h>

using namespace RSRush;
using namespace DirectX;

static const XMVECTOR MOUVEMENT_SPEED = { 10.f, 10.f, 10.f, 0.f };

RSRPD1Pawn::RSRPD1Pawn()
	: Pawn(mds::RAssetAuthority::None), RSRIPhysicalEntity(), m_prePhysicNewTransform(RSRush::RSRTransformMatrix(mds::TRS_IDENTITY))
{
	m_mainTransform.SetPositon(XMFLOAT3{ 0.f, 0.f,1.8f });
	m_prePhysicNewTransform = m_mainTransform;
	m_CameraData = {
			.Position = { 0.f,  0.f, 0.0f},
			.Target = { 5.f, 0.f,  0.0f },
			.Up = { 0.f, 0.f,  1.f },

			.FOV = (5.f / 12.f) * DirectX::XM_PI, //75°
			.AspectRatio = 16.f / 9.f,
			.NearClip = 0.1f,
			.FarClip = 1000.f
	};

	OnPositionUpdated();
}

RSRPD1Pawn::~RSRPD1Pawn()
{
}

void RSRush::RSRPD1Pawn::PrepassRequestMovement(const DirectX::XMFLOAT3& InAddMovement)
{
	m_prePhysicNewTransform.SetPositon(mds::unoptimized_add3(m_prePhysicNewTransform.GetTransform().Position, InAddMovement));
}

void RSRush::RSRPD1Pawn::PrepassRequestMovementLook(const DirectX::XMFLOAT2& InAddLookMovement)
{
	m_prePhysicLookDir = mds::unoptimized_add2(m_prePhysicLookDir, InAddLookMovement);
	m_prePhysicLookDir.x = fmod(m_prePhysicLookDir.x, 180.f);
	m_prePhysicLookDir.y = fmod(m_prePhysicLookDir.y, 180.f);
}

void RSRush::RSRPD1Pawn::OnInputFrameEnded()
{
	Pawn::OnInputFrameEnded();
	this->OnPhysicalObjectRequestMove(m_prePhysicNewTransform);
}

void RSRush::RSRPD1Pawn::OnPositionUpdated()
{
	m_CameraData.Position = mds::unoptimized_add3(m_mainTransform.GetTransform().Position, XMFLOAT3(-5.f, 0.f, 0.5f));

	XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(m_prePhysicLookDir.y), XMConvertToRadians(m_prePhysicLookDir.x));
	//
	XMVECTOR targetVector = XMVectorSet(5.f, 0.f, 0.f, 0.f);
	targetVector = XMVector3Rotate(targetVector, rot);
	targetVector = XMLoadFloat3(&m_CameraData.Position) + targetVector;

	XMStoreFloat3(&m_CameraData.Target, targetVector);
	//m_CameraData.Target = mds::unoptimized_add3(m_CameraData.Position, XMFLOAT3{ 5.f, 0.f, 0.f });
	m_prePhysicNewTransform = m_mainTransform;
}

void RSRush::RSRPD1Pawn::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	Pawn::OnAddedToScene(InThisWPtr, InScene);
	//At this point RSRPD1Pawn is registered memTree element

	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1Pawn> _thisPtr = std::dynamic_pointer_cast<RSRPD1Pawn>(InThisWPtr.lock());
		if (_thisPtr)
		{
			this->SetSelfReference(_thisPtr);
			RSRush::RSRPhysicManager::Get(this)->AddPhysicalEntity(this->GeneratePhysicBody());
		}
	}
}

void RSRush::RSRPD1Pawn::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1Pawn> _thisPtr = std::dynamic_pointer_cast<RSRPD1Pawn>(InThisWPtr.lock());
		if (_thisPtr)
		{
			RSRush::RSRPhysicManager::Get(this)->RemovePhysicalEntity(this->GetEditKey());
		}
	}
	Pawn::OnRemovedFromScene(InThisWPtr, InScene);
}

bool RSRush::RSRPD1Pawn::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	Pawn::LateTickSync(InGameTime, InDeltaTime);

	if (GetKey().bHasBeenRegistered)
	{
		m_mainTransform = this->GetLastResolvedPhysicBody().Transform;
		OnPositionUpdated();
	}
	
	return true;
}

RSRPhysicBody RSRush::RSRPD1Pawn::GeneratePhysicBody() const
{
	RSRPhysicBody result =
	{
		.Transform = m_mainTransform,
		.Colliders = RSRCollidersBodyLWPair(RSRush::PLAYER),
		.Dynamics = NoDynamics(10.f),
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

void RSRush::RSRPD1Pawn::OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InPhysicManager, InDeltaTime);
}

void RSRush::RSRPD1Pawn::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	//RSRLog::Log(LOG_DISPLAY, TEXT("RSRPD1Pawn Overlap event received !"));
}
