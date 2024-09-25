#pragma once
#include <DirectXMath.h>
#include <vector>

namespace mds
{
	namespace voxel
	{
		struct RVoxelSurface;
	};

	class RGreedyMesh
	{
	public:
		struct XMQuads
		{
			DirectX::XMINT2 Min;
			DirectX::XMINT2 SizeXY;
		};
	public:
		static void GenerateSurfaceQuads(voxel::RVoxelSurface& InOutVoxalPlane, std::vector<RGreedyMesh::XMQuads>& OutQuads, const bool bInShrinkVector = true);
	};
};