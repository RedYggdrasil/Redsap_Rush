#include "BasicRootSignature2D.hlsl"
#include "BasicPipeline2D.hlsli"

UICBV cbvData : register(b0);
Correction correction : register(b1);

[RootSignature(ROOTSIG2D)]
void main
(
    //-- In --
    in VS_INPUT inputs,
    //-- Out -- Order matter, must be the same of input of next shader
    out VS_OUTPUT outputs
)
{
    float2 px;
    //      MVP trs *       NormalizedPivot +           Scaled position
    px.x = /*+1.f * */  ( inputs.Position.x + (( /* + */inputs.Position.z) * cbvData.ratio.x) );
    px.y =        ( inputs.Position.y + (( /* + */inputs.Position.w) * cbvData.ratio.y) );
    px.x = (px.x + cbvData.rect.x) * cbvData.rect.z;
    px.y = (px.y + cbvData.rect.y) * cbvData.rect.w;
    px.y = -px.y;
    //{ PivotX, PivotY, PosX, PosY } 
    //inputs.Position
    //Rules of Transformation. Model -> View -> Projection
    //float2 px;
    //
    ////Model
    //px.x = (pos.x * correction.cosAngle) - (pos.y * correction.sinAngle);
    //px.y = (pos.x * correction.sinAngle) + (pos.y * correction.cosAngle);
    //
    ////View
    //px *= correction.zoom;
    //
    ////Projection
    //px.x *= correction.aspectRatio; 
    outputs.Position = float4(px.xy, 0.0f, 1.0f);
    outputs.Color = inputs.Color;
    outputs.UV = inputs.UV;
    outputs.TexIndex = inputs.TexIndex;
}

