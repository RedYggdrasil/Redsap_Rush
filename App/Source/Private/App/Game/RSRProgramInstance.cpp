#include "App/Game/RSRProgramInstance.h"

///MDS Library
#include "MDS/Threads/RTaskManager.h"
#include "MDS/Tools/System/RTimekeeper.h"


///App
#include "App/D3D/DXContext.h"
#include "App/Data/Trench/RSRVoxalGrid.h"
#include "App/Data/Trench/RSRVoxalPlane.h"
#include "App/Game/RSRPaths.h"
#include "App/Gameplay/GameManager.h"
#include "App/Gameplay/RSRScene.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Geometry/RSRMeshLoader.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Physic/Demo01/RSRPD1Scene.h"
#include "App/Render/RSRPipelines.h"
#include "App/System/RSRRandomizer.h"
#include "App/TheTrench/RSRTTScene.h"
#include "App/Tools/DebugLayer.h"
#include "App/Tools/ImageLoader.h"
#include "App/Tools/Inputs/RSRGameInput.h"
#include "App/Tools/Shader.h"
#include "App/Tools/Window.h"

#include <Tracy.hpp>


using Microsoft::WRL::ComPtr;
using namespace RSRush;



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

void RSRProgramInstance::RendererData::RenderFrame(const FrameData& InFrameData)
{
	std::unique_lock<std::mutex>lock(m_framesToRenderLock);
	FramesToRender.push(InFrameData);
	lock.unlock();
	m_cvHasframesToRender.notify_one();
}

void RSRProgramInstance::RendererData::WaitForFrame(const size_t InFrameIndex)
{
	if (LastRenderedFrameIndex >= InFrameIndex) { return; }
	std::unique_lock<std::mutex> lastRenderedframeLock(m_LastRenderedframeLock);
	m_cvLastRenderedframeChanged.wait(lastRenderedframeLock, [this, &InFrameIndex] { return LastRenderedFrameIndex >= InFrameIndex; });
	lastRenderedframeLock.unlock();
	return;
}

void RSRProgramInstance::RendererData::StopRenderer(const size_t InLastFrameIndex)
{
	WaitForFrame(InLastFrameIndex);
	std::unique_lock<std::mutex> lock(m_framesToRenderLock);
	bStopRenderer = true;
	lock.unlock();
	m_cvHasframesToRender.notify_one();
}

RSRProgramInstance::RSRProgramInstance()
: IRProgramRoot(), m_piStatistics(), m_bIsInitialized(false),
m_assetManager(this), m_basicShapes(this),
m_dxContext(this), m_dxWindow(), m_gameInput(),
m_meshLoader(this), m_physicManager()
{

}

