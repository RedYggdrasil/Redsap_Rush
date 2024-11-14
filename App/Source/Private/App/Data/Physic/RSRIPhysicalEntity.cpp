#include "App/Data/Physic/RSRIPhysicalEntity.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;

RSRPhysicBody* RSRIPhysicalEntity::GetPhysicalBodyPtr(RSRPhysicManager* InPhysicManager) const
{
    if (m_physicalBodyKey.bHasBeenRegistered)
    {
        return InPhysicManager->GetPhysicBody(m_physicalBodyKey);
    }
    return nullptr;
}

void RSRush::RSRIPhysicalEntity::OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime)
{
    if (m_physicalBodyKey.bHasBeenRegistered && m_transformedPhysicalObjectData.bHasNewTransform)
    {
        RSRPhysicBody* body = InPhysicManager->GetPhysicBody(m_physicalBodyKey);
        if (body)
        {
            body->Transform = m_transformedPhysicalObjectData.NewTransform;
            DirectX::XMMATRIX mat = XMLoadFloat4x4(&body->Transform.GetMatrix());
            body->Colliders.RecomputeWorld(mat, true);
        }
        else
        {
            RSRLog::Log(LOG_ERROR, TEXT("Cannot find RSRPhysicBody for RSRIPhysicalEntity !"));
        }
        m_transformedPhysicalObjectData.bHasNewTransform = false;
    }
}

RSRush::RSRIPhysicalEntity::RSRIPhysicalEntity()
: /*m_physicManager(InPhysicManager),*/ m_lastResolvedPhysicBody(UNSET_PHYSIC_BODY), m_physicalBodyKey
    {
        .SelfEntity = std::weak_ptr<RSRIPhysicalEntity>(),
        .LastKnownIndex = std::numeric_limits<size_t>::max(),
        .bHasBeenRegistered = false
    }, m_transformedPhysicalObjectData
    {
        .NewTransform = RSRTransformMatrix(mds::TRS_IDENTITY),
        .bHasNewTransform = false
    }

{
}

void RSRush::RSRIPhysicalEntity::OnPhysicalObjectRequestMove(const RSRTransformMatrix& InNewTransform)
{
    m_transformedPhysicalObjectData.NewTransform = InNewTransform;
    m_transformedPhysicalObjectData.bHasNewTransform = true;
}
