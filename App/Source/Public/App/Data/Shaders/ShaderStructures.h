#pragma once
#include "directxmath.h"
#include <stdint.h>
#include <dxgiformat.h>

namespace RSRush
{
	using UINT_VRTX_IDX = unsigned short;
	constexpr auto DXGI_FORMAT_RVRTX_IDX = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct MeshCBV
	{
		DirectX::XMFLOAT4X4 modelMatrix;
	};
	struct ContextCBV
	{
		DirectX::XMFLOAT4X4 viewProjectionMatrix;
	};

	struct ModelViewprojectionConstants
	{
		DirectX::XMFLOAT4X4 ViewProjMat;
		DirectX::XMFLOAT4X4 ModMat;
	public:
		static constexpr uint8_t O32B_VIEW_PROJ_MAT = 0ui8;
		static constexpr uint8_t S32B_VIEW_PROJ_MAT = 16ui8;

		static constexpr uint8_t O32B_MOD_MAT = O32B_VIEW_PROJ_MAT + S32B_VIEW_PROJ_MAT;
		static constexpr uint8_t S32B_MOD_MAT = 16ui8;

		static constexpr uint8_t S32B_STRUCT = O32B_MOD_MAT + S32B_MOD_MAT;
	};
	using MVPC = ModelViewprojectionConstants;

	struct MVP_DL_Consts
	{
	public:
		DirectX::XMFLOAT4X4 ViewProjMat;
		DirectX::XMFLOAT4X4 ModMat;
		DirectX::XMFLOAT4X4 invProjModMat;
		DirectX::XMFLOAT4	CamPos;
		DirectX::XMFLOAT4	lightPos_AmbLight;
		DirectX::XMFLOAT4	lightCol_SpecLight;
	public:
		ModelViewprojectionConstants ToMVC() const { return ModelViewprojectionConstants { .ViewProjMat = ViewProjMat, .ModMat = ModMat }; };
	public:
		static constexpr uint8_t O32B_VIEW_PROJ_MAT = 0ui8;
		static constexpr uint8_t S32B_VIEW_PROJ_MAT = 16ui8;

		static constexpr uint8_t O32B_MOD_MAT = O32B_VIEW_PROJ_MAT + S32B_VIEW_PROJ_MAT;
		static constexpr uint8_t S32B_MOD_MAT = 16ui8;

		static constexpr uint8_t O32B_INV_MOD_MAT = O32B_MOD_MAT + S32B_MOD_MAT;
		static constexpr uint8_t S32B_INV_MOD_MAT = 16ui8;

		static constexpr uint8_t O32B_CAM_POS = O32B_INV_MOD_MAT + S32B_INV_MOD_MAT;
		static constexpr uint8_t S32B_CAM_POS = 4ui8;

		static constexpr uint8_t O32B_LIGHT_POS_AMB_LIGHT = O32B_CAM_POS + S32B_CAM_POS;
		static constexpr uint8_t S32B_LIGHT_POS_AMB_LIGHT = 4ui8;

		static constexpr uint8_t O32B_LIGHT_COL_SPEC_LIGHT = O32B_LIGHT_POS_AMB_LIGHT + S32B_LIGHT_POS_AMB_LIGHT;
		static constexpr uint8_t S32B_LIGHT_COL_SPEC_LIGHT = 4ui8;

		static constexpr uint8_t S32B_STRUCT = O32B_LIGHT_COL_SPEC_LIGHT + S32B_LIGHT_COL_SPEC_LIGHT;
	};
	using MVPDLC = MVP_DL_Consts;

	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionUVColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT2 uv0;
		
		uint32_t textureIndex;
	};


	struct UICBV
	{
		DirectX::XMFLOAT4 rect;
		DirectX::XMFLOAT4 ratio; /*[ScaleX, ScaleY, Ratio, _padding]*/
		DirectX::XMFLOAT4 color;
	};

	//Given a Ui based on a base resolution of 1000 1000 and real screen 1920/1080
	// ScreenScale[0.52f, 0.926f]
	//CBV : 
	// ScreenScale FLOAT2[ScaleX, ScaleY]
	// Anchors FLOAT4
	// Pivot FLOAT2
	// Used to send per-vertex data to the vertex shader.
	struct VertexPosition2DUV
	{
		/*[x,y] : Pivot [-1, 1],  [z,w] : Position [-1, 1]*/
		DirectX::XMFLOAT4 pos;
		/* bIsPivotInPixel, bIsPositionInPixel, maxXPixelSize, maxYPixelSize */
		/*DirectX::XMFLOAT4 scaling;*/
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv0;

		uint32_t textureIndex;
	};
}