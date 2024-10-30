#pragma once
#include "App/Data/Shaders/ShaderStructures.h"
#include <vector>
#include <unordered_map>

namespace RSRush
{
	class RSRIInstancedMesheHolder
	{
	protected:
		std::unordered_map<uintptr_t, std::vector<RSRush::VertexBufferInstanceData>> m_vertexBufferDatas;
	public :
		void PushBackInstancesBufferData(const uintptr_t InMeshPtr, std::vector<RSRush::VertexBufferInstanceData>& InOutVertexBBufferInstanceDatas) const;
	};
};