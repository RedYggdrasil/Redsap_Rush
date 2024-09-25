#pragma once

#define NOMINMAX
#include "Gen_App/Config/AppConfig.h"

#include "App/Libs/WinInclude.h"
#include "MDS/Tools/Templates/Singleton.h"
#include "App/Gameplay/RSRScene.h"

#include <memory>
#include <unordered_set>
#include "MDS/Tools/REnum.h"
#include "MDS/Tools/RCollection.h"
#include <wrl/client.h>
#include <limits>

#include <mutex>
#include <thread>
#include <queue>

class Shader;
namespace RSRush
{
	enum class EStartScene : uint8_t
	{
		RENUM_MIN(Unknown),
		TheTrench,
		RENUM_MAX(PhysicDemo1)
	};
	RS_DEFINE_ENUM(EStartScene);

	struct RSRPIStatistics
	{
	public:
		size_t GameInputControllerSilent = 0;
		bool bTearingSupported = false;
	};
	class RSRRandomizer;
	class RSRProgramInstance : public mds::Singleton<RSRProgramInstance>
	{
	protected:
		struct FrameData
		{
			size_t FrameIndex;
			double GameTime;
			double DeltaTime;
		};
		struct RendererData
		{
			//Program Instance To Renderer Thread
			std::mutex m_framesToRenderLock;
			std::condition_variable m_cvHasframesToRender;
			bool bStopRenderer;
			std::queue<FrameData> FramesToRender;

			//Renderer Thread To Program Instance
			std::mutex m_LastRenderedframeLock;
			std::condition_variable m_cvLastRenderedframeChanged;
			size_t LastRenderedFrameIndex;
		public:
			void ResetDatas();
			void RenderFrame(const FrameData& InFrameData);
			void WaitForFrame(const size_t InFrameIndex);
			void StopRenderer(const size_t InLastFrameIndex);
		};
		RendererData m_rendererData;
	protected:
		bool m_bIsInitialized;

		mds::RUnorderedStringMap<std::unique_ptr<Shader>> m_shaders;

		std::unordered_set<std::shared_ptr<RSRScene>> m_scenes;
		std::shared_ptr<RSRScene> m_currentScene;

		std::unique_ptr<RSRush::RSRRandomizer> m_randomizer;

#pragma region PSOs
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature2D;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso2D;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature3D;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso3D;
		public:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSig2D() const { return m_rootSignature2D;}
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSO2D() const { return m_pso2D;}
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSig3D() const { return m_rootSignature3D;}
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSO3D() const { return m_pso3D;}
#if DEBUG_PHYSIC
	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignatureDebugPhysic;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_psoDebugPhysic;
	public:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSigDebugPhysic() const { return m_rootSignatureDebugPhysic; }
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSODebugPhysic() const { return m_psoDebugPhysic; }
#endif
#pragma endregion PSOs

#pragma region Statistics
	protected:
		RSRPIStatistics m_piStatistics;

	public:
		inline const RSRPIStatistics& GetPIStatistics() const { return m_piStatistics; }
		inline void ReportSilentGameInputController() 
		{ if (m_piStatistics.GameInputControllerSilent < std::numeric_limits<int>::max()) { ++m_piStatistics.GameInputControllerSilent; } }
		inline void SetTearingSupported(const bool InbTearingSupported) { m_piStatistics.bTearingSupported = InbTearingSupported; }
#pragma endregion Statistics

	protected:
		RSRProgramInstance();
	public:
		virtual ~RSRProgramInstance();
	public:
		void Run(int argc, char** argv);
		void InitializeProgram();
		void ShutdownProgram();

		void SetCurrentScene(std::shared_ptr<RSRScene> InNewCurrentScene);
		void ResetCurrentScene();
		std::shared_ptr<RSRScene> CreateScene(const EStartScene EInSceneType);
		template<class TScene>
		std::shared_ptr<TScene>CreateScene()
		{
			std::shared_ptr<TScene> scene = TScene::Constructor::Create(this);
			m_scenes.emplace(scene);
			return scene;
		};
	private:
		bool InitializePaths() const;
		bool UNIT_TEST();
		friend class mds::Singleton<RSRProgramInstance>;
	};
};