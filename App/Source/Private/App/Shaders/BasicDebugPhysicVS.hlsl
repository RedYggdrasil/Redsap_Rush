#include "BasicRootSignatureDebugPhysic.hlsl"
#include "BasicPipelineDebugPhysic.hlsli"

cbuffer VSConstants : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4x4 ModelMatrix;
}


[RootSignature(ROOTSIGDP)]
void main
(
    //-- In --
    in float3 pos : Position,
    in float3 norm : Normal,
    in float4 col : Color,
    in float2 uv : Texcoord,
    //-- Out -- Order matter, must be the same of input of next shader
    out float2 o_uv : Texcoord,
    out float4 o_col : OutColor,
    out float4 o_pos : SV_Position
)
{
    //Rules of Transformation. Model -> View -> Projection
    
    
    //Compute Position in Clip Space
    
    // Convert the position into homogeneous coordinates
    o_pos = float4(pos, 1.0f);
    
    // Apply the model matrix to transform from model space to world space
    
    o_pos = mul(ModelMatrix, o_pos);
    o_pos = mul(ViewProjectionMatrix, o_pos);
    o_uv = uv;
    o_col = col;
}

