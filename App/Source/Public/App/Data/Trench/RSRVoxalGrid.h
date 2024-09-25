#pragma once
#include "MDS/Tools/RCoordinate.h"
#include "MDS/Tools/REnum.h"
#include "MDS/Tools/RIterators.h"
#include "App/Data/RSRTransform.h"
#include <vector>

namespace RSRush
{
	class RSRVoxalPlane;
	struct RSRVoxelShape
	{
		virtual std::vector<RSRVoxalPlane> GeneratePlanes() = 0;
	};
	struct RSRVoxelRectangle
	{
	public:
		DirectX::XMINT3 Min;
		DirectX::XMINT3 Max;
	public:
		inline constexpr DirectX::XMINT3 size() const
		{ return { Max.x - Min.x, Max.y - Min.y, Max.z - Min.z }; }

		mds::ZYXInteratorMaxExcluded begin() const { return mds::ZYXInteratorMaxExcluded::GenerateBegin(Min, Max); }
		mds::ZYXInteratorMaxExcluded end() const { return mds::ZYXInteratorMaxExcluded::GenerateEnd(Min, Max); }

	public:
		std::vector<RSRVoxalPlane> GeneratePlanes() const;
		void GeneratePlanes(std::vector<RSRVoxalPlane>& InOutVector) const;
		DirectX::XMFLOAT3 ComputeCenterSVXL() const;
		DirectX::XMFLOAT3 ComputeExtendsSVXL() const;
	};
	class RSRVoxalGrid
	{
	public:
		struct VoxelData
		{
		public:
			enum class EState : uint8_t
			{
				RENUM_MIN(FREE),
				RENUM_MAX(FULL),
				RENUM_COUNT
			};
		public:
			EState State = EState::FREE;
		};
		static bool IsFree(const VoxelData::EState EInState) { return EInState == VoxelData::EState::FREE; }
		static VoxelData::EState FreeerState(const VoxelData::EState EInLeft, const VoxelData::EState EInRight)
		{ return static_cast<VoxelData::EState>(std::min(mds::UT_cast(EInLeft), mds::UT_cast(EInRight))); }
		static VoxelData::EState FullerState(const VoxelData::EState EInLeft, const VoxelData::EState EInRight)
		{ return static_cast<VoxelData::EState>(std::max(mds::UT_cast(EInLeft), mds::UT_cast(EInRight))); }
		struct VoxelDatas2D;
		struct VoxelDatas1D
		{
		private:
			std::vector<VoxelData>* mp_voxels;
			int32_t m_zCoordToIdx;
		public:
			VoxelData& operator [](int32_t InZCoord) {
				return (*mp_voxels)[m_zCoordToIdx + InZCoord];
			}

			VoxelData operator [](int32_t InZCoord) const {
				return (*mp_voxels)[m_zCoordToIdx + InZCoord];
			}
		private:
			VoxelDatas1D(std::vector<VoxelData>* InVoxels, int32_t InZCoordToFinalIdx)
				: mp_voxels(InVoxels), m_zCoordToIdx(InZCoordToFinalIdx) {}
			friend struct VoxelDatas2D;
		};
		struct VoxelDatas2D
		{
		private:
			std::vector<VoxelData>* mp_voxels;
			const size_t* mpc_sizes;
			const DirectX::XMINT3* mpc_minCoords;
			size_t m_xStartIndex;
		public:

			VoxelDatas1D/*&*/ operator [](const int32_t InYCoord)
			{
				const int32_t yIdx = InYCoord - mpc_minCoords->y;
				const int32_t yStartIndex = (int32_t)m_xStartIndex + (yIdx * (int32_t)mpc_sizes[2]);
				VoxelDatas1D v1D = VoxelDatas1D(mp_voxels, yStartIndex - mpc_minCoords->z);
				return v1D;
			}
			const VoxelDatas1D operator [](const int32_t InYCoord) const
			{
				const int32_t yIdx = InYCoord - mpc_minCoords->y;
				const int32_t yStartIndex = (int32_t)m_xStartIndex + (yIdx * (int32_t)mpc_sizes[2]);
				const VoxelDatas1D v1D = VoxelDatas1D(const_cast<std::vector<VoxelData>*>(mp_voxels), yStartIndex - mpc_minCoords->z);
				return v1D;
			}
		private:
			VoxelDatas2D(std::vector<VoxelData>* InVoxels, const size_t* pInSizes, const DirectX::XMINT3* pInMinCoords, const size_t InXStartIndex)
			: mp_voxels(InVoxels), mpc_sizes(pInSizes), mpc_minCoords(pInMinCoords), m_xStartIndex(InXStartIndex) {}
			friend class RSRVoxalGrid;
		};

	private:
		bool m_initialized = false;
		size_t m_sizes[3];
		DirectX::XMINT3 m_minCoord;
		std::vector<VoxelData> m_voxels;

