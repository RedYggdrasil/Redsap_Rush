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
#include "App/Managers/RSRTrenchManager.h"

#include <array>
#include <unordered_map>
#include <Tracy.hpp>
#include "MDS/Threads/RTaskFunction.h"
#include "MDS/Threads/RTaskManager.h"
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


RSRush::RSRVoxalTrench::RSRVoxalTrench(RSRTrenchManager* InTrenchManager, const std::vector<uint16_t>& InSideGreedbleTextureIDs, const std::vector<uint16_t>& InTopGreedbleTextureIDs)
:Super(InTrenchManager), m_grid(),
m_sideGreedbleTextureIDs(InSideGreedbleTextureIDs), m_topGreedbleTextureIDs(InTopGreedbleTextureIDs)
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
	public:
		inline XMFLOAT2 NormUVToTVerticeUV(const XMFLOAT2& InNormUV) const { return InNormUV; }
		VertexPositionUVColor TVertexFromNormVertAndSurNormCoord(const XMFLOAT3& InNormal, const XMFLOAT3& InVertPos, const XMFLOAT2& InNormPos, const uint16_t InTextureID)
		{
			return
			{
				.pos = InVertPos,
				.normal = InNormal,
				.color = DefaultColor,
				.uv0 = NormUVToTVerticeUV(InNormPos),
				.textureIndex = InTextureID
			};
		}
	};

	{
		ZoneScopedN("VoxalTrench Greeble");
		size_t cPlanes = planes.size();
		size_t cPlanesMinus = planes.size();
		std::atomic<size_t> doneTasks = 0;
		std::vector<std::shared_ptr<mds::RTask>> greebleSurfaceTasks;
		greebleSurfaceTasks.reserve(cPlanes);

		std::vector<std::vector<VertexPositionUVColor>> verticesPerPlanes(cPlanes, std::vector<VertexPositionUVColor>());
		std::vector<std::vector<UINT_VRTX_IDX>> tiranglesPerPlanes(cPlanes, std::vector<UINT_VRTX_IDX>());


		for (size_t i = 0; i < cPlanes; ++i)
		{			
			if (true)
			{
				greebleSurfaceTasks.push_back
				(std::make_shared<mds::RTaskFunction>
					(
						[
							pPlane = &planes[i], pVertices = &verticesPerPlanes[i], pTriangles = &tiranglesPerPlanes[i], 
							pGrid = &this->m_grid, DEFAULT_COLOR, instID = this->m_instanceID,
							pSideGreedbleTextureIDs = &this->m_sideGreedbleTextureIDs, pTopGreedbleTextureIDs = &this->m_topGreedbleTextureIDs
						]()->void
						{
							ZoneScopedN("VoxalTrench Greeble Plane");
							VertexProvider vertProv
							{
								.SurfaceNormal = NORMAL(pPlane->GetDirection()),
								.DefaultColor = DEFAULT_COLOR
							};
							XMFLOAT4X4 transposeTranslate = pPlane->GetSVXLTranslateTransformMatrix();

							bool bIsClockwise = !mds::RMath::GetIsDrawClockwise(NORMAL(pPlane->GetDirection()), transposeTranslate);
							mds::NSRGreeble::RGreeble<VertexPositionUVColor, VertexProvider, UINT_VRTX_IDX> greeble(vertProv);

							greeble.Generate
							(
								RSRRandomizer::Get().GetTrenchGenerator(instID),
								pPlane->GenerateRVoxelSurface(), transposeTranslate,
								NORMAL(pPlane->GetDirection()), bIsClockwise,
								*pSideGreedbleTextureIDs, *pTopGreedbleTextureIDs,
								*pVertices, *pTriangles
							);
							pGrid->FillPlaneInfos(pPlane);
						},
						[&doneTasks](std::shared_ptr<mds::RTask> InTask)->void
						{
							doneTasks.fetch_add(1, std::memory_order_relaxed);
						}
					)
				);
				//Use this instead to run on current thread
				//greebleSurfaceTasks.back()->operator()();
			}
		}


		mds::RTaskManager::Get().EnqueueTasks(greebleSurfaceTasks);
		
		while (doneTasks.load(std::memory_order_relaxed) < cPlanes)
		{
			// Busy-wait loop
			std::this_thread::yield();
		}

		//Initial Offsets
		size_t offsetVerts = vertices.size();
		size_t offsetTris = triangles.size();


		//resize vectors
		size_t cVertToAdd = 0;
		size_t cTrisToAdd = 0;
		for (size_t i = 0; i < cPlanes; ++i)
		{
			cVertToAdd += verticesPerPlanes[i].size();
			cTrisToAdd += tiranglesPerPlanes[i].size();
		}
		vertices.resize(vertices.size() + cVertToAdd);
		triangles.resize(triangles.size() + cTrisToAdd);

		//Copy data to end vector and fix triangle indexes
		for (size_t i = 0; i < cPlanes; ++i)
		{
			std::memcpy(vertices.data() + offsetVerts, verticesPerPlanes[i].data(), verticesPerPlanes[i].size() * sizeof(VertexPositionUVColor));
			std::memcpy(triangles.data() + offsetTris, tiranglesPerPlanes[i].data(), tiranglesPerPlanes[i].size() * sizeof(RSRush::UINT_VRTX_IDX));

			size_t newTrianglesSize = offsetTris + tiranglesPerPlanes[i].size();
			RSRush::UINT_VRTX_IDX typedOffsetVerts = (RSRush::UINT_VRTX_IDX)offsetVerts;

			for (size_t triangleIDX = offsetTris; triangleIDX < newTrianglesSize; ++triangleIDX)
			{
				triangles[triangleIDX] = triangles[triangleIDX] + typedOffsetVerts;
			}

			offsetVerts += verticesPerPlanes[i].size();
			offsetTris += tiranglesPerPlanes[i].size() /*(newTrianglesSize)*/;
		}
		//verticesPerPlanes.clear();
		//tiranglesPerPlanes.clear();
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
	static std::uniform_int_distribution<> dist(0, 12);
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