RSRProgramInstance::~RSRProgramInstance()
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
		//bool bIsTheTrench = true;
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

	while (!m_dxWindow.ShouldClose())
	{
		//Pre Frame Setups
		{
			ZoneScopedN("Pre Frame Setups");
			//Process windows messages
			m_dxWindow.Update();
			//Handle resizing
			if (m_dxWindow.ShouldResize())
			{
				if (RenderingFrameIndex >= FIRST_FRAME_INDEX)
				{
					m_rendererData.WaitForFrame(RenderingFrameIndex);
				}
				//Clean current Command Queue for buffers
				m_dxContext.FlushRender(DXWindow::GetFrameCount());
				//Resize
				m_dxWindow.Resize();
			}
		}

		//Notify Renderer of Frame Task
		{
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
		
		//Compute Frame
		{
			ZoneScopedN("Compute Frame");
			++CompteFrameIndex;
			ComputeFrameDeltaTimeSecond = mds::RTimekeeper::BeginNewFrame();
			ComputeFrameFromStartSecond = mds::RTimekeeper::GetFromStartSeconds();


			mds::RTaskManager::Get().CallMainThreadCallbacks();

			//Handle GameInputs
			m_gameInput.Update(ComputeFrameDeltaTimeSecond);
			m_currentScene->PrePassTick(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
			m_physicManager.UpdatePhysic(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
		}

		//Start rendering after an a first update has been done
		if (RenderingFrameIndex >= FIRST_FRAME_INDEX)
		{
			//Wait For Renderer
			ZoneScopedN("Wait For Renderer");
			m_rendererData.WaitForFrame(RenderingFrameIndex);
		}

		//Sync Compute and Rendering
		{
			ZoneScopedN("Sync Compute and Rendering");
			m_physicManager.LateTickSync(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
			//Update Program State in sync before next render
			m_currentScene->LateTickSync(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond);
		}
		//Upload new sync buffer data to GPU
		{
			ZoneScopedN("Upload new sync buffer data to GPU");
			ID3D12GraphicsCommandList7* frameUploadList = m_dxContext.InitFrameUploadList();
			m_currentScene->UpdateMeshInstanceBuffers(ComputeFrameFromStartSecond, ComputeFrameDeltaTimeSecond, frameUploadList);
			m_dxContext.ExecuteFrameUploadCommandList();
			m_currentScene->ClearUnusedMeshInstanceBuffers();
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

//	
	if (!m_dxContext.Init())
	{
		//std::cout << " Bad " << std::endl;
		return;
	}

	m_pipelines = std::make_unique<RSRPipelines>(this);

	if (!m_pipelines || !m_pipelines->GetIsCorrectlyLoaded())
	{
		RSRLog::LogException(TEXT("Cannot initialize RSRPipeline !"));
		return;
	}

	m_assetManager.Init();
	m_meshLoader.Init();

	_R_FALSE(m_dxWindow.Init(this));
	m_dxWindow.SetUsingVerticalSync(!GetPIStatistics().bTearingSupported || USE_VSYNC_DEFAULT_VALUE);
	_R_FALSE(m_basicShapes.Init(&m_assetManager));
	_R_FALSE(m_physicManager.Init(this, &m_basicShapes));

	_R_FALSE(m_gameInput.Init(this));

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

	m_gameInput.Shutdown();
	m_physicManager.Shutdown();
	m_basicShapes.Shutdown();
	m_dxWindow.Shutdown();

	m_meshLoader.ShutDown();
	m_assetManager.ShutDown();

	m_pipelines.reset();

	m_dxContext.Shutdown();

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
	m_physicManager.SetPhysicContext(m_currentScene->GetPhysicContext());
	m_gameInput.SetUserProvider(m_currentScene->GetGameManager().get());
}

void RSRProgramInstance::ResetCurrentScene()
{
	if (m_currentScene)
	{
		m_dxContext.FlushRender(DXWindow::GetFrameCount());
	}
	m_gameInput.SetUserProvider(nullptr);
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

	char separator = std::filesystem::path::preferred_separator; 
	std::string separatorString(1, separator);

	std::string assetFolderAndSeparatorConstruction = separatorString;
	assetFolderAndSeparatorConstruction.append(App_LOCAL_ASSET_PATH).append(separatorString);
	RSRPaths::m_assetFolderAndSeparator = assetFolderAndSeparatorConstruction;
	RSRPaths::m_assetFolderAndSeparator_str = RSRPaths::m_assetFolderAndSeparator.string();
	RSRPaths::m_shadersDiskPath_view = RSRPaths::m_assetFolderAndSeparator_str;

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

Microsoft::WRL::ComPtr<ID3D12RootSignature> RSRProgramInstance::GetRootSig2D() const
{
	return m_pipelines->GetRSRPSO2D().GetRootSig();
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> RSRProgramInstance::GetPSO2D() const
{
	return m_pipelines->GetRSRPSO2D().GetPSO();
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RSRProgramInstance::GetRootSig3D() const
{
	return m_pipelines->GetRSRPSO3D().GetRootSig();
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> RSRProgramInstance::GetPSO3D() const
{
	return m_pipelines->GetRSRPSO3D().GetPSO();
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RSRProgramInstance::GetRootSig3DInstanced() const
{
	return m_pipelines->GetRSRPSO3DInstanced().GetRootSig();
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> RSRProgramInstance::GetPSO3DInstanced() const
{
	return m_pipelines->GetRSRPSO3DInstanced().GetPSO();
}

#if DEBUG_PHYSIC
Microsoft::WRL::ComPtr<ID3D12RootSignature> RSRProgramInstance::GetRootSigDebugPhysic() const
{
	return m_pipelines->GetRSRPSO3DDebugPhysic().GetRootSig();
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> RSRProgramInstance::GetPSODebugPhysic() const
{
	return m_pipelines->GetRSRPSO3DDebugPhysic().GetPSO();
}
#endif
