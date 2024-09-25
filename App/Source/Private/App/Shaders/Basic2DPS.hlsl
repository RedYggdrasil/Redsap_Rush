#include "BasicRootSignature2D.hlsl"
#include "BasicPipeline2D.hlsli"

UICBV cbvData : register(b0);
Correction correction : register(b1);

Texture2D<float4> textures[] : register(t0);

sampler textureSampler : register(s0);

[RootSignature(ROOTSIG2D)]
void main
(
    //-- In --TexIndex
    in VS_OUTPUT vsOutput,

    //-- Out --
    out float4 pixel : SV_Target
)
{
    float4 texel = textures[vsOutput.TexIndex].Sample(textureSampler, vsOutput.UV);
    pixel = float4(texel.rgba * cbvData.color.rgba);
}