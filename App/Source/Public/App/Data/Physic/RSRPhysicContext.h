#pragma once
#include <DirectXMath.h>

namespace RSRush
{
	struct RSRPhysicContext
	{
	public:
		DirectX::XMFLOAT3 GlobalGravity;
	};
	namespace PhysicContext
	{
		constexpr RSRPhysicContext DEFAULT_EARTH =
		{
			.GlobalGravity = DirectX::XMFLOAT3{ 0.f, 0.f, -9.81f }
		};
		constexpr RSRPhysicContext DEFAULT_MOON =
		{
			.GlobalGravity = DirectX::XMFLOAT3{ 0.f, 0.f, -1.62f }
		};
	}
	
}