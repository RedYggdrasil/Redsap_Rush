#pragma once

#define NOMINMAX
//#include "MDS/Defines/MDS.h"
#include "App/Data/Physic/RSRPhysicBodyID.h"
#include "App/Data/Physic/RSRCollidersBody.h"
#include <memory>

namespace RSRush
{
	struct alignas(16) RSRDynamics
	{
		DirectX::XMFLOAT3 Velocity;
		DirectX::XMFLOAT3 Force;
		//Most 
		float InverseMass;
		bool bIsDynamic;
	public:
		static inline constexpr float InvertMass(const float InMass)
		{
			if (InMass > std::numeric_limits<float>::epsilon()) { return 1.f / InMass; }
			return 0.f;
		}
		static inline constexpr float RevertInvMass(const float InInverseMass)
		{
			if (InInverseMass > std::numeric_limits<float>::epsilon()) { return 1.f / InInverseMass; }
			return std::numeric_limits<float>::infinity();
		}
		inline constexpr float ComputeMass() const { return RSRDynamics::RevertInvMass(this->InverseMass); }
		inline constexpr bool InvMassFiniteNonZero() const 
		{ return this->InverseMass > std::numeric_limits<float>::epsilon() && this->InverseMass < std::numeric_limits<float>::max(); }
		inline void SetMass(const float InMass) { this->InverseMass = RSRDynamics::InvertMass(InMass); }
	};
	inline RSRDynamics NoDynamics(float InMass = 0.f)
	{
		return {
			.Velocity = DirectX::XMFLOAT3(),
			.Force = DirectX::XMFLOAT3(),
			.InverseMass = RSRDynamics::InvertMass(InMass),
			.bIsDynamic = false
		};
	}
	inline RSRDynamics Dynamics(float InMass = 0.f)
	{
		return {
			.Velocity = DirectX::XMFLOAT3(),
			.Force = DirectX::XMFLOAT3(),
			.InverseMass = RSRDynamics::InvertMass(InMass),
			.bIsDynamic = true
		};
	}

	struct alignas(16) RSRDynamicsReg
	{
		DirectX::XMVECTOR3 Velocity;
		DirectX::XMVECTOR3 Force;
		DirectX::XMVECTOR1 InverseMass;
		bool bIsDynamic;
	};

	inline RSRDynamicsReg XM_CALLCONV XMLoadRSRDynamicsReg(const RSRDynamics* InPSource) noexcept
	{
		assert(InPSource);
		return RSRDynamicsReg
		{
			.Velocity = DirectX::XMLoadFloat3(&InPSource->Velocity),
			.Force = DirectX::XMLoadFloat3(&InPSource->Force),
			.InverseMass = DirectX::XMLoadFloat(&InPSource->InverseMass),
			.bIsDynamic = InPSource->bIsDynamic
		};
	};
	inline void XM_CALLCONV XMStoreRSRDynamics(RSRDynamics* OutPDestination, const RSRDynamicsReg& InSource) noexcept
	{
		assert(OutPDestination);
		DirectX::XMStoreFloat3(&OutPDestination->Velocity, InSource.Velocity);
		DirectX::XMStoreFloat3(&OutPDestination->Force, InSource.Force);
		DirectX::XMStoreFloat(&OutPDestination->InverseMass, InSource.InverseMass);
		OutPDestination->bIsDynamic = InSource.bIsDynamic;
	};

	class RSRIPhysicalEntity;
	struct RSRPhysicBodyKey
	{
		std::weak_ptr<RSRIPhysicalEntity> SelfEntity;
		RSRPhysicBodyID physicBodyID;
		bool bHasBeenRegistered;
	};
	struct alignas(16) RSRPhysicBody
	{
		RSRTransformMatrix Transform;
		RSRCollidersBodyLWPair Colliders;
		RSRDynamics Dynamics;
		std::weak_ptr<RSRIPhysicalEntity> Entity;
	};
#define INLINE_CONSTRUCT_UNSET_PHYSIC_BODY(){\
		.Transform = RSRTransformMatrix(mds::TRS_IDENTITY),\
		.Colliders = RSRCollidersBodyLWPair(WORLD_STATIC),\
		.Dynamics = NoDynamics(0.f),\
		.Entity = std::weak_ptr<RSRIPhysicalEntity>()\
	}
	static const RSRPhysicBody UNSET_PHYSIC_BODY INLINE_CONSTRUCT_UNSET_PHYSIC_BODY();
};