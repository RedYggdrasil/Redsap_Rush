#include "App/Data/Meshes/RSRMesh.h"

#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"
#include <iostream>

using Microsoft::WRL::ComPtr;

UINT64 RSRush::RSRMesh::GetAlignedVertexBufferSize() const
{ 
    return mds::RMath::Align<UINT64>(this->GetUsedVertexBufferSize(), 1024);
}

UINT64 RSRush::RSRMesh::GetAlignedIndexBufferSize() const
{
    return mds::RMath::Align<UINT64>(this->GetUsedIndexBufferSize(), 1024);
}

bool RSRush::RSRMesh::FreeUploadBuffer()
{
    bool bSucess = true;
    if (m_uploadBufferPin.HasResource())
    {
        bSucess = m_uploadBufferPin.Release();
    }

    if (bSucess)
    {
        m_resourceState = (bool)m_vertexesBuffer ?
            mds::RResourceStateType::ResourceLive :
            mds::RResourceStateType::NeedUpload;
    }

    return bSucess;
}

bool RSRush::RSRMesh::FreeResourcesBuffer()
{
    m_vertexesBuffer.Reset();
    m_indexesBuffer.Reset();

    //If we had Upload resources, they would now be invalid as the resource toward witch they should upload no longer exist
    //FreeUploadBuffer also update m_resourceState
    FreeUploadBuffer();
    return true;
}

bool RSRush::RSRMesh::FreeAllBuffers()
{
    //Also free upload resrouce
    return FreeResourcesBuffer();
}

RSRush::RSRMesh::RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize, const std::vector<UINT_VRTX_IDX>& InIndexes)
: RSRMesh(bInApplyTransformationMatrix, InVertexUnitSize)
{
    m_indexes.reserve(InIndexes.size());
    for (const UINT_VRTX_IDX& index : InIndexes)
    {
        m_indexes.push_back(index);
    }
}

RSRush::RSRMesh::RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize, std::vector<UINT_VRTX_IDX>&& InIndexes)
: RSRMesh(bInApplyTransformationMatrix, InVertexUnitSize)
{
    m_indexes = std::move(InIndexes);
}

RSRush::RSRMesh::RSRMesh(bool bInApplyTransformationMatrix, const UINT InVertexUnitSize)
:RAsset(), m_bApplyTransformationMatrix(bInApplyTransformationMatrix), m_VertexUnitSize(InVertexUnitSize), m_indexes(0)
{
    m_type = mds::RAssetType::Mesh;
    m_resourceState = mds::RResourceStateType::NeedUpload;
}

RSRush::RSRMesh::~RSRMesh()
{
    bool bSucess = FreeAllBuffers();
}

bool RSRush::RSRMesh::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
    if (GetVertexCount() < 3)
    {
        RSRLog::LogError(L_PATH TEXT(" : No vertex to upload !"));
        return false;
    }
    if (m_uploadBufferPin.HasResource())
    {
        RSRLog::LogError(L_PATH TEXT(" : Previous Upload Buffer is still in use !"));
        return false;
    }

    //Upload Heap Properties
    D3D12_HEAP_PROPERTIES hpUpload{};
    hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
    hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hpUpload.CreationNodeMask = 0;
    hpUpload.VisibleNodeMask = 0;

    //Device Resource Heap Properties
    D3D12_HEAP_PROPERTIES hpDefault{};
    hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
    hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hpDefault.CreationNodeMask = 0;
    hpDefault.VisibleNodeMask = 0;

    const UINT64 AlignedVertexBufferSize = this->GetAlignedVertexBufferSize();
    const UINT64 AlignedIndexBufferSize = this->GetAlignedIndexBufferSize();
    const UINT64 TotalAlignedBufferSize = AlignedVertexBufferSize + AlignedIndexBufferSize;

    //--- Upload &Vertex Buffer ---
    
    //Vertice Buffer Description
    D3D12_RESOURCE_DESC rdv{};
    rdv.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rdv.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rdv.Width = AlignedVertexBufferSize;
    rdv.Height = 1; //Size for 2nd dimension of array
    rdv.DepthOrArraySize = 1; //Size for 3rd dimension of array
    rdv.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
    rdv.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
    rdv.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
    rdv.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
    rdv.Flags = D3D12_RESOURCE_FLAG_NONE;

    //Index Buffer Description
    D3D12_RESOURCE_DESC rdi{};
    rdi.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rdi.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rdi.Width = AlignedIndexBufferSize;
    rdi.Height = 1; //Size for 2nd dimension of array
    rdi.DepthOrArraySize = 1; //Size for 3rd dimension of array
    rdi.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
    rdi.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
    rdi.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
    rdi.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
    rdi.Flags = D3D12_RESOURCE_FLAG_NONE;

    //Upload Buffer Description
    D3D12_RESOURCE_DESC rdu{};
    rdu.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rdu.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    rdu.Width = TotalAlignedBufferSize;
    rdu.Height = 1; //Size for 2nd dimension of array
    rdu.DepthOrArraySize = 1; //Size for 3rd dimension of array
    rdu.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
    rdu.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
    rdu.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
    rdu.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
    rdu.Flags = D3D12_RESOURCE_FLAG_NONE;



    /*Our Upload Buffer will be layout as { [0] : Texture, [sizeof(Texture)] : VerticesData } */
    _RF_FAILED(InDevice->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &rdu, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_uploadBufferPin.Get())));

    //State common is valid for a simple buffer, but in other cases like a texture it need to start in D3D12_RESOURCE_STATE_COPY_DEST and then be set to it's common state
    _RF_FAILED(InDevice->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rdv, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_vertexesBuffer)));

    if (IndexedVertices())
    {
        //State common is valid for a simple buffer, but in other cases like a texture it need to start in D3D12_RESOURCE_STATE_COPY_DEST and then be set to it's common state
        _RF_FAILED(InDevice->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rdi, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_indexesBuffer)));
    }
    //Copy char* --> CPU Resource

    /*Adress on the Upload buffer for the vretices data*/
    char* uploadBufferAdress;
    D3D12_RANGE uploadRange = { .Begin = 0, .End = TotalAlignedBufferSize }; //Adress is our vertex buffer + index

    //Create a virtual pointer adress to copy our CPU memory to
    m_uploadBufferPin.Get()->Map(0, &uploadRange, (void**)&uploadBufferAdress);
    //Start to copy at adress of element 0 (so at the start of the underlying C style array, for actual size)
    memcpy(&uploadBufferAdress[0], this->GetVertexAdress(), this->GetUsedVertexBufferSize());
    if (IndexedVertices())
    {
        memcpy(&uploadBufferAdress[AlignedVertexBufferSize], &m_indexes[0], this->GetUsedIndexBufferSize());
    }
    m_uploadBufferPin.Get()->Unmap(0, &uploadRange);

    InUploadCommandList->CopyBufferRegion(m_vertexesBuffer.Get(), /*offset*/0, m_uploadBufferPin.Get().Get(), /*offset*/0, AlignedVertexBufferSize);
    if (IndexedVertices())
    {
        InUploadCommandList->CopyBufferRegion(m_indexesBuffer.Get(), /*offset*/0, m_uploadBufferPin.Get().Get(), /*offset*/AlignedVertexBufferSize, AlignedIndexBufferSize);
    }
    m_resourceState = mds::RResourceStateType::WaitingUpload;
    return true;
}

