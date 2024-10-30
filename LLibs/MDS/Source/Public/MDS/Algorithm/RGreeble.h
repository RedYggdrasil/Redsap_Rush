#pragma once
//#define DEVELOPPING_RGREEBLE

#include "DirectXMath.h"
#include "MDS/Tools/RMath.h"
#include "MDS/Tools/Math/RMath2D.h"
#include "MDS/Tools/RMathCollection.h"
#include "MDS/Tools/RTransform.h"
#include "MDS/Algorithm/RVoxelSurface.h"
#include "MDS/Algorithm/RGreedyMesh.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <random>
#include <unordered_set>

namespace mds
{
	namespace NSRGreeble
	{
		using namespace DirectX;
		using namespace mds::voxel;
//#define DEVELOPPING_RGREEBLE

#ifdef DEVELOPPING_RGREEBLE

		using TUintVertIdx = unsigned short;
		//Exemple Vertex Structure and VertexProvider Structure to create a RGreeble, used for developpement and demonstration purpuses
		struct TVertex
		{
			XMFLOAT3 pos;
			XMFLOAT3 normal;
			XMFLOAT3 color;
			XMFLOAT2 uv0;

			uint32_t textureIndex;
		};
		struct TVertexProvider
		{
			XMFLOAT3 SurfaceNormal;
			XMFLOAT3 DefaultColor;

		public:
			inline XMFLOAT2 NormUVToTVerticeUV(const XMFLOAT2& InNormUV) const { return InNormUV; }
			TVertex TVertexFromNormVertAndSurNormCoord(const XMFLOAT3& InNormal, const XMFLOAT3& InVertPos, const XMFLOAT2& InNormPos)
			{
				return
				{
					.pos = InVertPos,
					.normal = InNormal,
					.color = DefaultColor,
					.uv0 = NormUVToTVerticeUV(InNormPos)
				};
			}
		};
#endif // DEVELOPPING_RGREEBLE

#ifdef DEVELOPPING_RGREEBLE
		// If in development mode, don't define the template
#define RGREEBLE_TEMPLATE 
#define RGREEBLE RGreeble
#else
		// If not in development mode, define the template
#define RGREEBLE_TEMPLATE template <typename TVertex, typename TVertexProvider, typename TUintVertIdx>
#define RGREEBLE RGreeble<typename TVertex, typename TVertexProvider, typename TUintVertIdx>
#endif // DEVELOPPING_RGREEBLE

