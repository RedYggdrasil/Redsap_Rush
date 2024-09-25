#pragma once
#include "MDS/Defines/MDS.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"

using namespace DirectX;
namespace RSRush
{
	class RSRPhysicManager_Collisions
	{
	public:
		STATIC_CLASS_R5(RSRPhysicManager_Collisions);
		friend class RSRPhysicManager;
	private:

		inline static bool ComputeCollisionsAndOverlaps(RSRPhysicManager& pm, const double InDeltaTime, std::vector<ColbodyIDXsAndColResult>& OutCollisions, std::vector<std::pair<size_t, size_t>>& OutOverlaps)
		{
			OutCollisions.clear();
			OutOverlaps.clear();

			for (size_t i = 0, length = pm.m_physicalBodies.size(); i < length; ++i)
			{
				RSRPhysicBody& pBOne = pm.m_physicalBodies[i];
				const RSRCollidersBody& e1CBody = pBOne.Colliders.GetWorld();
				for (size_t j = i + 1; j < length; ++j)
				{
					RSRPhysicBody& pBTwo = pm.m_physicalBodies[j];
					const RSRCollidersBody& e2CBody = pBTwo.Colliders.GetWorld();

					RSRCollisionType colType = e1CBody.FillCollidesOrOverlaps(e2CBody, /*Out*/OutCollisions, true);

					if (colType == RSRCollisionType::None) { continue; }
					else if (colType == RSRCollisionType::Overlap)
					{
						OutOverlaps.push_back({ i, j });
					}
					else if (colType == RSRCollisionType::Collide)
					{
						//We are not intrested in witch specific colliders collided but only PhysicBody there
						ColbodyIDXsAndColResult& newIDXsCollision = OutCollisions.back();
						newIDXsCollision.FirstElementIndex = i;
						newIDXsCollision.SecondElementIndex = j;
					}
				}
			}
			return true;
		}
	};
};