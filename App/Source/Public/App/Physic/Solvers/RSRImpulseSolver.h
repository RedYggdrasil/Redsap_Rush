#pragma once
#include "App/Physic/Solvers/RSRSolver.h"

namespace RSRush
{
	class RSRImpulseSolver : public RSRSolver
	{
	public:
		inline static constexpr RSRSolverType SOLVER_TYPE = RSRSolverType::Impulse;
	public:
		virtual void Solve(RSRPhysicManager& InPhysicManager, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, const double InDeltaTime) R_PURE_OVERRIDE;
	public:
		RSRImpulseSolver();
		R_VIRTUAL_IMPLICIT ~RSRImpulseSolver() R_OVERRIDE_IMPLICIT;
	};
}
