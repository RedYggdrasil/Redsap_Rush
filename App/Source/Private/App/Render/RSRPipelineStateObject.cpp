#include "App/Render/RSRPipelineStateObject.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/D3D/DXContext.h"
#include "App/Render/RSRShaderList.h"
#include "App/Tools/Shader.h"



RSRush::RSRPipelineStateObject::RSRPipelineStateObject(RSRProgramInstance* InProgramInstance, const std::string_view InRootSigName, const std::string_view InVertexShaderName, const std::string_view InPixelShaderName)
: RSRPipelineStateObject(InProgramInstance, std::make_shared<Shader>(InRootSigName), std::make_shared<Shader>(InVertexShaderName), std::make_shared<Shader>(InPixelShaderName))
{
}

RSRush::RSRPipelineStateObject::RSRPipelineStateObject(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader)
: m_bIsCorrectlyLoaded(false), m_programInstance(InProgramInstance), m_rootSig(nullptr), m_rootSigShader(InRootSigShader), m_vertexShader(InVertexShader), m_pixelShader(InPixelShader), m_pipelineStateDesc()
{
	bool bAllSucessfull = true;
	bAllSucessfull = m_programInstance && bAllSucessfull;
	
	HRESULT result = m_programInstance->GetDXContect()->GetDevice()->CreateRootSignature(0, m_rootSigShader->GetBuffer(), m_rootSigShader->GetSize(), IID_PPV_ARGS(&m_rootSig));
	bAllSucessfull = SUCCEEDED(result) && bAllSucessfull;
	m_bIsCorrectlyLoaded = bAllSucessfull;
}

bool RSRush::RSRPipelineStateObject::CreatePSO()
{
	HRESULT result = m_programInstance->GetDXContect()->GetDevice()->CreateGraphicsPipelineState(&m_pipelineStateDesc, IID_PPV_ARGS(&m_pso));
	m_bIsCorrectlyLoaded = SUCCEEDED(result) && m_bIsCorrectlyLoaded;
	return SUCCEEDED(result);
}

RSRush::RSRPipelineStateObject::~RSRPipelineStateObject()
{
	m_rootSig.Reset();
	m_pixelShader.reset();
	m_vertexShader.reset();
	m_rootSigShader.reset();
	m_programInstance = nullptr;
	m_bIsCorrectlyLoaded = false;
}
