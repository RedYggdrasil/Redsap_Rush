#pragma once

#include <functional>
#include <d3d12.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "MDS/Defines/MDS.h"
#include "MDS/Tools/REnum.h"
#include "MDS/Tools/RMath.h"
#include "App/Data/Physic/RSRCollider.h"
#include "App/Data/Physic/RSRCollisionResult.h"

using namespace DirectX;
using namespace RSRush;

namespace RSRCols
{
	using FindContactFunc = std::function<RSRCollisionResult(const RSRCollider*, const RSRCollider*)>;

	static RSRCollisionResult OriboxOriboxCol(const RSRCollider* InLeftOribox, const RSRCollider* InRightOribox);
	static RSRCollisionResult OriboxSphereCol(const RSRCollider* InLeftSphere, const RSRCollider* InRightOriBox);
	static RSRCollisionResult SphereSphereCol(const RSRCollider* InLeftSphere, const RSRCollider* InRightSphere);

	static const FindContactFunc contactFuncs[2][2] =
	{
			// OriBox             Sphere
/*OriBox*/ { OriboxOriboxCol	, OriboxSphereCol	},
/*Sphere*/ { nullptr			, SphereSphereCol	}
	};

	static RSRCollisionResult ResolveCollision(const RSRCollider* InLeftCollider, const RSRCollider* InRightCollider)
	{
		bool bNeedSwapping = InLeftCollider->Type > InRightCollider->Type;
		
		if (bNeedSwapping)
		{
			std::swap(InLeftCollider, InRightCollider);
		}

		RSRCollisionResult result = contactFuncs[mds::UT_cast(InLeftCollider->Type)][mds::UT_cast(InRightCollider->Type)](InLeftCollider, InRightCollider);
		if (bNeedSwapping)
		{
			result.InvertPointsSelf();
		}
		return result;
	}

	//using FindContactFunc = RSRCollisionResult(*)( const RSRCollider*, const RSRCollider*);

	RSRCollisionResult OriboxOriboxCol(const RSRCollider* InLeftSphere, const RSRCollider* InRightOriBox)
	{
		RSRCollisionResult result;
		result.bDidCollide = false;
		return result;
	};

	inline XMVECTOR3 XM_CALLCONV ClosestPointOnOBB(const BoundingOrientedBox& InOBB, FXMVECTOR InPoint, bool& bOutIsSphereCenterInsideBox) noexcept
	{
		// Get the box center, extents, and orientation
		/*[0]*/ XMVECTOR obbCenter = XMLoadFloat3(&InOBB.Center);
		/*[1]*/ XMVECTOR obbExtents = XMLoadFloat3(&InOBB.Extents);
		/*[2]*/ XMVECTOR obbOrientation = XMLoadFloat4(&InOBB.Orientation);

		// Transform the point to the box's local space
		/*[3]*/ XMVECTOR localPoint = XMVector3InverseRotate(InPoint - obbCenter, obbOrientation);

		// Clamp the point to the box's extents
		/*[4]*/ XMVECTOR clampedPoint = XMVectorClamp(localPoint, -obbExtents, obbExtents);

		/*[5]*/XMVECTOR insideBox = XMVectorEqual(localPoint, clampedPoint);

		// If the point is inside the box, move it to the surface
		bOutIsSphereCenterInsideBox = XMVector3EqualInt(insideBox, XMVectorTrueInt());
		if (bOutIsSphereCenterInsideBox) {
			// Find the axis with the largest distance to the surface
			/*[6]*/XMVECTOR distances = XMVectorAbs(localPoint) - obbExtents;
			/*[7]*/XMVECTOR maxDistance = XMVectorMax(distances, XMVectorSplatY(distances));
			maxDistance = XMVectorMax(maxDistance, XMVectorSplatZ(distances));

			// Determine the sign of each component
			/*[8]*/XMVECTOR sign = XMVectorSelect(g_XMZero, XMVectorDivide(localPoint, XMVectorAbs(localPoint)), XMVectorNotEqual(localPoint, XMVectorZero()));

			// Move the point to the surface along the axis with the largest distance
			clampedPoint = XMVectorSelect(clampedPoint, XMVectorMultiplyAdd(sign, obbExtents, XMVectorZero()), XMVectorEqual(distances, maxDistance));
			
		}

		// Transform the clamped point back to world space
		/*[9]*/XMVECTOR closestPoint = XMVector3Rotate(clampedPoint, obbOrientation) + obbCenter;

		return closestPoint;
	}

