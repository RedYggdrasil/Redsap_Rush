#include "App/Physic/Solvers/RSRSolver.h"
#include "MDS/RTool.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Physic/Solvers/RSRImpulseSolver.h"
#include "App/Physic/Solvers/RSRPositionSolver.h"
#include "App/Tools/RSRLog.h"

using namespace RSRush;

RSRSolver* RSRGenericSolverFactory::CreateGenericSolver(const RSRSolverType InESolverType)
{
	switch (InESolverType)
	{
	case RSRSolverType::Unknown:
		RSRLog::Log(LOG_EXCEPTION, TEXT("Cannot create a generic solver type 'Unknown' !"));
		break;
	case RSRSolverType::NonGeneric:
		RSRLog::Log(LOG_EXCEPTION, TEXT("A Solver need to be of a generic type 'NonGeneric' to be create using RSRGenericSolverFactory given !"));
		break;
	case RSRSolverType::Position:
		return new RSRPositionSolver();
	case RSRSolverType::Impulse:
		return new RSRImpulseSolver();
	default:
		RSRLog::Log(LOG_EXCEPTION, TEXT("Solver type '{}' unimplemented in [RSRGenericSolverFactory::CreateGenericSolver] switch !"), mds::UT_cast(InESolverType));
		break;
	}
	return nullptr;
}

std::vector<RSRPhysicBody>& RSRush::RSRSolver::GetPhysicManagerPhysicBodies(RSRPhysicManager& InPhysicManager) const
{
	return InPhysicManager.m_physicBodies.Values.GetUnderlyingVector();
}

RSRush::RSRSolver::RSRSolver(const RSRSolverType InESolverType)
	:m_eSolverType(InESolverType)
{
}

RSRush::RSRSolver::~RSRSolver()
{
}
