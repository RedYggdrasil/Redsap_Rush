///Inspired by https://winter.dev/articles/physics-engine
///-->https://github.com/IainWinter/IwEngine/blob/3e2052855fea85718b7a499a7b1a3befd49d812b/IwEngine/src/physics/Collision/PositionSolver.cpp

#include "App/Physic/Solvers/RSRPositionSolver.h"

#include "MDS/Tools/RMath.h"
#include "App/Data/RSRTransform.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;
using namespace DirectX;

#define lPhys_USE_ORIGINAL false
void RSRPositionSolver::Solve(RSRPhysicManager& pm, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, const double InDeltaTime)
{
	//return;
	std::vector<RSRPhysicBody>& physicBodies = GetPhysicManagerPhysicBodies(pm);

	for (ColbodyIDXsAndColResult& IDXsCol : InIDXsCollisions)
	{
		RSRPhysicBody& firstBody = physicBodies[IDXsCol.FirstElementIndex];
		RSRPhysicBody& secondBody = physicBodies[IDXsCol.SecondElementIndex];


		XMVECTOR3 lNorm = XMLoadFloat3(&IDXsCol.CollisionResult.Normal);
		lNorm = XMVectorScale(lNorm, IDXsCol.CollisionResult.Depth);
#if lPhys_USE_ORIGINAL
		//In this approch each object get the full scaled normal applied if both dynamics
		//Also normal is contact point 1 to 2, but object 1 is scaled normal negative, this seems the wrong way around
		//I suspect original author has depth as a negative value, mine is an absolute value
		float firstRejectApply = (float)(int32_t)!firstBody.Dynamics.bIsDynamic;
		float secondRejectApply = (float)(int32_t)!secondBody.Dynamics.bIsDynamic;

		//This effectively divide by : 
		// 0 Static -> 1, 1 static -> 1, 2 static -> 2)
		lNorm = XMVectorScale(lNorm, 1 / std::max(1.f, firstRejectApply + secondRejectApply));

		//Apply first pos
		XMVECTOR3 newPos = XMLoadFloat3(&firstBody.Transform.GetTransform().Position);
		newPos = newPos - XMVectorScale(lNorm, (1.f - firstRejectApply));
		firstBody.Transform.SetPositon(newPos);

		//Apply second pos
		newPos = XMLoadFloat3(&secondBody.Transform.GetTransform().Position);
		newPos = newPos + XMVectorScale(lNorm, (1.f - secondRejectApply));
		secondBody.Transform.SetPositon(newPos);
#else
		//This is my interpretation of original author's intent, and makes more sens to me
		//In this approch each object get half the scaled normal applied if the other also move, or full is the other is static
		//Also normal is contact point 1 to 2, So I apply positive the normal case 1, and negative case 2 (from contact 2 to 1)

		float firstDynamic =	firstBody.Dynamics.bIsDynamic ? 1.f : 0.f;
		float secondDynamic =	secondBody.Dynamics.bIsDynamic ? 1.f : 0.f;

		//This effectively divide by : 
		// 0 dynamic -> 1, 1 dynamic -> 1, 2 dynamic -> 2)
		lNorm = XMVectorScale(lNorm, 1 / std::max(1.f, firstDynamic + secondDynamic));


		//Apply first pos
		/*[2]*/XMVECTOR3 newPos = XMLoadFloat3(&firstBody.Transform.GetTransform().Position);
		newPos = newPos + XMVectorScale(lNorm, firstDynamic);
		firstBody.Transform.SetPositon(newPos);

		//Apply second pos
		newPos = XMLoadFloat3(&secondBody.Transform.GetTransform().Position);
		newPos = newPos - XMVectorScale(lNorm, secondDynamic);
		secondBody.Transform.SetPositon(newPos);

#endif

	}
}
#undef lPhys_USE_ORIGINAL

RSRPositionSolver::RSRPositionSolver()
 :RSRSolver(RSRPositionSolver::SOLVER_TYPE)
{
}
RSRPositionSolver::~RSRPositionSolver()
{
}
