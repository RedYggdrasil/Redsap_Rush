#pragma once

#include "Gen_App/Config/AppConfig.h"
#include "App/Libs/WinInclude.h"
#include <wrl/client.h>
#include <memory>

class Shader;
namespace RSRush
{
	class RSRComputePipeline
	{

    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSig;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
        std::shared_ptr<Shader> m_rootSigShader;
        std::shared_ptr<Shader> m_computeShader;
        D3D12_COMPUTE_PIPELINE_STATE_DESC m_pipelineStateDesc;

    public:
        inline ID3D12RootSignature* GetRootSig() const { return m_rootSig.Get(); }
        inline ID3D12PipelineState* GetPSO() const { return m_pso.Get(); }

        RSRComputePipeline(std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InComputeShader);
        void Init(ID3D12Device10* InDevice);
    };
};