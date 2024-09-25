#pragma once

#include "App/Data/Physic/RSRPhysicBody.h"
#include <memory>

namespace RSRush
{
	class RSRIPhysicalEntity
	{
	private:
		RSRPhysicBody m_lastResolvedPhysicBody;
	protected:
		const RSRPhysicBody& GetLastResolvedPhysicBody() const { return m_lastResolvedPhysicBody; }
	protected:
		struct TransformedObjectData
		{
			RSRTransformMatrix NewTransform;
			bool bHasNewTransform;
		};
		RSRPhysicBodyKey m_physicalBodyKey;
		TransformedObjectData m_transformedPhysicalObjectData;

	public:
		inline void SetLastKnownIndex(const size_t InNewLastKnownIndex) { m_physicalBodyKey.LastKnownIndex = InNewLastKnownIndex; }
		inline void SetHasBeenRegistered(const bool InbHasBeenRegistered, const RSRPhysicBody& InInitialRegisteredPhysicBody) 
		{ m_physicalBodyKey.bHasBeenRegistered = InbHasBeenRegistered; m_lastResolvedPhysicBody = InInitialRegisteredPhysicBody; }
		inline bool GetHasBeenRegistered() const { return m_physicalBodyKey.bHasBeenRegistered; }

		const RSRPhysicBodyKey& GetKey() const { return m_physicalBodyKey; }
		RSRPhysicBodyKey& GetEditKey() { return m_physicalBodyKey; }
	private:
		//Private because we are not suppose to get Physical Body during certain timeframes
		RSRush::RSRPhysicBody* GetPhysicalBodyPtr() const;
	public:
		virtual void OnPhysicalPassEnded(const RSRPhysicBody& InResultPhysicalBody) {};
		/// <summary>
		/// Called to update m_WSPhysicalBody to new position this physical pass
		/// </summary>
		/// <param name="InDeltaTime">Delta time of the physical pass (may differ from frame delta time)</param>
		virtual void OnPhysicalPrePass(double InDeltaTime);

		virtual void OnOverlapWith(RSRIPhysicalEntity* InOther) {};
		virtual void OnPhysicPassEnded(const RSRPhysicBody& InResolvedPhysicBody) { m_lastResolvedPhysicBody = InResolvedPhysicBody; };

		RSRIPhysicalEntity(const RSRIPhysicalEntity& InOther) = delete;
		RSRIPhysicalEntity(RSRIPhysicalEntity&& InOther) noexcept = delete;
		RSRIPhysicalEntity& operator=(const RSRIPhysicalEntity& InOther) = delete;
		RSRIPhysicalEntity& operator=(RSRIPhysicalEntity&& InOther) = delete;
	protected:
		RSRIPhysicalEntity();
		void OnPhysicalObjectRequestMove(const RSRTransformMatrix& InNewTransform);
	public:
		virtual void SetSelfReference(std::weak_ptr<RSRIPhysicalEntity> InThisPtr)
		{
			m_physicalBodyKey.SelfEntity = InThisPtr;
		};

	};
}