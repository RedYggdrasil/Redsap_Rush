#include "MDS/Algorithm/RVoxelSurface.h"

using namespace DirectX;
using namespace mds::voxel;
RVoxelSurface::RVoxelSurfaceData RVoxelSurface::ComputeVoxelSurfaceData() const
{
	RVoxelSurface::RVoxelSurfaceData result;

	result.EffectiveShapePartCount = 0;
	result.EffectiveMinCoord = this->MaxCoord;
	result.EffectiveMaxCoord = this->MinCoord;

	bool bHasAnyTiles = false;
	for (const auto& tilePair : this->CoordToTileData)
	{
		if (tilePair.second.bIsShapePart)
		{
			++result.EffectiveShapePartCount;
			result.EffectiveMaxCoord.x = std::max(result.EffectiveMaxCoord.x, (tilePair.first.x + 1));
			result.EffectiveMaxCoord.y = std::max(result.EffectiveMaxCoord.y, (tilePair.first.y + 1));
			
			result.EffectiveMinCoord.x = std::min(result.EffectiveMinCoord.x, tilePair.first.x);
			result.EffectiveMinCoord.y = std::min(result.EffectiveMinCoord.y, tilePair.first.y);
		}
	}
	if (result.EffectiveShapePartCount < 1)
	{
		result.EffectiveMaxCoord = result.EffectiveMinCoord = this->MinCoord;
	}
	return result;
}

void mds::voxel::RVoxelSurface::FillShapePartsSet(std::unordered_set<DirectX::XMINT2, DirectX::XMINT2Hash, DirectX::XMINT2Equal>& InOutPartsSet) const
{
	size_t tilesCount = CoordToTileData.size();
	InOutPartsSet.reserve(InOutPartsSet.size() + tilesCount);

	for (const auto& tilePair : this->CoordToTileData)
	{
		InOutPartsSet.insert(tilePair.first);
	}
}
