#pragma once

#include "App/Data/Trench/RSRBasicTrench.h"
#include "App/Data/Trench/RSRVoxalGrid.h"
#include <memory>

namespace RSRush
{
	class RSRVoxelObstacle;
	class RSRVoxalTrench : public RSRBasicTrench
	{
	protected:
		RSRVoxalGrid m_grid;

		std::vector<std::unique_ptr<RSRVoxelObstacle>> m_obstacles;

		std::vector<uint16_t> m_sideGreedbleTextureIDs;
		std::vector<uint16_t> m_topGreedbleTextureIDs;
		//LOD 0 : Full Greeble MEsh
		//LOD 1 : Full Mesh & Obstacles, no greebles
		//LOD 2 : Mesh & (Obstacles : Back, Up, Left, Right)  ??
		//LOD 3 : Only Mesh borders
	public:
		RSRVoxalTrench(RSRTrenchManager* InTrenchManager, const std::vector<uint16_t>& InSideGreedbleTextureIDs, const std::vector<uint16_t>& InTopGreedbleTextureIDs);
		/*virtual*/ ~RSRVoxalTrench() /*override*/;
	protected:
		virtual void PositionSelf() override;
		virtual void GeneratingMeshThreadSensitive() override;
		virtual void GenerateGeometry() override;
		virtual void GeneratePhysicColliders() override;

		size_t GenerateObstacles();

		void SubdividePlanes(std::vector<RSRVoxalPlane>& InOutPlanes);
		void GeneratePlaneGeometry(RSRVoxalPlane& InOutPlane, std::vector<VertexPositionUVColor>& InOutVertices, std::vector<UINT_VRTX_IDX>& InOutTirangles);
	};
}