	RSRCollisionResult OriboxSphereCol(const RSRCollider* InLeftOriBox, const RSRCollider* InRightSphere)
	{
		assert(InLeftOriBox->Type == RSRColliderType::RectCollider && InRightSphere->Type == RSRColliderType::SphereCollider && " Wrong call to 'OriboxSphereCol'");

		RSRCollisionResult result;
		result.bDidCollide = false;

		const BoundingOrientedBox& lOBB = InLeftOriBox->RectData;
		const BoundingSphere& rBS = InRightSphere->SphereData;

		//Load sphere (Right)
		XMVECTOR3 rCenter = XMLoadFloat3(&rBS.Center);
		XMVECTOR1 rRadius = XMVectorReplicatePtr(&rBS.Radius);

		bool bIsSphereCenterInsideBox = false;
		//Find closes point on the box surface to the sphere
		XMVECTOR3 lSurfacePoint = ClosestPointOnOBB(lOBB, rCenter, /*Out*/bIsSphereCenterInsideBox);

		//Contact point to Shpere Center
		XMVECTOR3 SphereToOBBSurface = lSurfacePoint - rCenter;
		XMVECTOR1 SphereToOBBSurfaceLenght = XMVector3LengthSq(SphereToOBBSurface);


		//Special case, sphere sits on the rectangle's surface
		if (XMVectorGetX(SphereToOBBSurfaceLenght) <= std::numeric_limits<float>::epsilon())
		{
			bIsSphereCenterInsideBox = false;
			result.bDidCollide = true;
			result.Depth = XMVectorGetX(rRadius);
			XMStoreFloat3(&result.P1, lSurfacePoint);

			SphereToOBBSurface = XMLoadFloat3(&lOBB.Center) -rCenter;

			SphereToOBBSurface = XMVector3Normalize(SphereToOBBSurface);
			XMStoreFloat3(&result.Normal, SphereToOBBSurface);

			XMStoreFloat3(&result.P2, XMVectorMultiplyAdd(SphereToOBBSurface, rRadius, rCenter));
			return result;
		}

		//Box can have contacter point farther from center than radius because projected onto square edge, but but still be fully inside the square
		if (!bIsSphereCenterInsideBox && XMVector4Greater(SphereToOBBSurfaceLenght, XMVectorMultiply(rRadius, rRadius)))
		{
			result.bDidCollide = false;
			return result;
		}
		SphereToOBBSurfaceLenght = XMVectorSqrt(SphereToOBBSurfaceLenght);

		result.bDidCollide = true;
		XMStoreFloat3(&result.P1, lSurfacePoint);

		SphereToOBBSurface = XMVector3Normalize(SphereToOBBSurface);
		if (bIsSphereCenterInsideBox)
		{
			XMStoreFloat3(&result.Normal, XMVectorScale(SphereToOBBSurface, -1.f));
		}
		else
		{
			XMStoreFloat3(&result.Normal, SphereToOBBSurface);
		}

		// If sphere center is inside the square, we are looking for a point on sphere surface oposite to square contact point
		// Negating the radius sever 2 purpuse : 
		//								- to find the correct direction of the other point (on the other part of the sphere)
		//								- to correctly compute depth (will substract by - (-radius)  in case of sphere center inside box, 
		//									effectively resulting in  lenght(sphereCenter <-> contact distance) + radius
		if (bIsSphereCenterInsideBox)
		{
			result.Depth = XMVectorGetX(rRadius + SphereToOBBSurfaceLenght);
			rRadius = XMVectorScale(rRadius, -1.f); 
		}
		else
		{
			result.Depth = XMVectorGetX(rRadius - SphereToOBBSurfaceLenght);
		}

		XMVECTOR3 rSurfacePoint = XMVectorMultiplyAdd(SphereToOBBSurface, rRadius, rCenter);
		XMStoreFloat3(&result.P2, rSurfacePoint);

#if _DEBUG
		assert(result.Depth >= 0.f && "Depth must always be superior or equal to 0");
#endif
		return result;
	};

	RSRCollisionResult SphereSphereCol(const RSRCollider* InLeftSphere, const RSRCollider* InRightSphere)
	{
		assert(InLeftSphere->Type == RSRColliderType::SphereCollider && InRightSphere->Type == RSRColliderType::SphereCollider && " Wrong call to 'SphereSphereCol'");
		RSRCollisionResult result;
		result.bDidCollide = false;

		const BoundingSphere& lBS = InLeftSphere->SphereData;
		const BoundingSphere& rBS = InRightSphere->SphereData;
		
		//Load "Left"
		XMVECTOR lCenter = XMLoadFloat3(&lBS.Center);
		XMVECTOR lRadius = XMVectorReplicatePtr(&lBS.Radius);
		
		//Load "Left"Right"
		XMVECTOR rCenter = XMLoadFloat3(&rBS.Center);
		XMVECTOR rRadius = XMVectorReplicatePtr(&rBS.Radius);

		XMVECTOR RcenterLcenterVec = XMVectorSubtract(lCenter, rCenter);

		// [RcenterLcenterDist] Distance Right Center to Left Center, squared
		XMVECTOR RcenterLcenterDist = XMVector3LengthSq(RcenterLcenterVec);

		// Sum of Radiuses _THEN_ Collision Normal
		XMVECTOR RadSum_THEN_ColNorm = XMVectorAdd(lRadius, rRadius);

		float distsquared = XMVectorGetX(RcenterLcenterDist);
		//No Collision
		if (distsquared > XMVectorGetX(XMVectorMultiply(RadSum_THEN_ColNorm, RadSum_THEN_ColNorm)))
		{
			return result;
		}

		result.bDidCollide = true;

		//Degenerate case, sphere have the same center
		if (distsquared < 0.00001f)
		{
			result.Depth = XMVectorGetX(RadSum_THEN_ColNorm);
			RadSum_THEN_ColNorm = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		}
		else
		{
			// [RcenterLcenterDist] Distance Right Center to Left Center, exact
			RcenterLcenterDist = XMVectorSqrt(RcenterLcenterDist);
			//The center distance should always be shorter (or equal) than the sum of radius if collided
			result.Depth = /*std::abs*/-(XMVectorGetX(XMVectorSubtract(RcenterLcenterDist, RadSum_THEN_ColNorm)));
			RadSum_THEN_ColNorm = XMVector3Normalize(RcenterLcenterVec);
		}

		//--------------------
		//Much later comment
		//Aren't we returning the normal from Shape 2 toward shape 1 there ? is it intended ?
		//---------------------
		XMStoreFloat3(&result.Normal, RadSum_THEN_ColNorm);
		XMStoreFloat3(&result.P1, XMVectorMultiplyAdd(XMVectorNegate(RadSum_THEN_ColNorm), lRadius, lCenter));
		XMStoreFloat3(&result.P2, XMVectorMultiplyAdd(RadSum_THEN_ColNorm, rRadius, rCenter));
		return result;
	};
};