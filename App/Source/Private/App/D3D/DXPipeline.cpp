#include "App/D3D/DXPipeline.h"
#include "App/Tools/Shader.h"
#include "App/Tools/Window.h"

DXPipeline DXPipeline::Instance = DXPipeline();

bool DXPipeline::Init	(ID3D12RootSignature* InRootSignature2D, const Shader* InVertexShader2D, const Shader* InPixelShader2D,
						ID3D12RootSignature* InRootSignature3D, const class Shader* InVertexShader3D, const Shader* InPixelShader3D
#if DEBUG_PHYSIC		
						,ID3D12RootSignature* InRootSignatureDebugPhysic, const class Shader* InVertexShaderDebugPhysic, const Shader* InPixelShaderDebugPhysic
#endif
)
{
	SetupPipelineStateDesc2D(InRootSignature2D, InVertexShader2D, InPixelShader2D);
	SetupPipelineStateDesc3D(InRootSignature3D, InVertexShader3D, InPixelShader3D);
#if DEBUG_PHYSIC
	SetupPipelineStateDescDebugPhysic(InRootSignatureDebugPhysic, InVertexShaderDebugPhysic, InPixelShaderDebugPhysic);
#endif
	return true;
}

void DXPipeline::Shutdown()
{
}

void DXPipeline::SetupPipelineStateDesc2D(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader)
{

	//Input Assember
	static const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		// [O] Position : //vertex2D pivot | vertex2D position
		{
			.SemanticName = "Position", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = 0, //No alignement needed since "Position" is the first data element, otherwise can set manually to sum of last element sizes or D3D12_APPEND_ALIGNED_ELEMENT is automatic value
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [1] Color : //vertex4D Color
		{
			.SemanticName = "Color", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 4),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Color is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [2] UV0 : //vertex2D UV0
		{
			.SemanticName = "Texcoord", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 4) + (/*Color*/sizeof(float) * 4),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Color is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [3] textureIndex : //UINT32[1] Texindex
		{
			.SemanticName = "Texindex", //Texindex
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32_UINT, //UINT32
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 4) + (/*Color*/sizeof(float) * 4) + (/*Texcoord*/sizeof(float) * 2),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		}
	};
	m_pipelineStateDesc2D.pRootSignature = InRootSignature;
	m_pipelineStateDesc2D.InputLayout.NumElements = _countof(vertexLayout);
	m_pipelineStateDesc2D.InputLayout.pInputElementDescs = vertexLayout;

	m_pipelineStateDesc2D.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; //Indexing disabled

	//Vertex Shader
	if (!InVertexShader)
	{
		//Cannot have a working shader pipeline without a vertex shader
		return;
	}
	m_pipelineStateDesc2D.VS =
	{
		.pShaderBytecode = InVertexShader->GetBuffer(),
		.BytecodeLength = InVertexShader->GetSize()
	};

	//Geometry (Hull Shader, Tesselator, Domain Shader, Geometry Shader
	m_pipelineStateDesc2D.HS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDesc2D.DS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDesc2D.GS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };

	//Rasterizer
	m_pipelineStateDesc2D.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_pipelineStateDesc2D.RasterizerState =
	{
		.FillMode = D3D12_FILL_MODE_SOLID, // Wireframe or solid rendering of pixel within our triangle
		.CullMode = D3D12_CULL_MODE_NONE, // Todo later
		.FrontCounterClockwise = false, //Invert triangle rendering order to invert culling
		.DepthBias = 0,					//Depth bias : to render object at same pos ex : shadows or decals on surfaces
		.DepthBiasClamp = 0.0f,			//Depth bias : See https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
		.SlopeScaledDepthBias = 0.0f,	//Depth bias : 
		.DepthClipEnable = false,		// clipping to true by default if no stencil used
		.MultisampleEnable = false,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	//Pixel Shader
	if (InPixelShader)
	{
		m_pipelineStateDesc2D.PS =
		{
			.pShaderBytecode = InPixelShader->GetBuffer(),
			.BytecodeLength = InPixelShader->GetSize()
		};
	}
	//Stream output can capture the raw result of vertex, geometry or pixel shaders and store said results into a buffer for later use
	//This is optional and can be used for advanced computation technics 
	m_pipelineStateDesc2D.StreamOutput =
	{
		.pSODeclaration = nullptr,
		.NumEntries = 0,
		.pBufferStrides = nullptr,
		.NumStrides = 0,
		.RasterizedStream = 0
	};
	m_pipelineStateDesc2D.NodeMask = 0;
	m_pipelineStateDesc2D.CachedPSO = { .pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0 }; //Buffer of precompiled shader (compile on first lanch for GPU in modern games)
	
	m_pipelineStateDesc2D.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	//OutputMerger
	m_pipelineStateDesc2D.NumRenderTargets = 1;
	for (SIZE_T i = 0; i < m_pipelineStateDesc2D.NumRenderTargets; ++i)
	{
		m_pipelineStateDesc2D.RTVFormats[i] = DXWindowDefaults::SWAP_CHAIN_BUFFER_FORMAT;
	}
	m_pipelineStateDesc2D.DSVFormat = DXGI_FORMAT_UNKNOWN;
	//Output Merger Blend State
	m_pipelineStateDesc2D.BlendState = {
		.AlphaToCoverageEnable = false,		// for SMAA
		.IndependentBlendEnable = false,	// Same blend description for all render target (when using multiple RTV)
		.RenderTarget =
		{
			//[0] = 
			{
				.BlendEnable = true,				//Blend colors, if false nothing else there matter
				.LogicOpEnable = false,				//Use 'LogicOp' value for blending instead of specified RGB & Alpha below
				//Blending work by applying a blend to the "Source", another to the "Destionation", and then merging the two
				// results using the "BlendOp"
				//RGB blending
				.SrcBlend = D3D12_BLEND_SRC_ALPHA,		//Source is the new pixel computed
				.DestBlend = D3D12_BLEND_INV_SRC_ALPHA,		//BLEND_ZERO : take 0% of color | Destination is the value already on the buffer (ex : value of last clear)
				.BlendOp = D3D12_BLEND_OP_ADD,		//Add the two results
				//Alpha blending
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_ZERO,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.LogicOp = D3D12_LOGIC_OP_NOOP,		//Blending operation like 'and', 'xor' etc..., NOOP mean no operations
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL //D3D12_COLOR_WRITE_ENABLE value
			}
		}
	};
	//Output Merger Depth Stencil
	m_pipelineStateDesc2D.DepthStencilState = {
		.DepthEnable = false,								// using Depth ?
		.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,		// All or Zero
		.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS,			//Depth condition (ex : >, >=, <, etc ...)
		.StencilEnable = false,								// using Stencil for depth masking ?
		.StencilReadMask = 0,
		.StencilWriteMask = 0,
		.FrontFace = 
			{
				.StencilFailOp		= D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp		= D3D12_STENCIL_OP_KEEP,
				.StencilFunc		= D3D12_COMPARISON_FUNC_ALWAYS
			},
		.BackFace =
			{
				.StencilFailOp		= D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp		= D3D12_STENCIL_OP_KEEP,
				.StencilFunc		= D3D12_COMPARISON_FUNC_ALWAYS
			}
	};
	m_pipelineStateDesc2D.SampleMask = 0xFFFFFFFF;	//Keep every colors and alpha
	m_pipelineStateDesc2D.SampleDesc =
	{
		.Count = 1,		//MSAA Do 1 iteration ...
		.Quality = 0	// ..of doing nothing  ... (quality zero)
	};					// so a passthough, don't do MSAA

	//Indexing allow to reduce memory usage by accesing 
	// Indexing via index to vertex in an array
	// Strip reuse last 2 vertices to build next triangle with a new one, and again discarding first and adding another new etc...
}


