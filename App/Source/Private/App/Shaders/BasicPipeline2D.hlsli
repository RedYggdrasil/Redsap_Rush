struct MeshCBV
{
    float4x4 modelMatrix;
};
struct ContextCBV
{
    float4x4 viewProjectionMatrix;
};
struct Correction
{
    float aspectRatio;
    float zoom;
    float sinAngle;
    float cosAngle;
};

struct UICBV
{
    float4 rect;
    float4 ratio;
    float4 color;
};

struct VS_INPUT
{
    /* { PivotX, PivotY, PosX, PosY } */
    float4 Position : Position;
    /* { R, G, B, A } */
    float4 Color : Color;
    /* { U, V } */
    float2 UV : Texcoord;
    uint TexIndex : Texindex;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 UV : Texcoord;
    uint TexIndex : TexIndex;
};