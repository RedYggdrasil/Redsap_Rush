#include "MDS/Tools/RMath.h"


#define _USE_MATH_DEFINES
#include <math.h>

const double mds::RMath::PI = M_PI;

//Pure math
double mds::RMath::Cos(double rad)
{
	return cos(rad);
}

double mds::RMath::Sin(double rad)
{
	return sin(rad);
}

double mds::RMath::Tan(double rad)
{
	return tan(rad);
}

//DirectXMath
using namespace DirectX;

DirectX::XMFLOAT3 mds::RMath::ToEuler(DirectX::XMFLOAT4 InQuat) noexcept
{
	const float xx = InQuat.x * InQuat.x;
	const float yy = InQuat.y * InQuat.y;
	const float zz = InQuat.z * InQuat.z;

	const float m31 = 2.f * InQuat.x * InQuat.z + 2.f * InQuat.y * InQuat.w;
	const float m32 = 2.f * InQuat.y * InQuat.z - 2.f * InQuat.x * InQuat.w;
	const float m33 = 1.f - 2.f * xx - 2.f * yy;

	const float cy = sqrtf(m33 * m33 + m31 * m31);
	const float cx = atan2f(-m32, cy);
	if (cy > 16.f * FLT_EPSILON)
	{
		const float m12 = 2.f * InQuat.x * InQuat.y + 2.f * InQuat.z * InQuat.w;
		const float m22 = 1.f - 2.f * xx - 2.f * zz;

		return XMFLOAT3{ cx, atan2f(m31, m33), atan2f(m12, m22) };
	}
	else
	{
		const float m11 = 1.f - 2.f * yy - 2.f * zz;
		const float m21 = 2.f * InQuat.x * InQuat.y - 2.f * InQuat.z * InQuat.w;

		return XMFLOAT3{ cx, 0.f, atan2f(-m21, m11) };
	}
}

XMMATRIX mds::RMath::TransformMatrixDeg(const XMFLOAT3& InLocation, const XMFLOAT3& InDegreeRotation, const XMFLOAT3& InScale)
{
	XMVECTOR vTranslation = XMLoadFloat3(&InLocation);
	XMVECTOR vRotation = { XMConvertToRadians(InDegreeRotation.x),XMConvertToRadians(InDegreeRotation.y), XMConvertToRadians(InDegreeRotation.z), 0.f };
	XMVECTOR vScale = XMLoadFloat3(&InScale);
	
	// Create the transformation matrix
	return XMMatrixTransformation(
		XMVectorZero(), // scaling origin
		XMQuaternionIdentity(), // rotation origin
		vScale, // scale vector
		XMVectorZero(), // rotation origin
		XMQuaternionRotationRollPitchYawFromVector(vRotation), // rotation quaternion
		vTranslation // translation vector
	);
}
XMMATRIX mds::RMath::TransformMatrixRad(const XMFLOAT3 & InLocation, const XMFLOAT3 & InRadianRotation, const XMFLOAT3 & InScale)
{
	XMVECTOR vTranslation = XMLoadFloat3(&InLocation);
	XMVECTOR vRotation = XMLoadFloat3(&InRadianRotation);
	XMVECTOR vScale = XMLoadFloat3(&InScale);
	
	// Create the transformation matrix
	return XMMatrixTransformation(
		XMVectorZero(), // scaling origin
		XMQuaternionIdentity(), // rotation origin
		vScale, // scale vector
		XMVectorZero(), // rotation origin
		XMQuaternionRotationRollPitchYawFromVector(vRotation), // rotation quaternion
		vTranslation // translation vector
	);
}

BoundingBox mds::RMath::AABBFromOrientedBox(const BoundingOrientedBox& InOrientedBox)
{
	BoundingBox AABB;
	XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
	InOrientedBox.GetCorners(corners);
	BoundingBox::CreateFromPoints(AABB, BoundingOrientedBox::CORNER_COUNT, corners, sizeof(XMFLOAT3));
	return AABB;
}

