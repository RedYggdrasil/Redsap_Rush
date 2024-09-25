#pragma once
#include "Gen_App/Config/AppConfig.h"
#include "MDS/Tools/RMath.h"

#include <vector>
#include <d3d12.h>
#include <directxcollision.h>
#include "MDS/Tools/RCoordinate.h"
#include "MDS/Tools/RAllocator.h"
#include "App/Data/Physic/RSRCollider.h"
#include "App/Data/Physic/RSRCollisionResult.h"
#include "App/Data/Physic/RSRColMask.h"

namespace RSRush
{
	struct alignas(16) RSRCollidersBody
	{
	public:
		RSRColBehavior Behavior;

		DirectX::BoundingBox AABB;

		/// <summary>
		/// Heap array of Colliders
		/// </summary>
		std::vector<RSRCollider, RAligndAlloc16<RSRCollider>> Colliders;

	public:
		void ComputeAABB();

	public:
		static void ComputeWorldFromLocal(const DirectX::XMMATRIX& InOuterWorldMatrix, const RSRCollidersBody& InLocal, RSRCollidersBody* OutWorld);
		static void ComputeLocalFromWorld(const DirectX::XMMATRIX& InOuterWorldMatrix, const RSRCollidersBody& InWorld, RSRCollidersBody* OutLocal);

	public:
		bool Intersects(const RSRCollidersBody& InOther) const { return RSRCollidersBody::Intersects(*this, InOther); }
		static bool Intersects(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight);
		
		RSRCollisionType FillCollidesOrOverlaps(const RSRCollidersBody& InOther, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision) const { return RSRCollidersBody::FillCollidesOrOverlaps(*this, InOther, OutIDXCollisionResults, bFillOnlyDeepestCollision); }
		static RSRCollisionType FillCollidesOrOverlaps(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision);

		static bool TestCollidersForOneOverlap(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight);
		static ColIDXsAndColResult TestCollidersForDeepestCollision(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight);
		static size_t TestCollidersForCollisions(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight, std::vector<ColIDXsAndColResult>& InOutColResults);
	public:
		RSRCollidersBody() = default;
		RSRCollidersBody(const RSRColBehavior& InColBehavior);
		RSRCollidersBody(const RSRCollidersBody& InOther);
		RSRCollidersBody& operator=(const RSRCollidersBody& InOther);

#if DEBUG_PHYSIC
	public:
		bool DebugDraw(ID3D12GraphicsCommandList7* InUploadCommandList) const;
#endif
	};
	struct alignas(16) RSRCollidersBodyLWPair
	{
	protected:
		alignas(16) RSRCollidersBody m_localPB;
		alignas(16) RSRCollidersBody m_worldPB;
		bool m_bLocalToWorldDirty = false;
	public:
		inline RSRCollidersBody& DirtyGetLocal() { DirtyLtoW(); return m_localPB; }
		inline const RSRCollidersBody& GetLocal() const { return m_localPB; }

		inline const RSRCollidersBody& GetWorld() const { return m_worldPB; }

		inline void DirtyLtoW() { m_bLocalToWorldDirty = true; }
		inline bool IsLtoWDirty() { return m_bLocalToWorldDirty; }

		void RecomputeWorld(const DirectX::XMMATRIX& InOuterWorldMatrix, bool bInForceDirty = false);
	public:
		RSRCollidersBodyLWPair() = default;
		RSRCollidersBodyLWPair(const RSRColBehavior& InColBehavior);
		static RSRCollidersBodyLWPair FromLocalCollidersBody(const RSRCollidersBody& InLocalColliders, const DirectX::XMMATRIX& InOuterWorldMatrix);
	};
};