#pragma once
#include "App/Render/RSRPipelineStateObject.h"

namespace RSRush
{

	class RSR3DInstancedPSO : public RSRPipelineStateObject
	{

	public:
		RSR3DInstancedPSO(RSRProgramInstance* InProgramInstance);
		RSR3DInstancedPSO(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader);

	public:
		R_VIRTUAL_IMPLICIT ~RSR3DInstancedPSO() R_OVERRIDE_IMPLICIT;
	};
};