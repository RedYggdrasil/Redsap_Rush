#pragma once

#include "App/Libs/WinInclude.h"
#include "MDS/Tools/System/IRProgramMemNode.h"
#include "DirectXMath.h"
#include <vector>
#include <memory>
#include <deque>
#include <random>

namespace RSRush
{
	class RSRTrench;
	class RSRPlayerPath;
	class RSRProgramInstance;
	class RSRTTScene;
	class RSRTrenchManager : public mds::IRProgramMemNode
	{
		std::mt19937_64 gen;
		std::vector<uint16_t> m_sideGreedbleTextureIDs;
		std::vector<uint16_t> m_topGreedbleTextureIDs;
	private :
		RSRTTScene* m_owningScene;
	public:
#pragma region Instance
		RSRTrenchManager(const RSRTrenchManager&) = delete;
		RSRTrenchManager& operator=(const RSRTrenchManager&) = delete;
		RSRProgramInstance* GetProgramInstance() const;
#pragma endregion

	protected:
		std::weak_ptr<RSRush::RSRPlayerPath> m_pPlayerPath;

		// Renderable segments (with vertices loaded on GPU)
		std::deque<std::shared_ptr<RSRTrench>> m_segments;

		/*True for Generating and Uploading phase*/
		bool m_currentlyGeneratingSegments = false;
		bool m_currentlyUploading = false;
		double m_nextGenerateTime = 0.;
		UINT64 m_waitingForUploadFence = 0;
		std::vector<std::unique_ptr<RSRTrench>> m_generatingSegments;
	public:
		virtual void BeginNewTrench(double InCurrentProgression, std::weak_ptr<RSRush::RSRPlayerPath> InPlayerPath);

		void UpdateTrench(double InDeltaTime, double InCurrentProgression, UINT64 InCurrentUploadFence);
		bool ClearTrench();
	public:
		bool UploadBeingNewResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeUploadBuffers();
		bool FreeResourceBuffers();
	public:
		bool DrawTrench(DirectX::XMFLOAT4 InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList);
	public:
		RSRTrenchManager(RSRTTScene* InOwnerScene, std::vector<uint16_t>&& InSideGreedbleTextureIDs, std::vector<uint16_t>&& InTopGreedbleTextureIDs);
		using mds::IRProgramMemNode::InitMemNode;
	private:
		bool IsNeedToGenerateSegments(double InCurrentProgression) const;

		std::unique_ptr<RSRush::RSRTrench> GetNewTrenchInstance();
		bool IsTrenchGenerationTaskOnging() const;
		void GenerateNewTrench();
		void StartUploadingTrenchMesh();
		void OnUploadingTrenchMeshEnded();
	};
}