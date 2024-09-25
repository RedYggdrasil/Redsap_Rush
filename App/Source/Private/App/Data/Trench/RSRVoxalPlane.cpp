#include "App/Data/Trench/RSRVoxalPlane.h"
#include "App/Tools/RSRLog.h"

using namespace DirectX;
using namespace RSRush;
using namespace RSRush::RSRTrenchDefs;

RSRVoxalPlane::RSRVoxalPlane()
:m_initialized(false), m_surfaceDirection(ESurfaceDirection::NONE), m_defaultDepth(0)
{
}

RSRVoxalPlane::RSRVoxalPlane(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir)
:m_initialized(false), m_surfaceDirection(ESurfaceDirection::NONE), m_defaultDepth(0)
{
	Init(InMin, InMax, EinSurfaceDir);
}

RSRush::RSRVoxalPlane::RSRVoxalPlane(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir, const int32_t InDefaultDepth)
:m_initialized(false), m_surfaceDirection(ESurfaceDirection::NONE), m_defaultDepth(0)
{
	Init(InMin, InMax, EinSurfaceDir, InDefaultDepth);
}

RSRVoxalPlane::RSRVoxalPlane(RSRVoxalPlane&& ROther) noexcept
:m_initialized(ROther.m_initialized), m_surfaceDirection(ROther.m_surfaceDirection),
m_sizes(), m_minCoord(ROther.m_minCoord), m_defaultDepth(ROther.m_defaultDepth),
m_tiles(std::move(ROther.m_tiles))
{
	std::move(std::begin(ROther.m_sizes), std::end(ROther.m_sizes), std::begin(m_sizes));
}

RSRVoxalPlane& RSRVoxalPlane::operator=(RSRVoxalPlane&& ROther) noexcept
{
	if (this != &ROther) 
	{
		m_initialized = ROther.m_initialized;
		m_surfaceDirection = ROther.m_surfaceDirection;
		m_minCoord = ROther.m_minCoord;
		m_defaultDepth = ROther.m_defaultDepth;
		
		m_tiles = std::move(ROther.m_tiles);
		std::move(std::begin(ROther.m_sizes), std::end(ROther.m_sizes), std::begin(m_sizes));
	}
	return *this;
}

bool RSRVoxalPlane::Init(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir, const int32_t InDefaultDepth)
{
	if (m_initialized)
	{
		RSRLog::Log(LOG_WARNING, TEXT("RSRVoxalGrid is being reinitialized !)"));
		DeInit();
	}

	if (InMax.x <= InMin.x || InMax.y <= InMin.y)
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot Intialize RSRVoxalPlane with Min >= Max ! (Min : '{}', Max : '{}')"), mds::_tostring(InMin), mds::_tostring(InMax));
		return m_initialized;
	}

	m_minCoord = InMin;

	m_surfaceDirection = EinSurfaceDir;
	m_defaultDepth = InDefaultDepth;

	m_sizes[0] = InMax.x - InMin.x;
	m_sizes[1] = InMax.y - InMin.y;
	size_t totalSize = m_sizes[0] * m_sizes[1];
	m_tiles.resize(totalSize);

	m_initialized = true;
	return m_initialized;
}

void RSRVoxalPlane::DeInit(bool bInFromDestructor)
{
	if (!bInFromDestructor)
	{
		m_tiles.clear();
		m_minCoord = { 0, 0 };
		std::fill<size_t*, size_t>(m_sizes, m_sizes + _countof(m_sizes), 0);
	}
	m_initialized = false;
}

RSRVoxalPlane::Normalizer RSRush::RSRVoxalPlane::CreateNormalizer() const
{
	XMFLOAT2 minFlt = XMFLOAT2 { (float)m_minCoord.x, (float)m_minCoord.y };
	XMFLOAT2 sizeFlt = XMFLOAT2{ (float)m_sizes[0], (float)m_sizes[1] };

	return Normalizer
	{
		.Min = XMLoadFloat2(&minFlt),
		.Size = XMLoadFloat2(&sizeFlt)
	};
}

DirectX::XMINT3 RSRush::RSRVoxalPlane::GetCoord3(const DirectX::XMINT2& InLocalCoord, int32_t InDepthValue) const
{
	return To3D(InLocalCoord, InDepthValue, m_surfaceDirection);
}

DirectX::XMFLOAT4X4 RSRush::RSRVoxalPlane::GetTranslateTransformMatrix(int32_t InDepthValue) const
{
	return TRANSPOSITION_DEPTH(m_surfaceDirection, (float)InDepthValue);
}

DirectX::XMFLOAT4X4 RSRush::RSRVoxalPlane::GetTranslateTransformMatrix(float InDepthValue) const
{
	return TRANSPOSITION_DEPTH(m_surfaceDirection, InDepthValue);
}

DirectX::XMFLOAT4X4 RSRush::RSRVoxalPlane::GetSVXLTranslateTransformMatrix(int32_t InDepthValue) const
{
	return mds::RMath::ScaleMatrixAxis(GetTranslateTransformMatrix(SVXL(InDepthValue)), VOXEL_SIZE_3D, false);
}


RSRVoxalPlane::~RSRVoxalPlane()
{
	if (m_initialized)
	{
		DeInit(true);
	}
}

mds::voxel::RVoxelSurface RSRush::RSRVoxalPlane::GenerateRVoxelSurface() const
{
	mds::voxel::RVoxelSurface result;
	result.Gobal_VXL_ToLocal = 1;
	result.DepthLevel = 0;
	result.CoordToTileData.reserve(this->m_tiles.size());

	for (const XMINT2& tile : *this)
	{
		const TileData& tileData = (*this)[tile];
		if (tileData.Free())
		{
			result.CoordToTileData.insert({ tile, mds::voxel::RTileData{.bIsShapePart = tileData.Free()} });
		}
	}

	result.MinCoord = this->Min();
	result.MaxCoord = this->Max();

	return result;
}

bool RSRVoxalPlane::UNIT_TEST_CLASS()
{
	TileData* pTileEntry = nullptr;
	RSRVoxalPlane planeOne = RSRVoxalPlane({ -3, -1 }, { -1, 2 }, ESurfaceDirection::FRWRD);

	assert(&planeOne.m_tiles[0] == &planeOne[-3][-1] && "[UT][RSRVoxalGrid][0] Error with Indexed acess !");
	assert(&planeOne.m_tiles[1] == &planeOne[-3][0] && "[UT][RSRVoxalGrid][1] Error with Indexed acess !");

	assert(&planeOne.m_tiles[planeOne.CoordToIdx({ -3, 0})] == &planeOne[-3][0] && "[UT][RSRVoxalGrid][2] Error with Indexed acess !");

	assert(&planeOne.m_tiles[planeOne.m_tiles.size() - 1] == &planeOne[-2][1] && "[UT][RSRVoxalGrid][3] Error with Indexed acess !");
	pTileEntry = &planeOne[{ -2, 1 }];
	assert(&planeOne.m_tiles[planeOne.m_tiles.size() - 1] == pTileEntry && "[UT][RSRVoxalGrid][4] Error with Indexed acess !");

	return true;
}
