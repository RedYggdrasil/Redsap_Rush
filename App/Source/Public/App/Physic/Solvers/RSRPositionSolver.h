#pragma once
#include "App/Physic/Solvers/RSRSolver.h"

namespace RSRush
{
	class RSRPositionSolver : public RSRSolver
	{
	public:
		inline static constexpr RSRSolverType SOLVER_TYPE = RSRSolverType::Position;
	public:
		virtual void Solve(RSRPhysicManager& InPhysicManager, std::vector<ColbodyIDXsAndColResult>& InIDXsCollisions, const double InDeltaTime) R_PURE_OVERRIDE;
	public:
		RSRPositionSolver();
		R_VIRTUAL_IMPLICIT ~RSRPositionSolver() R_OVERRIDE_IMPLICIT;
	};
}
