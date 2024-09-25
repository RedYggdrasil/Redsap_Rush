#include "App/Data/Trench/RSRVoxalGrid.h"

#include "MDS/Tools/RMath.h"
#include "App/Data/Trench/RSRVoxalPlane.h"
#include "App/Tools/RSRLog.h"

using namespace DirectX;
using namespace RSRush;
using namespace RSRush::RSRTrenchDefs;

RSRVoxalGrid::RSRVoxalGrid()
:m_initialized(false)
{
}

RSRVoxalGrid::RSRVoxalGrid(const XMINT3 InMin, const XMINT3 InMax)
:m_initialized(false) 
{
	Init(InMin, InMax);
}

bool RSRVoxalGrid::Init(const XMINT3 InMin, const XMINT3 InMax)
{
	if (m_initialized)
	{
		RSRLog::Log(LOG_WARNING, TEXT("RSRVoxalGrid is being reinitialized !)"));
		DeInit();
	}

	if (InMax.x <= InMin.x || InMax.y <= InMin.y || InMax.z <= InMin.z)
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot Intialize RSRVoxalGrid with Min >= Max ! (Min : '{}', Max : '{}')"), mds::_tostring(InMin), mds::_tostring(InMax));
		return m_initialized;
	}

	m_minCoord = InMin;

	m_sizes[0] = InMax.x - InMin.x;
	m_sizes[1] = InMax.y - InMin.y;
	m_sizes[2] = InMax.z - InMin.z;
	size_t totalSize = m_sizes[0] * m_sizes[1] * m_sizes[2];
	m_voxels.resize(totalSize);


	m_initialized = true;
	return m_initialized;
}

void RSRVoxalGrid::DeInit(bool bInFromDestructor)
{
	if (!bInFromDestructor)
	{
		m_voxels.clear();
		m_minCoord = { 0, 0, 0 };
		std::fill<size_t*, size_t>(m_sizes, m_sizes + _countof(m_sizes), 0);
	}
	m_initialized = false;
}

RSRVoxalGrid::~RSRVoxalGrid()
{
	if (m_initialized)
	{
		DeInit(true);
	}
}

bool RSRVoxalGrid::FillPlaneInfos(RSRVoxalPlane* InOutVoxalPlane, const int32_t InPlaneDepth) const
{
	constexpr auto IsObstructed = [] (RSRVoxalGrid::VoxelData::EState EInState) -> bool {
		return EInState == RSRVoxalGrid::VoxelData::EState::FULL;
	};
	const XMINT3 l_min = this->m_minCoord;
	const XMINT3 l_max = this->Max();

	auto InRange = [l_min, l_max](const XMINT3& InCoord ) -> bool {
		return 
			InCoord.x >= l_min.x && InCoord.y >= l_min.y && InCoord.z >= l_min.z &&
			InCoord.x < l_max.x && InCoord.y < l_max.y && InCoord.z < l_max.z;
		};
	//This should inform the plane's tile of obstruction (don't draw)
	if (!InOutVoxalPlane || !InOutVoxalPlane->GetIsInitialized())
	{
		RSRLog::Log(LOG_ERROR, TEXT("RSRVoxalGrid Cannot fill infos for invalid RSRVoxalPlane !)"));
		return false;
	}

	ESurfaceDirection dir = InOutVoxalPlane->GetDirection();
	XMINT3 dirIncr = DIRECTION(dir);

	for (const XMINT2& coord : *InOutVoxalPlane)
	{
		XMINT3 coord3d = InOutVoxalPlane->GetCoord3(coord, InPlaneDepth);
		XMINT3 OuterVoxel = mds::unoptimized_add3(coord3d, dirIncr);
		if (InRange(OuterVoxel))
		{
			InOutVoxalPlane->operator[](coord).bObstructed = IsObstructed(this->operator[](OuterVoxel).State);
		}
		else
		{
			InOutVoxalPlane->operator[](coord).bObstructed = false;
		}
	}

	return true;
}

