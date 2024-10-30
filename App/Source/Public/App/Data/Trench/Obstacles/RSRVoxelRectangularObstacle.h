#pragma once

#include "App/Data/Trench/Obstacles/RSRVoxelObstacle.h"
#include "App/Data/Trench/RSRVoxalGrid.h"

namespace RSRush
{
	class RSRVoxelRectangularObstacle : public RSRVoxelObstacle
	{
	public:
		static constexpr EObstacleType TYPE = EObstacleType::Rectangle;
	protected:
		RSRVoxelRectangle m_voxelArea;
	public:
		RSRVoxelRectangularObstacle();
		/*virtual*/ ~RSRVoxelRectangularObstacle()/*override*/;
	public:
		using RSRVoxelObstacle::AttemptPlacement;
		virtual bool AttemptPlacement(std::mt19937_64& InOutGenerator, class RSRVoxalGrid& InOutGrid, const RSRush::RSRTransformMatrix& InTransformMatrix, const RSRVoxelRectangle& InSpawnArea, uint8_t InMaxAttemptCount = 10) R_PURE_OVERRIDE;

		virtual bool FillVoxalGridData(RSRVoxalGrid& InOutGrid) R_PURE_OVERRIDE; 
		virtual bool FillVoxalPlanes(std::vector<RSRVoxalPlane>& InOutVoxalPlanes) R_PURE_OVERRIDE;
		virtual bool FillAdditionalGeometry(std::vector<VertexPositionUVColor>& InOutVertices, std::vector<UINT_VRTX_IDX>& InOutTriangles) R_PURE_OVERRIDE;

		virtual bool FillPhysicBodyData(RSRCollidersBody& InOutPhysicBody) R_PURE_OVERRIDE;
	private:
		bool AttemptHorizontalBarrierPacement(std::mt19937_64& InOutGenerator, const RSRVoxelRectangle& InSpawnArea, RSRVoxelRectangle& OutResultingObstacle);
		bool AttemptVerticalSpirePacement(std::mt19937_64& InOutGenerator, const RSRVoxelRectangle& InSpawnArea, RSRVoxelRectangle& OutResultingObstacle);

	};
};