#pragma once

#include "Gen_App/Config/AppConfig.h"
#include "App/Libs/WinInclude.h"

#include <wrl/client.h>

struct DXAVertex2D
{
	float x, y;
	float u, v;
};

class DXPipeline
{
private:
	static DXPipeline Instance;
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineStateDesc2D;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineStateDesc3D;
#if DEBUG_PHYSIC
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineStateDescDebugPhysic;
#endif
public:
	bool Init(
		ID3D12RootSignature* InRootSignature2D, const class Shader* InVertexShader2D, const Shader* InPixelShader2D,
		ID3D12RootSignature* InRootSignature3D, const class Shader* InVertexShader3D, const Shader* InPixelShader3D
#if DEBUG_PHYSIC
		,ID3D12RootSignature* InRootSignatureDebugPhysic, const class Shader* InVertexShaderDebugPhysic, const Shader* InPixelShaderDebugPhysic
#endif
	);
	void Shutdown();
	inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc2D() const { return m_pipelineStateDesc2D; }
	inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDesc3D() const { return m_pipelineStateDesc3D; }
#if DEBUG_PHYSIC
	inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetStateDescDebugPhysic() const { return m_pipelineStateDescDebugPhysic; }
#endif
public:
	DXPipeline(const DXPipeline&) = delete;
	DXPipeline& operator=(const DXPipeline&) = delete;
	inline static DXPipeline& Get()
	{
		return Instance;
	}

private:
	void SetupPipelineStateDesc2D(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader);
	void SetupPipelineStateDesc3D(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader);
#if DEBUG_PHYSIC
	void SetupPipelineStateDescDebugPhysic(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader);
#endif
private:
	DXPipeline() = default;
};