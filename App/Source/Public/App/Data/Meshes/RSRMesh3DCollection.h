#pragma once

#include "MDS/Tools/Assets/RAsset.h"
#include "App/Data/RSRTransform.h"
//#include "MDS/Tools/RResourceStateType.h"
#include <memory>
#include <unordered_map>

class ID3D12Device10;
class ID3D12GraphicsCommandList7;
namespace RSRush
{
	class RSRMesh3D;
	typedef std::weak_ptr<RSRush::RSRMesh3D> RSRWeakMesh3DhPtr;
	typedef std::shared_ptr<RSRush::RSRMesh3D> RSRSharedMesh3DPtr;
	
	struct RSRMesh3DDatas
	{
		public:
			bool m_bIsDrawnAsInstance;
			RSRush::RSRTransformMatrix m_mainTransform;
			mds::RAssetAuthority m_mainMeshAuthority;
			RSRSharedMesh3DPtr m_mesh;
	};

	class RSRMesh3DCollection
	{
	private:
		bool m_bUseLocalTransformations;
		std::unordered_map<uintptr_t, RSRMesh3DDatas> m_meshPartsData;
	public:
		bool UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList);
	public:
		bool DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const unsigned int InInstanceCount = 1) const;
		bool DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const RSRTransformMatrix& InPinnedTransformMatrix, const unsigned int InInstanceCount = 1) const;

	public:
		RSRMesh3DCollection(std::unordered_map<uintptr_t, RSRMesh3DDatas>&& InMeshParts, bool bInUseLocalTransformations = true);
		RSRMesh3DCollection(RSRMesh3DDatas&& InMeshData, bool bInUseLocalTransformations = true);
		virtual ~RSRMesh3DCollection();
	};
};