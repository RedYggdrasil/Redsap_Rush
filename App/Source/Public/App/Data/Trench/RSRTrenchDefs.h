#pragma once

#include <DirectXMath.h>
#include "MDS/RTool.h"
#include "MDS/Tools/REnum.h"
#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"

namespace RSRush
{

	namespace RSRTrenchDefs
	{
		enum class ESurfaceDirection : uint8_t
		{
			NONE = 0 << 0,
			FRWRD = 1 << 0,
			BACK = 1 << 1,
			LEFT = 1 << 2,
			RIGHT = 1 << 3,
			UP = 1 << 4,
			DOWN = 1 << 5
		};
		RS_DEFINE_ENUM_FLAG(ESurfaceDirection);

		inline std::string _tostring(const ESurfaceDirection EInSurfaceDirection)
		{
			std::string result;
			switch (EInSurfaceDirection)
			{
			case ESurfaceDirection::FRWRD:
				result = TEXT("FRWRD");
				break;
			case ESurfaceDirection::BACK:
				result = TEXT("BACK");
				break;
			case ESurfaceDirection::LEFT:
				result = TEXT("LEFT");
				break;
			case ESurfaceDirection::RIGHT:
				result = TEXT("RIGHT");
				break;
			case ESurfaceDirection::UP:
				result = TEXT("UP");
				break;
			case ESurfaceDirection::DOWN:
				result = TEXT("DOWN");
				break;
			case ESurfaceDirection::NONE:
				result = TEXT("NONE");
				break;
			default:
				result = std::format(TEXT("ESurfaceDirection Unimplemented({})"), (uint8_t)EInSurfaceDirection);
				break;
			}
			return result;
		}
		static constexpr float VOXEL_SIZE = 0.5f;//0.f;
		static constexpr DirectX::XMFLOAT3 VOXEL_SIZE_3D = { VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE };
		static constexpr float SVXL(const int32_t InVoxelSize) { return InVoxelSize * VOXEL_SIZE; };
		static constexpr float SVXL(const float InVoxelSize) { return InVoxelSize * VOXEL_SIZE; };
		static constexpr DirectX::XMFLOAT3 SVXL(const DirectX::XMINT3& InVoxelCoord) { return { SVXL(InVoxelCoord.x),SVXL(InVoxelCoord.y), SVXL(InVoxelCoord.z), }; };

		static constexpr int32_t /*Y*/HALF_TRENCH_WIDTH_VXL = 4;
		static constexpr float /*Y*/HALF_TRENCH_WIDTH = SVXL(HALF_TRENCH_WIDTH_VXL);

		static constexpr int32_t TRENCH_WIDTH_VXL = HALF_TRENCH_WIDTH_VXL * 2;
		static constexpr float TRENCH_WIDTH = SVXL(TRENCH_WIDTH_VXL);

		static constexpr int32_t SIDES_WIDTH_VXL = 4;
		static constexpr float SIDES_WIDTH = SVXL(SIDES_WIDTH_VXL);

		static constexpr int32_t /*Y*/HALF_TOTAL_WIDTH_VXL = HALF_TRENCH_WIDTH_VXL + SIDES_WIDTH_VXL;
		static constexpr float /*Y*/HALF_TOTAL_WIDTH = SVXL(HALF_TOTAL_WIDTH_VXL);

		static constexpr int32_t TOTAL_WIDTH_VXL = HALF_TOTAL_WIDTH_VXL * 2;
		static constexpr float TOTAL_WIDTH = SVXL(TOTAL_WIDTH_VXL);

		static constexpr int32_t GROUND_HEIGHT_VXL = -4;
		static constexpr float GROUND_HEIGHT = SVXL(GROUND_HEIGHT_VXL);

		static constexpr int32_t ROOF_HEIGHT_VXL = -GROUND_HEIGHT_VXL;
		static constexpr float ROOF_HEIGHT = SVXL(ROOF_HEIGHT_VXL);

		static constexpr float CENTER_HEIGHT_VXL = (GROUND_HEIGHT_VXL + ROOF_HEIGHT_VXL) * 0.5f;
		static constexpr float CENTER_HEIGHT = SVXL(CENTER_HEIGHT_VXL);

		static constexpr float HALF_HEIGHT_VXL =
			(
				((ROOF_HEIGHT_VXL - GROUND_HEIGHT_VXL) >= ((int32_t)0)) ?
				(ROOF_HEIGHT_VXL - GROUND_HEIGHT_VXL)
				:
				-(ROOF_HEIGHT_VXL - GROUND_HEIGHT_VXL)
				)
			* 0.5f;
		static constexpr float HALF_HEIGHT = SVXL(HALF_HEIGHT_VXL);

		static constexpr int32_t GROUND_TO_UNDER_MESH_VXL = -2;
		static constexpr float GROUND_TO_UNDER_MESH = SVXL(GROUND_TO_UNDER_MESH_VXL);

