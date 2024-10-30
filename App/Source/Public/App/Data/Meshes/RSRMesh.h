#pragma once

#include "App/Libs/WinInclude.h"

#include "App/Data/RSRTransform.h"
#include "App/Data/Shaders/ShaderStructures.h"
#include "MDS/Tools/Assets/RAsset.h"
#include "MDS/Tools/Pins/RID3D12RPin.h"

#include <wrl/client.h>
#include <memory>
#include <vector>

namespace RSRush
{
	class RSRMesh;
	typedef std::weak_ptr<RSRush::RSRMesh> RSRWeakMeshPtr;
	typedef std::shared_ptr<RSRush::RSRMesh> RSRSharedMeshPtr;

#define l_ComPtr	Microsoft::WRL::ComPtr
	class RSRMesh : public mds::RAsset
	{
	protected:

		mds::RID3D12RPin m_uploadBufferPin;
		//CPU Resources
		//std::vector<VertexPositionUVColor> m_vertexes;
		const bool m_bApplyTransformationMatrix;
		const UINT m_VertexUnitSize;
		std::vector<UINT_VRTX_IDX> m_indexes;

		//GPU Resources
		l_ComPtr<ID3D12Resource2> m_vertexesBuffer;
		//GPU Resources
		l_ComPtr<ID3D12Resource2> m_indexesBuffer;

	public:
		inline bool IndexedVertices() const { return m_indexes.size() > 0; }
		virtual UINT GetVertexCount() const = 0;// { return (UINT) m_vertexes.size(); }
		inline UINT64 GetVertexUnitSize() const { return m_VertexUnitSize; };
		inline UINT64 GetUsedVertexBufferSize() const { return GetVertexUnitSize() * GetVertexCount(); };
		UINT64 GetAlignedVertexBufferSize() const;

		inline UINT GetIndexCount() const { return (UINT)m_indexes.size(); }
		inline UINT64 GetUsedIndexBufferSize() const { return sizeof(UINT_VRTX_IDX) * GetIndexCount(); };
		UINT64 GetAlignedIndexBufferSize() const;

		virtual const void* GetVertexAdress() const = 0;// { return m_vertexes; }
		inline const std::vector<UINT_VRTX_IDX>& GetIndexes() const { return m_indexes; }

		inline const l_ComPtr<ID3D12Resource2>& GetVertexesBuffer() const { return m_vertexesBuffer; }
		inline const l_ComPtr<ID3D12Resource2>& GetIndexesBuffer() const { return m_indexesBuffer; }

	public:
		bool FreeUploadBuffer();
		bool FreeResourcesBuffer();
		bool FreeAllBuffers();

	protected:
		RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize, const std::vector<UINT_VRTX_IDX>& InIndexes);
		RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize, std::vector<UINT_VRTX_IDX>&& InIndexes);
		RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize);

	public:
		/*virtual*/~RSRMesh()/*override*/;
	public:
		virtual bool UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList);
	public:
		bool DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const UINT InInstanceCount = 1) const;
		bool DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const RSRTransformMatrix& InPinnedTransformMatrix, const UINT InInstanceCount = 1) const;
	};
#undef l_ComPtr
}