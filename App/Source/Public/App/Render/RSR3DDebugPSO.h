#pragma once
#include "App/Render/RSRPipelineStateObject.h"


#if DEBUG_PHYSIC
namespace RSRush
{

	class RSR3DDebugPSO : public RSRPipelineStateObject
	{

	public:
		RSR3DDebugPSO(RSRProgramInstance* InProgramInstance);
		RSR3DDebugPSO(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader);

	public:
		R_VIRTUAL_IMPLICIT ~RSR3DDebugPSO() R_OVERRIDE_IMPLICIT;
	};
};
#endif