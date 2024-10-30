#include "App/Render/RSRPipelines.h"

RSRush::RSRPipelines::RSRPipelines(RSRProgramInstance* InProgramInstance)
: m_bIsCorrectlyLoaded(false), m_programInstance(InProgramInstance),
m_pipelineStateDesc2D(InProgramInstance), m_pipelineStateDesc3D(InProgramInstance),
m_pipelineStateDesc3DInstanced(InProgramInstance)
#if DEBUG_PHYSIC
,m_pipelineStateDescDebugPhysic(InProgramInstance)
#endif
{
	m_bIsCorrectlyLoaded = m_pipelineStateDesc2D.GetIsCorrectlyLoaded() && m_pipelineStateDesc3D.GetIsCorrectlyLoaded()
		&& m_pipelineStateDesc3DInstanced.GetIsCorrectlyLoaded()
#if DEBUG_PHYSIC
		&& m_pipelineStateDescDebugPhysic.GetIsCorrectlyLoaded()
#endif
		;
}

RSRush::RSRPipelines::~RSRPipelines()
{
}
