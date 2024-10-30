#pragma once
#include "App/Render/RSRPipelineStateObject.h"

namespace RSRush
{

	class RSR2DPSO : public RSRPipelineStateObject
	{

	public:
		RSR2DPSO(RSRProgramInstance* InProgramInstance);
		RSR2DPSO(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader);

	public:
		R_VIRTUAL_IMPLICIT ~RSR2DPSO() R_OVERRIDE_IMPLICIT;
	};
};