void mds::RMath::ExtendAABBWithOrientedBox(BoundingBox& InOutAAB, const BoundingOrientedBox& InOrientedBox)
{
	XMFLOAT3 Min = XMFLOAT3
		(InOutAAB.Center.x - InOutAAB.Extents.x, InOutAAB.Center.y - InOutAAB.Extents.y, InOutAAB.Center.z - InOutAAB.Extents.z);
	XMFLOAT3 Max = XMFLOAT3
		(InOutAAB.Center.x + InOutAAB.Extents.x, InOutAAB.Center.y + InOutAAB.Extents.y, InOutAAB.Center.z + InOutAAB.Extents.z);


	XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
	InOrientedBox.GetCorners(corners);
	for (size_t i = 0; i < BoundingOrientedBox::CORNER_COUNT; ++i)
	{
		Min.x = std::min(Min.x, corners[i].x);
		Min.y = std::min(Min.y, corners[i].y);
		Min.z = std::min(Min.z, corners[i].z);

		Max.x = std::max(Max.x, corners[i].x);
		Max.y = std::max(Max.y, corners[i].y);
		Max.z = std::max(Max.z, corners[i].z);
	}

	InOutAAB.Center = XMFLOAT3
	(
		(Min.x + Max.x) * 0.5f,
		(Min.y + Max.y) * 0.5f,
		(Min.z + Max.z) * 0.5f
	);
	InOutAAB.Extents = XMFLOAT3
	(
		(Max.x - Min.x)* 0.5f,
		(Max.y - Min.y)* 0.5f,
		(Max.z - Min.z)* 0.5f
	);
}

void mds::RMath::ExtendAABBWithSphere(BoundingBox& InOutAAB, const BoundingSphere& InSphere)
{
	XMFLOAT3 Min = XMFLOAT3
	(
		InOutAAB.Center.x - InOutAAB.Extents.x,
		InOutAAB.Center.y - InOutAAB.Extents.y,
		InOutAAB.Center.z - InOutAAB.Extents.z
	);
	XMFLOAT3 Max = XMFLOAT3
	(
		InOutAAB.Center.x + InOutAAB.Extents.x,
		InOutAAB.Center.y + InOutAAB.Extents.y,
		InOutAAB.Center.z + InOutAAB.Extents.z
	);

	// Calculate the min and max points of the sphere
	XMFLOAT3 SphereMin = XMFLOAT3
	(
		InSphere.Center.x - InSphere.Radius,
		InSphere.Center.y - InSphere.Radius,
		InSphere.Center.z - InSphere.Radius
	);
	XMFLOAT3 SphereMax = XMFLOAT3
	(
		InSphere.Center.x + InSphere.Radius,
		InSphere.Center.y + InSphere.Radius,
		InSphere.Center.z + InSphere.Radius
	);

	// Update the min and max points of the AABB
	Min.x = std::min(Min.x, SphereMin.x);
	Min.y = std::min(Min.y, SphereMin.y);
	Min.z = std::min(Min.z, SphereMin.z);

	Max.x = std::max(Max.x, SphereMax.x);
	Max.y = std::max(Max.y, SphereMax.y);
	Max.z = std::max(Max.z, SphereMax.z);

	// Update the center and extents of the AABB
	InOutAAB.Center = XMFLOAT3
	(
		(Min.x + Max.x) * 0.5f,
		(Min.y + Max.y) * 0.5f,
		(Min.z + Max.z) * 0.5f
	);
	InOutAAB.Extents = XMFLOAT3
	(
		(Max.x - Min.x) * 0.5f,
		(Max.y - Min.y) * 0.5f,
		(Max.z - Min.z) * 0.5f
	);
}


DirectX::XMVECTOR mds::RMath::ComputeNormalFromPointsClockwise(DirectX::FXMVECTOR InPointOne, DirectX::FXMVECTOR InPointTwo, DirectX::FXMVECTOR InPointThree)
{
	//Two vectors residing on the plane
	DirectX::XMVECTOR vec1 = DirectX::XMVectorSubtract(InPointTwo, InPointOne);
	DirectX::XMVECTOR vec2 = DirectX::XMVectorSubtract(InPointThree, InPointOne);

	// Cross product gives the normal
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(vec1, vec2);

	return DirectX::XMVector3Normalize(norm);
}

