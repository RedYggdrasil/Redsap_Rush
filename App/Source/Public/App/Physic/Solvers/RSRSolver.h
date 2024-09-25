#pragma once
#include "MDS/Defines/MDS.h"
#include "MDS/Tools/REnum.h"
#include "App/Data/Physic/RSRCollisionResult.h"
#include <vector>

namespace RSRush 
{
	class RSRPhysicManager;
	struct RSRPhysicBody;
	
	enum class RSRSolverType : uint8_t
	{
		RENUM_MIN_VAL(Unknown, 0),
		NonGeneric = 1,
		Position = 2,
		RENUM_MAX_VAL(Impulse, 3)
	};
	RS_DEFINE_ENUM(RSRSolverType);

	constexpr bool RSRSolverTypeIsGeneric(RSRSolverType InESolverType)
	{ return  InESolverType > RSRSolverType::NonGeneric; }
	class RSRSolver 
	{
	protected:
		RSRSolverType m_eSolverType;
	public:
		inline RSRSolverType GetSolverType() const { return m_eSolverType; };

		virtual void Solve(RSRPhysicManager& InPhysicManager, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, const double InDeltaTime) R_PURE;
	protected:
		std::vector<RSRPhysicBody>& GetPhysicManagerPhysicBodies(RSRPhysicManager& InPhysicManager) const;
	protected:
		RSRSolver(const RSRSolverType InESolverType);
	public:
		virtual ~RSRSolver();
	};
	class RSRGenericSolverFactory
	{
	public:
		STATIC_CLASS_R5(RSRGenericSolverFactory);
	public:
		static RSRSolver* CreateGenericSolver(const RSRSolverType InESolverType);
	};
}
