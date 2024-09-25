#include "App/Data/Physic/RSRPPConnection.h"

#include "MDS/Tools/RMath.h"

#include "App/Game/RSRPlayerPath.h"
#include "App/Managers/RSRPhysicManager.h"

#include "App/Tools/RSRLog.h"
using namespace RSRush;
using namespace DirectX;

const RSRPPConnection RSRPPConnection::INVALID_CONNECTION = RSRPPConnection();

std::shared_ptr<RSRPPConnection> RSRPPConnection::Create(RSRPlayerPath& OwningPath, const uint16_t InNextNodeIndex, const XMFLOAT3& InStartLocation, const XMFLOAT3& InEndLocation)
{
	std::shared_ptr<RSRPPConnection> r = std::make_shared<RSRPPConnection>();
	r->SetSelfReference(r);

	r->NextNodeIndex = InNextNodeIndex;

	RSRPhysicBody result =
	{
		.Transform = RSRTransformMatrix(mds::TRS_IDENTITY),
		.Colliders = RSRCollidersBodyLWPair(PAWN_PATH),
		.Dynamics = NoDynamics(0.f),
		.Entity = r
	};
	RSRCollidersBody& localColliders = result.Colliders.DirtyGetLocal();

	localColliders.Colliders.resize(1);

	RSRCollider* col = &localColliders.Colliders[0];
	*col = RSRCollider(RSRColliderType::RectCollider);

	XMVECTOR start = XMLoadFloat3(&InStartLocation);
	XMVECTOR endLoc = XMLoadFloat3(&InEndLocation);
	XMVECTOR segment = endLoc - start;


	XMStoreFloat3(&col->RectData.Center, start + (segment * 0.5f));

	float halfDistance = XMVectorGetX(XMVector3Length(segment)) * 0.5f;
	col->RectData.Extents = { halfDistance, 1.f, 1.f };

	XMStoreFloat4(&col->RectData.Orientation, mds::RMath::LookAtQuatRad(start, endLoc));

	XMMATRIX lMat = XMLoadFloat4x4(&result.Transform.GetMatrix());
	result.Colliders.RecomputeWorld(lMat, true);

	RSRPhysicManager::Get().AddPhysicalEntity(result);

	return r;
}

void RSRush::RSRPPConnection::Clean(std::shared_ptr<RSRPPConnection> InConnection)
{
	RSRPhysicManager::Get().RemovePhysicalEntity(InConnection->GetEditKey());
}

RSRPPConnection::RSRPPConnection()
:RSRIPhysicalEntity(), NextNodeIndex(UINT16_MAX)
{
}


bool RSRush::RSRPPConnection::IsValid() const
{
	return this->NextNodeIndex < RSRPPNode::MAX_PARALLEL_PATHS;
}
