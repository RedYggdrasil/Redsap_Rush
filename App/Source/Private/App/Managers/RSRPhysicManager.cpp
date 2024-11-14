#include "App/Managers/RSRPhysicManager.h"
#include "App/Tools/RSRLog.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Managers/RSRPhysicManager_Dynamics.hpp"
#include "App/Managers/RSRPhysicManager_Collisions.hpp"

#include <algorithm>
#include <format>
#include <Tracy.hpp>

using namespace RSRush;


std::vector<RSRPhysicBody>::iterator RSRush::RSRPhysicManager::FindEntity(std::weak_ptr<RSRIPhysicalEntity> InPtr)
{
	return std::find_if(m_physicalBodies.begin(), m_physicalBodies.end(), [&InPtr](const RSRPhysicBody& ptr1) {
		return mds::WPtrEqual(InPtr, ptr1.Entity);
		});
}

void RSRush::RSRPhysicManager::OptimisationReportReindexing()
{
	++m_optimisation_count_reindexing;
	if (m_optimisation_count_reindexing % 10 == 0)
	{
		RSRLog::Log(LOG_DISPLAY, TEXT("[RSRPhysicManager] {} delete Redindexing so far !"), m_optimisation_count_reindexing);
	}
}

void RSRush::RSRPhysicManager::AddPhysicalEntities(const RSRPhysicBody* InStartingValues, size_t InSizes)
{
	if (InSizes > 0)
	{
		size_t newSize = m_physicalBodies.size() + InSizes;
		m_physicalBodies.reserve(newSize);
		for (size_t i = 0; i < InSizes; ++i)
		{
			AddPhysicalEntity(InStartingValues[i]);
		}
	}
}

size_t RSRush::RSRPhysicManager::MissFindEntityIndex(const std::weak_ptr<RSRIPhysicalEntity>& InEntity) const
{
	RSRLog::LogError(L_PATH TEXT("A missFind entity has been called in RSRPhysicManager, to investigate"));

	if (InEntity.expired())
	{
		RSRLog::LogError(L_PATH TEXT("Empty entity given !"));
		return std::numeric_limits<size_t>::max();
	}
	const auto it = std::find_if(m_physicalBodies.begin(), m_physicalBodies.end(), [&InEntity](const RSRPhysicBody& ptr1) {
		return mds::WPtrEqual(InEntity, ptr1.Entity);
		});
	if (it == m_physicalBodies.end()) {
		RSRLog::LogError(L_PATH TEXT("Empty is not in array !"));
		return std::numeric_limits<size_t>::max();
	}
	return std::distance(m_physicalBodies.begin(), it);
}

void RSRush::RSRPhysicManager::RemovePhysicSolver(std::vector<std::shared_ptr<RSRSolver>>::iterator InIterSolver)
{
	if (InIterSolver != m_physicalSolvers.end())
	{
		m_physicalSolvers.erase(InIterSolver);
	}
}

void RSRush::RSRPhysicManager::AddPhysicalEntity(const RSRPhysicBody& InStartingValue)
{
	std::shared_ptr<RSRIPhysicalEntity> entityPtr = InStartingValue.Entity.lock();
	if (!entityPtr)
	{
		RSRLog::LogError(L_PATH TEXT("Empty entity given !"));
		return;
	}
	if (entityPtr->GetHasBeenRegistered())
	{
		RSRLog::Log(LOG_ERROR, TEXT("RSRIPhysicalEntity Has Already Been Registered !"));
		return;
	}
	entityPtr->SetLastKnownIndex(m_physicalBodies.size());
	entityPtr->SetHasBeenRegistered(true, InStartingValue);
	m_physicalBodies.push_back(InStartingValue);
}

