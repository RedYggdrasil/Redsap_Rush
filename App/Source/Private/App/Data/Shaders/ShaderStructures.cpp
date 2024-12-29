#include "App/Data/Shaders/ShaderStructures.h"
#include "MDS/Tools/RMath.h"
#include <limits>

using namespace RSRush;
using namespace DirectX;

InstanceModelMatrixes RSRush::InstanceModelMatrixes::FromTrsMatrix(const mds::RTransformMatrix& InTrsMatrix)
{
	DirectX::XMFLOAT4X4  InverseTranspose = InTrsMatrix.ComputeInverseTransposeAtOrigin();
	return InstanceModelMatrixes
	{
		.ModMat = InTrsMatrix.GetMatrix(),
		.InvProjModMat = InverseTranspose
	};
}

std::array<DirectX::XMFLOAT3, 2> RSRush::ComputeExtremums(const VertexPositionUVColor* InData, const size_t InCount)
{
	if (InCount < 1)
	{
		return { DirectX::XMFLOAT3(), DirectX::XMFLOAT3() };
	}
	constexpr float max = std::numeric_limits<float>::infinity();
	constexpr float min = -std::numeric_limits<float>::infinity();
	std::array<DirectX::XMFLOAT3, 2> returnValues = {/*Min*/ DirectX::XMFLOAT3(max, max, max), /*Max*/ DirectX::XMFLOAT3(min, min, min) };

	for (size_t i = 0; i < InCount; ++i)
	{
		returnValues[0].x = std::min(returnValues[0].x, InData[i].pos.x);
		returnValues[0].y = std::min(returnValues[0].y, InData[i].pos.y);
		returnValues[0].z = std::min(returnValues[0].z, InData[i].pos.z);

		returnValues[1].x = std::max(returnValues[1].x, InData[i].pos.x);
		returnValues[1].y = std::max(returnValues[1].y, InData[i].pos.y);
		returnValues[1].z = std::max(returnValues[1].z, InData[i].pos.z);
	}
	return returnValues;
}

std::array<DirectX::XMFLOAT3, 2> RSRush::ComputeCenterExtends(const VertexPositionUVColor* InData, const size_t InCount)
{
	std::array<DirectX::XMFLOAT3, 2> extremums = RSRush::ComputeExtremums(InData, InCount);
	XMVECTOR3 extMin = XMLoadFloat3(&extremums[0]);
	XMVECTOR3 extMax = XMLoadFloat3(&extremums[1]);
	XMVECTOR3 lCenter = (extMin + extMax) * 0.5f;
	XMVECTOR3 lExtends = extMax - lCenter;
	std::array<DirectX::XMFLOAT3, 2> returnValue;
	XMStoreFloat3(&returnValue[0], lCenter);
	XMStoreFloat3(&returnValue[1], lExtends);
	return returnValue;
}
