#pragma once

#include "App/Data/Meshes/RSRMesh.h"
//#include "MDS/Tools/RCoordinate.h"
#include <memory>
#include <vector>

namespace RSRush
{
	class RSRMeshLODs
	{
	protected:
		std::vector<std::shared_ptr<RSRush::RSRMesh>> m_meshes;
		std::vector<float> m_maxSquaredDistances;

	public:
		const std::shared_ptr<RSRush::RSRMesh>& GetMeshForLOD(const size_t LODLevel) const { return m_meshes[LODLevel]; }
		std::shared_ptr<RSRush::RSRMesh>& GetMeshForLOD(const size_t LODLevel) { return m_meshes[LODLevel]; }
		size_t LODSize() const { return m_meshes.size(); }

		mds::RResourceStateType GetMinResourceState() const;
		bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeUploadBuffers();
		bool FreeResourceBuffers();
		bool FreeAllBuffers();

	public:
		RSRMeshLODs();
		/*virtual*/~RSRMeshLODs()/*override*/;
	
	public:
		void Insert(std::shared_ptr<RSRush::RSRMesh> InMesh, const float InMaxDrawDistance);
		bool DrawMesh(const DirectX::XMFLOAT3& InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList, const RSRTransformMatrix& InPinnedTransformMatrix) const
		{
			DirectX::XMVECTOR loadedCameraPos = DirectX::XMLoadFloat3(&InCameraPosition);
			return DrawMesh(loadedCameraPos, InDrawCommandList, InPinnedTransformMatrix);
		}
		bool XM_CALLCONV DrawMesh(DirectX::FXMVECTOR InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList, const RSRTransformMatrix& InPinnedTransformMatrix) const;
	};
};