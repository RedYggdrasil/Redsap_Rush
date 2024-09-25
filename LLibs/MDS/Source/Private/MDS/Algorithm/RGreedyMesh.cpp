#include "MDS/Algorithm/RGreedyMesh.h"

#include "MDS/Algorithm/RVoxelSurface.h"
#include "MDS/Tools/RMath.h"

#include <algorithm>

using namespace DirectX;
using namespace mds::voxel;

void mds::RGreedyMesh::GenerateSurfaceQuads(RVoxelSurface& InOutVoxalPlane, std::vector<RGreedyMesh::XMQuads>& OutQuads, const bool bInShrinkVector)
{
	//8bytes uint
	size_t sizesT2[2];
	InOutVoxalPlane.GetSizes(/*Out*/sizesT2);
	OutQuads.reserve(OutQuads.size() + std::max(InOutVoxalPlane.GetSize() / 3, (size_t)20));

	//4bytes int
	XMINT2 sizesI2{ (int32_t)sizesT2[0], (int32_t)sizesT2[1] };


	bool** mask = new bool* [sizesT2[0]];
	for (size_t i = 0; i < sizesT2[0]; ++i)
	{
		mask[i] = new bool[sizesT2[1]];
		memset(mask[i], 0, sizeof(bool) * sizesT2[1]);
	}
	const XMINT2 minPlane = InOutVoxalPlane.MinCoord;
	const XMINT2 maxPlane = InOutVoxalPlane.MaxCoord;

	auto coordToIdxX = [minPlane](int32_t coordX) -> int32_t {
		return coordX - minPlane.x;
		};
	auto coordToIdxY = [minPlane](int32_t coordY) -> int32_t {
		return coordY - minPlane.y;
		};

	for (int32_t cY = minPlane.y; cY < maxPlane.y; ++cY)
	{
		for (int32_t cX = minPlane.x; cX < maxPlane.x; ++cX)
		{
			const RTileData& td = InOutVoxalPlane[XMINT2{ cX, cY }];
			int32_t readX = coordToIdxX(cX);
			int32_t readY = coordToIdxY(cY);
			if (td.bIsShapePart && !mask[readX][readY])
			{
				int32_t dx = 1, dy = 1;
				//In this yLine, starting at cY, seach the maximum width possible
				while (cY + dy < maxPlane.y && InOutVoxalPlane[XMINT2{ cX, cY + dy }].bIsShapePart && !mask[coordToIdxX(cX)][coordToIdxY(cY + dy)])
				{
					++dy;
				}
				int32_t localMaxY = cY + dy;
				//for how many Xlines can a rectangle of this width (cy -> localMaxY) be extended ?
				for (int32_t extendX = cX + 1; extendX < maxPlane.x; ++extendX)
				{
					bool bCanExtend = true;
					for (int32_t currentY = cY; currentY < localMaxY; ++currentY)
					{
						if (!InOutVoxalPlane[XMINT2{ extendX, currentY }].bIsShapePart || mask[coordToIdxX(extendX)][coordToIdxY(currentY)])
						{
							bCanExtend = false;
							break;
						}
					}
					if (bCanExtend)
					{
						++dx;
					}
					else
					{
						break;
					}
				}
				OutQuads.push_back(
					XMQuads{
						.Min = {cX, cY},
						.SizeXY = {dx, dy}
					}
				);
				int32_t quadMaxX = cX + dx;
				int32_t quadMaxY = cY + dy;
				for (int32_t maskedX = cX; maskedX < quadMaxX; ++maskedX)
				{
					for (int32_t maskedY = cY; maskedY < quadMaxY; ++maskedY)
					{
						mask[coordToIdxX(maskedX)][coordToIdxY(maskedY)] = true;
					}
				}
			}
		}
	}

	if (bInShrinkVector)
	{
		OutQuads.shrink_to_fit();
	}

	for (size_t i = 0; i < sizesT2[0]; ++i)
	{
		delete[] mask[i];
	}
	delete[] mask;
}
