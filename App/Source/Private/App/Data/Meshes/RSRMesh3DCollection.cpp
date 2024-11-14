#include "App/Data/Meshes/RSRMesh3DCollection.h"
#include "MDS/Tools/RMath.h"
#include "App/Libs/WinInclude.h"
#include "App/Data/Meshes/RSRMesh3D.h"

using namespace RSRush;
using namespace DirectX;
bool RSRMesh3DCollection::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool bAllSucessfull = true;
	for (auto pair : m_meshPartsData)
	{
		bAllSucessfull = pair.second.m_mesh->UploadResources(InDevice, InUploadCommandList) && bAllSucessfull;
	}
	return bAllSucessfull;
}

bool RSRMesh3DCollection::DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const unsigned int InInstanceCount) const
{
	bool bAllSucessfull = true;
	for (auto pair : m_meshPartsData)
	{
		bAllSucessfull = pair.second.m_mesh->DrawMesh(InUploadCommandList, InInstanceCount) && bAllSucessfull;
	}
	return bAllSucessfull;
}

bool RSRMesh3DCollection::DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const RSRTransformMatrix& InPinnedTransformMatrix, const unsigned int InInstanceCount) const
{
	bool bAllSucessfull = true;


	if (!m_bUseLocalTransformations)
	{
		RSRMesh3D::ApplyTransformToRootSig(InUploadCommandList, InPinnedTransformMatrix);
	}
	for (const auto& pair : m_meshPartsData)
	{
		const RSRMesh3DDatas& datas = pair.second;
		if (datas.m_bIsDrawnAsInstance) { continue; }
		if (m_bUseLocalTransformations)
		{
			
		}

		bAllSucessfull = pair.second.m_mesh->DrawMesh(InUploadCommandList, InInstanceCount) && bAllSucessfull;
	}
	return bAllSucessfull;
}

RSRMesh3DCollection::RSRMesh3DCollection(std::unordered_map<uintptr_t, RSRMesh3DDatas>&& InMeshParts, bool bInUseLocalTransformations)
: m_bUseLocalTransformations(bInUseLocalTransformations), m_meshPartsData(std::move(InMeshParts))
{
}

RSRMesh3DCollection::RSRMesh3DCollection(RSRMesh3DDatas&& InMeshData, bool bInUseLocalTransformations)
: m_bUseLocalTransformations(bInUseLocalTransformations), m_meshPartsData()
{
	m_meshPartsData.reserve(1);
	m_meshPartsData.emplace((uintptr_t)InMeshData.m_mesh.get(), std::move(InMeshData));
}

RSRMesh3DCollection::~RSRMesh3DCollection()
{
}