bool RSRVoxalGrid::FillPlaneInfos(RSRVoxalPlane* InOutVoxalPlane) const
{
	return  FillPlaneInfos(InOutVoxalPlane, InOutVoxalPlane->GetDefaultDepth());
}

bool RSRVoxalGrid::UNIT_TEST_CLASS()
{
	VoxelData* pGridEntry = nullptr;
	RSRVoxalGrid gridOne = RSRVoxalGrid({ -3, -1, 4 }, { -1, 2, 12 });

	assert(&gridOne.m_voxels[0] == &gridOne[-3][-1][4] && "[UT][RSRVoxalGrid][0] Error with Indexed acess !");
	assert(&gridOne.m_voxels[1] == &gridOne[-3][-1][5] && "[UT][RSRVoxalGrid][1] Error with Indexed acess !");

	assert(&gridOne.m_voxels[gridOne.CoordToIdx({ -3, 0, 4 })] == &gridOne[-3][0][4] && "[UT][RSRVoxalGrid][2] Error with Indexed acess !");

	assert(&gridOne.m_voxels[gridOne.m_voxels.size() - 1] == &gridOne[-2][1][11] && "[UT][RSRVoxalGrid][3] Error with Indexed acess !");
	pGridEntry = &gridOne[{ -2, 1, 11 }];
	assert(&gridOne.m_voxels[gridOne.m_voxels.size() - 1] == pGridEntry && "[UT][RSRVoxalGrid][4] Error with Indexed acess !");

	return true;
}

std::vector<RSRVoxalPlane> RSRVoxelRectangle::GeneratePlanes() const
{
	std::vector<RSRVoxalPlane> planes;
	GeneratePlanes(planes);
	return planes;
}

void RSRVoxelRectangle::GeneratePlanes(std::vector<RSRVoxalPlane>& InOutVector) const
{
	
	size_t idx = InOutVector.size();
	InOutVector.resize(idx + 6);

	InOutVector[idx + 0].Init(mds::yz(Min), mds::yz(Max), ESurfaceDirection::BACK, Min.x);
	InOutVector[idx + 1].Init(mds::yz(Min), mds::yz(Max), ESurfaceDirection::FRWRD, Max.x);

	InOutVector[idx + 2].Init(mds::xz(Min), mds::xz(Max), ESurfaceDirection::LEFT, Min.y);
	InOutVector[idx + 3].Init(mds::xz(Min), mds::xz(Max), ESurfaceDirection::RIGHT, Max.y);

	InOutVector[idx + 4].Init(mds::xy(Min), mds::xy(Max), ESurfaceDirection::DOWN, Min.z);
	InOutVector[idx + 5].Init(mds::xy(Min), mds::xy(Max), ESurfaceDirection::UP, Max.z);
}

XMFLOAT3 RSRush::RSRVoxelRectangle::ComputeCenterSVXL() const
{
	XMFLOAT3 minFlt = SVXL(this->Min);
	XMFLOAT3 maxFlt = SVXL(this->Max);
	XMVECTOR minPoint = XMLoadFloat3(&minFlt);
	XMVECTOR maxPoint = XMLoadFloat3(&maxFlt);
	XMVECTOR MidPoint = minPoint + ((maxPoint - minPoint) * 0.5f);

	XMFLOAT3 result;
	XMStoreFloat3(&result, MidPoint);
	return result;
}

XMFLOAT3 RSRush::RSRVoxelRectangle::ComputeExtendsSVXL() const
{
	XMFLOAT3 minFlt = SVXL(this->Min);
	XMFLOAT3 maxFlt = SVXL(this->Max); 
	XMVECTOR minPoint = XMLoadFloat3(&minFlt);
	XMVECTOR maxPoint = XMLoadFloat3(&maxFlt);
	XMVECTOR extends = ((maxPoint - minPoint) * 0.5f);

	XMFLOAT3 result;
	XMStoreFloat3(&result, extends);
	return result;
}
