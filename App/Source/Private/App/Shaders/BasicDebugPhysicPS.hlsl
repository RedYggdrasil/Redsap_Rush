#include "BasicRootSignatureDebugPhysic.hlsl"

[RootSignature(ROOTSIGDP)]
void main
(
    //-- In --
    in float2 uv : Texcoord,
    in float4 col : OutColor,

    //-- Out --
    out float4 pixel : SV_Target
)
{
    pixel = float4(col.xyz, 0.5);
}