#include "App/GPUCompute/RSRComputePipeline.h"

#include "App/Game/RSRProgramInstance.h"
#include "App/Tools/RSRLog.h"
#include "App/Tools/Shader.h"
#include "App/Tools/ImageLoader.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include <string>

using namespace RSRush;
using namespace DirectX;

RSRComputePipeline::RSRComputePipeline(std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InComputeShader)
    : m_rootSig(nullptr), m_rootSigShader(InRootSigShader), m_computeShader(InComputeShader), m_pipelineStateDesc()
{
}

void RSRComputePipeline::Init(ID3D12Device10* InDevice)
{
    HRESULT result = InDevice->CreateRootSignature(0, m_rootSigShader->GetBuffer(), m_rootSigShader->GetSize(), IID_PPV_ARGS(&m_rootSig));
    bool bAllSucessfull = SUCCEEDED(result);

    m_pipelineStateDesc.pRootSignature = m_rootSig.Get();
    m_pipelineStateDesc.CS =
    {
        .pShaderBytecode = m_computeShader->GetBuffer(),
        .BytecodeLength = m_computeShader->GetSize()
    };
    m_pipelineStateDesc.NodeMask = 0;
    m_pipelineStateDesc.CachedPSO = { .pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0 }; //Buffer of precompiled shader (compile on first lanch for GPU in modern games)
    m_pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    result = InDevice->CreateComputePipelineState(&m_pipelineStateDesc, IID_PPV_ARGS(&m_pso));
}