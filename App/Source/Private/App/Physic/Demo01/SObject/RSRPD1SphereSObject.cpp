#include "App/Physic/Demo01/SObject/RSRPD1SphereSObject.h"
#include "App/Managers/RSRPhysicManager.h"

#include <DirectXMath.h>
#include "MDS/Tools/RMath.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/SceneObject/RSROScene.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;

RSRush::RSRPD1SphereSObject::RSRPD1SphereSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform, const RSRColBehavior& InColBehavior, const bool InbHasDynamics)
:RSRSObject(mds::RAssetAuthority::None, true), RSRIPhysicalEntity(), m_colBehavior(InColBehavior), m_hasDynamics(InbHasDynamics), m_instanceData(std::move(InInstanceData))
{
	m_mainTransform.SetTransform(InTransform);
	m_mainMesh = RSRBasicShapes::Get().GetDefSphere();
}

RSRush::RSRPD1SphereSObject::RSRPD1SphereSObject(InstanceDatas&& InInstanceData, const RSRTransform& InTransform)
:RSRPD1SphereSObject(std::move(InInstanceData), InTransform, RSRPD1SphereSObject::DEFAULT_COL_BEHAVIOR, RSRPD1SphereSObject::bDEFAULT_HAS_DYNAMICS)
{
}

RSRPD1SphereSObject::RSRPD1SphereSObject(InstanceDatas&& InInstanceData)
:RSRPD1SphereSObject(std::move(InInstanceData), mds::TRS_IDENTITY, RSRPD1SphereSObject::DEFAULT_COL_BEHAVIOR, RSRPD1SphereSObject::bDEFAULT_HAS_DYNAMICS)
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

void RSRush::RSRPD1SphereSObject::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRPD1SphereSObject> _thisPtr = std::dynamic_pointer_cast<RSRPD1SphereSObject>(InThisWPtr.lock());
		if (_thisPtr)
		{
			RSRush::RSRPhysicManager::Get().RemovePhysicalEntity(this->GetEditKey());
			if (std::shared_ptr<RSROScene> lockedScene = InScene.lock())
			{
				lockedScene->UnregisterInstancedMesheHolder(/*should be this*/_thisPtr.get(), m_mainMesh);
			}
		}
	}
}

bool RSRush::RSRPD1SphereSObject::LateTickSync(const double InGameTime, const double InDeltaTime)
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

void RSRush::RSRPD1SphereSObject::OnPhysicalPrePass(double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InDeltaTime);
}

void RSRush::RSRPD1SphereSObject::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	//RSRLog::Log(LOG_DISPLAY, TEXT("RSRPD1SphereSObject Overlap event received !"));
}

