#include "App/Data/Trench/Obstacles/RSRVoxelObstacle.h"
#include "App/Data/Trench/RSRVoxalGrid.h"

//Obstacles childs
#include "App/Data/Trench/Obstacles/RSRVoxelRectangularObstacle.h"
#include "App/Tools/RSRLog.h"
#include "App/Managers/RSRPhysicManager.h"

#include <random>

using namespace DirectX;
using namespace RSRush;

RSRVoxelObstacle::RSRVoxelObstacle(const EObstacleType EInObstacleType)
:m_obstacleType(EInObstacleType), m_placed(false), m_apd(RSRTransformMatrix(RSRTransform()), nullptr, nullptr, nullptr)
{
}

RSRVoxelObstacle::~RSRVoxelObstacle()
{
}


RSRush::RSRVoxelObstacle::AttemptPlacementData::AttemptPlacementData(const RSRush::RSRTransformMatrix& InTransformMatrix, DirectX::XMMATRIX* InLoadedTRSMatrix, RSRVoxalGrid* InOutGrid, RSRCollidersBodyLWPair* InLocalPair)
:TransformMatrix(RSRTransform()), lp_loadedTRSMatrix(InLoadedTRSMatrix), p_VoxalGrid(InOutGrid), lp_PhysicBodyPair(InLocalPair)
{
}
void RSRush::RSRVoxelObstacle::InitPlacementData(const RSRush::RSRTransformMatrix& InTransformMatrix, DirectX::XMMATRIX* InLoadedTRSMatrix, RSRVoxalGrid* InOutGrid, RSRCollidersBodyLWPair* InLocalPair)
{
	m_apd = AttemptPlacementData(InTransformMatrix, InLoadedTRSMatrix, InOutGrid, InLocalPair);
}

void RSRush::RSRVoxelObstacle::ClearPlacementData()
{
	m_apd = AttemptPlacementData(RSRTransformMatrix(RSRTransform()), nullptr, nullptr, nullptr);
}

bool RSRVoxelObstacle::AttemptPlacement(RSRPhysicManager* InPhysicManager, std::mt19937_64& InOutGenerator, RSRVoxalGrid& InOutGrid, const RSRush::RSRTransformMatrix& InTransformMatrix, uint8_t InMaxAttemptCount)
{
	return this->AttemptPlacement(InPhysicManager, InOutGenerator, InOutGrid, InTransformMatrix, RSRVoxelRectangle{ .Min = InOutGrid.Min(), .Max = InOutGrid.Max() }, InMaxAttemptCount);
}

RSRVoxelObstacle* RSRVoxelObstacle::HeapCreateNewObstacle(std::mt19937_64* InOutGenerator)
{
	//uint8_t not supported by uniform_int_distribution, si we are casting to and from uint16_t instead
    static std::uniform_int_distribution <uint16_t/*mds::UT<EObstacleType>*/> dist(mds::UT_cast(EObstacleType::MIN), mds::UT_cast(EObstacleType::MAX));
	EObstacleType ToSpawnType = static_cast<EObstacleType>((mds::UT<EObstacleType>)dist(*InOutGenerator));
	
	return HeapCreateNewObstacle(ToSpawnType);
}

RSRVoxelObstacle* RSRVoxelObstacle::HeapCreateNewObstacle(const EObstacleType InType)
{
	switch (InType)
	{
	case EObstacleType::Rectangle:
		return new RSRVoxelRectangularObstacle();
	default:
		RSRLog::Log(LOG_ERROR, TEXT("Unimplemented case {} in switch for 'RSRVoxelObstacle::HeapCreateNewObstacle'"), mds::UT_cast(InType));
		return nullptr;
	}
}

bool RSRush::RSRVoxelObstacle::AvalableVoxelSpace(RSRPhysicManager* InPhysicManager, const RSRVoxelRectangle& InVoxelSpace)
{
    RSRCollidersBody& localBody = m_apd.lp_PhysicBodyPair->DirtyGetLocal();
	localBody.Behavior = TEST_HAZARD_PLACEMENT;
    localBody.Colliders.resize(1);
    RSRCollider* bCol = &localBody.Colliders[0];

    bCol->Type = RSRColliderType::RectCollider;
    bCol->RectData =
    {
        /*.Center = */      InVoxelSpace.ComputeCenterSVXL(),
        /*.Extents = */     InVoxelSpace.ComputeExtendsSVXL(),
        /*.Orientation = */ /*QuatIdentity*/{0.f, 0.f, 0.f, 1.f}
    };

    localBody.ComputeAABB();

    m_apd.lp_PhysicBodyPair->RecomputeWorld(*m_apd.lp_loadedTRSMatrix, true);

	return !InPhysicManager->IsOverlappingAny(m_apd.lp_PhysicBodyPair->GetWorld(), std::weak_ptr<RSRIPhysicalEntity>());
}