bool RSRush::RSRMesh::DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const UINT InInstanceCount) const
{
    D3D12_VERTEX_BUFFER_VIEW vbv
    {
        .BufferLocation = this->GetVertexesBuffer()->GetGPUVirtualAddress(),
        .SizeInBytes = m_VertexUnitSize * this->GetVertexCount(),
        .StrideInBytes = m_VertexUnitSize
    };
    InUploadCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    InUploadCommandList->IASetVertexBuffers(0, 1, &vbv);

    if (IndexedVertices())
    {
        D3D12_INDEX_BUFFER_VIEW ibv
        {
            .BufferLocation = this->GetIndexesBuffer()->GetGPUVirtualAddress(),
            .SizeInBytes = sizeof(UINT_VRTX_IDX) * this->GetIndexCount(),
            .Format = DXGI_FORMAT_RVRTX_IDX
        };
        InUploadCommandList->IASetIndexBuffer(&ibv);
        InUploadCommandList->DrawIndexedInstanced(this->GetIndexCount(), InInstanceCount, 0, 0, 0);
    }
    else
    {
        InUploadCommandList->DrawInstanced(this->GetVertexCount(), InInstanceCount, 0, 0);
    }
    return true;
}

bool RSRush::RSRMesh::DrawMesh(ID3D12GraphicsCommandList7* InUploadCommandList, const RSRTransformMatrix& InPinnedTransformMatrix, const UINT InInstanceCount) const
{
    //will this return temp ?
    if (m_bApplyTransformationMatrix)
    {
        ApplyTransformToRootSig(InUploadCommandList, InPinnedTransformMatrix);
    }
    return DrawMesh(InUploadCommandList, InInstanceCount);
}

void RSRush::RSRMesh::ApplyTransformToRootSig(ID3D12GraphicsCommandList7* InUploadCommandList, const RSRTransformMatrix& InPinnedTransformMatrix)
{
    //MVPDLC::S32B_MOD_MAT
    InUploadCommandList->SetGraphicsRoot32BitConstants(0, MVPDLC::S32B_MOD_MAT, &InPinnedTransformMatrix.GetMatrix(), MVPDLC::O32B_MOD_MAT);

    //Compute InverseTranspose matrix for normal computations
    RSRTransformMatrix NoTranslationMatrix = InPinnedTransformMatrix;
    NoTranslationMatrix.SetPositon(DirectX::XMFLOAT3(0.f, 0.f, 0.f));

    DirectX::XMMATRIX TransformMatrix = DirectX::XMLoadFloat4x4(&NoTranslationMatrix.GetMatrix());
    DirectX::XMMATRIX InverseTranspose = mds::RMath::InverseTranspose(TransformMatrix);

    DirectX::XMFLOAT4X4  StoredInverseTranspose;
    DirectX::XMStoreFloat4x4(&StoredInverseTranspose, InverseTranspose);

    InUploadCommandList->SetGraphicsRoot32BitConstants(0, MVPDLC::S32B_INV_MOD_MAT, &StoredInverseTranspose, MVPDLC::O32B_INV_MOD_MAT);
}