		inline size_t Idx3ToIdx(const size_t InIdxs[3]) const 
		{ return InIdxs[0] * (m_sizes[1] * m_sizes[2]) + InIdxs[1] * (m_sizes[2]) + InIdxs[2]; }
		inline void IdxToIdx3(const size_t InIdx, size_t OutIdx[3]) const
		{
			size_t lIdx = InIdx;
			OutIdx[0] = lIdx / (m_sizes[1] * m_sizes[2]);
			lIdx -= OutIdx[0] * (m_sizes[1] * m_sizes[2]);
			OutIdx[1] = lIdx / m_sizes[2];
			OutIdx[2] = lIdx % m_sizes[2];
		}

		inline DirectX::XMINT3 Idx3ToCoord(const size_t InIdxs[3]) const
		{ return { (int32_t)InIdxs[0] + m_minCoord.x, (int32_t)InIdxs[1] + m_minCoord.y, (int32_t)InIdxs[2] + m_minCoord.z }; }
		
		inline void CoordToIdx3(const DirectX::XMINT3 InCoord, size_t OutIdx[3]) const
		{ OutIdx[0] = InCoord.x - m_minCoord.x; OutIdx[1] = InCoord.y - m_minCoord.y; OutIdx[2] = InCoord.z - m_minCoord.z; }

		inline size_t CoordToIdx(const DirectX::XMINT3 InCoord) const
		{ size_t Idxs[3]; CoordToIdx3(InCoord, /*Out*/Idxs); return Idx3ToIdx(Idxs); }
		
		inline DirectX::XMINT3 IdxToCoord(const size_t InIdx) const
		{ size_t Idxs[3]; IdxToIdx3(InIdx, /*Out*/Idxs); return Idx3ToCoord(Idxs); }

	public:
		inline const DirectX::XMINT3& Min() const { return m_minCoord; }
		inline DirectX::XMINT3 Max() const 
		{ return DirectX::XMINT3{ m_minCoord.x + (int32_t)m_sizes[0], m_minCoord.y + (int32_t)m_sizes[1], m_minCoord.z + (int32_t)m_sizes[2] }; };

		mds::ZYXInteratorMaxExcluded begin() const { return mds::ZYXInteratorMaxExcluded::GenerateBegin(m_minCoord, this->Max()); }
		mds::ZYXInteratorMaxExcluded end() const { return mds::ZYXInteratorMaxExcluded::GenerateEnd(m_minCoord, this->Max()); }

		RSRVoxalGrid();
		/// <summary>
		/// Create & Initial a Voxal Grid with InMin(included) to InMax(excluded)
		/// </summary>
		/// <param name="InMin">Min coordinate (included)</param>
		/// <param name="InMax">Max coordinate (excluded)</param>
		RSRVoxalGrid(const DirectX::XMINT3 InMin, const DirectX::XMINT3 InMax);
		/// <summary>
		/// Initial a Voxal Grid with InMin(included) to InMax(excluded)
		/// </summary>
		/// <param name="InMin">Min coordinate (included)</param>
		/// <param name="InMax">Max coordinate (excluded)</param>
		/// <returns>valid initialization</returns>
		bool Init(const DirectX::XMINT3 InMin, const DirectX::XMINT3 InMax);
		void DeInit(bool bInFromDestructor = false);
		inline bool GetIsInitialized() const { return m_initialized; };
		~RSRVoxalGrid();

		VoxelData& operator [](const DirectX::XMINT3 InCoord) { return m_voxels[CoordToIdx(InCoord)]; }
		VoxelData operator [](const DirectX::XMINT3 InCoord) const { return m_voxels[CoordToIdx(InCoord)]; }


		VoxelDatas2D/*&*/ operator [](const int32_t InXCoord)
		{ 
			const int32_t InXIndex = InXCoord - m_minCoord.x;
			VoxelDatas2D v2D = VoxelDatas2D(&m_voxels, m_sizes, &m_minCoord, InXIndex * (int32_t)(m_sizes[1] * m_sizes[2]));
			return v2D;
		}
		const VoxelDatas2D operator [](const int32_t InXCoord) const
		{
			const int32_t InXIndex = InXCoord - m_minCoord.x;
			const VoxelDatas2D v2D = VoxelDatas2D(const_cast<std::vector<VoxelData>*>(&m_voxels), m_sizes, &m_minCoord, InXIndex * (int32_t)(m_sizes[1] * m_sizes[2]));
			return v2D;
		}

		public:
			bool FillPlaneInfos(RSRVoxalPlane* InOutVoxalPlane, const int32_t InPlaneDepth) const;
			bool FillPlaneInfos(RSRVoxalPlane* InOutVoxalPlane) const;
		public:

			template <typename Txmint3Enumerable>
			RSRVoxalGrid::VoxelData::EState GetObstructerState(const Txmint3Enumerable& InCoordCollection) const
			{
				RSRVoxalGrid::VoxelData::EState freeState = RSRVoxalGrid::VoxelData::EState::FREE;

				for (const DirectX::XMINT3& Coord : InCoordCollection)
				{
					freeState = FullerState(freeState, (*this)[Coord].State);
					if (freeState == RSRVoxalGrid::VoxelData::EState::MAX) { break; }
				}
				return freeState;
			}
			//VoxelData::EState GetObstructerState(const RSRVoxelRectangle& InVoxelRectangle) const;
	public:
		static bool UNIT_TEST_CLASS();
	};
};