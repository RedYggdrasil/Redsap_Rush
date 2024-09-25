#pragma once
#include "MDS/Tools/RMathCollection.h"

#include <unordered_set>
#include <unordered_map>
#include <DirectXMath.h>

namespace mds
{
	namespace voxel
	{
		struct RTileData
		{
		public:
			bool bIsShapePart;
		};
		static constexpr RTileData OUT_OF_RANGE{ .bIsShapePart = false };
		struct RVoxelSurface
		{
		public:
			uint32_t Gobal_VXL_ToLocal;
			uint8_t DepthLevel;
			std::unordered_map<DirectX::XMINT2, RTileData, DirectX::XMINT2Hash, DirectX::XMINT2Equal> CoordToTileData;
			///<summary>Inclusive</summary>
			DirectX::XMINT2 MinCoord;
			///<summary>Exclusive</summary>
			DirectX::XMINT2 MaxCoord;
			inline const RTileData& operator [](const DirectX::XMINT2& InCoord) const
			{ return ((CoordToTileData.contains(InCoord)) ? CoordToTileData.at(InCoord) : OUT_OF_RANGE); }
		public:
			inline void GetSizes(size_t OutSizes[2]) const { OutSizes[0] = (size_t)(MaxCoord.x - MinCoord.x), OutSizes[1] = (size_t)(MaxCoord.y - MinCoord.y); };
			inline size_t GetSize() const { return (size_t)(MaxCoord.x - MinCoord.x) * (size_t)(MaxCoord.y - MinCoord.y); };

			struct RVoxelSurfaceData
			{
				size_t EffectiveShapePartCount = 0;
				///<summary>Inclusive</summary>
				DirectX::XMINT2 EffectiveMinCoord;
				///<summary>Exclusive</summary>
				DirectX::XMINT2 EffectiveMaxCoord;
			};
			RVoxelSurfaceData ComputeVoxelSurfaceData() const;

			void FillShapePartsSet(std::unordered_set<DirectX::XMINT2, DirectX::XMINT2Hash, DirectX::XMINT2Equal>& InOutPartsSet) const;
		};
	};
};