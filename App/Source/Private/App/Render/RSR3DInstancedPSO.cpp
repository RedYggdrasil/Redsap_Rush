#include "App/Render/RSR3DInstancedPSO.h"
#include "App/Render/RSRShaderList.h"
#include "App/Tools/Shader.h"
#include "App/Tools/Window.h"

using namespace RSRush;

RSR3DInstancedPSO::RSR3DInstancedPSO(RSRProgramInstance* InProgramInstance)
	: RSR3DInstancedPSO(InProgramInstance, std::make_shared<Shader>(EShaderName::BasicRootSignatureInstanced3D), std::make_shared<Shader>(EShaderName::basicVS3DInstancedShader), std::make_shared<Shader>(EShaderName::basicPS3DInstancedShader))
{

}

RSR3DInstancedPSO::RSR3DInstancedPSO(RSRProgramInstance* InProgramInstance, std::shared_ptr<Shader> InRootSigShader, std::shared_ptr<Shader> InVertexShader, const std::shared_ptr<Shader> InPixelShader)
	: RSRPipelineStateObject(InProgramInstance, InRootSigShader, InVertexShader, InPixelShader)
{
	bool bStateDescSucessfull = m_bIsCorrectlyLoaded;
	if (bStateDescSucessfull)
	{	//Input Assember
		static const D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
		{
			// VERTEX [O] pos : //float[3] Position
			{
				.SemanticName = "Position", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
				.InputSlot = 0, //Source vertex buffer, the first as it is directly in vertex buffer
				.AlignedByteOffset = 0, //No alignement needed since "Position" is the first data element, otherwise can set manually to sum of last element sizes or D3D12_APPEND_ALIGNED_ELEMENT is automatic value
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 0 // 0 per vertex
			},
			// VERTEX [1] norm : //float[3] Normal
			{
				.SemanticName = "Normal", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
				.InputSlot = 0, //Source vertex buffer, the first as it is directly in vertex buffer
				.AlignedByteOffset = (/*Position*/sizeof(float) * 3), //offset by the 3 first float position
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 0 // 0 per vertex
			},
			// VERTEX [2] color : //float[3] Color
			{
				.SemanticName = "Color", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32_FLOAT, //float[3]
				.InputSlot = 0, //Source vertex buffer, the first as it is directly in vertex buffer
				.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Color is used per vertex or per vertex index
				.InstanceDataStepRate = 0 // 0 per vertex
			},
			// VERTEX [3] uv0 : //float[2] Texcoord
			{
				.SemanticName = "Texcoord", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32_FLOAT, //float[2]
				.InputSlot = 0, //Source vertex buffer, the first as it is directly in vertex buffer
				.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 0 // 0 per vertex
			},
			// VERTEX [4] textureIndex : //UINT32[1] Texindex
			{
				.SemanticName = "Texindex", //Texindex
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32_UINT, //UINT16
				.InputSlot = 0, //Source vertex buffer, the first as it is directly in vertex buffer
				.AlignedByteOffset = (/*Position*/sizeof(float) * 3) + (/*Normal*/sizeof(float) * 3) + (/*Color*/sizeof(float) * 3) + (/*Texcoord*/sizeof(float) * 2),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 0 // 0 per vertex
			},

			// INSTANCE [O] Modelmat : //float[4] Modelmat[0]
			{
				.SemanticName = "Modelmat", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = 0, //No alignement needed since "Position" is the first data element, otherwise can set manually to sum of last element sizes or D3D12_APPEND_ALIGNED_ELEMENT is automatic value
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [1] Modelmat : //float[4] Modelmat[1]
			{
				.SemanticName = "Modelmat", //Name of the input for the shader
				.SemanticIndex = 1, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = (/*Modelmat[0]*/sizeof(float) * 4) * 1, //offset by the 4 first float Modelmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [2] Modelmat : //float[4] Modelmat[2]
			{
				.SemanticName = "Modelmat", //Name of the input for the shader
				.SemanticIndex = 2, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = (/*Modelmat[0]*/sizeof(float) * 4) * 2, //offset by the 2 * 4 first float Modelmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [3] Modelmat : //float[4] Modelmat[3]
			{
				.SemanticName = "Modelmat", //Name of the input for the shader
				.SemanticIndex = 3, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = (/*Modelmat[0]*/sizeof(float) * 4) * 3, //offset by the 3 * 4 first float Modelmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [4] Modelinvmat : //float[4] Modelinvmat[0]
			{
				.SemanticName = "Modelinvmat", //Name of the input for the shader
				.SemanticIndex = 0, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = (/*Modelmat[0]*/sizeof(float) * 4) * 4, //offset by the 4 * 4 first float Modelmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [5] Modelinvmat : //float[4] Modelinvmat[1]
			{
				.SemanticName = "Modelinvmat", //Name of the input for the shader
				.SemanticIndex = 1, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 1), //offset by the 4 * 4 first float Modelmat + 1 * 4 first float Modelinvmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [6] Modelinvmat : //float[4] Modelinvmat[2]
			{
				.SemanticName = "Modelinvmat", //Name of the input for the shader
				.SemanticIndex = 2, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 2), //offset by the 4 * 4 first float Modelmat + 2 * 4 first float Modelinvmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [7] Modelinvmat : //float[4] Modelinvmat[3]
			{
				.SemanticName = "Modelinvmat", //Name of the input for the shader
				.SemanticIndex = 3, // if multiple input share the name '.SemanticName', this index differenciate them. ex : "TEXTCOORD" 0, 1, 2 ...
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, //float[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 3), //offset by the 4 * 4 first float Modelmat + 3 * 4 first float Modelinvmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},
			// INSTANCE [8] ToGlobalTextureIndex : //uint[4]
			{
				.SemanticName = "ToGlobalTextureIndex", //Name of the input for the shader
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R8G8B8A8_UINT, //uint8[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 4), //offset by the 4 * 4 first float Modelmat + 4 * 4 first float Modelinvmat
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},// INSTANCE [9] ToGlobalTextureIndex : //uint[4]
			{
				.SemanticName = "ToGlobalTextureIndex", //Name of the input for the shader
				.SemanticIndex = 1,
				.Format = DXGI_FORMAT_R8G8B8A8_UINT, //uint8[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 4) + (sizeof(uint32_t) * 1), //offset by the 4 * 4 first float Modelmat + 4 * 4 first float Modelinvmat + 1 * 4 first packed uint8[4]
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},// INSTANCE [10] ToGlobalTextureIndex : //uint[4]
			{
				.SemanticName = "ToGlobalTextureIndex", //Name of the input for the shader
				.SemanticIndex = 2,
				.Format = DXGI_FORMAT_R8G8B8A8_UINT, //uint8[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 4) + (sizeof(uint32_t) * 2), //offset by the 4 * 4 first float Modelmat + 4 * 4 first float Modelinvmat + 2 * 4 first packed uint8[4]
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},// INSTANCE [11] ToGlobalTextureIndex : //uint[4]
			{
				.SemanticName = "ToGlobalTextureIndex", //Name of the input for the shader
				.SemanticIndex = 3,
				.Format = DXGI_FORMAT_R8G8B8A8_UINT, //uint8[4]
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 4) + (sizeof(uint32_t) * 3), //offset by the 4 * 4 first float Modelmat + 4 * 4 first float Modelinvmat + 3 * 4 first packed uint8[4]
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			},// INSTANCE [12] Emissiveness : //float
			{
				.SemanticName = "Emissiveness",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32_FLOAT, //float
				.InputSlot = 1, //Source vertex buffer, the second as it is on Instance Vertex Data
				.AlignedByteOffset = ((/*Modelmat[0]*/sizeof(float) * 4) * 4) + ((/*Modelinvmat[0]*/sizeof(float) * 4) * 4) + (sizeof(uint32_t) * 4),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, // Vertex is used per vertex or per vertex index
				.InstanceDataStepRate = 1 // 1 per instance
			}
		};
		m_pipelineStateDesc.pRootSignature = this->m_rootSig.Get();
		m_pipelineStateDesc.InputLayout.NumElements = _countof(vertexLayout);
		m_pipelineStateDesc.InputLayout.pInputElementDescs = vertexLayout;

		m_pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; //Indexing disabled

		//Vertex Shader
		if (!InVertexShader)
		{
			//Cannot have a working shader pipeline without a vertex shader
			return;
		}
		m_pipelineStateDesc.VS =
		{
			.pShaderBytecode = InVertexShader->GetBuffer(),
			.BytecodeLength = InVertexShader->GetSize()
		};

		//Geometry (Hull Shader, Tesselator, Domain Shader, Geometry Shader
		m_pipelineStateDesc.HS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
		m_pipelineStateDesc.DS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };
		m_pipelineStateDesc.GS = { .pShaderBytecode = nullptr, .BytecodeLength = 0 };

		//Rasterizer
		m_pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		m_pipelineStateDesc.RasterizerState =
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
			m_pipelineStateDesc.PS =
			{
				.pShaderBytecode = InPixelShader->GetBuffer(),
				.BytecodeLength = InPixelShader->GetSize()
			};
		}
		//Stream output can capture the raw result of vertex, geometry or pixel shaders and store said results into a buffer for later use
		//This is optional and can be used for advanced computation technics 
		m_pipelineStateDesc.StreamOutput =
		{
			.pSODeclaration = nullptr,
			.NumEntries = 0,
			.pBufferStrides = nullptr,
			.NumStrides = 0,
			.RasterizedStream = 0
		};
		m_pipelineStateDesc.NodeMask = 0;
		m_pipelineStateDesc.CachedPSO = { .pCachedBlob = nullptr, .CachedBlobSizeInBytes = 0 }; //Buffer of precompiled shader (compile on first lanch for GPU in modern games)

		m_pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		//OutputMerger
		m_pipelineStateDesc.NumRenderTargets = 1;
		for (SIZE_T i = 0; i < m_pipelineStateDesc.NumRenderTargets; ++i)
		{
			m_pipelineStateDesc.RTVFormats[i] = DXWindowDefaults::SWAP_CHAIN_BUFFER_FORMAT;
		}
		m_pipelineStateDesc.DSVFormat = DXWindowDefaults::DEPTH_BUFFER_ENABLED;
		//Output Merger Blend State
		m_pipelineStateDesc.BlendState = {
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
		m_pipelineStateDesc.DepthStencilState = {
			.DepthEnable = TRUE,										// using Depth ?
			.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,				// All or Zero
			.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL,			//Depth condition (ex : >, >=, <, etc ...)
			.StencilEnable = FALSE,										// using Stencil for depth masking ?
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

		m_pipelineStateDesc.SampleMask = UINT_MAX;	//Keep every colors and alpha
		m_pipelineStateDesc.SampleDesc =
		{
			.Count = 1,		//MSAA Do 1 iteration ...
			.Quality = 0	// ..of doing nothing  ... (quality zero)
		};					// so a passthough, don't do MSAA

		//Indexing allow to reduce memory usage by accesing 
		// Indexing via index to vertex in an array
		// Strip reuse last 2 vertices to build next triangle with a new one, and again discarding first and adding another new etc...
	}
	bStateDescSucessfull = bStateDescSucessfull && CreatePSO();


	m_bIsCorrectlyLoaded = bStateDescSucessfull;
}

RSR3DInstancedPSO::~RSR3DInstancedPSO()
{

}
