
cbuffer VSConstants : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4 CameraPosition;
    float4 LightPosition_AmbiantInt;
    float4 LightColor_SpecularInt;
}

struct Correction
{
    float aspectRatio;
    float zoom;
    float sinAngle;
    float cosAngle;
};

struct VS_INPUT
{
    float3 Position : Position;
    float3 Normal : Normal;
    float3 Color : Color;
    float2 UV : Texcoord;
    uint TexIndex : Texindex;
};

struct VS_INSTANCE
{
    float4x4 ModelMatrix : Modelmat;
    float4x4 InvProjModel : Modelinvmat;
    uint4 ToGlobalTextureIndex[4] : ToGlobalTextureIndex;
    float Emissiveness : Emissiveness;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 WorldPosition : FRAG_POSITION;
    float3 WorldNormal : FRAG_NORMAL;
    float2 UV : Texcoord;
    uint TexIndex : TexIndex;
    float Emissiveness : Emissiveness;
};