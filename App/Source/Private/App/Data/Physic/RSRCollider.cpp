#include "App/Data/Physic/RSRCollider.h"

#include "MDS/Tools/RMath.h"

#include "App/Data/RSRTransform.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"


using namespace RSRush;
using namespace DirectX;

static const BoundingOrientedBox DEFAULT_BOUNDING_ORIENTED_BOX =
{
	{ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 1.f }
};

static const BoundingSphere DEFAULT_BOUNDING_SPHERE =
{
	{ 0.f, 0.f, 0.f }, 0.f
};

RSRCollider::RSRCollider()
:Type(RSRColliderType::RectCollider), RectData(DEFAULT_BOUNDING_ORIENTED_BOX)
{
}

RSRCollider::RSRCollider(RSRColliderType InType)
:Type(InType)
{
	if (Type == RSRColliderType::RectCollider)
	{
		RectData = DEFAULT_BOUNDING_ORIENTED_BOX;
	}
	else if (Type == RSRColliderType::SphereCollider)
	{
		SphereData = DEFAULT_BOUNDING_SPHERE;
	}
	else
	{
		RectData = DEFAULT_BOUNDING_ORIENTED_BOX;
		RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::RSRCollider'"));
	}
}

RSRCollider::RSRCollider(const DirectX::BoundingOrientedBox& InRectData)
:Type(RSRColliderType::RectCollider), RectData(InRectData)
{
}

RSRCollider::RSRCollider(const DirectX::BoundingSphere& InSphereData)
:Type(RSRColliderType::SphereCollider), SphereData(InSphereData)
{
}

RSRCollider RSRCollider::TransformCopy(const XMMATRIX& InTransform) const
{
	RSRCollider result = *this;
	switch (this->Type)
	{
	case RSRColliderType::RectCollider:
		this->RectData.Transform(result.RectData, InTransform);
		break;
	case RSRColliderType::SphereCollider:
		this->SphereData.Transform(result.SphereData, InTransform);
		break;
	default:
		RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::OverlapWith'"));
	}
	return result;
}
bool RSRCollider::OverlapWith(const RSRCollider& InOther) const
{
	bool oneIsRect = (this->Type == RSRColliderType::RectCollider);
	const RSRCollider* col1 = (oneIsRect) ? this : &InOther;
	const RSRCollider* col2 = (!oneIsRect) ? this : &InOther;
	switch (col1->Type)
	{
		case RSRColliderType::RectCollider:
		{
			switch (col2->Type)
			{
				case RSRColliderType::RectCollider:
					return col1->RectData.Intersects(col2->RectData);
				case RSRColliderType::SphereCollider:
					return col1->RectData.Intersects(col2->SphereData);
				default:
					RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::OverlapWith'"));
			}
			break;
		}
		case RSRColliderType::SphereCollider:
		{
			switch (col2->Type)
			{
				case RSRColliderType::RectCollider:						//This should not happen because of early swap
					return col1->SphereData.Intersects(col2->RectData);	//This should not happen because of early swap
				case RSRColliderType::SphereCollider:
					return col1->SphereData.Intersects(col2->SphereData);
				default:
					RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::OverlapWith'"));
			}
			break;
		}
		default:
		{
			RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::OverlapWith'"));
			break;
		}
	}
	return false;
}

DirectX::BoundingBox RSRCollider::ComputeBoundBox() const
{
	switch (this->Type)	
	{
		case RSRColliderType::RectCollider:
			return mds::RMath::AABBFromOrientedBox(this->RectData);
		case RSRColliderType::SphereCollider:
			return mds::RMath::AABBFromSphere(this->SphereData);
		default:
			RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::ComputeBoundBox'"));
	}
	return DirectX::BoundingBox({0.f, 0.f, 0.f}, { 0.f, 0.f, 0.f });
}

void RSRCollider::ExtendBoundBox(DirectX::BoundingBox* InOutBoundingBox) const
{
	switch (this->Type)
	{
	case RSRColliderType::RectCollider:
		mds::RMath::ExtendAABBWithOrientedBox(*InOutBoundingBox, this->RectData);
		break;
	case RSRColliderType::SphereCollider:
		mds::RMath::ExtendAABBWithSphere(*InOutBoundingBox, this->SphereData);
		break;
	default:
		RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::ComputeBoundBox'"));
		break;
	}
}

#if DEBUG_PHYSIC
bool RSRCollider::DebugDraw(mds::IRProgramMemElem* InContext, ID3D12GraphicsCommandList7* InUploadCommandList) const
{
	RSRPhysicManager* physicManager = InContext->GetRoot<RSRProgramInstance>()->GetPhysicManager();
	return this->DebugDraw(physicManager, InUploadCommandList);
}

