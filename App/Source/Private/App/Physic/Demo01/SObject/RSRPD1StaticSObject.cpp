#include "App/Physic/Demo01/SObject/RSRPD1StaticSObject.h"
#include "App/Managers/RSRPhysicManager.h"
#include <DirectXMath.h>
#include "MDS/Tools/RMath.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/SceneObject/RSROScene.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;



RSRush::RSRPD1StaticSObject::RSRPD1StaticSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform, const RSRColBehavior& InColBehavior, const bool InbHasDynamics)
:RSRSObject(mds::RAssetAuthority::None, true), RSRIPhysicalEntity(), m_colBehavior(InColBehavior), m_hasDynamics(InbHasDynamics), m_instanceData(std::move(InInstanceData))
{
	m_mainTransform.SetTransform(InTransform);
	//At this point RSRPD1StaticSObject is not yet registered memTree element, cannot acess basic shapes
	m_mainMesh = nullptr;
}

RSRush::RSRPD1StaticSObject::RSRPD1StaticSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform)
	:RSRPD1StaticSObject(std::move(InInstanceData), InTransform, RSRPD1StaticSObject::DEFAULT_COL_BEHAVIOR, RSRPD1StaticSObject::bDEFAULT_HAS_DYNAMICS)
{
}

RSRPD1StaticSObject::RSRPD1StaticSObject(InstanceDatas&& InInstanceData)
	:RSRPD1StaticSObject(std::move(InInstanceData), mds::TRS_IDENTITY, RSRPD1StaticSObject::DEFAULT_COL_BEHAVIOR, RSRPD1StaticSObject::bDEFAULT_HAS_DYNAMICS)
{
}

void RSRush::RSRPD1StaticSObject::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	RSRSObject::OnAddedToScene(InThisWPtr, InScene);
	//At this point RSRPD1StaticSObject is registered memTree element
	m_mainMesh = RSRBasicShapes::Get(this)->GetDefSquare();

	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1StaticSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1StaticSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			this->SetSelfReference(_thisPtr);
			RSRush::RSRPhysicManager::Get(this)->AddPhysicalEntity(this->GeneratePhysicBody());
			if (std::shared_ptr<RSROScene> lockedScene = InScene.lock())
			{
				lockedScene->RegisterInstancedMesheHolder(/*should be this*/_thisPtr.get(), m_mainMesh);
				if (!m_hasDynamics)
				{
					RSRTransformMatrix NoTranslationMatrix = m_mainTransform;
					NoTranslationMatrix.SetPositon(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

					DirectX::XMMATRIX TransformMatrix = DirectX::XMLoadFloat4x4(&NoTranslationMatrix.GetMatrix());
					DirectX::XMMATRIX InverseTranspose = mds::RMath::InverseTranspose(TransformMatrix);

					DirectX::XMFLOAT4X4  StoredInverseTranspose;
					DirectX::XMStoreFloat4x4(&StoredInverseTranspose, InverseTranspose);

					m_vertexBufferDatas.insert_or_assign
					(
						reinterpret_cast<uintptr_t>(m_mainMesh.get()),
						std::vector<RSRush::VertexBufferInstanceData>
						{
							VertexBufferInstanceData
							{
								.ModelMatrixes = InstanceModelMatrixes
								{
									.ModMat = m_mainTransform.GetMatrix(),
									.InvProjModMat = StoredInverseTranspose
								},
								.Datas = m_instanceData
							}
						}
					);
				}
			}
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
			RSRush::RSRPhysicManager::Get(this)->RemovePhysicalEntity(this->GetEditKey());
			if (std::shared_ptr<RSROScene> lockedScene = InScene.lock())
			{
				lockedScene->UnregisterInstancedMesheHolder(/*should be this*/_thisPtr.get(), m_mainMesh);
			}
		}
	}
	RSRSObject::OnRemovedFromScene(InThisWPtr, InScene);
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

		RSRTransformMatrix NoTranslationMatrix = m_mainTransform;
		NoTranslationMatrix.SetPositon(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

		DirectX::XMMATRIX TransformMatrix = DirectX::XMLoadFloat4x4(&NoTranslationMatrix.GetMatrix());
		DirectX::XMMATRIX InverseTranspose = mds::RMath::InverseTranspose(TransformMatrix);

		DirectX::XMFLOAT4X4  StoredInverseTranspose;
		DirectX::XMStoreFloat4x4(&StoredInverseTranspose, InverseTranspose);

		m_vertexBufferDatas.insert_or_assign
		(
			reinterpret_cast<uintptr_t>(m_mainMesh.get()),
			std::vector<RSRush::VertexBufferInstanceData>
			{
				VertexBufferInstanceData
				{
					.ModelMatrixes = InstanceModelMatrixes
					{
						.ModMat = m_mainTransform.GetMatrix(),
						.InvProjModMat = StoredInverseTranspose
					},
					.Datas = m_instanceData
				}
			}
		);
	}
	return bAllSucessfull;
}

void RSRush::RSRPD1StaticSObject::OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InPhysicManager, InDeltaTime);
}

void RSRush::RSRPD1StaticSObject::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	//RSRLog::Log(LOG_DISPLAY, TEXT("RSRPD1StaticSObject Overlap event received !"));
}

