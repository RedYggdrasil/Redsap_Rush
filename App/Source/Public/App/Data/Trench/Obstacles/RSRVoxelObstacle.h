#pragma once

#include "MDS/Tools/REnum.h"
#include <random>
#include <DirectXMath.h>
#include "App/Data/RSRTransform.h"
#include "App/Data/Shaders/ShaderStructures.h"

namespace RSRush
{
	struct RSRVoxelRectangle;
	struct RSRCollidersBodyLWPair;
	class RSRPhysicManager;
	struct RSRCollidersBody;
	class RSRVoxalGrid;
	class RSRVoxalPlane;
	class RSRVoxelObstacle
	{
	public:
		enum class EObstacleType : uint8_t
		{
			RENUM_MIN_MAX(Rectangle),
			RENUM_COUNT
		};
	protected :
		const EObstacleType m_obstacleType;
		bool m_placed = false;
	public:
		RSRVoxelObstacle() = delete;
		virtual ~RSRVoxelObstacle();
		inline EObstacleType GetObstacleType() const { return m_obstacleType; }
		inline bool GetPlaced() const { return m_placed; }
	protected:
		RSRVoxelObstacle(const EObstacleType EInObstacleType);
	public:
		struct AttemptPlacementData
		{
		public:
			/*TRS matrix, local copy*/
			RSRTransformMatrix TransformMatrix;
			DirectX::XMMATRIX* lp_loadedTRSMatrix = nullptr;

			/*External VoxalGrid*/
			RSRVoxalGrid* p_VoxalGrid = nullptr;
			/*Local stack PhysicBody pair*/
			RSRCollidersBodyLWPair* lp_PhysicBodyPair = nullptr;
		public:
			AttemptPlacementData(const RSRush::RSRTransformMatrix& InTransformMatrix, DirectX::XMMATRIX* InLoadedTRSMatrix, RSRVoxalGrid* InOutGrid, RSRCollidersBodyLWPair* InLocalPair);
		};
		AttemptPlacementData m_apd;
		void InitPlacementData(const RSRush::RSRTransformMatrix& InTransformMatrix, DirectX::XMMATRIX* InLoadedTRSMatrix, RSRVoxalGrid* InOutGrid, RSRCollidersBodyLWPair* InLocalPair);
		void ClearPlacementData();
		bool HasPlacementData() const { return m_apd.lp_loadedTRSMatrix; }
	public:
		virtual bool AttemptPlacement(RSRPhysicManager* InPhysicManager, std::mt19937_64& InOutGenerator, RSRVoxalGrid& InOutGrid, const RSRush::RSRTransformMatrix& InTransformMatrix, const RSRVoxelRectangle& InSpawnArea, uint8_t InMaxAttemptCount = 10) R_PURE;
		bool AttemptPlacement(RSRPhysicManager* InPhysicManager, std::mt19937_64& InOutGenerator, RSRVoxalGrid& InOutGrid, const RSRush::RSRTransformMatrix& InTransformMatrix, uint8_t InMaxAttemptCount = 10);
	
		virtual bool FillVoxalGridData(RSRVoxalGrid& InOutGrid) R_PURE;
		virtual bool FillVoxalPlanes(std::vector<RSRVoxalPlane>& InOutVoxalPlanes) R_PURE;
		virtual bool FillAdditionalGeometry(std::vector<VertexPositionUVColor>& InOutVertices, std::vector<UINT_VRTX_IDX>& InOutTriangles) R_PURE;
		virtual bool FillPhysicBodyData(RSRCollidersBody& InOutPhysicBody) R_PURE;
	public:
		static RSRVoxelObstacle* HeapCreateNewObstacle(std::mt19937_64* InOutGenerator);
		static RSRVoxelObstacle* HeapCreateNewObstacle(const EObstacleType InType);
	protected:
		bool AvalableVoxelSpace(RSRPhysicManager* InPhysicManager, const RSRVoxelRectangle& InVoxelSpace);

	};
}