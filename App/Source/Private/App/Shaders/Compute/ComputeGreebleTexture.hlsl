#include "ComputeRootSignature.hlsl"
#include "ComputePipeline.hlsli"
#include "../SimplexNoise2D.hlsli"


RWTexture2D<float4> buffer : register(u0);


[RootSignature(ROOTSIGCOMPUTE)]
[numthreads(8, 8, 1)]
void main
(
    //-- In --
    uint3 dispatchThreadID : SV_DispatchThreadID
)
{
    int width;
    int height;
    buffer.GetDimensions(width, height);
    
    float2 uv = dispatchThreadID.xy / float2(width, height);
    float grayscale = snoise01(uv * 4.f);
    float roundAmount = 3.f;
    int grayscale10 = round(grayscale * roundAmount);
    grayscale = float(grayscale10) * (1.f / roundAmount);
    buffer[dispatchThreadID.xy] = float4(grayscale, grayscale, grayscale, 1.0f);
}