		RGREEBLE_TEMPLATE
		class RGreeble
		{
			friend class RGreebleLibary;
			struct RGLineData
			{
				XMINT2 StartCoord;
				XMINT2 EndCoord;
				inline XMINT2 ForwardVector() const
				{
					return XMINT2
					(
						std::clamp<int32_t>(EndCoord.x - StartCoord.x, -1, +1),
						std::clamp<int32_t>(EndCoord.y - StartCoord.y, -1, +1)
					);
				}
				XMINT2 RightVector() const
				{
					return mds::RMath2D::RightVector ( ForwardVector() );
				}
				uint32_t Length() const { if (StartCoord.x != EndCoord.x) { return std::abs(EndCoord.x - StartCoord.x); } else { return std::abs(EndCoord.y - StartCoord.y); } }
			};
			struct RGContourData
			{
			public:
				std::vector<RGLineData> Lines;
			};
			struct RGSurfaceData
			{
				DirectX::XMFLOAT4X4 TransposeMatrix;
				bool bFaceClockwise;
				DirectX::XMFLOAT3 DepthAxis;
				RVoxelSurface::RVoxelSurfaceData ParentCoordSurfaceData;
				RVoxelSurface::RVoxelSurfaceData SelfCoordSurfaceData;
				RGContourData ParentContourData;
				RGContourData ChildContourData;
				float ExtrusionHeight = 0.f;
				int32_t TileDefRatioRelatParent = 1;
				//Chamfer effective distance is actually -1.5, -0.5, 0.5, 1.5, 2.5 ect ...
				int32_t ChildVXLChamfer = 1; // 0 => -0.5, 1 => 0.5 etc...
				XMINT2 ParentToChildChamfer(const XMINT2& Dir) const
				{	//child grid is moved 0.5 to the bottom left, so 
					return Dir;/* XMINT2
					{
						-1 + Dir.x * ((false && (Dir.x < 0) ) ? (ChildVXLChamfer - 1) : ChildVXLChamfer),
						2 + Dir.y *  ((false && (Dir.y < 0) ) ? (ChildVXLChamfer - 1) : ChildVXLChamfer),
					};/**/
				}
				XMINT2 ParentToChildPosChamfer(const XMINT2& ParentPos, const XMINT2& Dir) const
				{
					return mds::unoptimized_add2
					(
						mds::unoptimized_multiply2(ParentPos, TileDefRatioRelatParent),
						ParentToChildChamfer(Dir)
					);
				};
				inline XMINT2 ParentToChildPos(const XMINT2& ParentPos) const
				{
					return mds::unoptimized_multiply2(ParentPos, TileDefRatioRelatParent);
				};
			};
			struct RGMemoryTree
			{
			public :
				std::weak_ptr<RGMemoryTree> ParentElement;
				std::vector<std::shared_ptr<RGMemoryTree>> ChildElements;
				RVoxelSurface ParentCoordVoxelSurface;
				RVoxelSurface SelfCoordVoxelSurface;
				RGSurfaceData SurfaceData;
			};
			using SPMemTree = std::shared_ptr<RGMemoryTree>;
			struct RGContext
			{
			public :
				std::mt19937_64 Gen;
				std::vector<TVertex>& Vertexes;
				std::vector<TUintVertIdx>& Triangles;
				DirectX::XMFLOAT4X4 DefaultTranspose;
				bool bFaceClockwise;
				DirectX::XMFLOAT3 DefaultScale;
				DirectX::XMFLOAT3 DefaultTranslate;
				DirectX::XMFLOAT3 DepthAxis; //Should be the normal ?
				std::shared_ptr<RGMemoryTree> MemoryTreeRoot;
				//std::unordered_map<RVoxelSurface*, std::weak_ptr<RGMemoryTree>> surfaceToMemory;
				std::vector<uint16_t> SideGreedbleTextureIDs;
				std::vector<uint16_t> TopGreedbleTextureIDs;
				std::uniform_int_distribution<uint16_t> Dist_SideGreedbleTextureIDs;
				std::uniform_int_distribution<uint16_t> Dist_TopGreedbleTextureIDs;
			};
		private :
			TVertexProvider m_vertexProvider;
			std::unique_ptr<RGContext> mp_context;
		public:

			RGreeble(const TVertexProvider& InVertexProvider)
			:m_vertexProvider(InVertexProvider), mp_context(nullptr){ };

			void Generate
			(
				std::mt19937_64&& InGenerator, RVoxelSurface&& InPlane, const DirectX::XMFLOAT4X4& InPlaneToModelSpace, 
				const DirectX::XMFLOAT3& InPlaneExtrusionDir, const bool bInFaceClockwise, 
				const std::vector<uint16_t>& InSideGreedbleTextureIDs, const std::vector<uint16_t>& InTopGreedbleTextureIDs,
				std::vector<TVertex>& InOutVertices, std::vector<TUintVertIdx>& InOutTirangles
			);
			void GreebleSurface(SPMemTree InParent)
			{
				SubdivideSurfaceByTileGrowth(InParent);
				DrawFinalSurface(InParent);
				for (auto& childElement : InParent->ChildElements)
				{
					DrawContour(childElement);
					DrawFinalSurface(childElement);
				}
			}

			std::vector<std::unique_ptr<RVoxelSurface>> SubdivideSurface(SPMemTree InParent);
			void SubdivideSurfaceByTileGrowth(SPMemTree InParent);
			void DuplicateParentTileSetWithScale(SPMemTree InMemChild, const bool bInComputeSurfaceData = true);
			void GenerateContourAndChildTile(SPMemTree InParent);
			void DrawContour(SPMemTree InMemLeaf);
			void DrawFinalSurface(SPMemTree InMemLeaf);
		};
	};
};
#ifndef IS_INCLUDE_FROM_GREEBLE_IMPLS

#define IS_INCLUDE_FROM_GREEBLE_DECL
#include "MDS/Algorithm/RGreebleImpls.h"
#undef IS_INCLUDE_FROM_GREEBLE_DECL

#endif // !IS_INCLUDE_FROM_GREEBLE_IMPLS