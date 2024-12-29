#pragma once
#include "Gen_App/Config/AppConfig.h"

#include "RCB/Tools/RCBVectorMap.h"
#define NOMINMAX
#include <vector>
#include <memory>
#include "MDS/Tools/System/IRProgramMemNode.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Data/Physic/RSRIPhysicalEntity.h"
#include "App/Data/Physic/RSRPhysicContext.h"

#include "App/Physic/Solvers/RSRSolver.h"

#include <limits>
#include <mutex>
namespace RSRush
{
	class RSRBasicShapes;
	class RSRProgramInstance;

	class RSRPhysicManager : public mds::IRProgramMemElem
	{
	private:
		size_t m_optimisation_count_reindexing = 0;

		RSRPhysicContext m_physicContext;
		rcb::RCBVectorMap<RSRPhysicBodyID, RSRPhysicBody, true> m_physicBodies;
		//std::vector<RSRPhysicBody> m_physicalBodies;
		std::vector<std::shared_ptr<RSRSolver>> m_physicalSolvers;
		//std::mutex  m_physicalPassLock;

	private:
		void OptimisationReportReindexing();
	public:
		void AddPhysicalEntities(const RSRPhysicBody* InStartingValues, size_t InSizes);
		void AddPhysicalEntity(const RSRPhysicBody& InStartingValue);
		void RemovePhysicalEntity(RSRPhysicBodyKey& InOutKey, const bool bInUpdateIndexes = true);

		std::weak_ptr<RSRSolver> AddPhysicSolver(const RSRSolverType InEGenericSolverType);
		std::weak_ptr<RSRSolver> AddPhysicSolver(std::shared_ptr<RSRSolver>&& InSolver);


		void RemovePhysicSolver(const RSRSolverType InEGenericSolverType);
		void RemovePhysicSolver(std::weak_ptr<RSRSolver> InSolver);

		void ClearDangling();
		void ClearAllPhysicSolvers();
		inline void SetPhysicContext(const RSRPhysicContext& InContext) { m_physicContext = InContext; };
		void UpdatePhysic(const double InGameTime, const double InDeltaTime);
		void LateTickSync(const double InGameTime, const  double InDeltaTime);

	public:
		static RSRPhysicManager* Get(const mds::IRProgramMemElem* InProgramMemElem);
		static RSRPhysicManager* Get(RSRProgramInstance* InProgramInstance);
	public:
		RSRPhysicManager();
		RSRPhysicManager(const RSRPhysicManager&) = delete;
		RSRPhysicManager& operator=(const RSRPhysicManager&) = delete;

		bool Init(RSRProgramInstance* InProgramInstance, class RSRBasicShapes* InBasicShapes);
		void Shutdown();
	public :
		std::vector<std::shared_ptr<RSRIPhysicalEntity>> FindOverlappedEntities(const RSRCollidersBody& InCollidersBody, const std::weak_ptr<RSRIPhysicalEntity> InEntityToIgnore, const size_t InMaxReturnedObject = std::numeric_limits<size_t>::max()) const;
		inline std::vector<std::shared_ptr<RSRIPhysicalEntity>> FindOverlappedEntities(const RSRPhysicBody& InPhysicBody, const size_t InMaxReturnedObject = std::numeric_limits<size_t>::max()) const
		{ return FindOverlappedEntities(InPhysicBody.Colliders.GetWorld(), InPhysicBody.Entity, InMaxReturnedObject); }
		inline bool IsOverlappingAny(const RSRCollidersBody& InCollidersBody, const std::weak_ptr<RSRIPhysicalEntity> InEntityToIgnore) const { return FindOverlappedEntities(InCollidersBody, InEntityToIgnore, 1).size() > 0; }
		inline bool IsOverlappingAny(const RSRPhysicBody& InPhysicBody) const { return FindOverlappedEntities(InPhysicBody, 1).size() > 0; }

	protected:
		std::vector<std::pair<size_t, size_t>> ComputeOverlaping();
		
	public:
		inline RSRPhysicBody* GetPhysicBody(const RSRPhysicBodyKey& InKey)
		{
			
			if (m_physicBodies.contains(InKey.physicBodyID)) { return &m_physicBodies[InKey.physicBodyID]; }
			return nullptr;
		}
	private:

		void RemovePhysicSolver(std::vector<std::shared_ptr<RSRSolver>>::iterator InIterSolver);
	
	private:
		friend class RSRPhysicManager_Collisions;
		friend class RSRPhysicManager_Dynamics;
		friend class RSRSolver;
#if DEBUG_PHYSIC
	public:
		static constexpr const char* DEBUG_SQUARE_NAME = TEXT("PHYSIC_SQUARE");
		static constexpr const char* DEBUG_SPHERE_NAME = TEXT("PHYSIC_SPHERE");
		static constexpr const char* DEBUG_AABB_NAME = TEXT("PHYSIC_AABB");

	private:
		std::vector<RSRPhysicBody> m_debugLastFramephysicalBodies;
		RSRush::RSRSharedMesh3DPtr m_debugSquare = nullptr;
		RSRush::RSRSharedMesh3DPtr m_debugSphere = nullptr;
		RSRush::RSRSharedMesh3DPtr m_debugAABB = nullptr;
	public:
		const RSRush::RSRSharedMesh3DPtr& GetDebugSquare() const { return m_debugSquare; }
		const RSRush::RSRSharedMesh3DPtr& GetDebugSphere() const { return m_debugSphere; }
		const RSRush::RSRSharedMesh3DPtr& GetDebugAABB() const { return m_debugAABB; }

	public:
		bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeUploadBuffers();
		bool DrawPhysic(ID3D12GraphicsCommandList7* InUploadCommandList);
	private:
		void DebugCopyDataForDrawing();
#endif
	};
}