DirectX::XMFLOAT3 mds::RMath::QuatToEuler(DirectX::XMFLOAT4 InQuaternion)
{
	float w = InQuaternion.w;
	float x = InQuaternion.x;
	float y = InQuaternion.y;
	float z = InQuaternion.z;

	float yaw = atan2f(2.0f * (y * z + w * x), w * w - x * x - y * y + z * z);
	float pitch = asinf(-2.0f * (x * z - w * y));
	float roll = atan2f(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z);
	return { roll , pitch , yaw };
}

DirectX::XMVECTOR XM_CALLCONV mds::RMath::QuatToEulerDegXMVECTOR(DirectX::FXMVECTOR InQuaternion)
{
	float w = DirectX::XMVectorGetW(InQuaternion);
	float x = DirectX::XMVectorGetX(InQuaternion);
	float y = DirectX::XMVectorGetY(InQuaternion);
	float z = DirectX::XMVectorGetZ(InQuaternion);

	float yaw = atan2f(2.0f * (y * z + w * x), w * w - x * x - y * y + z * z);
	float pitch = asinf(-2.0f * (x * z - w * y));
	float roll = atan2f(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z);
	
	roll = XMConvertToDegrees(roll);
	pitch = XMConvertToDegrees(pitch);
	yaw = XMConvertToDegrees(yaw);
	
	return XMVectorSet(yaw, pitch, roll,  0.f);
}

DirectX::XMVECTOR XM_CALLCONV mds::RMath::LookAtQuatRad(DirectX::FXMVECTOR InFromPosition, DirectX::FXMVECTOR InToPosition)
{
	return FromForwardVectorQuatRad(InToPosition - InFromPosition);
}

DirectX::XMVECTOR XM_CALLCONV mds::RMath::FromForwardVectorQuatRad(DirectX::FXMVECTOR InForwardVector)
{
	XMVECTOR NewX = XMVector3Normalize(InForwardVector);
	XMFLOAT3 upVec = ((fabsf(XMVectorGetZ(NewX)) < (1.f)) ? XMFLOAT3(0.f, 0.f, 1.f) : XMFLOAT3(1.f, 0.f, 0.f));
	XMVECTOR UpVector = XMLoadFloat3(&upVec);

	XMVECTOR NewY = XMVector3Cross(UpVector, NewX);
	NewY = XMVector3Normalize(NewY);
	XMVECTOR NewZ = XMVector3Cross(NewX, NewY);

	// Create a rotation matrix
	DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixIdentity();
	lookAtMatrix.r[0] = NewX; // X axis
	lookAtMatrix.r[1] = NewY; // Y axis
	lookAtMatrix.r[2] = NewZ; // Z axis
	return XMQuaternionRotationMatrix(lookAtMatrix);

}

bool mds::RMath::GetIsDrawClockwise(FXMVECTOR InExpectedNormalDir, DirectX::FXMMATRIX InTransposeMatrix)
{
	XMVECTOR X0Y0 = DirectX::XMVector3Transform({ 0.f, 0.f, 0.f, 1.f }, InTransposeMatrix);
	XMVECTOR X1Y0 = DirectX::XMVector3Transform(XMVECTOR{ 1.f, 0.f, 0.f, 1.f }, InTransposeMatrix);
	XMVECTOR X0Y1 = DirectX::XMVector3Transform(XMVECTOR{ 0.f, 1.f, 0.f, 1.f }, InTransposeMatrix);
	XMVECTOR DeducedClockwiseNormal = mds::RMath::ComputeNormalFromPointsClockwise(X0Y0, X1Y0, X0Y1);
	float vectorX = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DeducedClockwiseNormal, InExpectedNormalDir));

	return vectorX < FLT_EPSILON;
}
