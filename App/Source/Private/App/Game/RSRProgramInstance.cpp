#include "App/Game/RSRProgramInstance.h"

#include "App/Game/RSRPaths.h"
#include "MDS/Threads/RTaskManager.h"

#include "App/Data/Trench/RSRVoxalGrid.h"
#include "App/Data/Trench/RSRVoxalPlane.h"

#include "App/Gameplay/GameManager.h"

#include "App/System/RSRRandomizer.h"

#include "App/Physic/Demo01/RSRPD1Scene.h"
#include "App/TheTrench/RSRTTScene.h"
#include "App/Tools/DebugLayer.h"
#include "App/Tools/ImageLoader.h"


#include "App/Libs/WinInclude.h"
#include "App/D3D/DXContext.h"
#include "App/D3D/DXPipeline.h"
#include "App/Tools/Shader.h"
#include "App/Tools/Window.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Tools/Inputs/RSRGameInput.h"
#include "App/Gameplay/RSRScene.h"
#include <MDS/Tools/System/RTimekeeper.h>
#include <Tracy.hpp>

using Microsoft::WRL::ComPtr;

using namespace RSRush;


namespace RSRush
{
	namespace EShaderName
	{
		std::string BasicRootSignature2D("BasicRootSignature2D.cso");
		std::string BasicRootSignature3D("BasicRootSignature3D.cso");
#if DEBUG_PHYSIC
		std::string BasicRootSignatureDebugPhysic("BasicRootSignatureDebugPhysic.cso");
#endif
		std::string basicVS2DShader("Basic2DVS.cso");
		std::string basicPS2DShader("Basic2DPS.cso");

		std::string basicVS3DShader("Basic3DVS.cso");
		std::string basicPS3DShader("Basic3DPS.cso");

#if DEBUG_PHYSIC
		std::string basicVSDebugPhysicShader("BasicDebugPhysicVS.cso");
		std::string basicPSDebugPhysicShader("BasicDebugPhysicPS.cso");
#endif
	};
}

void RSRProgramInstance::RendererData::ResetDatas()

{
	std::unique_lock<std::mutex> lockFramesToRender(m_framesToRenderLock);
	bStopRenderer = false;
	while (!FramesToRender.empty()) {
		FramesToRender.pop();
	}
	lockFramesToRender.unlock();

	std::unique_lock<std::mutex> lockLastRendered(m_LastRenderedframeLock);
	LastRenderedFrameIndex = 0;
	lockLastRendered.unlock();
}

void RSRush::RSRProgramInstance::RendererData::RenderFrame(const FrameData& InFrameData)
{
	std::unique_lock<std::mutex>lock(m_framesToRenderLock);
	FramesToRender.push(InFrameData);
	lock.unlock();
	m_cvHasframesToRender.notify_one();
}

void RSRush::RSRProgramInstance::RendererData::WaitForFrame(const size_t InFrameIndex)
{
	if (LastRenderedFrameIndex >= InFrameIndex) { return; }
	std::unique_lock<std::mutex> lastRenderedframeLock(m_LastRenderedframeLock);
	m_cvLastRenderedframeChanged.wait(lastRenderedframeLock, [this, &InFrameIndex] { return LastRenderedFrameIndex >= InFrameIndex; });
	lastRenderedframeLock.unlock();
	return;
}

void RSRush::RSRProgramInstance::RendererData::StopRenderer(const size_t InLastFrameIndex)
{
	WaitForFrame(InLastFrameIndex);
	std::unique_lock<std::mutex> lock(m_framesToRenderLock);
	bStopRenderer = true;
	lock.unlock();
	m_cvHasframesToRender.notify_one();
}

RSRush::RSRProgramInstance::RSRProgramInstance()
: mds::Singleton<RSRProgramInstance>(), m_piStatistics(), m_bIsInitialized(false)
{

}

RSRush::RSRProgramInstance::~RSRProgramInstance()
{
	if (m_bIsInitialized)
	{
		ShutdownProgram();
	}
}