void RSRush::RSRPhysicManager::RemovePhysicalEntity(RSRPhysicBodyKey& InOutKey, const bool bInUpdateIndexes)
{
	InOutKey.bHasBeenRegistered = false;
	size_t deletedIndex = std::numeric_limits<size_t>::max();
	if (PlausibleIndex(InOutKey))
	{
		deletedIndex = InOutKey.LastKnownIndex;
		m_physicalBodies.erase(m_physicalBodies.begin() + InOutKey.LastKnownIndex);
	}
	else if (InOutKey.SelfEntity.expired())
	{
		ClearDangling();
	}
	else
	{
		auto it = FindEntity(InOutKey.SelfEntity);
		if (it != m_physicalBodies.end())
		{
			deletedIndex = std::distance(m_physicalBodies.begin(), it);
			m_physicalBodies.erase(it);
		}
		else
		{
			RSRLog::Log(LOG_EXCEPTION, TEXT("Cannot find entity to remove in RSRPhysicManager !"));
		}
	}


	size_t newSize = m_physicalBodies.size();
	if (bInUpdateIndexes && newSize > 0 &&  deletedIndex < (newSize - 1))
	{
		{
			for (; deletedIndex < newSize; ++deletedIndex)
			{
				if (std::shared_ptr<RSRIPhysicalEntity> ptr = m_physicalBodies[deletedIndex].Entity.lock())
				{
					ptr->GetEditKey().LastKnownIndex = deletedIndex;
				}
			}
		};
		OptimisationReportReindexing();
	}
}

std::weak_ptr<RSRSolver> RSRush::RSRPhysicManager::AddPhysicSolver(const RSRSolverType InEGenericSolverType)
{
	if (!RSRSolverTypeIsGeneric(InEGenericSolverType))
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot create a non generic Physic Solver Type {}"), mds::UT_cast(InEGenericSolverType));
		return std::weak_ptr<RSRSolver>();
	}
	return AddPhysicSolver(std::shared_ptr<RSRSolver>(RSRGenericSolverFactory::CreateGenericSolver(InEGenericSolverType)));
}

std::weak_ptr<RSRSolver> RSRush::RSRPhysicManager::AddPhysicSolver(std::shared_ptr<RSRSolver>&& InSolver)
{
	m_physicalSolvers.push_back(std::move(InSolver));
	return m_physicalSolvers.back();
}

void RSRush::RSRPhysicManager::RemovePhysicSolver(const RSRSolverType InEGenericSolverType)
{
	auto it = std::find_if(m_physicalSolvers.begin(), m_physicalSolvers.end(), [&InEGenericSolverType](const std::shared_ptr<RSRSolver>& ptr1) {
		return ptr1->GetSolverType() == InEGenericSolverType;
		});
	RemovePhysicSolver(it);
}

void RSRush::RSRPhysicManager::RemovePhysicSolver(std::weak_ptr<RSRSolver> InSolver)
{
	if (InSolver.expired())
	{
		RSRLog::Log(LOG_ERROR, TEXT("[RSRPhysicManager::RemovePhysicSolver] Solver pointer already expired !"));
		return;
	}
	std::shared_ptr<RSRSolver> sptrInSolver = InSolver.lock();
	if (sptrInSolver)
	{
		auto it = std::find(m_physicalSolvers.begin(), m_physicalSolvers.end(), sptrInSolver);
		RemovePhysicSolver(it);
	}

}

void RSRPhysicManager::ClearDangling()
{
	bool bDidDeleteOne = false;
	std::erase_if(m_physicalBodies, [&bDidDeleteOne](const RSRPhysicBody& ptr1)
	{
		bool bWillDelete = ptr1.Entity.expired();
		bDidDeleteOne = bWillDelete || bDidDeleteOne;
		return bWillDelete;
	});
	if (bDidDeleteOne)
	{
		for (size_t i = 0; i < m_physicalBodies.size(); ++i)
		{
			if (std::shared_ptr<RSRIPhysicalEntity>ptr = m_physicalBodies[i].Entity.lock())
			{
				ptr->GetEditKey().LastKnownIndex = i;
			}
		}
		OptimisationReportReindexing();
	}
}

void RSRush::RSRPhysicManager::ClearAllPhysicSolvers()
{ 
	while(m_physicalSolvers.size() > 0)
	{
		// Access the element using the index
		RemovePhysicSolver(m_physicalSolvers.end() - 1);
	}
}