		static constexpr int32_t UNDER_MESH_VXL = GROUND_HEIGHT_VXL + GROUND_TO_UNDER_MESH_VXL;
		static constexpr float UNDER_MESH = SVXL(UNDER_MESH_VXL);


		static constexpr int32_t /*X*/HALF_TRENCH_LENGHT_VXL = 10;
		static constexpr float /*X*/HALF_TRENCH_LENGHT = SVXL(HALF_TRENCH_LENGHT_VXL);

		static constexpr int32_t BASIC_TRENCH_X_LENGHT_VXL = HALF_TRENCH_LENGHT_VXL * 2;
		static constexpr float BASIC_TRENCH_X_LENGHT = SVXL(BASIC_TRENCH_X_LENGHT_VXL);

		static constexpr DirectX::XMFLOAT3 N_UP =		DirectX::XMFLOAT3(0.f, 0.f, 1.f);
		static constexpr DirectX::XMFLOAT3 N_DOWN =		DirectX::XMFLOAT3(0.f, 0.f, -1.f);
		static constexpr DirectX::XMFLOAT3 N_RIGHT =	DirectX::XMFLOAT3(0.f, 1.f, 0.f);
		static constexpr DirectX::XMFLOAT3 N_LEFT =		DirectX::XMFLOAT3(0.f, -1.f, 0.f);
		static constexpr DirectX::XMFLOAT3 N_FRWRD =	DirectX::XMFLOAT3(1.f, 0.f, 0.f);
		static constexpr DirectX::XMFLOAT3 N_BACK =		DirectX::XMFLOAT3(-1.f, 0.f, 0.f);

		static constexpr DirectX::XMINT3 DIR_UP		= DirectX::XMINT3(+0, +0, +1);
		static constexpr DirectX::XMINT3 DIR_DOWN	= DirectX::XMINT3(+0, +0, -1);
		static constexpr DirectX::XMINT3 DIR_RIGHT	= DirectX::XMINT3(+0, +1, +0);
		static constexpr DirectX::XMINT3 DIR_LEFT	= DirectX::XMINT3(+0, -1, +0);
		static constexpr DirectX::XMINT3 DIR_FRWRD	= DirectX::XMINT3(+1, +0, +0);
		static constexpr DirectX::XMINT3 DIR_BACK	= DirectX::XMINT3(-1, +0, +0);

		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_UP = mds::RMath::TranspostionMatrix
		(
			DirectX::XMFLOAT3(1.f, 0.f, 0.f), //X become X
			DirectX::XMFLOAT3(0.f, 1.f, 0.f), //Y become Y
			DirectX::XMFLOAT3(0.f, 0.f, 1.f), //Depth become Z
			DirectX::XMFLOAT3(0.f, 0.f, 0.f)  //Origin stay Origin
		);
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_DOWN = TRANSPOSE2D_UP;

		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_RIGHT = mds::RMath::TranspostionMatrix
		(
			DirectX::XMFLOAT3(1.f, 0.f, 0.f), //X become X
			DirectX::XMFLOAT3(0.f, 0.f, 1.f), //Y become Z
			DirectX::XMFLOAT3(0.f, 1.f, 0.f), //Depth become Y
			DirectX::XMFLOAT3(0.f, 0.f, 0.f)  //Origin stay Origin
		);
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_LEFT = TRANSPOSE2D_RIGHT;
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_FRWRD = mds::RMath::TranspostionMatrix
		(
			DirectX::XMFLOAT3(0.f, 1.f, 0.f), //X become Y
			DirectX::XMFLOAT3(0.f, 0.f, 1.f), //Y become Z
			DirectX::XMFLOAT3(1.f, 0.f, 0.f), //Depth become X
			DirectX::XMFLOAT3(0.f, 0.f, 0.f)  //Origin stay Origin
		);
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSE2D_BACK = TRANSPOSE2D_FRWRD;


		static constexpr DirectX::XMFLOAT3 NORMAL(ESurfaceDirection EInDirection) 
		{
			switch (EInDirection)
			{
			case ESurfaceDirection::FRWRD:
				return N_FRWRD;
			case ESurfaceDirection::BACK:
				return N_BACK;
			case ESurfaceDirection::LEFT:
				return N_LEFT;
			case ESurfaceDirection::RIGHT:
				return N_RIGHT;
			case ESurfaceDirection::UP:
				return N_UP;
			case ESurfaceDirection::DOWN:
				return N_DOWN;
			case ESurfaceDirection::NONE:
				//Fallback
			default:
				return DirectX::XMFLOAT3(0.f, 0.f, 0.f);
			}
		}

