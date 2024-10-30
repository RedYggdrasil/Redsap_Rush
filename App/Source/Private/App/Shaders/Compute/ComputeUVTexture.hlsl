#include "ComputeRootSignature.hlsl"
#include "ComputePipeline.hlsli"


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
    buffer[dispatchThreadID.xy] = float4(uv.xy, 0.0f, 1.0f);
}