void RSRPhysicManager::UpdatePhysic(const double InGameTime, const double InDeltaTime)
{
	ZoneScoped;
	//Clear nullrefs, should never be needed
	ClearDangling();

	//Allow physical object to update their positons and states
	for (RSRPhysicBody& physicalBody : m_physicalBodies)
	{
		if (std::shared_ptr<RSRIPhysicalEntity> entity = physicalBody.Entity.lock())
		{
			entity->OnPhysicalPrePass(this, InDeltaTime);
		}
	}

	RSRPhysicManager_Dynamics::ApplyGravityToForce(*this, InDeltaTime);


	//Inexact approch, but ensure collision takes precedence other overlaps
	// Issue is object may yet be pushed into other overlaps
	//If this ever cause issues, compute overlaps after physic solvers
	std::vector<std::pair<size_t, size_t>> overlapingObjects;
	std::vector<ColbodyIDXsAndColResult> collisionsObjects;
	RSRPhysicManager_Collisions::ComputeCollisionsAndOverlaps(*this, InDeltaTime, /*Out*/collisionsObjects, /*Out*/overlapingObjects);

#if false && _DEBUG
	//Big spamming unoptimized log, to remove as soon as basic physic implemented 
	for (auto& colPr : collisionsObjects)
	{
		RSRLog::Log(LOG_DISPLAY, TEXT("Found collision with data '{}'"), mds::_tostring(colPr));
	}
#endif

	for (size_t i = 0; i < m_physicalSolvers.size(); ++i)
	{
		if (m_physicalSolvers[i])
		{
			m_physicalSolvers[i]->Solve(*this, /*InOut*/collisionsObjects, InDeltaTime);
		}
	}

	//std::string log = std::format("Found {} among {} entities !\n", overlapingObjects.size(), m_physicalBodies.size());
	//RSRLog::LogWarning(log);
	for (auto& OverlapPair : overlapingObjects)
	{
		std::shared_ptr<RSRIPhysicalEntity> firstEntity = m_physicalBodies[OverlapPair.first].Entity.lock();
		std::shared_ptr<RSRIPhysicalEntity> secondEntity = m_physicalBodies[OverlapPair.second].Entity.lock();
		if (firstEntity && secondEntity)
		{
			firstEntity->OnOverlapWith(secondEntity.get());
			secondEntity->OnOverlapWith(firstEntity.get());
		}
	}

	
	RSRPhysicManager_Dynamics::ApplyForcesToObjets(*this, InDeltaTime);

	for (RSRPhysicBody& physicalBody : m_physicalBodies)
	{
		XMMATRIX lTrsMat = XMLoadFloat4x4(&physicalBody.Transform.GetMatrix());
		physicalBody.Colliders.RecomputeWorld(lTrsMat, true);

		std::shared_ptr<RSRIPhysicalEntity> entity = physicalBody.Entity.lock();
		if (!entity) { continue; }
		entity->OnPhysicPassEnded(physicalBody);
	}
}

void RSRush::RSRPhysicManager::LateTickSync(const double InGameTime, const double InDeltaTime)
{
#if DEBUG_PHYSIC
	DebugCopyDataForDrawing();
#endif
}

RSRPhysicManager* RSRush::RSRPhysicManager::Get(const mds::IRProgramMemElem* InProgramMemElem)
{
	return InProgramMemElem->GetRoot<RSRush::RSRProgramInstance>()->GetPhysicManager();
}

RSRPhysicManager* RSRush::RSRPhysicManager::Get(RSRProgramInstance* InProgramInstance)
{
	return InProgramInstance->GetPhysicManager();
}

bool RSRPhysicManager::Init(RSRProgramInstance* InProgramInstance, RSRBasicShapes* InBasicShapes)
{
	this->InitMemElem(InProgramInstance);
	bool allSucessfull = true;
#if DEBUG_PHYSIC
	if (!InBasicShapes) { return false; }
	m_debugSquare = InBasicShapes->GetRegisterNewSquare(RSRPhysicManager::DEBUG_SQUARE_NAME, { 1.f, 0.f, 0.f }, 0Ui32, nullptr);
	//TODO : create a function for default spheres
	m_debugSphere = InBasicShapes->GetRegisterNewSphere(RSRPhysicManager::DEBUG_SPHERE_NAME, { 1.f, 0.f, 0.f }, 0Ui32, nullptr);

	m_debugAABB = InBasicShapes->GetRegisterNewSquare(RSRPhysicManager::DEBUG_AABB_NAME, { 0.f, 1.f, 0.f }, 0Ui32, nullptr);
	allSucessfull = m_debugSquare && m_debugSphere && m_debugSquare && allSucessfull;
#endif
	return allSucessfull;
}

void RSRPhysicManager::Shutdown()
{
#if _DEBUG
	if (m_physicalBodies.size() > 0)
		{ RSRLog::Log(LOG_ERROR, TEXT("[PhysicManager] {} PhysicBody remains !"), m_physicalBodies.size()); }
#endif
	ClearDangling();
	ClearAllPhysicSolvers();
#if DEBUG_PHYSIC
	m_debugLastFramephysicalBodies.clear();
	if (m_debugSquare)	{ m_debugSquare->FreeAllBuffers(); }
	if (m_debugSphere)	{ m_debugSphere->FreeAllBuffers(); }
	if (m_debugAABB)	{ m_debugAABB->FreeAllBuffers(); }
	m_debugSquare.reset();
	m_debugSphere.reset();
	m_debugAABB.reset();
#endif
	this->ResetMemTreeData();
}


