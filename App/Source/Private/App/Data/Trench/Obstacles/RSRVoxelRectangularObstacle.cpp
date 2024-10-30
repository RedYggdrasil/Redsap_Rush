#include "App/Data/Trench/Obstacles/RSRVoxelRectangularObstacle.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Data/Physic/RSRPhysicBody.h"
#include "App/Data/Trench/RSRTrenchDefs.h"
//#include "App/Data/Trench/RSRVoxalPlane.h"
//#include 

using namespace DirectX;
using namespace RSRush;


RSRush::RSRVoxelRectangularObstacle::RSRVoxelRectangularObstacle()
:RSRVoxelObstacle(RSRVoxelRectangularObstacle::TYPE)
{
}

RSRush::RSRVoxelRectangularObstacle::~RSRVoxelRectangularObstacle()
{
}



//static constexpr float TRENCH_WIDTH = SVXL(TRENCH_WIDTH_VXL);

constexpr bool LOG_ATTEMPT_FAILURE = false;
#define LOGIF(Expr) if(LOG_ATTEMPT_FAILURE) {Expr;}
#define RETURN_FALSE_LOGIF(Expr) if(LOG_ATTEMPT_FAILURE) {Expr;} return false;
bool RSRVoxelRectangularObstacle::AttemptPlacement(std::mt19937_64& InOutGenerator, RSRVoxalGrid& InOutGrid, const RSRush::RSRTransformMatrix& InTransformMatrix, const RSRVoxelRectangle& InSpawnArea, uint8_t InMaxAttemptCount)
{
	RSRCollidersBodyLWPair physicBodyPair(TEST_HAZARD_PLACEMENT);

	XMMATRIX trsMatrix = XMLoadFloat4x4(&InTransformMatrix.GetMatrix());

	InitPlacementData(InTransformMatrix, &trsMatrix, &InOutGrid, &physicBodyPair);

	physicBodyPair.RecomputeWorld(trsMatrix, true);

	bool bSucessfullyPlaced = false;
	RSRVoxelRectangle VoxelPlacement;

	std::uniform_int_distribution<int32_t> distType(0, 1);

	for (uint8_t nbAttempt = 0; nbAttempt < InMaxAttemptCount; ++nbAttempt)
	{
		switch (distType(InOutGenerator))
		{
		case 0 :
			bSucessfullyPlaced = AttemptHorizontalBarrierPacement(InOutGenerator, InSpawnArea, /*Out*/ VoxelPlacement);
			break;
		case 1 :
			bSucessfullyPlaced = AttemptVerticalSpirePacement(InOutGenerator, InSpawnArea, /*Out*/ VoxelPlacement);
			break;
		default:
			RSRLog::Log(LOG_ERROR, TEXT("Out of bounds result for RectangularObstacle in 'RSRVoxelRectangularObstacle::AttemptPlacement' !"));
			bSucessfullyPlaced = false;
			break;
		}

		if (bSucessfullyPlaced)
		{
			//Handle sucess
			m_voxelArea = VoxelPlacement;
			ClearPlacementData();

			return m_placed = true;
		}
	}

	m_voxelArea = RSRVoxelRectangle();
	ClearPlacementData();
	return m_placed = false;
}

bool RSRVoxelRectangularObstacle::FillVoxalGridData(RSRVoxalGrid& InOutGrid)
{
	if (!m_placed)
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot fill voxel grid data on a non-placed RSRVoxelRectangularObstacle !"));
		return false;
	}
	for (const DirectX::XMINT3& coord : m_voxelArea)
	{
		InOutGrid[coord].State = RSRVoxalGrid::VoxelData::EState::FULL;
	}
	return true;
}
bool RSRVoxelRectangularObstacle::FillVoxalPlanes(std::vector<RSRVoxalPlane>& InOutVoxalPlanes)
{
	if (!m_placed)
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot fill voxel plane data on a non-placed RSRVoxelRectangularObstacle !"));
		return false;
	}
	m_voxelArea.GeneratePlanes(InOutVoxalPlanes);
	return true;
}
bool RSRVoxelRectangularObstacle::FillAdditionalGeometry(std::vector<VertexPositionUVColor>& InOutVertices, std::vector<UINT_VRTX_IDX>& InOutTriangles)
{
	//No addition geometry needed for this type of obstacle, everything is handled by voxel and planes
	return true;
}
bool RSRVoxelRectangularObstacle::FillPhysicBodyData(RSRCollidersBody& InOutPhysicBody)
{
	if (!m_placed)
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot fill physic body data on a non-placed RSRVoxelRectangularObstacle !"));
		return false;
	}
	InOutPhysicBody.Colliders.push_back(RSRCollider());
	RSRCollider* bCol = &InOutPhysicBody.Colliders.back();

	bCol->Type = RSRColliderType::RectCollider;
	bCol->RectData =
	{
		/*.Center = */      m_voxelArea.ComputeCenterSVXL(),
		/*.Extents = */     m_voxelArea.ComputeExtendsSVXL(),
		/*.Orientation = */ /*QuatIdentity*/{0.f, 0.f, 0.f, 1.f}
	};
	return true;
}

static constexpr int32_t RO_MIN_HEIGHT_VXL = 1;
static constexpr int32_t RO_MAX_HEIGHT_VXL = 10;

static constexpr int32_t RO_MIN_DEPTH_VXL = 1;
static constexpr int32_t RO_MAX_DEPTH_VXL = 10;

