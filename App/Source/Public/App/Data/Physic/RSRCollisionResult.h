#pragma once

//Inspired by https://winter.dev/articles/physics-engine
#include "MDS/Tools/RLog.h"
#include "MDS/Tools/RMath.h"
#include <string>
#include <DirectXMath.h>

namespace RSRush
{
	/// <summary>
	/// Collider Collision
	/// </summary>
	struct alignas(16) RSRCollisionResult
	{
	public:
		DirectX::XMFLOAT3 P1;
		DirectX::XMFLOAT3 P2;
		DirectX::XMFLOAT3 Normal; // P2 - P1, normalized
		float Depth; // Length of P2 - P1
		bool bDidCollide; // Is the result a collision
	public:
		inline RSRCollisionResult InvertPoints() const
		{
			RSRCollisionResult result = *this;
			std::swap(result.P1, result.P2);
			result.Normal = mds::unoptimized_negate3(result.Normal);
			return result;
		}
		inline RSRCollisionResult& InvertPointsSelf()
		{
			std::swap(this->P1, this->P2);
			this->Normal = mds::unoptimized_negate3(this->Normal);
			return *this;
		}
	};

	/// <summary>
	/// Generic structure for a collision between two elements reference by an index
	/// </summary>
	struct RSRIDXsAndCollisionResult
	{
		size_t FirstElementIndex;
		size_t SecondElementIndex;
		RSRCollisionResult CollisionResult;
	};

	/// <summary>
	/// a RSRIDXsAndCollisionResult where IDXs represents RSRCollider
	/// </summary>
	using ColIDXsAndColResult = RSRIDXsAndCollisionResult;
	/// <summary>
	/// a RSRIDXsAndCollisionResult where IDXs represent RSRCollidersBody
	/// </summary>
	using ColbodyIDXsAndColResult = RSRIDXsAndCollisionResult;
};
namespace mds
{
	inline std::string _tostring(const RSRush::RSRCollisionResult& InCR)
	{
		return std::format(TEXT("[didCol : {}, P1 : {}, P2 : {}, Norm : {}, Depth : {}]"), 
			InCR.bDidCollide,  mds::_tostring(InCR.P1), mds::_tostring(InCR.P2), mds::_tostring(InCR.Normal), InCR.Depth);
	}
	inline std::string _tostring(const RSRush::RSRIDXsAndCollisionResult& InIDXCR)
	{
		return std::format(TEXT("{{ IDX1 : {}, IDX2 : {}, ColResult : {} }}"),
			InIDXCR.FirstElementIndex, InIDXCR.SecondElementIndex, mds::_tostring(InIDXCR.CollisionResult));
	}
}