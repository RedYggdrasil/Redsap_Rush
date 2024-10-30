#pragma once
#include "App/Render/RSRPipelineStateObject.h"

namespace RSRush
{

	class RSR3DPSO : public RSRPipelineStateObject
	{

	public:
		RSR3DPSO(RSRProgramInstance* InProgramInstance);
		RSR3DPSO(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader);

	public:
		R_VIRTUAL_IMPLICIT ~RSR3DPSO() R_OVERRIDE_IMPLICIT;
	};
};