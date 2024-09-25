#include "App/Data/Physic/RSRCollidersBody.h"
#include "App/Tools/RSRLog.h"
#include "MDS/Tools/RMath.h"

#include "App/Data/Physic/RSRCollisionFuncs.hpp"


using namespace RSRush;
using namespace DirectX;

void RSRCollidersBodyLWPair::RecomputeWorld(const DirectX::XMMATRIX& InOuterWorldMatrix, bool bInForceDirty)
{
	if (bInForceDirty || m_bLocalToWorldDirty)
	{
		RSRCollidersBody::ComputeWorldFromLocal(InOuterWorldMatrix, this->m_localPB, &this->m_worldPB);
		m_bLocalToWorldDirty = false;
	}
}

RSRush::RSRCollidersBodyLWPair::RSRCollidersBodyLWPair(const RSRColBehavior& InColBehavior)
	: m_localPB(InColBehavior), m_worldPB(InColBehavior)
{
}

RSRCollidersBodyLWPair RSRush::RSRCollidersBodyLWPair::FromLocalCollidersBody(const RSRCollidersBody& InLocalColliders, const DirectX::XMMATRIX& InOuterWorldMatrix)
{
	RSRCollidersBodyLWPair result = RSRCollidersBodyLWPair(InLocalColliders.Behavior);
	RSRCollidersBody* localColliders = &result.DirtyGetLocal();
	(*localColliders) = InLocalColliders;

	localColliders->ComputeAABB();
	result.RecomputeWorld(InOuterWorldMatrix, true);
	return result;
}

void RSRush::RSRCollidersBody::ComputeAABB()
{
	if (Colliders.size() < 1)
	{
		AABB = BoundingBox({ 0.f,0.f ,0.f }, { 0.f,0.f ,0.f });
		return;
	}
	AABB = Colliders[0].ComputeBoundBox();
	for (size_t i = 1; i < Colliders.size(); ++i)
	{
		Colliders[i].ExtendBoundBox(&AABB);
	}
}

void RSRCollidersBody::ComputeWorldFromLocal(const DirectX::XMMATRIX& InOuterWorldMatrix, const RSRCollidersBody& InLocal, RSRCollidersBody* OutWorld)
{
	(*OutWorld) = InLocal;
	for (size_t i = 0, length = InLocal.Colliders.size(); i < length; ++i)
	{
		OutWorld->Colliders[i] = InLocal.Colliders[i].TransformCopy(InOuterWorldMatrix);
	}
	OutWorld->ComputeAABB();
}

void RSRCollidersBody::ComputeLocalFromWorld(const DirectX::XMMATRIX& InOuterWorldMatrix, const RSRCollidersBody& InWorld, RSRCollidersBody* OutLocal)
{
	(*OutLocal) = InWorld;

	// Compute the inverse of the parent's world transformation matrix
	XMMATRIX invParentWorld = XMMatrixInverse(NULL, InOuterWorldMatrix);

	for (size_t i = 0, length = InWorld.Colliders.size(); i < length; ++i)
	{
		// Compute the inverse of the parent's world transformation matrix
		OutLocal->Colliders[i] = InWorld.Colliders[i].TransformCopy(invParentWorld);
	}
	OutLocal->ComputeAABB();
}

bool RSRush::RSRCollidersBody::Intersects(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight)
{
	if (!InLeft.Behavior.Interact(InRight.Behavior))
	{
		return false;
	}

	if (!InLeft.AABB.Intersects(InRight.AABB))
	{
		return false;
	}
	for (const RSRCollider& leftCol : InLeft.Colliders)
	{
		for (const RSRCollider& rightCol : InRight.Colliders)
		{
			if (leftCol.OverlapWith(rightCol))
			{
				return true;
			}
		}
	}
	return false;
}

