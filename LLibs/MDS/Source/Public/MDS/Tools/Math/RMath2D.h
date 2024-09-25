#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "MDS/Tools/RMath.h"

namespace mds
{
	//Top
	constexpr DirectX::XMINT2 DIR2D_XPLUS_YZERO = DirectX::XMINT2{ +1, +0 }, DIR2D_TOP = DIR2D_XPLUS_YZERO;
	//Top-Right
	constexpr DirectX::XMINT2 DIR2D_XPLUS_YPLUS = DirectX::XMINT2{ +1, +1 }, DIR2D_TOP_RIGHT = DIR2D_XPLUS_YPLUS;

	//Right
	constexpr DirectX::XMINT2 DIR2D_XZERO_YPLUS = DirectX::XMINT2{ +0, +1 }, DIR2D_RIGHT = DIR2D_XZERO_YPLUS;
	//Bottom-Right
	constexpr DirectX::XMINT2 DIR2D_XMINS_YPLUS = DirectX::XMINT2{ -1, +1 }, DIR2D_BOTTOM_RIGHT = DIR2D_XMINS_YPLUS;

	//Bottom
	constexpr DirectX::XMINT2 DIR2D_XMINS_YZERO = DirectX::XMINT2{ -1, +0 }, DIR2D_BOTTOM = DIR2D_XMINS_YZERO;
	//Bottom-Left
	constexpr DirectX::XMINT2 DIR2D_XMINS_YMINS = DirectX::XMINT2{ -1, -1}, DIR2D_BOTTOM_LEFT = DIR2D_XMINS_YMINS;

	//Left
	constexpr DirectX::XMINT2 DIR2D_XZERO_YMINS = DirectX::XMINT2{ +0, -1 }, DIR2D_LEFT = DIR2D_XZERO_YMINS;
	//Top-left
	constexpr DirectX::XMINT2 DIR2D_XPLUS_YMINS = DirectX::XMINT2{ +1, -1 }, DIR2D_TOP_LEFT = DIR2D_XPLUS_YMINS;

	inline constexpr DirectX::XMINT2 DIRECTIONS2D_4[4] = { DIR2D_TOP , DIR2D_RIGHT, DIR2D_BOTTOM, DIR2D_LEFT };
	inline constexpr DirectX::XMINT2 DIRECTIONS2D_8[8] = { DIR2D_TOP , DIR2D_TOP_RIGHT, DIR2D_RIGHT, DIR2D_BOTTOM_RIGHT, DIR2D_BOTTOM, DIR2D_BOTTOM_LEFT,  DIR2D_LEFT, DIR2D_TOP_LEFT };
	class RMath2D
	{
	public:
		inline constexpr static DirectX::XMINT2 ForwardVector(const DirectX::XMINT2& InVector)
		{
			return InVector;
		}
		inline constexpr static DirectX::XMINT2 RightVector(const DirectX::XMINT2& InVector)
		{
			return DirectX::XMINT2(-InVector.y, InVector.x);
		}
		inline constexpr static DirectX::XMINT2 LeftVector(const DirectX::XMINT2& InVector)
		{
			return DirectX::XMINT2(InVector.y, -InVector.x);
		}
		inline constexpr static DirectX::XMINT2 BackVector(const DirectX::XMINT2& InVector)
		{
			return DirectX::XMINT2(-InVector.x, -InVector.y);
		}
	};
};