void RSRProgramInstance::Run(int argc, char** argv)
{
	UNIT_TEST();
	InitializePaths();
	if (!m_bIsInitialized)
	{
		InitializeProgram();
	}

	//DXWindow::Get().SetFullscreen(true);
	{
		std::shared_ptr<RSRScene> scene = CreateScene(START_SCENE);
		scene->Load();
		SetCurrentScene(scene);
		scene.reset();
	}
	

	//Reset Renderer orders
	m_rendererData.ResetDatas();
	
	auto ThreadRenderFunc = [this]() -> void
		{
			do
			{
				FrameData frameTask;
				{
					//Acquire Frame Data
					ZoneScopedN("Acquire Frame Data");
					std::unique_lock<std::mutex> frameToRenderLock(m_rendererData.m_framesToRenderLock);
					m_rendererData.m_cvHasframesToRender.wait(frameToRenderLock, [this] { return m_rendererData.bStopRenderer || !m_rendererData.FramesToRender.empty(); });
					if (m_rendererData.bStopRenderer && m_rendererData.FramesToRender.empty())
					{
						return;
					}
					frameTask = std::move(m_rendererData.FramesToRender.front());
					m_rendererData.FramesToRender.pop();
					frameToRenderLock.unlock();
				}

				{
					//Render Frame Data
					ZoneScopedN("Render Scene");
					bool _ = m_currentScene->Render(frameTask.GameTime, frameTask.DeltaTime);
				}

				{
					//Notify ended frame
					ZoneScopedN("Notify ended frame");
					std::unique_lock<std::mutex> frameRenderedLock(m_rendererData.m_LastRenderedframeLock);
					m_rendererData.LastRenderedFrameIndex = std::max(m_rendererData.LastRenderedFrameIndex, frameTask.FrameIndex);
					frameRenderedLock.unlock();
					m_rendererData.m_cvLastRenderedframeChanged.notify_one();
				}
			} while (true);
		};

	//Renderer Thread
	std::thread rendererThread(ThreadRenderFunc);

	mds::RTimekeeper::Initialize();
	//force a credible BeginNewFrame time value
	mds::RTimekeeper::SleepFor(16);

	constexpr size_t FIRST_FRAME_INDEX = 1;

	double ComputeFrameDeltaTimeSecond = mds::RTimekeeper::GetLastTick();
	double ComputeFrameFromStartSecond = mds::RTimekeeper::GetFromStartSeconds();
	size_t CompteFrameIndex = 0;

	double RenderingFrameDeltaTimeSecond = 0.f;
	double RenderingFrameFromStartSecond = 0.f;
	size_t RenderingFrameIndex = 0;// std::min(FIRST_FRAME_INDEX - 1, FIRST_FRAME_INDEX);

	while (!DXWindow::Get().ShouldClose())
	{
		{
			//Pre Frame Setups
			ZoneScopedN("Pre Frame Setups");
			//Process windows messages
			DXWindow::Get().Update();
			//Handle resizing
			if (DXWindow::Get().ShouldResize())
			{
				if (RenderingFrameIndex >= FIRST_FRAME_INDEX)
				{
					m_rendererData.WaitForFrame(RenderingFrameIndex);
				}
				//Clean current Command Queue for buffers
				DXContext::Get().FlushRender(DXWindow::GetFrameCount());
				//Resize
				DXWindow::Get().Resize();
			}
		}
		
		{
			//Notify Renderer of Frame Task
			ZoneScopedN("Notify Renderer of Frame Task");
			RenderingFrameDeltaTimeSecond = ComputeFrameDeltaTimeSecond;
			RenderingFrameFromStartSecond = ComputeFrameFromStartSecond;
			RenderingFrameIndex = CompteFrameIndex;

			//Start rendering after an a first update has been done
			if (RenderingFrameIndex >= FIRST_FRAME_INDEX)
			{
				m_rendererData.RenderFrame
				(
					FrameData
					{
						.FrameIndex = RenderingFrameIndex,
						.GameTime = RenderingFrameFromStartSecond,
						.DeltaTime = RenderingFrameDeltaTimeSecond
					}
				);
			}
		}

		{
			//Compute Frame
			ZoneScopedN("Compute Frame");
			++CompteFrameIndex;
			ComputeFrameDeltaTimeSecond = mds::RTimekeeper::BeginNewFrame();
			ComputeFrameFromStartSecond = mds::RTimekeeper::GetFromStartSeconds();


			mds::RTaskManager::Get().CallMainThreadCallbacks();

			//Handle GameInputs
			RSRGameInput::Get().Update(ComputeFrameDeltaTimeSecond);
			m_currentScene->PrePassTick(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
			RSRPhysicManager::Get().UpdatePhysic(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
		}

		//Start rendering after an a first update has been done
		if (RenderingFrameIndex >= FIRST_FRAME_INDEX)
		{
			//Wait For Renderer
			ZoneScopedN("Wait For Renderer");
			m_rendererData.WaitForFrame(RenderingFrameIndex);
		}

		{
			//Sync Compute and Rendering
			ZoneScopedN("Sync Compute and Rendering");
			RSRPhysicManager::Get().LateTickSync(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
			//Update Program State in sync before next render
			m_currentScene->LateTickSync(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
		}
		FrameMark;
	}
	m_rendererData.StopRenderer(RenderingFrameIndex);
	rendererThread.join();

	ShutdownProgram();
}


void RSRProgramInstance::InitializeProgram()
{
	CADebugLayer::Get().Init();
	m_randomizer = std::make_unique<RSRRandomizer>();
	m_randomizer->Initialize(/*uint64_t seed or none (random seed)*/);
	mds::RTaskManager::Get().Init(12);

	// --- Shaders ---
	m_shaders.emplace(EShaderName::BasicRootSignature2D, std::make_unique<Shader>(EShaderName::BasicRootSignature2D));
	
	m_shaders.emplace(EShaderName::BasicRootSignature3D, std::make_unique<Shader>(EShaderName::BasicRootSignature3D));
#if DEBUG_PHYSIC
	m_shaders.emplace(EShaderName::BasicRootSignatureDebugPhysic, std::make_unique<Shader>(EShaderName::BasicRootSignatureDebugPhysic));
#endif

	m_shaders.emplace(EShaderName::basicVS2DShader, std::make_unique<Shader>(EShaderName::basicVS2DShader));
	m_shaders.emplace(EShaderName::basicPS2DShader, std::make_unique<Shader>(EShaderName::basicPS2DShader));

	m_shaders.emplace(EShaderName::basicVS3DShader, std::make_unique<Shader>(EShaderName::basicVS3DShader));
	m_shaders.emplace(EShaderName::basicPS3DShader, std::make_unique<Shader>(EShaderName::basicPS3DShader));

#if DEBUG_PHYSIC
	m_shaders.emplace(EShaderName::basicVSDebugPhysicShader, std::make_unique<Shader>(EShaderName::basicVSDebugPhysicShader));
	m_shaders.emplace(EShaderName::basicPSDebugPhysicShader, std::make_unique<Shader>(EShaderName::basicPSDebugPhysicShader));
#endif
	
	if (!DXContext::Get().Init())
	{
		//std::cout << " Bad " << std::endl;
		return;
	}
	// --- Create root signature ---
	_R_FAILED(DXContext::Get().GetDevice()->CreateRootSignature(0, m_shaders[EShaderName::BasicRootSignature2D]->GetBuffer(), m_shaders[EShaderName::BasicRootSignature2D]->GetSize(), IID_PPV_ARGS(&m_rootSignature2D)));
	_R_FAILED(DXContext::Get().GetDevice()->CreateRootSignature(0, m_shaders[EShaderName::BasicRootSignature3D]->GetBuffer(), m_shaders[EShaderName::BasicRootSignature3D]->GetSize(), IID_PPV_ARGS(&m_rootSignature3D)));
#if DEBUG_PHYSIC
	_R_FAILED(DXContext::Get().GetDevice()->CreateRootSignature(0, m_shaders[EShaderName::BasicRootSignatureDebugPhysic]->GetBuffer(), m_shaders[EShaderName::BasicRootSignatureDebugPhysic]->GetSize(), IID_PPV_ARGS(&m_rootSignatureDebugPhysic)));
#endif

	RSRAssetManager* pAssetManager = &RSRAssetManager::Get();

	if (!DXPipeline::Get().Init
			(
				m_rootSignature2D.Get(), m_shaders[EShaderName::basicVS2DShader].get(), m_shaders[EShaderName::basicPS2DShader].get(),
				m_rootSignature3D.Get(), m_shaders[EShaderName::basicVS3DShader].get(), m_shaders[EShaderName::basicPS3DShader].get()
#if DEBUG_PHYSIC
				, m_rootSignatureDebugPhysic.Get(), m_shaders[EShaderName::basicVSDebugPhysicShader].get(), m_shaders[EShaderName::basicPSDebugPhysicShader].get()
#endif
			)
		)
	{
		RSRLog::LogException(TEXT("Cannot initialize DXPipeline !"));
		return;
	}
	_R_FAILED(DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&DXPipeline::Get().GetStateDesc2D(), IID_PPV_ARGS(&m_pso2D)));
	_R_FAILED(DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&DXPipeline::Get().GetStateDesc3D(), IID_PPV_ARGS(&m_pso3D)));
#if DEBUG_PHYSIC
	_R_FAILED(DXContext::Get().GetDevice()->CreateGraphicsPipelineState(&DXPipeline::Get().GetStateDescDebugPhysic(), IID_PPV_ARGS(&m_psoDebugPhysic)));
#endif

	_R_FALSE(DXWindow::Get().Init());
	_R_FALSE(RSRBasicShapes::Get().Init(pAssetManager));
	_R_FALSE(RSRPhysicManager::Get().Init(&RSRBasicShapes::Get()));
	_R_FALSE(RSRGameInput::Get().Init());



	m_bIsInitialized = true;
}

void RSRProgramInstance::ShutdownProgram()
{
	ResetCurrentScene();
	for (auto scene : m_scenes)
	{
		if (scene && scene->GetIsLoaded())
		{
			scene->UnLoad();
		}
	}
	m_scenes.clear();
	m_shaders.clear();

	RSRGameInput::Get().Shutdown();
	RSRPhysicManager::Get().Shutdown();
	RSRBasicShapes::Get().Shutdown();
	DXWindow::Get().Shutdown();


#if DEBUG_PHYSIC
	m_psoDebugPhysic.Reset();
	m_rootSignatureDebugPhysic.Reset();
#endif

	m_pso3D.Reset();
	m_rootSignature3D.Reset();

	m_pso2D.Reset();
	m_rootSignature2D.Reset();

	DXPipeline::Get().Shutdown();
	DXContext::Get().Shutdown();

	m_randomizer.reset();

	mds::RTaskManager::Get().Shutdown();
	CADebugLayer::Get().Shutdown();
	m_bIsInitialized = false;
}

void RSRProgramInstance::SetCurrentScene(std::shared_ptr<RSRScene> InNewCurrentScene)
{
	if (InNewCurrentScene && m_currentScene == InNewCurrentScene)
	{
		RSRLog::LogWarning(TEXT("Currently active Scene was already the active Scene !"));
		return;
	}
	ResetCurrentScene();
	if (!InNewCurrentScene) { return; }
	m_currentScene = InNewCurrentScene;
	RSRPhysicManager::Get().SetPhysicContext(m_currentScene->GetPhysicContext());
	RSRGameInput::Get().SetUserProvider(m_currentScene->GetGameManager().get());
}

void RSRProgramInstance::ResetCurrentScene()
{
	if (m_currentScene)
	{
		DXContext::Get().FlushRender(DXWindow::GetFrameCount());
	}
	RSRGameInput::Get().SetUserProvider(nullptr);
	m_currentScene.reset();
}

std::shared_ptr<RSRScene> RSRProgramInstance::CreateScene(const EStartScene EInSceneType)
{
	switch (EInSceneType)
	{
	case EStartScene::TheTrench:
		return RSRProgramInstance::CreateScene<RSRTTScene>();
	case EStartScene::PhysicDemo1:
		return RSRProgramInstance::CreateScene<RSRPD1Scene>();
	case EStartScene::Unknown:
		//FallThough
	default:
		RSRLog::Log(LOG_ERROR, TEXT("Unexpected EStartScene type 'EStartScene value {}' in swith RSRProgramInstance::CreateScene !"), mds::UT_cast(EInSceneType));
		return RSRProgramInstance::CreateScene<RSRTTScene>();
		break;
	}
}

bool RSRProgramInstance::InitializePaths() const
{
	wchar_t moduleFileName[MAX_PATH];
	GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);

	RSRPaths::m_runnablePath = moduleFileName;
	RSRPaths::m_runnablePath.remove_filename();
	RSRPaths::m_runnablePath_str = RSRPaths::m_runnablePath.string();
	RSRPaths::m_runnablePath_view = RSRPaths::m_runnablePath_str;

#if	USE_App_PROJECT_ASSETS
	RSRPaths::m_assetsDiskPath = App_PROJECT_ASSET_PATH;
#else
	
	RSRPaths::m_assetsDiskPath = RSRPaths::m_runnablePath / App_LOCAL_ASSET_PATH / "";
	//RSRPaths::m_assetsDiskPath 
#endif
	RSRPaths::m_assetsDiskPath_str = RSRPaths::m_assetsDiskPath.string();
	RSRPaths::m_assetsDiskPath_view = RSRPaths::m_assetsDiskPath_str;


#if	USE_App_PROJECT_ASSETS
	RSRPaths::m_shadersDiskPath = RSRPaths::m_runnablePath;
	RSRPaths::m_shadersDiskPath.append(App_LOCAL_SHADER_PATH);
#else
	RSRPaths::m_shadersDiskPath = RSRPaths::m_runnablePath;
	RSRPaths::m_shadersDiskPath.append(App_LOCAL_SHADER_PATH);
#endif
	RSRPaths::m_shadersDiskPath_str = RSRPaths::m_shadersDiskPath.string();
	RSRPaths::m_shadersDiskPath_view = RSRPaths::m_shadersDiskPath_str;

	RSRLog::Log(LOG_DISPLAY, TEXT("filepaths : '{}'"), RSRPaths::m_runnablePath_str);
	//RSRPaths::
	return true;
}

bool RSRProgramInstance::UNIT_TEST()
{
	bool bAllSucessfull = true;
	bAllSucessfull = RSRVoxalGrid::UNIT_TEST_CLASS() && bAllSucessfull;
	bAllSucessfull = RSRVoxalPlane::UNIT_TEST_CLASS() && bAllSucessfull;
	
	return bAllSucessfull;
}

