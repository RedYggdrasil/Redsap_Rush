#pragma once
#include "Gen_App/Config/AppConfig.h"
#include "MDS/Defines/MDS.h"
#include "App/Libs/WinInclude.h"
#include <memory>
#include <wrl/client.h>
#include <d3d12.h>

class Shader;
namespace RSRush
{
	class RSRProgramInstance;
	class RSRPipelineStateObject
	{
	protected:
		bool m_bIsCorrectlyLoaded;
		RSRProgramInstance* m_programInstance;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSig;
		std::shared_ptr<Shader> m_rootSigShader;
		std::shared_ptr<Shader> m_vertexShader;
		std::shared_ptr<Shader> m_pixelShader;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineStateDesc;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;

	protected:
		RSRPipelineStateObject(RSRProgramInstance* InProgramInstance, const std::string_view InRootSigName, const std::string_view InVertexShaderName, const std::string_view InPixelShaderName);
		RSRPipelineStateObject(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader);
	
		bool CreatePSO();
	public:
		bool GetIsCorrectlyLoaded() const { return m_bIsCorrectlyLoaded; }
		inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc() const { return m_pipelineStateDesc; }
		inline Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSig() const { return m_rootSig; }
		inline Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSO() const { return m_pso; }

		virtual ~RSRPipelineStateObject();
	};
};