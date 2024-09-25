#include "MDS/Tools/RTransform.h"

#include "MDS/Tools/RMath.h"
//#include "App/Tools/RSRLog.h"

using namespace DirectX;
using namespace mds;

XMMATRIX RTransform::ComputeTRSMatrix() const
{
    XMVECTOR vScale = XMLoadFloat3(&Scale);
    XMVECTOR vRotation =
    {
        XMConvertToRadians(Rotation.x),
        XMConvertToRadians(Rotation.y),
        XMConvertToRadians(Rotation.z), 0.f
    };
    XMVECTOR vTranslation = XMLoadFloat3(&Position);


    // Create the transformation matrix
    return XMMatrixTransformation(
        XMVectorZero(),                                            // scaling origin
        XMQuaternionIdentity(),                                    // rotation origin
        vScale,                                                             // scale vector
        XMVectorZero(),                                            // rotation origin
        XMQuaternionRotationRollPitchYawFromVector(vRotation),     // rotation quaternion
        vTranslation                                                        // translation vector
    );
}

DirectX::XMMATRIX XM_CALLCONV RTransform::ComputeTRSMatrix(DirectX::FXMVECTOR InQuaternionRad) const
{
    XMVECTOR vScale = XMLoadFloat3(&Scale);
    XMVECTOR vRotation =
    {
        XMConvertToRadians(Rotation.x),
        XMConvertToRadians(Rotation.y),
        XMConvertToRadians(Rotation.z), 0.f
    };
    XMVECTOR vTranslation = XMLoadFloat3(&Position);


    // Create the transformation matrix
    return XMMatrixTransformation(
        XMVectorZero(),                                            // scaling origin
        XMQuaternionIdentity(),                                    // rotation origin
        vScale,                                                             // scale vector
        XMVectorZero(),                                            // rotation origin
        InQuaternionRad,     // rotation quaternion
        vTranslation                                                        // translation vector
    );
}

RTransformMatrix::RTransformMatrix(const RTransform& InTransform)
{
    SetTransform(InTransform);
}

void RTransformMatrix::SetTransform(const RTransform& InTransform)
{
    m_transform = InTransform;
    XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
}

void RTransformMatrix::SetPositon(const XMFLOAT3& InPosition)
{
    m_transform.Position = InPosition;
    XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
}

void RTransformMatrix::SetRotation(const XMFLOAT3& InRotation)
{
    m_transform.Rotation = InRotation;
    XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
}

void RTransformMatrix::SetScale(const XMFLOAT3& InScale)
{
    m_transform.Scale = InScale;
    XMStoreFloat4x4(&this->m_model, m_transform.ComputeTRSMatrix());
}

void RTransformMatrix::LookAtPosition(const XMFLOAT3& InTarget, const XMFLOAT3& InUpVector)
{
    XMVECTOR Location = XMLoadFloat3(&this->m_transform.Position);
    XMVECTOR AtLocation = XMLoadFloat3(&InTarget);
    XMVECTOR UpVector = XMLoadFloat3(&InUpVector);

    XMMATRIX lookAtMatrix = XMMatrixLookAtLH(Location, AtLocation, UpVector);
    XMVECTOR rotationQuaternion = XMQuaternionRotationMatrix(lookAtMatrix);
    XMVECTOR eulerLookAtAngle = mds::RMath::QuatToEulerDegXMVECTOR(rotationQuaternion);
    XMFLOAT3 eulerLookAtFLT3;
    XMStoreFloat3(&eulerLookAtFLT3, eulerLookAtAngle);
    this->SetRotation(eulerLookAtFLT3);
}
