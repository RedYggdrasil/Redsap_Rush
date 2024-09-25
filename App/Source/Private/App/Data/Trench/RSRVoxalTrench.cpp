#include "App/Data/Trench/RSRVoxalTrench.h"

#include "MDS/Algorithm/RGreeble.h"

#include "App/Managers/RSRAssetManager.h"
//
//#include "App/Game/RSRPlayerPath.h"
#include "MDS/Tools/RMathCollection.h"
#include "MDS/RTool.h"

#include "App/Data/Trench/RSRVoxalPlane.h"
#include "App/Data/Trench/Tools/RSRVoxelTools.h"
#include "App/System/RSRRandomizer.h"
#include "App/Tools/RSRLog.h"
#include "App/Data/Trench/Obstacles/RSRVoxelObstacle.h"

#include <array>
#include <unordered_map>
#include <Tracy.hpp>
//
//#include <format>
//
//#include <array>
//#include <algorithm>
//#include <utility>
//#include <DirectXMath.h>
//#include <random>

using namespace DirectX;
using namespace RSRush;
using namespace RSRush::RSRTrenchDefs;

using Super = RSRBasicTrench;

RSRVoxalTrench::RSRVoxalTrench()
	:Super(), m_grid()
{
}

RSRVoxalTrench::~RSRVoxalTrench()
{
}

void RSRush::RSRVoxalTrench::PositionSelf()
{
	Super::PositionSelf();
	m_grid.Init
	(
		XMINT3(-HALF_TRENCH_LENGHT_VXL, -HALF_TOTAL_WIDTH_VXL, UNDER_MESH_VXL),
		XMINT3(+HALF_TRENCH_LENGHT_VXL, +HALF_TOTAL_WIDTH_VXL, ROOF_HEIGHT_VXL)
	);
}

void RSRush::RSRVoxalTrench::GeneratingMeshThreadSensitive()
{
	GenerateObstacles();
	RSRBasicTrench::GeneratingMeshThreadSensitive();
}

void RSRVoxalTrench::GenerateGeometry()
{
	ZoneScopedN("VoxalTrench GenerateGeometry");

	std::mt19937_64 gen = RSRRandomizer::Get().GetTrenchGenerator(m_instanceID);
	static std::uniform_int_distribution<> dist(0, 3);
	uint16_t UseTextureIndex = dist(gen);

	XMFLOAT3 DEFAULT_COLOR = XMFLOAT3(1.f, 1.f, 1.f);

	std::array FUv0 = std::to_array<XMFLOAT2>(
		{
			//F00
			{0.f, 0.f},
			//F01
			{1.f, 0.f},
			//F02
			{1.f, 1.f},
			//F03
			{ 0.f, 1.f }
		});

	for (const DirectX::XMINT3& coord : BASIC_UNDER_VOLUME)
	{
		m_grid[coord].State = RSRVoxalGrid::VoxelData::EState::FULL;
	}
	for (const DirectX::XMINT3& coord : BASIC_LEFT_TRENCH)
	{
		m_grid[coord].State = RSRVoxalGrid::VoxelData::EState::FULL;
	}
	for (const DirectX::XMINT3& coord : BASIC_RIGHT_TRENCH)
	{
		m_grid[coord].State = RSRVoxalGrid::VoxelData::EState::FULL;
	}

	for (auto& obstacle : m_obstacles)
	{
		obstacle->FillVoxalGridData(m_grid);
	}

	std::vector<RSRVoxalPlane> planes;
	planes.reserve(6 * 3);
	BASIC_UNDER_VOLUME.GeneratePlanes(planes);
	BASIC_LEFT_TRENCH.GeneratePlanes(planes);
	BASIC_RIGHT_TRENCH.GeneratePlanes(planes);

	constexpr ESurfaceDirection ExcludedDirections = ESurfaceDirection::FRWRD | ESurfaceDirection::BACK;
	for (size_t i = planes.size() - 1; ; --i)
	{
		if (any(planes[i].GetDirection() & ExcludedDirections))
		{
			planes.erase(planes.begin() + i);
		}
		if (i == (size_t)0) { break; }
	}

	for (auto& obstacle : m_obstacles)
	{
		obstacle->FillVoxalPlanes(planes);
	}

	for (RSRVoxalPlane& plane : planes)
	{
		m_grid.FillPlaneInfos(&plane);
	}

	SubdividePlanes(/*InOut*/planes);

	//std::vector<RSRVoxelTools::XMQuads> meshQuads;
	//
	//
	//struct QuadPlainAssociation
	//{
	//public:
	//	size_t planeIdx;
	//	size_t startIdxInArr;
	//	size_t endIdxInArr;
	//	std::unordered_map<XMINT2, UINT_VRTX_IDX, XMINT2Hash, XMINT2Equal> posToVerticeIdx;
	//};
	//std::vector<QuadPlainAssociation> quadPlainAssociation(planes.size());


	std::vector<VertexPositionUVColor> vertices;

	std::vector<UINT_VRTX_IDX> triangles;

	struct VertexProvider
	{
		XMFLOAT3 SurfaceNormal;
		XMFLOAT3 DefaultColor;
		uint32_t DefaultTextureIdx;
	public:
		inline XMFLOAT2 NormUVToTVerticeUV(const XMFLOAT2& InNormUV) const { return InNormUV; }
		VertexPositionUVColor TVertexFromNormVertAndSurNormCoord(const XMFLOAT3& InNormal, const XMFLOAT3& InVertPos, const XMFLOAT2& InNormPos)
		{
			return
			{
				.pos = InVertPos,
				.normal = InNormal,
				.color = DefaultColor,
				.uv0 = NormUVToTVerticeUV(InNormPos),
				.textureIndex = DefaultTextureIdx
			};
		}
	};


	{
		ZoneScopedN("VoxalTrench Greeble");
		for (RSRVoxalPlane& plane : planes)
		{
			VertexProvider vertProv
			{
				.SurfaceNormal = NORMAL(plane.GetDirection()),
				.DefaultColor = DEFAULT_COLOR,
				.DefaultTextureIdx = UseTextureIndex
			};
			//int32_t depthValue = plane.GetDefaultDepth();
			//mds::RTransform trs;
			//XMINT3 coord3D = plane.GetCoord3(InCoord2D);
			XMFLOAT4X4 transposeTranslate = plane.GetSVXLTranslateTransformMatrix();

			bool bIsClockwise = !mds::RMath::GetIsDrawClockwise(NORMAL(plane.GetDirection()), transposeTranslate);
			mds::NSRGreeble::RGreeble<VertexPositionUVColor, VertexProvider, UINT_VRTX_IDX> greeble(vertProv);

			greeble.Generate(RSRRandomizer::Get().GetTrenchGenerator(m_instanceID), plane.GenerateRVoxelSurface(), transposeTranslate, NORMAL(plane.GetDirection()), bIsClockwise, vertices, triangles);
			m_grid.FillPlaneInfos(&plane);
		}
	}

	for (auto& obstacle : m_obstacles)
	{
		obstacle->FillAdditionalGeometry(vertices, triangles);
	}

	RSRSharedMesh3DPtr voxalTrenchMesh = RSRAssetManager::Get().AddAsset<RSRMesh3D>
		(
			mds::NameDynamicAsset(mds::RAssetType::Mesh, std::format("Trench{}_Basic", m_instanceID)),
			false,
			vertices,
			triangles
			);
	m_mainMesh.Insert(voxalTrenchMesh, FLT_MAX);
	//RSRLog::Log(LOG_DISPLAY, TEXT("Generated {} quads"), meshQuads.size());
	//size_t quadI = 0;
	//for (auto quad : meshQuads)
	//{
	//	if (false)
	//	{
	//		RSRLog::Log(LOG_DISPLAY, TEXT("Quad [{}] : Min : [{}], Size : [{}]"), quadI, mds::_tostring(quad.Min), mds::_tostring(quad.SizeXY));
	//	}
	//	++quadI;
	//}
}

