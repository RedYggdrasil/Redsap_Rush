#pragma once
#include <vector>
#include "MDS/Algorithm/RVoxelSurface.h"

#include "App/Data/RSRTransform.h"
#include "App/Data/Trench/RSRTrench.h"
//#include "DirectXMath.h"

namespace RSRush
{
	class RSRVoxalPlane
	{
	public:
		struct alignas(16) Normalizer
		{
		public:
			DirectX::XMVECTOR Min;
			DirectX::XMVECTOR Size;
		public:
			inline void Normalize(const DirectX::XMINT2& InCoord, DirectX::XMFLOAT2* InOutDest) const
			{
				using namespace DirectX;
				*InOutDest = DirectX::XMFLOAT2{ (float)InCoord.x, (float)InCoord.y };
				DirectX::XMVECTOR coord = DirectX::XMLoadFloat2(InOutDest);
				DirectX::XMVECTOR coordm = coord - Min;
				XMStoreFloat2(InOutDest, coordm / Size);
			}
		};
		struct TileData
		{
		public:
			bool bObstructed = false;
		public:
			inline constexpr bool Free() const { return !bObstructed; }
		};
		struct TileData1D
		{
		private:
			std::vector<TileData>* mp_tiles;
			int32_t m_yCoordToIdx;
		public:
			TileData& operator [](int32_t InYCoord) {
				return (*mp_tiles)[m_yCoordToIdx + InYCoord];
			}

			TileData operator [](int32_t InYCoord) const {
				return (*mp_tiles)[m_yCoordToIdx + InYCoord];
			}
		private:
			TileData1D(std::vector<TileData>* InTiles, int32_t InYCoordToFinalIdx)
				: mp_tiles(InTiles), m_yCoordToIdx(InYCoordToFinalIdx) {}
			friend class RSRVoxalPlane;
		};
	private:
		bool m_initialized = false;
		RSRush::RSRTrenchDefs::ESurfaceDirection m_surfaceDirection;

		size_t m_sizes[2];
		DirectX::XMINT2 m_minCoord;
		int32_t m_defaultDepth;
		std::vector<TileData> m_tiles;

		inline size_t Idx2ToIdx(const size_t InIdxs[2]) const
		{
			return InIdxs[0] * m_sizes[1] + InIdxs[1];
		}
		inline void IdxToIdx2(const size_t InIdx, size_t OutIdx[2]) const
		{
			OutIdx[0] = InIdx / m_sizes[1];
			OutIdx[1] = InIdx % m_sizes[1];
		}

		inline DirectX::XMINT2 Idx2ToCoord(const size_t InIdxs[2]) const
		{
			return { (int32_t)InIdxs[0] + m_minCoord.x, (int32_t)InIdxs[1] + m_minCoord.y };
		}

		inline void CoordToIdx2(const DirectX::XMINT2 InCoord, size_t OutIdx[2]) const
		{

			OutIdx[0] = InCoord.x - m_minCoord.x; OutIdx[1] = InCoord.y - m_minCoord.y;
		}

		inline size_t CoordToIdx(const DirectX::XMINT2 InCoord) const
		{
			return (InCoord.x - m_minCoord.x) * m_sizes[1] + (InCoord.y - m_minCoord.y);
		}

		inline DirectX::XMINT2 IdxToCoord(const size_t InIdx) const
		{
			size_t Idxs[2]; IdxToIdx2(InIdx, /*Out*/Idxs); return Idx2ToCoord(Idxs);
		}

	public:
		RSRVoxalPlane();
		RSRVoxalPlane(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir);
		RSRVoxalPlane(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir, const int32_t InDefaultDepth);
		
		RSRVoxalPlane(RSRVoxalPlane&& ROther) noexcept;
		RSRVoxalPlane& operator=(RSRVoxalPlane&& ROther) noexcept;

