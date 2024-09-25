#include "BasicRootSignature3D.hlsl"
#include "BasicPipeline3D.hlsli"

Texture2D<float4> textures[] : register(t0);
sampler textureSampler : register(s0);

[RootSignature(ROOTSIG3D)]
void main
(
    //-- In --TexIndex
    in VS_OUTPUT vsOutput,
    //-- Out --
    out float4 pixel : SV_Target
)
{
    float4 texel = textures[vsOutput.TexIndex].Sample(textureSampler, vsOutput.UV);

    
    //phong
    
    float3 norm = normalize(vsOutput.WorldNormal);
    
    float3 lightPos = LightPosition_AmbiantInt.xyz; // float3(50.0f, 30.0f, 40.0f);
    float3 lightDir = normalize(lightPos - vsOutput.WorldPosition);
    
    //Ambiant
    float ambiantStrength = LightPosition_AmbiantInt.w;
    float3 ambiantColor = LightColor_SpecularInt.rgb; //float3(1.0f, 0.0f, 1.0f);
    float3 ambiant = mul(ambiantColor, ambiantStrength);
    
    
    //Diffuse
    float3 diffuseStrength = 1.0f;
    float3 diffuseColor = LightColor_SpecularInt.rgb; //float3(0.0f, 1.0f, 0.6f);
    diffuseColor = mul(diffuseColor, diffuseStrength);
    
    float3 diffuseEffectiveStrength = max(dot(norm, lightDir), 0.0f);
    float3 diffuse = mul(diffuseEffectiveStrength, diffuseColor);
    
    
    //Specular
    float3 specularStrength = LightColor_SpecularInt.a;
    float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    float3 viewDirection = normalize(((float3) CameraPosition) - vsOutput.WorldPosition);
    float3 reflectionDirection = reflect(-lightDir, norm);
    float specIntencity = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 64.0f);
    float3 specular = specularStrength * specularColor * specIntencity;
    
    float3 pResult = (ambiant + diffuse + specular) * texel.rgb;
    pixel = float4(pResult, 1.0f);
}