		static constexpr DirectX::XMINT3 DIRECTION(ESurfaceDirection EInDirection)
		{
			switch (EInDirection)
			{
			case ESurfaceDirection::FRWRD:
				return DIR_FRWRD;
			case ESurfaceDirection::BACK:
				return DIR_BACK;
			case ESurfaceDirection::LEFT:
				return DIR_LEFT;
			case ESurfaceDirection::RIGHT:
				return DIR_RIGHT;
			case ESurfaceDirection::UP:
				return DIR_UP;
			case ESurfaceDirection::DOWN:
				return DIR_DOWN;
			case ESurfaceDirection::NONE:
				//Fallback
			default:
				return DirectX::XMINT3(0, 0, 0);
			}
		}
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSITION(ESurfaceDirection EInDirection)
		{
			switch (EInDirection)
			{
			case ESurfaceDirection::FRWRD:
				return TRANSPOSE2D_FRWRD;
			case ESurfaceDirection::BACK:
				return TRANSPOSE2D_BACK;
			case ESurfaceDirection::LEFT:
				return TRANSPOSE2D_LEFT;
			case ESurfaceDirection::RIGHT:
				return TRANSPOSE2D_RIGHT;
			case ESurfaceDirection::UP:
				return TRANSPOSE2D_UP;
			case ESurfaceDirection::DOWN:
				return TRANSPOSE2D_DOWN;
			case ESurfaceDirection::NONE:
				//Fallback
			default:
				return TRANSPOSE2D_UP;
			}
		}
		/// <summary>
		/// Create a transposition and tranlation matrix. translation is applied to resulting axis
		/// </summary>
		/// <param name="EInDirection"></param>
		/// <param name="InTranslation">Post translation (given X, Y, Z are post tranlation, ex : local-to-world, InTranslation would be applied to resulting world axis)</param>
		/// <returns></returns>
		static constexpr DirectX::XMFLOAT4X4 TRANSPOSITION_TRANSLATION(ESurfaceDirection EInDirection, const DirectX::XMFLOAT3& InTranslation )
		{
			DirectX::XMFLOAT4X4 transposeMatrix = TRANSPOSITION(EInDirection);
			transposeMatrix._41 = InTranslation.x;
			transposeMatrix._42 = InTranslation.y;
			transposeMatrix._43 = InTranslation.z;
			transposeMatrix._44 = 1.f;
			return transposeMatrix;
		}

		/// <summary>
		/// Create a transposition and tranlation matrix. translation is applied to pre-translation axis
		/// </summary>
		/// <param name="EInDirection"></param>
		/// <param name="InTranslation">Pre translation (given X, Y, Z are pre tranlation, ex : local-to-world, InTranslation would be applied to preexisting local axis)</param>
		/// <returns></returns>
		static DirectX::XMFLOAT4X4 TRANSPOSITION_PRE_TRANSLATION(ESurfaceDirection EInDirection, const DirectX::XMFLOAT3& InTranslation)
		{
			DirectX::XMFLOAT4X4 transposeMatrix = TRANSPOSITION(EInDirection);
			DirectX::XMMATRIX loadedTransposition = DirectX::XMLoadFloat4x4(&transposeMatrix);
			DirectX::XMVECTOR loadedTranslation = DirectX::XMLoadFloat3(&InTranslation);
			DirectX::XMFLOAT3 transposedTranlation;
			DirectX::XMStoreFloat3(&transposedTranlation, DirectX::XMVector3Transform(loadedTranslation, loadedTransposition));

			transposeMatrix._41 = transposedTranlation.x;
			transposeMatrix._42 = transposedTranlation.y;
			transposeMatrix._43 = transposedTranlation.z;
			transposeMatrix._44 = 1.f;
			return transposeMatrix;
		}
		static DirectX::XMFLOAT4X4 TRANSPOSITION_DEPTH(ESurfaceDirection EInDirection, const float InDepth)
		{
			return TRANSPOSITION_PRE_TRANSLATION(EInDirection, DirectX::XMFLOAT3(0.f, 0.f, InDepth));
		}


		template<typename TNumericalXY>
		inline constexpr typename mds::TypeLookupTNumericalXYToAggr3D<TNumericalXY>::type To3D(const TNumericalXY& InNum2, typename mds::TypeLookupTNumericalAggrToUnderlying<TNumericalXY>::type InDepth, const ESurfaceDirection EInDir)
		{
			switch (EInDir)
			{
			case ESurfaceDirection::FRWRD:
				//FallThough
			case ESurfaceDirection::BACK:
				return { InDepth, InNum2.x, InNum2.y };
			case ESurfaceDirection::LEFT:
				//FallThough
			case ESurfaceDirection::RIGHT:
				return { InNum2.x, InDepth , InNum2.y };
			case ESurfaceDirection::UP:
				//FallThough
			case ESurfaceDirection::DOWN:
				return { InNum2.x, InNum2.y , InDepth };
			case ESurfaceDirection::NONE:
				//FallThough
			default:
				RSRLog::Log(LOG_ERROR, TEXT("Falltough to default for ESurfaceDirection : {}"), _tostring(EInDir));
				return { InNum2.x, InNum2.x , InNum2.x };
			}
		};
	};
};