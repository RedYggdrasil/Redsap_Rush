#include "App/Gameplay/Scene/RSRIInstancedMesheHolder.h"
#include <algorithm>
#include <iterator>
#include <App/Tools/RSRLog.h>

void RSRush::RSRIInstancedMesheHolder::PushBackInstancesBufferData(const uintptr_t InMeshPtr, std::vector<RSRush::VertexBufferInstanceData>& InOutVertexBBufferInstanceDatas) const
{

    auto meshInstancePairIT = m_vertexBufferDatas.find(InMeshPtr);
    if (meshInstancePairIT != m_vertexBufferDatas.end() && meshInstancePairIT->second.size() > 0)
    {
        const std::vector<RSRush::VertexBufferInstanceData>& vertexInstanceDatas = meshInstancePairIT->second;
        size_t previousCount = InOutVertexBBufferInstanceDatas.size();
        InOutVertexBBufferInstanceDatas.resize(
            previousCount + vertexInstanceDatas.size()
        );
        std::memcpy(
            /*Adress of vector start*/InOutVertexBBufferInstanceDatas.data() /*Typed pointer arithmetic + operator*/ + previousCount,
            vertexInstanceDatas.data(),
            vertexInstanceDatas.size() * sizeof(RSRush::VertexBufferInstanceData)
        );
    }
}
