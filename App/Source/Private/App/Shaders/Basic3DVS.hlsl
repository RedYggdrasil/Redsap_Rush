#include "BasicRootSignature3D.hlsl"
#include "BasicPipeline3D.hlsli"


[RootSignature(ROOTSIG3D)]
void main
(
    //-- In --
    in VS_INPUT inputs,
    //-- Out -- 
    out VS_OUTPUT outputs
)
{
    outputs.WorldNormal = mul((float3x3) InvProjModel, inputs.Normal);
    
    outputs.Position = mul(ModelMatrix, float4(inputs.Position, 1.0f));
    outputs.WorldPosition = outputs.Position.xyz;
    
    
    outputs.Position = mul(ViewProjectionMatrix, outputs.Position);
    outputs.UV = inputs.UV;
    outputs.TexIndex = inputs.TexIndex;
}

