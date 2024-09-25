///Inspired by https://winter.dev/articles/physics-engine
///-->https://github.com/IainWinter/IwEngine/blob/3e2052855fea85718b7a499a7b1a3befd49d812b/IwEngine/src/physics/Dynamics/ImpulseSolver.cpp

#include "App/Physic/Solvers/RSRImpulseSolver.h"

#include "MDS/Tools/RMath.h"
#include "App/Data/RSRTransform.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"


using namespace RSRush;
using namespace DirectX;

void RSRImpulseSolver::Solve(RSRPhysicManager& pm, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, const double InDeltaTime)
{
	std::vector<RSRPhysicBody>& physicBodies = GetPhysicManagerPhysicBodies(pm);

	for (ColbodyIDXsAndColResult& IDXsCol : InIDXsCollisions)
	{
		/// Load values ///

		/*[0] Temp registery*/
		RSRPhysicBody& firstBody	= physicBodies[IDXsCol.FirstElementIndex];
		RSRPhysicBody& secondBody	= physicBodies[IDXsCol.SecondElementIndex];

		/*[1]*/XMVECTOR3 lFirstVelocity		= XMLoadFloat3(&firstBody.Dynamics.Velocity);
		/*[2]*/XMVECTOR3 lSecondVelocity	= XMLoadFloat3(&secondBody.Dynamics.Velocity);

		//relative velocity of how fast the object are moving relative to the other
		// Exemple : Because A is moving in +X and B -X, B is going "twice" as fast toward -X in realation to X
		// EX : [ A ->(X:+1)  (X:-1)<- B ]  ==> [ -1 - +1 ] ==> [x:-2]
		/*[3]*/XMVECTOR3 lRelativeVelocity = lSecondVelocity - lFirstVelocity;

		//The normal from contact points A->B
		/*[4]*/XMVECTOR3 lNormal = XMLoadFloat3(&IDXsCol.CollisionResult.Normal);
		
		//Speed alongside the contact normal
		/*[5]*/XMVECTOR1 contactNormalSpeed = XMVector3Dot(lRelativeVelocity, lNormal);

		float firstInvMass	= firstBody.Dynamics.InvMassFiniteNonZero()		? firstBody	.Dynamics.InverseMass : 1.f;
		float secondInvMass = secondBody.Dynamics.InvMassFiniteNonZero()	? secondBody.Dynamics.InverseMass : 1.f;

		
		/// Start computing Impulse ///
		if (XMVectorGetX(contactNormalSpeed) < 0.f)
			continue;
		//Bounciness, or elesticity of elements
		float e = (firstBody.Dynamics.bIsDynamic ? 1.f : 1.0f)
			* (secondBody.Dynamics.bIsDynamic ? 1.0f : 1.f);

		/*[6]*/XMVECTOR1 j = XMVectorScale(XMVectorScale(contactNormalSpeed, -(1.0f + e)), 1.f / (firstInvMass + secondInvMass));

		/*[7]*/XMVECTOR1 impulse = XMVectorMultiply(j, lNormal);

		if (firstBody.Dynamics.bIsDynamic)
		{
			lFirstVelocity -= impulse * firstInvMass;
			XMStoreFloat3(&firstBody.Dynamics.Velocity, lFirstVelocity);
		}

		if (secondBody.Dynamics.bIsDynamic)
		{
			lSecondVelocity += impulse * secondInvMass;
			XMStoreFloat3(&secondBody.Dynamics.Velocity, lSecondVelocity);
		}
	}
}

RSRush::RSRImpulseSolver::RSRImpulseSolver()
:RSRSolver(RSRImpulseSolver::SOLVER_TYPE)
{
}

RSRImpulseSolver::~RSRImpulseSolver()
{
}