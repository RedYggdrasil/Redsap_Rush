#pragma once
#include "Gen_App/Config/AppConfig.h"
#include "App/Libs/WinInclude.h"
#include <wrl/client.h>
#include <memory>
#include <string>
#include <string_view>

class Shader;
namespace RSRush
{
	class RSRTexture2D;
	class RSRComputeTextureTask
	{
	protected:
		std::unique_ptr<class RSRComputeTextureBuffer> m_texture;
		std::unique_ptr<class RSRComputePipeline> m_greeblePipeline;
		uint16_t m_sizeX;
		uint16_t m_sizeY;
		uint16_t m_bPP;
		uint16_t m_cC;
		DXGI_FORMAT m_dXGIFormat;

		std::string m_rootSingatureShaderName;
		std::shared_ptr<Shader> m_rootSingatureShader;
		std::string m_computeShaderName;
		std::shared_ptr<Shader> m_computeShader;

	protected:
		RSRComputeTextureTask(const std::string_view InRootSingatureShaderName, const std::string_view InComputeShaderName, uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat);
		RSRComputeTextureTask(std::shared_ptr<Shader> InRootSingatureShader, const std::shared_ptr<Shader> InComputeShader, uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat);
	private :
		RSRComputeTextureTask(uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat);

	public:
		virtual void CreateResources(ID3D12Device10* InDevice);
		virtual void Compute(ID3D12GraphicsCommandList7* InCommandList);
		Microsoft::WRL::ComPtr<ID3D12Resource2> GetResultBuffer() const;
		std::shared_ptr<RSRTexture2D> ResultBufferToTexture2D() const;

	public:
		virtual ~RSRComputeTextureTask();
	};
};

#define COMPUTE_TEXTURE_TASK_CLASS(ClassName, RootSigShaderName, ComputeShaderName) namespace RSRush\
{\
	class ClassName : public RSRush::RSRComputeTextureTask\
	{\
	public:\
		ClassName (uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat)\
		: RSRush::RSRComputeTextureTask(std::string(#RootSigShaderName), std::string(#ComputeShaderName), InSizeX, InSizeY, InBPP, InCC, InDXGIFormat)\
		{}\
		ClassName\
		(std::shared_ptr<Shader> InSharedRootSingatureShader, const std::shared_ptr<Shader> InSharedComputeShader, uint16_t InSizeX, uint16_t InSizeY, uint16_t InBPP, uint16_t InCC, DXGI_FORMAT InDXGIFormat)\
		: RSRush::RSRComputeTextureTask(InSharedRootSingatureShader, InSharedComputeShader, InSizeX, InSizeY, InBPP, InCC, InDXGIFormat)\
		{}\
	public:\
		R_VIRTUAL_IMPLICIT ~ClassName() R_OVERRIDE_IMPLICIT\
		{ }\
	};\
};