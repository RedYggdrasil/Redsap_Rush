#include "App/Data/Meshes/RSRMeshLODs.h"
#include "App/Tools/RSRLog.h"
#include "DirectXMath.h"

using namespace DirectX;
using namespace RSRush;

mds::RResourceStateType RSRMeshLODs::GetMinResourceState() const
{
	size_t c = LODSize();
	if (c < 1) { return mds::RResourceStateType::Unknown; }
	mds::RResourceStateType resourceState = mds::RResourceStateType::MAX;
	for (size_t i = 0; i < c; ++i)
	{
		resourceState = REnumMin(resourceState, m_meshes[i]->GetResourceState());
	}
	return resourceState;
}
bool RSRMeshLODs::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRMesh>& Mesh : m_meshes)
	{
		if (Mesh->GetResourceState() == mds::RResourceStateType::NeedUpload)
		{
			if (!Mesh->UploadResources(InDevice, InUploadCommandList))
			{
				bCumulativeSucess = false;
			}
		}
	}
	return bCumulativeSucess;
}

bool RSRMeshLODs::FreeUploadBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRMesh>& Mesh : m_meshes)
	{
		if (Mesh->GetResourceState() == mds::RResourceStateType::WaitingUpload)
		{
			bCumulativeSucess = Mesh->FreeUploadBuffer() && bCumulativeSucess;
		}
	}
	return bCumulativeSucess;
}

bool RSRMeshLODs::FreeResourceBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRMesh>& Mesh : m_meshes)
	{
		bCumulativeSucess = Mesh->FreeResourcesBuffer() && bCumulativeSucess;
	}
	return bCumulativeSucess;
}

bool RSRush::RSRMeshLODs::FreeAllBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRMesh>& Mesh : m_meshes)
	{
		bCumulativeSucess = Mesh->FreeAllBuffers() && bCumulativeSucess;
	}
	return bCumulativeSucess;
}

RSRush::RSRMeshLODs::RSRMeshLODs()
{
}
RSRush::RSRMeshLODs::~RSRMeshLODs()
{
}

void RSRush::RSRMeshLODs::Insert(std::shared_ptr<RSRush::RSRMesh> InMesh, const float InMaxDrawDistance)
{
	size_t index = 0;
	size_t cLOD = this->LODSize();
	float maxDistSquared = InMaxDrawDistance * InMaxDrawDistance;
	for (/*_*/; index < cLOD && m_maxSquaredDistances[index] < maxDistSquared; ++index)
	{ }

	if (index == cLOD)
	{
		m_meshes.push_back(InMesh);
		m_maxSquaredDistances.push_back(maxDistSquared);
	}
	else if (m_maxSquaredDistances[index] == maxDistSquared)
	{
		m_meshes[index].reset();
		m_meshes[index] = InMesh;
	}
	else
	{
		m_meshes.insert(m_meshes.begin() + index, InMesh);
		m_maxSquaredDistances.insert(m_maxSquaredDistances.begin() + index, maxDistSquared);
	}
}

bool RSRush::RSRMeshLODs::DrawMesh(DirectX::FXMVECTOR InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList, const RSRTransformMatrix& InPinnedTransformMatrix) const
{
	XMMATRIX trsMat = XMLoadFloat4x4(&InPinnedTransformMatrix.GetMatrix());
	float lengthSquare = XMVectorGetX(XMVector3LengthSq(InCameraPosition - trsMat.r[3]));

	size_t cDistsMinus = m_maxSquaredDistances.size();
	//The Last distance is always considered infinite and is only kept as reference in a future farer mesh in inserted
	if (cDistsMinus >= 1) { cDistsMinus -= 1; }

	size_t index = 0;
	for (; index < cDistsMinus; ++index)
	{
		if (lengthSquare <= m_maxSquaredDistances[index])
		{
			return m_meshes[index]->DrawMesh(InDrawCommandList, InPinnedTransformMatrix);
		}
	}
	if (m_meshes.size() < 1)
	{
		RSRLog::Log(LOG_ERROR, TEXT("NO Mesh in MESHLODs to draw !"));
		return false;
	}
	return m_meshes.back()->DrawMesh(InDrawCommandList, InPinnedTransformMatrix);
}