bool RSRush::RSRCollider::DebugDraw(RSRPhysicManager* InPhysicManager, ID3D12GraphicsCommandList7* InUploadCommandList) const
{
	switch (this->Type)
	{
	case RSRColliderType::RectCollider:
		return RSRCollider::DebugDrawOrientedBox(InPhysicManager, this->RectData, InUploadCommandList);
	case RSRColliderType::SphereCollider:
		return RSRCollider::DebugDrawSphere(InPhysicManager, this->SphereData, InUploadCommandList);
	default:
		RSRLog::LogError(TEXT("Unimplement case in 'RSRCollider::DebugDraw'"));
		return false;
	}
}

bool RSRCollider::DebugDrawOrientedBox(RSRPhysicManager* InPhysicManager, const DirectX::BoundingOrientedBox& InOrientedBox, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	const RSRush::RSRSharedMesh3DPtr& squareMesh = InPhysicManager->GetDebugSquare();
	//InOrientedBox.Center
	
	if (squareMesh)
	{
		XMVECTOR vTranslation = DirectX::XMLoadFloat3(&InOrientedBox.Center);
		XMVECTOR vRotation = DirectX::XMLoadFloat4(&InOrientedBox.Orientation);
		XMVECTOR vScale = DirectX::XMLoadFloat3(&InOrientedBox.Extents);
		vScale *= 2.f;

		XMMATRIX transformMatrix = DirectX::XMMatrixTransformation(
			DirectX::XMVectorZero(), // scaling origin
			DirectX::XMQuaternionIdentity(), // rotation origin
			vScale, // scale vector
			DirectX::XMVectorZero(), // rotation origin
			vRotation,//DirectX::XMQuaternionRotationRollPitchYawFromVector(vRotation), // rotation quaternion
			vTranslation // translation vector
		);

		// Store the result back into a XMFLOAT4X4
		DirectX::XMFLOAT4X4 modelMatrix;
		DirectX::XMStoreFloat4x4(&modelMatrix, transformMatrix);
		InUploadCommandList->SetGraphicsRoot32BitConstants(0, ModelViewprojectionConstants::S32B_MOD_MAT, &modelMatrix, ModelViewprojectionConstants::O32B_MOD_MAT);

		return squareMesh->DrawMesh(InUploadCommandList);
	}
	return false;
}
bool RSRCollider::DebugDrawSphere(RSRPhysicManager* InPhysicManager, const DirectX::BoundingSphere& InSphere, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	const RSRush::RSRSharedMesh3DPtr& sphereMesh = InPhysicManager->GetDebugSphere();
	//InOrientedBox.Center

	if (sphereMesh)
	{
		XMVECTOR vTranslation = DirectX::XMLoadFloat3(&InSphere.Center);
		XMVECTOR vRotation = DirectX::XMQuaternionIdentity();
		float diameter = InSphere.Radius * 2.f;
		XMVECTOR vScale = DirectX::XMVECTOR{ diameter, diameter, diameter, 0.f };

		XMMATRIX transformMatrix = DirectX::XMMatrixTransformation(
			DirectX::XMVectorZero(), // scaling origin
			DirectX::XMQuaternionIdentity(), // rotation origin
			vScale, // scale vector
			DirectX::XMVectorZero(), // rotation origin
			DirectX::XMQuaternionRotationRollPitchYawFromVector(vRotation), // rotation quaternion
			vTranslation // translation vector
		);

		// Store the result back into a XMFLOAT4X4
		DirectX::XMFLOAT4X4 modelMatrix;
		DirectX::XMStoreFloat4x4(&modelMatrix, transformMatrix);
		InUploadCommandList->SetGraphicsRoot32BitConstants(0, ModelViewprojectionConstants::S32B_MOD_MAT, &modelMatrix, ModelViewprojectionConstants::O32B_MOD_MAT);

		return sphereMesh->DrawMesh(InUploadCommandList);
	}
	return false;
}
bool RSRush::RSRCollider::DebugDrawBox(RSRPhysicManager* InPhysicManager, const DirectX::BoundingBox& InBox, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	const RSRush::RSRSharedMesh3DPtr& squareMesh = InPhysicManager->GetDebugAABB();

	if (squareMesh)
	{
		XMVECTOR vTranslation = DirectX::XMLoadFloat3(&InBox.Center);
		XMVECTOR vScale = DirectX::XMLoadFloat3(&InBox.Extents);
		vScale *= 2.f;

		XMMATRIX transformMatrix = DirectX::XMMatrixTransformation(
			DirectX::XMVectorZero(), // scaling origin
			DirectX::XMQuaternionIdentity(), // rotation origin
			vScale, // scale vector
			DirectX::XMVectorZero(), // rotation origin
			DirectX::XMQuaternionIdentity(), // rotation quaternion
			vTranslation // translation vector
		);

		// Store the result back into a XMFLOAT4X4
		DirectX::XMFLOAT4X4 modelMatrix;
		DirectX::XMStoreFloat4x4(&modelMatrix, transformMatrix);
		InUploadCommandList->SetGraphicsRoot32BitConstants(0, ModelViewprojectionConstants::S32B_MOD_MAT, &modelMatrix, ModelViewprojectionConstants::O32B_MOD_MAT);

		return squareMesh->DrawMesh(InUploadCommandList);
	}
	return false;
}
#endif
