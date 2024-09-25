#pragma once
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"

#include <algorithm>
#include <format>
#include <Tracy.hpp>

using namespace DirectX;
namespace RSRush
{
	class RSRPhysicManager_Dynamics
	{
	public:
		STATIC_CLASS_R5(RSRPhysicManager_Dynamics);
		friend class RSRPhysicManager;
	private:

		static void ComputeDynamics(RSRPhysicManager& pm, const double InDeltaTime)
		{
			float fInDeltaTime = (float)InDeltaTime;
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}
			XMVECTOR3 lGobalGravity = XMLoadFloat3(&pm.m_physicContext.GlobalGravity);

			size_t count = pm.m_physicalBodies.size();

			for (size_t i = 0; i < count; ++i)
			{
				RSRPhysicBody& physicalBody = pm.m_physicalBodies[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					RSRDynamics& dynamics = physicalBody.Dynamics;
					XMVECTOR3 lForce = XMLoadFloat3(&dynamics.Force);
					XMVECTOR3 lVelocity = XMLoadFloat3(&dynamics.Velocity);
					XMVECTOR3 lPosition = XMLoadFloat3(&physicalBody.Transform.GetTransform().Position);

					lForce += XMVectorScale(lGobalGravity, dynamics.ComputeMass());
					lVelocity += XMVectorScale(lForce, dynamics.InverseMass * fInDeltaTime);
					lPosition += XMVectorScale(lVelocity, fInDeltaTime);

					dynamics.Force = XMFLOAT3{ 0.f, 0.f, 0.f };
					XMStoreFloat3(&dynamics.Velocity, lVelocity);
					physicalBody.Transform.SetPositon(lPosition);

				}
			}
		}

		static void ApplyGravityToForce(RSRPhysicManager& pm, const double InDeltaTime)
		{
			float fInDeltaTime = (float)InDeltaTime;
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}
			XMVECTOR3 lGobalGravity = XMLoadFloat3(&pm.m_physicContext.GlobalGravity);

			size_t count = pm.m_physicalBodies.size();

			for (size_t i = 0; i < count; ++i)
			{
				RSRPhysicBody& physicalBody = pm.m_physicalBodies[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					RSRDynamics& dynamics = physicalBody.Dynamics;
					XMVECTOR3 lForce = XMLoadFloat3(&dynamics.Force);

					lForce += XMVectorScale(lGobalGravity, dynamics.ComputeMass());
					XMStoreFloat3(&dynamics.Force, lForce);

				}
			}
		}

		/// <summary>
		/// Apply all forces to velocities and positions, reset forces for this frame
		/// </summary>
		/// <param name="pm"></param>
		/// <param name="InDeltaTime"></param>
		static void ApplyForcesToObjets(RSRPhysicManager& pm, const double InDeltaTime)
		{
			float fInDeltaTime = (float)InDeltaTime;
			if (fInDeltaTime < std::numeric_limits<float>::epsilon())
			{
				return;
			}

			size_t count = pm.m_physicalBodies.size();

			for (size_t i = 0; i < count; ++i)
			{
				RSRPhysicBody& physicalBody = pm.m_physicalBodies[i];
				if (physicalBody.Dynamics.bIsDynamic)
				{
					RSRDynamics& dynamics = physicalBody.Dynamics;
					XMVECTOR3 lForce = XMLoadFloat3(&dynamics.Force);
					XMVECTOR3 lVelocity = XMLoadFloat3(&dynamics.Velocity);
					XMVECTOR3 lPosition = XMLoadFloat3(&physicalBody.Transform.GetTransform().Position);

					lVelocity += XMVectorScale(lForce, dynamics.InverseMass * fInDeltaTime);
					lPosition += XMVectorScale(lVelocity, fInDeltaTime);

					dynamics.Force = XMFLOAT3{ 0.f, 0.f, 0.f };
					XMStoreFloat3(&dynamics.Velocity, lVelocity);
					physicalBody.Transform.SetPositon(lPosition);

				}
			}
		}
	};
};