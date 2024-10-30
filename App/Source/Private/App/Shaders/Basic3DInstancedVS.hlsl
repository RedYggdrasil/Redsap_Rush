#include "BasicRootSignature3DInstanced.hlsl"
#include "BasicPipeline3DInstanced.hlsli"

uint GetUint8FromUint4(uint4 data, uint index)
{
    uint uintIndex = index / 4;
    uint byteIndex = index % 4;
    return (data[uintIndex] >> (byteIndex * 8)) & 0xFF;
}

[RootSignature(ROOTSIG3DINSTANCED)]
void main
(
    //-- In --
    in VS_INPUT inputs,
    in VS_INSTANCE instances,
    //-- Out -- 
    out VS_OUTPUT outputs
)
{
    outputs.WorldNormal = mul((float3x3) instances.InvProjModel, inputs.Normal);
    
    outputs.Position = mul(instances.ModelMatrix, float4(inputs.Position, 1.0f));
    outputs.WorldPosition = outputs.Position.xyz;
    
    outputs.Position = mul(ViewProjectionMatrix, outputs.Position);
    outputs.UV = inputs.UV;

    
    //It seems describing root as uint8*4 map to full array of 4 uint, so HLSL is uint4 ToGlobalTextureIndex[4] : ToGlobalTextureIndex;
    outputs.TexIndex = instances.ToGlobalTextureIndex[inputs.TexIndex / 4][inputs.TexIndex % 4]; //(uint) GetUint8FromUint4(instances.ToGlobalTextureIndex, 4/*inputs.TexIndex*/ );
    outputs.Emissiveness = instances.Emissiveness;
}

