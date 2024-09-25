#include "App/Data/RSRCameraData.h"

#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"

using namespace DirectX;
using namespace RSRush;

XMMATRIX RSRCameraData::ComputeView() const
{
    // Convert them to XMVECTOR
    XMVECTOR vPosition = XMLoadFloat3(&Position);
    XMVECTOR vTarget = XMLoadFloat3(&Target);
    XMVECTOR vUp = XMLoadFloat3(&Up);

    // Create the view matrix
   return XMMatrixLookAtLH(vPosition, vTarget, vUp);
}

XMMATRIX RSRCameraData::ComputeProjection() const
{
    // Create the projection matrix
    return XMMatrixPerspectiveFovLH(FOV, AspectRatio, NearClip, FarClip);
}

XMMATRIX RSRCameraData::ComputeViewProjection() const
{
    return XMMatrixMultiply(ComputeView(), ComputeProjection());
}