static constexpr int32_t RO_MIN_WIDTH_VXL = 1;
static constexpr int32_t RO_MAX_WIDTH_VXL = 10;

bool RSRVoxelRectangularObstacle::AttemptHorizontalBarrierPacement(std::mt19937_64& InOutGenerator, const RSRVoxelRectangle& InSpawnArea, RSRVoxelRectangle& OutResultingObstacle)
{

	if (InSpawnArea.Max.x - RO_MIN_DEPTH_VXL < InSpawnArea.Min.x)
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Not enought depth to spawn a RSRVoxelRectangularObstacle")));
	}
	if (InSpawnArea.Max.z - RO_MIN_HEIGHT_VXL < InSpawnArea.Min.z)
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Not enought height to spawn a RSRVoxelRectangularObstacle")));
	}


	OutResultingObstacle =
	{
		.Min = XMINT3
		{
			/*X*/0, //Variable
			/*Y*/InSpawnArea.Min.y,
			/*Z*/0 //Variable
		},
		.Max = XMINT3
		{
			/*X*/0, //Variable
			/*Y*/InSpawnArea.Max.y,
			/*Z*/0 //Variable
		}
	};
	//X (Depth)

	std::uniform_int_distribution<int32_t> distStart(InSpawnArea.Min.x, InSpawnArea.Max.x - RO_MIN_DEPTH_VXL);
	OutResultingObstacle.Min.x = distStart(InOutGenerator);
	std::uniform_int_distribution<int32_t> distEnd(OutResultingObstacle.Min.x + RO_MIN_DEPTH_VXL, InSpawnArea.Max.x);
	OutResultingObstacle.Max.x = distEnd(InOutGenerator);

	//Z (Height)

	std::uniform_int_distribution<int32_t> distGround(InSpawnArea.Min.z, InSpawnArea.Max.z - RO_MIN_HEIGHT_VXL);
	OutResultingObstacle.Min.z = distGround(InOutGenerator);
	std::uniform_int_distribution<int32_t> distHigh(OutResultingObstacle.Min.z + RO_MIN_HEIGHT_VXL, InSpawnArea.Max.z);
	OutResultingObstacle.Max.z = distHigh(InOutGenerator);


	if (!RSRVoxalGrid::IsFree(m_apd.p_VoxalGrid->GetObstructerState(OutResultingObstacle)))
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Genereted space is voxel occupied")));
	};
	if (!AvalableVoxelSpace(OutResultingObstacle))
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Physically obstructed space")));
	}

	
	return true;
}

bool RSRush::RSRVoxelRectangularObstacle::AttemptVerticalSpirePacement(std::mt19937_64& InOutGenerator, const RSRVoxelRectangle& InSpawnArea, RSRVoxelRectangle& OutResultingObstacle)
{
	if (InSpawnArea.Max.x - RO_MIN_DEPTH_VXL < InSpawnArea.Min.x)
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Not enought depth to spawn a RSRVoxelRectangularObstacle")));
	}
	if (InSpawnArea.Max.z - RO_MIN_HEIGHT_VXL < InSpawnArea.Min.z)
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Not enought height to spawn a RSRVoxelRectangularObstacle")));
	}


	OutResultingObstacle =
	{
		.Min = XMINT3
		{
			/*X*/0, //Variable
			/*Y*/0, //Variable
			/*Z*/InSpawnArea.Min.z 
		},
		.Max = XMINT3
		{
			/*X*/0, //Variable
			/*Y*/0, //Variable
			/*Z*/0 //Variable
		}
	};
	//X (Depth)

	std::uniform_int_distribution<int32_t> distStartDepth(InSpawnArea.Min.x, InSpawnArea.Max.x - RO_MIN_DEPTH_VXL);
	OutResultingObstacle.Min.x = distStartDepth(InOutGenerator);
	std::uniform_int_distribution<int32_t> distEndDepth(OutResultingObstacle.Min.x + RO_MIN_DEPTH_VXL, InSpawnArea.Max.x);
	OutResultingObstacle.Max.x = distEndDepth(InOutGenerator);


	//Y (Side)

	std::uniform_int_distribution<int32_t> distStartWidth(InSpawnArea.Min.y, InSpawnArea.Max.y - RO_MIN_WIDTH_VXL);
	OutResultingObstacle.Min.y = distStartWidth(InOutGenerator);
	std::uniform_int_distribution<int32_t> distEndWidth(OutResultingObstacle.Min.y + RO_MIN_WIDTH_VXL, InSpawnArea.Max.y);
	OutResultingObstacle.Max.y = distEndWidth(InOutGenerator);

	//Z (Height)

	OutResultingObstacle.Min.z = InSpawnArea.Min.z;
	std::uniform_int_distribution<int32_t> distHigh(OutResultingObstacle.Min.z + RO_MIN_HEIGHT_VXL, InSpawnArea.Max.z);
	OutResultingObstacle.Max.z = distHigh(InOutGenerator);


	if (!RSRVoxalGrid::IsFree(m_apd.p_VoxalGrid->GetObstructerState(OutResultingObstacle)))
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Genereted space is voxel occupied")));
	};
	if (!AvalableVoxelSpace(OutResultingObstacle))
	{
		RETURN_FALSE_LOGIF(RSRLog::Log(LOG_DISPLAY, TEXT("Physically obstructed space")));
	}


	return true;
}