std::vector<std::shared_ptr<RSRIPhysicalEntity>> RSRush::RSRPhysicManager::FindOverlappedEntities(const RSRCollidersBody& InCollidersBody, const std::weak_ptr<RSRIPhysicalEntity> InEntityToIgnore, const size_t InMaxReturnedObject) const
{
	bool bHasIgnore = !InEntityToIgnore.expired();
	std::shared_ptr<RSRIPhysicalEntity> entityToIgnore = nullptr;
	if (bHasIgnore)
	{
		entityToIgnore = InEntityToIgnore.lock();
		bHasIgnore = entityToIgnore->GetHasBeenRegistered() && bHasIgnore;
	}
	std::vector<std::shared_ptr<RSRIPhysicalEntity>> overlapped;


	for (const RSRPhysicBody& physicalBody : m_physicalBodies)
	{
		std::shared_ptr<RSRIPhysicalEntity> testedEntity = physicalBody.Entity.lock();
		if (!testedEntity || (bHasIgnore && testedEntity == entityToIgnore)) { continue; }

		if (InCollidersBody.Intersects(physicalBody.Colliders.GetWorld()))
		{
			overlapped.push_back(testedEntity);
			if (overlapped.size() >= InMaxReturnedObject)
			{
				break;
			}
		}
	}
	return overlapped;
}


std::vector<std::pair<size_t, size_t>> RSRPhysicManager::ComputeOverlaping()
{
	std::vector<std::pair<size_t, size_t>> overlapingObjects = std::vector<std::pair<size_t, size_t>>();
	
	for (size_t i = 0, length = m_physicalBodies.size(); i < length; ++i)
	{
		RSRPhysicBody& pBOne = m_physicalBodies[i];
		const RSRCollidersBody& e1CBody = pBOne.Colliders.GetWorld();
		for (size_t j = i + 1; j < length; ++j)
		{
			RSRPhysicBody & pBTwo = m_physicalBodies[j];
			const RSRCollidersBody& e2CBody = pBTwo.Colliders.GetWorld();

			if (e1CBody.Intersects(e2CBody))
			{
				overlapingObjects.push_back({ i, j });
			}
		}
	}

	return overlapingObjects;
}

RSRPhysicManager::RSRPhysicManager()
:mds::IRProgramMemElem(), m_physicContext(PhysicContext::DEFAULT_EARTH)
{
}

#if DEBUG_PHYSIC
bool RSRush::RSRPhysicManager::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool allSucessfull = true;
	allSucessfull = m_debugSquare->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
	allSucessfull = m_debugSphere->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
	allSucessfull = m_debugAABB->UploadResources(InDevice, InUploadCommandList) && allSucessfull;
	return allSucessfull;
}

bool RSRush::RSRPhysicManager::FreeUploadBuffers()
{
	bool allSucessfull = true;
	allSucessfull = m_debugSquare->FreeUploadBuffer() && allSucessfull;
	allSucessfull = m_debugSphere->FreeUploadBuffer() && allSucessfull;
	allSucessfull = m_debugAABB->FreeUploadBuffer() && allSucessfull;
	return allSucessfull;
}

bool RSRPhysicManager::DrawPhysic(ID3D12GraphicsCommandList7* InUploadCommandList)
{
	ZoneScopedN("[DEBUG] Physic Draw");
	bool bAllSucessfull = true;

	for (const RSRPhysicBody& physicalBody : m_debugLastFramephysicalBodies)
	{
		bAllSucessfull = physicalBody.Colliders.GetWorld().DebugDraw(InUploadCommandList) && bAllSucessfull;
	}
	return bAllSucessfull;
}
void RSRush::RSRPhysicManager::DebugCopyDataForDrawing()
{
	ZoneScopedN("[DEBUG] Physic Copy");
	m_debugLastFramephysicalBodies.clear();
	m_debugLastFramephysicalBodies.reserve(m_physicalBodies.size());
	for (const RSRPhysicBody& body : m_physicalBodies)
	{
		m_debugLastFramephysicalBodies.push_back(body);
	}
}
#endif