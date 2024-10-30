#include "App/Gameplay/Scene/RSRSceneInstancedMeshes.h"

#include "App/Data/Meshes/RSRMesh.h"
#include "App/Gameplay/RSRScene.h"
#include "App/Gameplay/Scene/RSRIInstancedMesheHolder.h"
#include "App/Tools/RSRLog.h"
#include <algorithm>
#include <unordered_set>

void RSRush::RSRSceneInstancedMeshes::Register(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
    auto it = m_meshToObjects.find(InMesh);
    if (it != m_meshToObjects.end()) 
    {
        std::vector<RSRIInstancedMesheHolder*>& objects = it->second;
        objects.push_back(InMeshHolder);
        ++m_size;
    }
    else {
        m_meshToObjects.emplace(InMesh, std::vector<RSRIInstancedMesheHolder*>{InMeshHolder});
        ++m_size;
    }

}

void RSRush::RSRSceneInstancedMeshes::Unregister(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh)
{
    auto it = m_meshToObjects.find(InMesh);
    if (it != m_meshToObjects.end())
    {
        std::vector<RSRIInstancedMesheHolder*>& objects = it->second;
        size_t oldSize = objects.size();
        objects.erase(std::remove(objects.begin(), objects.end(), InMeshHolder), objects.end());
        m_size = m_size - (oldSize - objects.size());
        if (objects.size() < 1)
        {
            m_meshToObjects.erase(InMesh);
        }
    }
}

void RSRush::RSRSceneInstancedMeshes::DrawInstances(ID3D12GraphicsCommandList7* InRenderCommandList)
{
    if (m_size > 0)
    {
        for (auto& pair : m_meshToObjects)
        {
            uintptr_t meshPtr = reinterpret_cast<uintptr_t>(pair.first.get());
            size_t instanceCount = m_previousMeshInstanceCount[meshPtr];
            if (instanceCount > 0)
            {
                RSRMeshInstances* instanceBuffers = nullptr;
                auto meshBuffersIT = m_MeshInstancesBuffers.find(meshPtr);
                if (meshBuffersIT != m_MeshInstancesBuffers.end())
                {
                    instanceBuffers = &meshBuffersIT->second;
                }

                if (instanceBuffers)
                {
                    D3D12_VERTEX_BUFFER_VIEW vbvInstances
                    {
                        .BufferLocation = instanceBuffers->GetInstancesBuffer()->GetGPUVirtualAddress(),
                        .SizeInBytes = (UINT)instanceBuffers->GetInstanceBufferSize(instanceBuffers->GetCurrentRenderInstanceCount()),
                        .StrideInBytes = (UINT)instanceBuffers->GetInstanceUnitSize()
                    };

                    InRenderCommandList->IASetVertexBuffers(1, 1, &vbvInstances);
                    pair.first->DrawMesh(InRenderCommandList, (UINT)instanceBuffers->GetCurrentRenderInstanceCount());
                }
                
            }
        }
    }
}

void RSRush::RSRSceneInstancedMeshes::UpdateUploadBuffer()
{
    if (m_size > 0)
    {
        
        std::unordered_map<uintptr_t, size_t> oldMeshInstanceCount = m_previousMeshInstanceCount;
        m_previousMeshInstanceCount.clear();
        m_previousMeshInstanceCount.reserve(m_meshToObjects.size());
        for (auto& pair : m_meshToObjects)
        {
            std::vector<VertexBufferInstanceData> instancesData;
            uintptr_t meshPtr = reinterpret_cast<uintptr_t>(pair.first.get());

            auto previousDrawDataIT = oldMeshInstanceCount.find(meshPtr);
            if (previousDrawDataIT != oldMeshInstanceCount.end())
            {
                instancesData.reserve(previousDrawDataIT->second);
            }
            //Start fill instancesData
            for (const RSRIInstancedMesheHolder* meshHolder : pair.second)
            {
                if (meshHolder)
                {
                    meshHolder->PushBackInstancesBufferData(meshPtr, instancesData);
                }
            }
            RSRMeshInstances* meshBuffers = nullptr;
            auto meshBuffersIT = m_MeshInstancesBuffers.find(meshPtr);
            if (meshBuffersIT != m_MeshInstancesBuffers.end())
            {
                meshBuffers = &meshBuffersIT->second;
            }
            else
            {
                auto newBuffersPair = m_MeshInstancesBuffers.emplace(meshPtr, RSRMeshInstances(sizeof(VertexBufferInstanceData)));
                meshBuffers = &newBuffersPair.first->second;
            }
            meshBuffers->UpdateUploadBuffer(DXContext::Get().GetDevice().Get(), instancesData.data(), instancesData.size());

            m_previousMeshInstanceCount.emplace(meshPtr, instancesData.size());
        }
    }
}

void RSRush::RSRSceneInstancedMeshes::UploadResources(ID3D12GraphicsCommandList7* InUploadCommandList)
{
    if (m_size > 0)
    {

        for (auto& pair : m_meshToObjects)
        {
            uintptr_t meshPtr = reinterpret_cast<uintptr_t>(pair.first.get());

            RSRMeshInstances* meshBuffers = nullptr;
            auto meshBuffersIT = m_MeshInstancesBuffers.find(meshPtr);
            if (meshBuffersIT != m_MeshInstancesBuffers.end())
            {
                meshBuffers = &meshBuffersIT->second;
            }
            if (meshBuffers)
            {
                meshBuffers->CopyUploadBufferToGPU(DXContext::Get().GetDevice().Get(), InUploadCommandList);
            }
        }
    }
}

void RSRush::RSRSceneInstancedMeshes::ClearUnusedBuffers()
{
    std::unordered_set<uintptr_t> presentMeshes;
    for (const auto& pair : m_meshToObjects) 
    {
        presentMeshes.insert(reinterpret_cast<uintptr_t>(pair.first.get()));
    }

    for (auto it = m_previousMeshInstanceCount.begin(); it != m_previousMeshInstanceCount.end(); ) 
    {
        if (presentMeshes.find(it->first) == presentMeshes.end()) 
        {
            it = m_previousMeshInstanceCount.erase(it);
        }
        else 
        {
            ++it;
        }
    }

    for (auto it = m_MeshInstancesBuffers.begin(); it != m_MeshInstancesBuffers.end(); )
    {
        if (presentMeshes.find(it->first) == presentMeshes.end())
        {
            it->second.FreeAllBuffers();
            it = m_MeshInstancesBuffers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