		bool Init(const DirectX::XMINT2 InMin, const DirectX::XMINT2 InMax, const RSRTrenchDefs::ESurfaceDirection EinSurfaceDir, const int32_t InDefaultDepth = 0);
		void DeInit(bool bInFromDestructor = false);
		inline bool GetIsInitialized() const { return m_initialized; };
		inline int32_t GetDefaultDepth() const { return m_defaultDepth; }

		inline RSRush::RSRTrenchDefs::ESurfaceDirection GetDirection() const { return m_surfaceDirection; };
		inline DirectX::XMINT2 Min() const { return m_minCoord; }
		inline DirectX::XMINT2 Max() const 
		{ return DirectX::XMINT2{ m_minCoord.x + (int32_t)m_sizes[0], m_minCoord.y + (int32_t)m_sizes[1] }; };
		inline size_t GetSize() const { return m_sizes[0] * m_sizes[1]; }
		inline DirectX::XMINT2 GetSizes() const { return DirectX::XMINT2{ (int32_t)m_sizes[0], (int32_t)m_sizes[1] }; };
		inline void GetSizes(size_t OutSizes[2]) const { OutSizes[0] = m_sizes[0], OutSizes[1] = m_sizes[1]; };

		Normalizer CreateNormalizer() const;
		DirectX::XMINT3 GetCoord3(const DirectX::XMINT2& InLocalCoord, int32_t InDepthValue) const;
		DirectX::XMFLOAT4X4 GetTranslateTransformMatrix() const { return GetTranslateTransformMatrix(m_defaultDepth); }
		DirectX::XMFLOAT4X4 GetTranslateTransformMatrix(int32_t InDepthValue) const;
		DirectX::XMFLOAT4X4 GetTranslateTransformMatrix(float InDepthValue) const;
		DirectX::XMFLOAT4X4 GetSVXLTranslateTransformMatrix() const { return GetSVXLTranslateTransformMatrix(m_defaultDepth); }
		DirectX::XMFLOAT4X4 GetSVXLTranslateTransformMatrix(int32_t InDepthValue) const;
		inline DirectX::XMINT3 GetCoord3(const DirectX::XMINT2& InLocalCoord) const { return GetCoord3(InLocalCoord, m_defaultDepth); }
		inline DirectX::XMINT3 GetMin3() const { return GetCoord3(m_minCoord, m_defaultDepth); }
		inline DirectX::XMINT3 GetMin3(int32_t InDepthValue) const { return GetCoord3(m_minCoord, InDepthValue); }

		virtual ~RSRVoxalPlane();
		mds::voxel::RVoxelSurface GenerateRVoxelSurface() const;
	public:

		mds::YXInteratorMaxExcluded begin() const { return mds::YXInteratorMaxExcluded::GenerateBegin(m_minCoord, this->Max()); }
		mds::YXInteratorMaxExcluded end() const { return mds::YXInteratorMaxExcluded::GenerateEnd(m_minCoord, this->Max()); }


		TileData& operator [](const DirectX::XMINT2 InCoord) { return m_tiles[CoordToIdx(InCoord)]; }
		TileData operator [](const DirectX::XMINT2 InCoord) const { return m_tiles[CoordToIdx(InCoord)]; }


		TileData1D/*&*/ operator [](const int32_t InXCoord)
		{
			const int32_t InXIndex = InXCoord - m_minCoord.x;
			const int32_t xStartIndex = (InXIndex * (int32_t)m_sizes[1]);
			TileData1D T1D = TileData1D(&m_tiles, xStartIndex - m_minCoord.y);
			return T1D;
		}
		const TileData1D operator [](const int32_t InXCoord) const
		{
			const int32_t InXIndex = InXCoord - m_minCoord.x;
			const int32_t xStartIndex = (InXIndex * (int32_t)m_sizes[1]);
			TileData1D T1D = TileData1D(const_cast<std::vector<TileData>*>(&m_tiles), xStartIndex - m_minCoord.y);
			return T1D;
		}
	public:
		static bool UNIT_TEST_CLASS();
	};
};