RSRCollisionType RSRush::RSRCollidersBody::FillCollidesOrOverlaps(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight, std::vector<ColIDXsAndColResult>& OutIDXCollisionResults, bool bFillOnlyDeepestCollision)
{
	//Collision always take precedence over overlaps
	bool bCanCollide = InLeft.Behavior.Collide(InRight.Behavior);
	if (!bCanCollide)
	{
		if (!InLeft.Behavior.Interact(InRight.Behavior))
		{
			return RSRCollisionType::None; //Cannot collide or overlaps, so return
		}
	}
	if (!InLeft.AABB.Intersects(InRight.AABB))
	{
		return RSRCollisionType::None; //Objects are too far away
	}
	if (!bCanCollide)
	{
		return TestCollidersForOneOverlap(InLeft, InRight) ? RSRCollisionType::Overlap : RSRCollisionType::None;
	}
	else
	{
		if (bFillOnlyDeepestCollision)
		{
			ColIDXsAndColResult temp = TestCollidersForDeepestCollision(InLeft, InRight);
			if (temp.CollisionResult.bDidCollide)
			{
				OutIDXCollisionResults.push_back(temp);
				return RSRCollisionType::Collide;
			}
			return RSRCollisionType::None;
		}
		else
		{
			size_t nbNewCollisions = TestCollidersForCollisions(InLeft, InRight, /*InOut*/OutIDXCollisionResults);
			return (nbNewCollisions > 0) ? RSRCollisionType::Collide : RSRCollisionType::None;
		}
	}
}

bool RSRush::RSRCollidersBody::TestCollidersForOneOverlap(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight)
{
	for (const RSRCollider& leftCol : InLeft.Colliders)
	{
		for (const RSRCollider& rightCol : InRight.Colliders)
		{
			if (leftCol.OverlapWith(rightCol))
			{
				return true;
			}
		}
	}
	return false;
}

ColIDXsAndColResult RSRush::RSRCollidersBody::TestCollidersForDeepestCollision(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight)
{
	ColIDXsAndColResult result;
	result.CollisionResult.bDidCollide = false;
	result.CollisionResult.Depth = 0.f;

	RSRCollisionResult temp;
	
	for (size_t i = 0; i < InLeft.Colliders.size(); ++i)
	{
		for (size_t j = 0; j < InRight.Colliders.size(); ++j)
		{
			temp = RSRCols::ResolveCollision(&InLeft.Colliders[i], &InRight.Colliders[j]);
			if (temp.bDidCollide)
			{
				if (
					!result.CollisionResult.bDidCollide ||
					result.CollisionResult.Depth < temp.Depth
					)
				{
					result.FirstElementIndex = i;
					result.SecondElementIndex = j;
					result.CollisionResult = temp;
				}
			}
		}
	}
	return result;
}

size_t RSRush::RSRCollidersBody::TestCollidersForCollisions(const RSRCollidersBody& InLeft, const RSRCollidersBody& InRight, std::vector<ColIDXsAndColResult>& InOutColResults)
{
	size_t nbCollisions = 0;
	RSRCollisionResult temp;
	for (size_t i = 0; i < InLeft.Colliders.size(); ++i)
	{
		for (size_t j = 0; j < InRight.Colliders.size(); ++j)
		{
			temp = RSRCols::ResolveCollision(&InLeft.Colliders[i], &InRight.Colliders[j]);
			if (temp.bDidCollide)
			{
				++nbCollisions;
				InOutColResults.push_back
				(
					ColIDXsAndColResult
					{
						.FirstElementIndex = i,
						.SecondElementIndex = j,
						.CollisionResult = temp
					}
				);
			}
		}
	}
	return nbCollisions;
}


RSRush::RSRCollidersBody::RSRCollidersBody(const RSRColBehavior& InColBehavior)
	:Behavior(InColBehavior)
{
}

RSRush::RSRCollidersBody::RSRCollidersBody(const RSRCollidersBody& InOther)
	: Behavior(InOther.Behavior), AABB(InOther.AABB), Colliders(InOther.Colliders.size())
{
	std::copy(InOther.Colliders.begin(), InOther.Colliders.end(), Colliders.begin());
}

RSRCollidersBody& RSRush::RSRCollidersBody::operator=(const RSRCollidersBody& InOther)
{
	this->Behavior = InOther.Behavior;
	this->AABB = InOther.AABB;
	this->Colliders.resize(InOther.Colliders.size());

	std::copy(InOther.Colliders.begin(), InOther.Colliders.end(), this->Colliders.begin());
	return *this;
}

#if DEBUG_PHYSIC
bool RSRush::RSRCollidersBody::DebugDraw(ID3D12GraphicsCommandList7* InUploadCommandList) const
{
	bool allSucessfull = true;
	if (((UINT64)this->Behavior.SelfMask & (UINT64)DEBUG_PHYSIC_MASK) != 0)
	{
#if DEBUG_PHYSIC_AABB
		allSucessfull = RSRCollider::DebugDrawBox(AABB, InUploadCommandList) && allSucessfull;
#endif
		for (const RSRCollider& col : Colliders)
		{
			allSucessfull = col.DebugDraw(InUploadCommandList) && allSucessfull;
		}
	}
	return allSucessfull;
}
#endif
