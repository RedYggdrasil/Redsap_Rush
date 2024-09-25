#pragma once
#include "DirectXMath.h"
#include <vector>

namespace RSRush
{
	class RSRVoxalPlane;

	class RSRVoxelTools
	{
	public:
		struct XMQuads
		{
			DirectX::XMINT2 Min;
			DirectX::XMINT2 SizeXY;
		};
	public:
		static void GreedyMesh(RSRVoxalPlane* InOutVoxalPlane, std::vector<RSRVoxelTools::XMQuads>& OutQuads, const bool bInShrinkVector = true);
	};
};