void RSRVoxalTrench::GeneratePhysicColliders()
{
	RSRCollidersBody& localBody = this->m_localColliders;
	size_t startIDX = localBody.Colliders.size();
	//Probably at least 1 collider per obstacle
	localBody.Colliders.reserve(startIDX + m_obstacles.size());
	bool bCumulativeSucess = true;
	for (auto& obstacle : m_obstacles)
	{
		bCumulativeSucess = obstacle->FillPhysicBodyData(localBody) && bCumulativeSucess;
	}
	Super::GeneratePhysicColliders();
}

void PushBackObstacle(std::vector<std::unique_ptr<RSRVoxelObstacle>>& InOutObstacleList, std::mt19937_64& InOutGenerator)
{
	InOutObstacleList.push_back(std::unique_ptr<RSRVoxelObstacle>(RSRVoxelObstacle::HeapCreateNewObstacle(&InOutGenerator)));
}


size_t RSRush::RSRVoxalTrench::GenerateObstacles()
{
	ZoneScopedN("VoxalTrench GenerateObstacles");
	std::mt19937_64 gen = RSRRandomizer::Get().GetTrenchGenerator(m_instanceID);
	static std::uniform_int_distribution<> dist(0, 3);
	size_t nbObstaclesMax = dist(gen);

	m_obstacles.clear();
	m_obstacles.reserve(nbObstaclesMax);

	size_t sucessfullyPlaced = 0;

	for (size_t i = 0; i < nbObstaclesMax; ++i)
	{
		PushBackObstacle(/*InOut*/m_obstacles, /*InOut*/gen);

		if (m_obstacles.back()->AttemptPlacement(gen,m_grid, m_mainTransform, 10U))
		{
			++sucessfullyPlaced;
		}
		else 
		{
			m_obstacles.pop_back();
		}
	}

	m_obstacles.shrink_to_fit();
	return sucessfullyPlaced;
}

void RSRush::RSRVoxalTrench::SubdividePlanes(std::vector<RSRVoxalPlane>& InOutPlanes)
{
	//To Implement to get smaller, more intrestingly shaped planes.
	return;
}

struct RSRTileData
{
public:
	bool IsShapePart;
};
struct RSRSurfaceShape
{
public:
	uint32_t Gobal_VXL_ToLocal;
	uint8_t DepthLevel;
	std::unordered_map<XMINT2, RSRTileData, XMINT2Hash, XMINT2Equal> CoordToTileData;
	XMINT2 MinCoord;
	XMINT2 MaxCoord;
};

void RSRush::RSRVoxalTrench::GeneratePlaneGeometry(RSRVoxalPlane& InOutPlane, std::vector<VertexPositionUVColor>& InOutVertices, std::vector<UINT_VRTX_IDX>& InOutTirangles)
{
	RSRSurfaceShape rootSurfaceShape
	{
		.Gobal_VXL_ToLocal = 1,
		.DepthLevel = 0,
		.MinCoord = InOutPlane.Min(),
		.MaxCoord = InOutPlane.Max()
	};
	rootSurfaceShape.CoordToTileData.reserve(InOutPlane.GetSize());
	for (XMINT2 coord : InOutPlane)
	{
		rootSurfaceShape.CoordToTileData.insert({ coord, RSRTileData{.IsShapePart = InOutPlane[coord].Free()} });
	}
	return;
}
