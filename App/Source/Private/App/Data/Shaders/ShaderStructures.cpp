#include "App/Data/Shaders/ShaderStructures.h"

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