void DXPipeline::SetupPipelineStateDesc3D(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader)
{
	//Input Assember
	static const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		// [O] pos : //float[3] Position
		{
			.SemanticName = "Position", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = 0, //No alignement needed since "Position" is the first data element, otherwise can set manually to sum of last element sizes or D3D12_APPEND_ALIGNED_ELEMENT is automatic value
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [1] norm : //float[3] Normal
		{
			.SemanticName = "Normal", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3), //offset by the 3 first float position
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [2] color : //float[3] Color
		{
			.SemanticName = "Color", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Color is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [3] uv0 : //float[2] Texcoord
		{
			.SemanticName = "Texcoord", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32_FLOAT, //float[2]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [4] textureIndex : //UINT32[1] Texindex
		{
			.SemanticName = "Texindex", //Texindex
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32_UINT, //UINT16
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3) + (/*Texcoord*/sizeof(float) * 2),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		}
	};
	m_pipelineStateDesc3D.pRootSignature = InRootSignature;
	m_pipelineStateDesc3D.InputLayout.NumElements = _countof(vertexLayout);
	m_pipelineStateDesc3D.InputLayout.pInputElementDescs = vertexLayout;

	m_pipelineStateDesc3D.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; //Indexing disabled

	//Vertex Shader
	if (!InVertexShader)
	{
		//Cannot have a working shader pipeline without a vertex shader
		return;
	}
	m_pipelineStateDesc3D.VS =
	{
		.pShaderBytecode = InVertexShader->GetBuffer(),
		.BytecodeLength = InVertexShader->GetSize()
	};

	//Geometry (Hull Shader, Tesselator, Domain Shader, Geometry Shader
	m_pipelineStateDesc3D.HS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDesc3D.DS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDesc3D.GS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };

	//Rasterizer
	m_pipelineStateDesc3D.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	m_pipelineStateDesc3D.RasterizerState =
	{
		.FillMode = D3D12_FILL_MODE_SOLID,								// Wireframe or solid rendering of pixel within our triangle
		.CullMode = D3D12_CULL_MODE_BACK,								// Todo later
		.FrontCounterClockwise = FALSE,									//Invert triangle rendering order to invert culling
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,							//Depth bias : to render object at same pos ex : shadows or decals on surfaces
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,				//Depth bias : See https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
		.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	//Depth bias : 
		.DepthClipEnable = TRUE,										// clipping to true by default if no stencil used
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	//Pixel Shader
	if (InPixelShader)
	{
		m_pipelineStateDesc3D.PS =
		{
			.pShaderBytecode = InPixelShader->GetBuffer(),
			.BytecodeLength = InPixelShader->GetSize()
		};
	}
	//Stream output can capture the raw result of vertex, geometry or pixel shaders and store said results into a buffer for later use
	//This is optional and can be used for advanced computation technics 
	m_pipelineStateDesc3D.StreamOutput =
	{
		.pSODeclaration = nullptr,
		.NumEntries = 0,
		.pBufferStrides = nullptr,
		.NumStrides = 0,
		.RasterizedStream = 0
	};
	m_pipelineStateDesc3D.NodeMask = 0;
	m_pipelineStateDesc3D.CachedPSO = { .pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0 }; //Buffer of precompiled shader (compile on first lanch for GPU in modern games)
	
	m_pipelineStateDesc3D.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	//OutputMerger
	m_pipelineStateDesc3D.NumRenderTargets = 1;
	for (SIZE_T i = 0; i < m_pipelineStateDesc3D.NumRenderTargets; ++i)
	{
		m_pipelineStateDesc3D.RTVFormats[i] = DXWindowDefaults::SWAP_CHAIN_BUFFER_FORMAT;
	}
	m_pipelineStateDesc3D.DSVFormat = DXWindowDefaults::DEPTH_BUFFER_ENABLED;
	//Output Merger Blend State
	m_pipelineStateDesc3D.BlendState = {
		.AlphaToCoverageEnable = FALSE,		// for SMAA
		.IndependentBlendEnable = FALSE,	// Same blend description for all render target (when using multiple RTV)
		.RenderTarget =
		{
			//[0] = 
			{
				.BlendEnable = FALSE,				//Blend colors, if false nothing else there matter
				.LogicOpEnable = FALSE,				//Use 'LogicOp' value for blending instead of specified RGB & Alpha below
				//Blending work by applying a blend to the "Source", another to the "Destionation", and then merging the two
				// results using the "BlendOp"
				//RGB blending
				.SrcBlend = D3D12_BLEND_ONE,		//Source is the new pixel computed
				.DestBlend = D3D12_BLEND_ZERO,		//BLEND_ZERO : take 0% of color | Destination is the value already on the buffer (ex : value of last clear)
				.BlendOp = D3D12_BLEND_OP_ADD,		//Add the two results
				//Alpha blending
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_ZERO,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.LogicOp = D3D12_LOGIC_OP_NOOP,		//Blending operation like 'and', 'xor' etc..., NOOP mean no operations
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL //D3D12_COLOR_WRITE_ENABLE value
			}
		}
	};

	//Output Merger Depth Stencil
	m_pipelineStateDesc3D.DepthStencilState = {
		.DepthEnable = TRUE,										// using Depth ?
		.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,				// All or Zero
		.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL,			//Depth condition (ex : >, >=, <, etc ...)
		.StencilEnable = FALSE,										// using Stencil for depth masking ?
		.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
		.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
		.FrontFace = 
			{
				.StencilFailOp		= D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp		= D3D12_STENCIL_OP_KEEP,
				.StencilFunc		= D3D12_COMPARISON_FUNC_ALWAYS
			},
		.BackFace =
			{
				.StencilFailOp		= D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp		= D3D12_STENCIL_OP_KEEP,
				.StencilFunc		= D3D12_COMPARISON_FUNC_ALWAYS
			}
	};

	m_pipelineStateDesc3D.SampleMask = UINT_MAX;	//Keep every colors and alpha
	m_pipelineStateDesc3D.SampleDesc =
	{
		.Count = 1,		//MSAA Do 1 iteration ...
		.Quality = 0	// ..of doing nothing  ... (quality zero)
	};					// so a passthough, don't do MSAA

	//Indexing allow to reduce memory usage by accesing 
	// Indexing via index to vertex in an array
	// Strip reuse last 2 vertices to build next triangle with a new one, and again discarding first and adding another new etc...

}

#if DEBUG_PHYSIC
void DXPipeline::SetupPipelineStateDescDebugPhysic(ID3D12RootSignature* InRootSignature, const Shader* InVertexShader, const Shader* InPixelShader)
{
	//Input Assember
	static const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		// [O] pos : //float[3] Position
		{
			.SemanticName = "Position", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = 0, //No alignement needed since "Position" is the first data element, otherwise can set manually to sum of last element sizes or D3D12_APPEND_ALIGNED_ELEMENT is automatic value
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [1] norm : //float[3] Normal
		{
			.SemanticName = "Normal", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3), //offset by the 3 first float position
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [2] color : //float[3] Color
		{
			.SemanticName = "Color", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3), //offset by the 3 first float position
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Color is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [3] uv0 : //float[2] Texcoord
		{
			.SemanticName = "Texcoord", //Name of the input for the shader
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32G32_FLOAT, //float[2]
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3), //offset by the 2 first float3 parameters
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		},
		// [4] textureIndex : //uint32_t[1] Texindex
		{
			.SemanticName = "Texindex", //Texindex
			.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
			.Format = DXGI_FORMAT_R32_UINT, //UINT16
			.InputSlot = 0, //Source vertex buffer, the first and only one
			.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3) + (/*Texcoord*/sizeof(float) * 2), //offset by the 2 first float3 parameters
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
			.InstanceDataStepRate = 0 // 0 per vertex
		}
	};
	m_pipelineStateDescDebugPhysic.pRootSignature = InRootSignature;
	m_pipelineStateDescDebugPhysic.InputLayout.NumElements = _countof(vertexLayout);
	m_pipelineStateDescDebugPhysic.InputLayout.pInputElementDescs = vertexLayout;

	m_pipelineStateDescDebugPhysic.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; //Indexing disabled

	//Vertex Shader
	if (!InVertexShader)
	{
		//Cannot have a working shader pipeline without a vertex shader
		return;
	}
	m_pipelineStateDescDebugPhysic.VS =
	{
		.pShaderBytecode = InVertexShader->GetBuffer(),
		.BytecodeLength = InVertexShader->GetSize()
	};

	//Geometry (Hull Shader, Tesselator, Domain Shader, Geometry Shader
	m_pipelineStateDescDebugPhysic.HS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDescDebugPhysic.DS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
	m_pipelineStateDescDebugPhysic.GS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };

	//Rasterizer
	m_pipelineStateDescDebugPhysic.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	m_pipelineStateDescDebugPhysic.RasterizerState =
	{
		.FillMode = D3D12_FILL_MODE_SOLID,								// Wireframe or solid rendering of pixel within our triangle
		.CullMode = D3D12_CULL_MODE_BACK,								// Todo later
		.FrontCounterClockwise = FALSE,									//Invert triangle rendering order to invert culling
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,							//Depth bias : to render object at same pos ex : shadows or decals on surfaces
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,				//Depth bias : See https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
		.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	//Depth bias : 
		.DepthClipEnable = TRUE,										// clipping to true by default if no stencil used
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	//Pixel Shader
	if (InPixelShader)
	{
		m_pipelineStateDescDebugPhysic.PS =
		{
			.pShaderBytecode = InPixelShader->GetBuffer(),
			.BytecodeLength = InPixelShader->GetSize()
		};
	}
	//Stream output can capture the raw result of vertex, geometry or pixel shaders and store said results into a buffer for later use
	//This is optional and can be used for advanced computation technics 
	m_pipelineStateDescDebugPhysic.StreamOutput =
	{
		.pSODeclaration = nullptr,
		.NumEntries = 0,
		.pBufferStrides = nullptr,
		.NumStrides = 0,
		.RasterizedStream = 0
	};
	m_pipelineStateDescDebugPhysic.NodeMask = 0;
	m_pipelineStateDescDebugPhysic.CachedPSO = { .pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0 }; //Buffer of precompiled shader (compile on first lanch for GPU in modern games)

	m_pipelineStateDescDebugPhysic.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	//OutputMerger
	m_pipelineStateDescDebugPhysic.NumRenderTargets = 1;
	for (SIZE_T i = 0; i < m_pipelineStateDescDebugPhysic.NumRenderTargets; ++i)
	{
		m_pipelineStateDescDebugPhysic.RTVFormats[i] = DXWindowDefaults::SWAP_CHAIN_BUFFER_FORMAT;
	}
	m_pipelineStateDescDebugPhysic.DSVFormat = DXWindowDefaults::DEPTH_BUFFER_ENABLED;
	//Output Merger Blend State
	m_pipelineStateDescDebugPhysic.BlendState = {
		.AlphaToCoverageEnable = FALSE,		// for SMAA
		.IndependentBlendEnable = FALSE,	// Same blend description for all render target (when using multiple RTV)
		.RenderTarget =
		{
			//[0] = 
			{
				.BlendEnable = TRUE,				//Blend colors, if false nothing else there matter
				.LogicOpEnable = FALSE,				//Use 'LogicOp' value for blending instead of specified RGB & Alpha below
				//Blending work by applying a blend to the "Source", another to the "Destionation", and then merging the two
				// results using the "BlendOp"
				//RGB blending
				.SrcBlend = D3D12_BLEND_SRC_ALPHA,		//Source is the new pixel computed
				.DestBlend = D3D12_BLEND_INV_SRC_ALPHA,		//BLEND_ZERO : take 0% of color | Destination is the value already on the buffer (ex : value of last clear)
				.BlendOp = D3D12_BLEND_OP_ADD,		//Add the two results
				//Alpha blending
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_ZERO,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.LogicOp = D3D12_LOGIC_OP_NOOP,		//Blending operation like 'and', 'xor' etc..., NOOP mean no operations
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL //D3D12_COLOR_WRITE_ENABLE value
			}
		}
	};

	//Output Merger Depth Stencil
	m_pipelineStateDescDebugPhysic.DepthStencilState = {
		.DepthEnable = TRUE,								// using Depth ?
		.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,		// All or Zero
		.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL,			//Depth condition (ex : >, >=, <, etc ...)
		.StencilEnable = FALSE,								// using Stencil for depth masking ?
		.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
		.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
		.FrontFace =
			{
				.StencilFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp = D3D12_STENCIL_OP_KEEP,
				.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
			},
		.BackFace =
			{
				.StencilFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
				.StencilPassOp = D3D12_STENCIL_OP_KEEP,
				.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS
			}
	};
	m_pipelineStateDescDebugPhysic.SampleMask = UINT_MAX;	//Keep every colors and alpha
	m_pipelineStateDescDebugPhysic.SampleDesc =
	{
		.Count = 1,		//MSAA Do 1 iteration ...
		.Quality = 0	// ..of doing nothing  ... (quality zero)
	};					// so a passthough, don't do MSAA

}
#endif
