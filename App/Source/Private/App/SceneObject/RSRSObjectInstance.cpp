#include "App/SceneObject/RSRSObjectInstance.h"
#include "App/SceneObject/RSROScene.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;

using Super = RSRSObject;


RSRSObjectInstance::RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, const RSRTransform& InTransform, InstanceDatas&& InInstanceData, const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsDrawnAsInstance)
	:RSRSObject(InMainMeshAuthority, bInIsDrawnAsInstance), m_instanceData(std::move(InInstanceData))
{
	m_mainTransform.SetTransform(InTransform);
	m_mainMesh = InMesh;
}

RSRSObjectInstance::RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, const RSRTransform& InTransform, InstanceDatas&& InInstanceData)
	:RSRSObjectInstance(InMesh, InTransform, std::move(InInstanceData), mds::RAssetAuthority::None, true)
{
}

RSRSObjectInstance::RSRSObjectInstance(RSRush::RSRSharedMesh3DPtr InMesh, InstanceDatas&& InInstanceData)
	:RSRSObjectInstance(InMesh, mds::TRS_IDENTITY, std::move(InInstanceData), mds::RAssetAuthority::None, true)
{
}

RSRSObjectInstance::~RSRSObjectInstance()
{
}

void RSRush::RSRSObjectInstance::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	RSRSObject::OnAddedToScene(InThisWPtr, InScene);
	if (m_bIsDrawnAsInstance && !InThisWPtr.expired())
	{
		std::shared_ptr<RSRSObjectInstance> _thisPtr = std::dynamic_pointer_cast<RSRSObjectInstance>(InThisWPtr.lock());
		if (_thisPtr)
		{
			if (std::shared_ptr<RSROScene> lockedScene = InScene.lock())
			{
				lockedScene->RegisterInstancedMesheHolder(/*should be this*/_thisPtr.get(), m_mainMesh);

				m_vertexBufferDatas.insert_or_assign
				(
					reinterpret_cast<uintptr_t>(m_mainMesh.get()),
					std::vector<RSRush::VertexBufferInstanceData>
					{
						VertexBufferInstanceData
						{
							.ModelMatrixes = InstanceModelMatrixes::FromTrsMatrix(m_mainTransform),
							.Datas = m_instanceData
						}
					}
				);
			}
		}
	}
}

void RSRush::RSRSObjectInstance::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (m_bIsDrawnAsInstance && !InThisWPtr.expired())
	{
		std::shared_ptr<RSRSObjectInstance> _thisPtr = std::dynamic_pointer_cast<RSRSObjectInstance>(InThisWPtr.lock());
		if (_thisPtr)
		{
			if (std::shared_ptr<RSROScene> lockedScene = InScene.lock())
			{
				lockedScene->UnregisterInstancedMesheHolder(/*should be this*/_thisPtr.get(), m_mainMesh);
			}
		}
	}
	RSRSObject::OnRemovedFromScene(InThisWPtr, InScene);
}
