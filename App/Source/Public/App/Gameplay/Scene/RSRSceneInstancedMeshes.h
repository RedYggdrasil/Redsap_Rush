#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "App/Data/Meshes/RSRMeshInstances.h"

struct ID3D12GraphicsCommandList7;
namespace RSRush
{
	//struct MeshHolderData
	//{
	//public :
	//	std::vector<RSRIInstancedMesheHolder*> MeshHolders;
	//	std::vector<size_t> MeshInstanceDatas;
	//};
	//class GameManager;
	//class RSRProgramInstance;
	class RSRMesh;
	class RSRIInstancedMesheHolder;
	struct RSRSceneInstancedMeshes
	{
	private:
		size_t m_size = 0;
		std::unordered_map<std::shared_ptr<RSRMesh>, std::vector<RSRIInstancedMesheHolder*>> m_meshToObjects;
		std::unordered_map<uintptr_t, size_t> m_previousMeshInstanceCount;
		std::unordered_map<uintptr_t, RSRMeshInstances> m_MeshInstancesBuffers;
	public:
		size_t size() const { return m_size; }
		void Register(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh);
		void Unregister(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh);
		void DrawInstances(ID3D12GraphicsCommandList7* InRenderCommandList);

		void UpdateUploadBuffer();
		void UploadResources(ID3D12GraphicsCommandList7* InUploadCommandList);
		void ClearUnusedBuffers();
	};
};