#include "App/Physic/Demo01/RSRPD1Scene.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1GameManager.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1PlayerController.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1Pawn.h"
#include "App/Physic/Demo01/SObject/RSRPD1StaticSObject.h"
#include "App/Physic/Demo01/SObject/RSRPD1SphereSObject.h"
#include <App/Data/Textures/RSRTexture2D.h>
#include "App/D3D/DXContext.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Physic/LD/RSRLDReader.h"
#include "App/Tools/Window.h"
#include "App/Tools/RSRLog.h"
#include <Tracy.hpp>
#include <random>

using namespace RSRush;
using namespace DirectX;

static constexpr XMFLOAT3 BALL_SPAWN_POSITIONS[] = 
	{ 
		XMFLOAT3{ 11.6f, -3.6f,  10.f },XMFLOAT3{ 11.6f, 3.6f,  10.f },
		XMFLOAT3{  6.2f, -6.5f,  10.f },XMFLOAT3{ 6.2f, 6.5f,  10.f }
	};
static constexpr size_t BALL_SPAWN_POSITIONS_COUNT = _countof(BALL_SPAWN_POSITIONS);

RSRPD1Scene::RSRPD1Scene(Private)
	: RSROScene()
{
}

RSRPD1Scene::~RSRPD1Scene()
{
}
static std::vector<RSRLDElem> FillLevelData();
static const std::vector<RSRLDElem> LDElems = FillLevelData();

bool RSRPD1Scene::Load()
{
	bool bAllSucessfull = RSROScene::Load();
	bool bNoGravity = false;
	if (bNoGravity)
	{
		this->m_physicContext = RSRPhysicContext
		{
			.GlobalGravity = XMFLOAT3{ 0.f, 0.f , 0.00f}
		};
	}
	RSRAssetManager* pAssetManager = &RSRAssetManager::Get();
	m_gameManager = std::make_shared<RSRPD1GameManager>();

	m_gameManager->InitializeGame(this->m_thisWPtr);

	bAllSucessfull = RSRLDReader::FillLDDatas(LDElems, this) && bAllSucessfull;

	float groundSphereRadius = 1000.f;/*000000000*/
	float width = 50.f;

	if (true)
	{
		AddNewSObject(std::make_shared<RSRPD1SphereSObject>
			(
				RSRTransform{
					.Position = { 11.6f, -3.6f,  10.f },
					.Rotation = { 0.f, 0.f,  0.f },
					.Scale = { 1.f, 1.f, 1.f }
				}
			)
		);
		AddNewSObject(std::make_shared<RSRPD1SphereSObject>
			(
				RSRTransform{
					.Position = { 11.6f, 3.6f,  10.f },
					.Rotation = { 0.f, 0.f,  0.f },
					.Scale = { 1.f, 1.f, 1.f }
				}
			)
		);
		AddNewSObject(std::make_shared<RSRPD1SphereSObject>
			(
				RSRTransform{
					.Position = { 6.2f, -6.5f,  10.f },
					.Rotation = { 0.f, 0.f,  0.f },
					.Scale = { 1.f, 1.f, 1.f }
				}
			)
		);
		AddNewSObject(std::make_shared<RSRPD1SphereSObject>
			(
				RSRTransform{
					.Position = { 6.2f, 6.5f,  10.f },
					.Rotation = { 0.f, 0.f,  0.f },
					.Scale = { 1.f, 1.f, 1.f }
				}
			)
		);

	}


	//AddNewSObject(std::make_shared<RSRPD1SphereSObject>
	//	(
	//		RSRTransform{
	//			.Position = { 0.f, 0.0f,  groundSphereRadius * -0.5f },
	//			.Rotation = { 0.f, 0.f,  0.f },
	//			.Scale = { groundSphereRadius, groundSphereRadius, groundSphereRadius }
	//		}, WORLD_STATIC, false
	//	)
	//);

	RSRush::RSRPhysicManager::Get().AddPhysicSolver(RSRSolverType::Position);
	RSRush::RSRPhysicManager::Get().AddPhysicSolver(RSRSolverType::Impulse);

	

	//Copy CPU Resource --> GPU Resource
	auto* verticesCmdList = DXContext::Get().InitRenderCommandList();

	_RF_FALSE(RSRush::RSRBasicShapes::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#if DEBUG_PHYSIC
	_RF_FALSE(RSRush::RSRPhysicManager::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#endif

	UploadSOResources(DXContext::Get().GetDevice().Get(), verticesCmdList);

	DXContext::Get().ExecuteRenderCommandList();
	verticesCmdList = nullptr;

	RSRush::RSRBasicShapes::Get().FreeUploadBuffers();
#if DEBUG_PHYSIC
	RSRush::RSRPhysicManager::Get().FreeUploadBuffers();
#endif
	FreeSOResourceBuffers();


	//Textures
	static const std::string auge_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Textures")) / TEXT("auge_512_512_BGRA_32BPP.png")).string();
	m_3dTextures.push_back(pAssetManager->AddTextureAsset(auge_512_512_BGRA_32BPP, false));

	static const std::string auge_512_512_BGRA_32BPP_R = (std::filesystem::path(TEXT("Textures")) / TEXT("auge_512_512_BGRA_32BPP_R.png")).string();
	m_3dTextures.push_back(pAssetManager->AddTextureAsset(auge_512_512_BGRA_32BPP_R, false));

	static const std::string auge_512_512_BGRA_32BPP_G = (std::filesystem::path(TEXT("Textures")) / TEXT("auge_512_512_BGRA_32BPP_G.png")).string();
	m_3dTextures.push_back(pAssetManager->AddTextureAsset(auge_512_512_BGRA_32BPP_G, false));

	static const std::string auge_512_512_BGRA_32BPP_B = (std::filesystem::path(TEXT("Textures")) / TEXT("auge_512_512_BGRA_32BPP_B.png")).string();
	m_3dTextures.push_back(pAssetManager->AddTextureAsset(auge_512_512_BGRA_32BPP_B, false));

	static const std::string target_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Sprites")) / TEXT("target_512_512_BGRA_32BPP.png")).string();
	m_2dTextures.push_back(pAssetManager->AddTextureAsset(target_512_512_BGRA_32BPP, false));

	static const std::string heart_ai_1 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_ai_1.png")).string();
	m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_ai_1, false));

	static const std::string heart_ai_0 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_ai_0.png")).string();
	m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_ai_0, false));

	//Copy CPU Resource --> GPU Resource
	auto* allocationCmdList = DXContext::Get().InitRenderCommandList();

	RSRush::RSRTexture2D::UploadResources(m_3dTextures, DXContext::Get().GetDevice().Get(), allocationCmdList, m_srvheap3D);

	RSRush::RSRTexture2D::UploadResources(m_2dTextures, DXContext::Get().GetDevice().Get(), allocationCmdList, m_srvheap2D);

	//EyeTexture->UploadResources(DXContext::Get().GetDevice().Get(), allocationCmdList, srvheap3D);

	//Execute Upload resources CommandList.
	DXContext::Get().ExecuteRenderCommandList();
	allocationCmdList = nullptr;

	return  bAllSucessfull;
}

bool RSRPD1Scene::UnLoad()
{
	bool bAllSucessfull = true;

	m_srvheap2D.Reset();
	m_srvheap3D.Reset();
	//RSRush::RSRPhysicManager::Get().RemovePhysicSolver(RSRSolverType::Impulse);
	//RSRush::RSRPhysicManager::Get().RemovePhysicSolver(RSRSolverType::Position);
	RSRush::RSRPhysicManager::Get().ClearAllPhysicSolvers();

	for (size_t i = 0; i < m_3dTextures.size(); ++i)
	{
		bAllSucessfull = m_3dTextures[i]->FreeResourcesBuffer() && bAllSucessfull;
	}
	for (size_t i = 0; i < m_2dTextures.size(); ++i)
	{
		bAllSucessfull = m_2dTextures[i]->FreeResourcesBuffer() && bAllSucessfull;
	}

	m_gameManager->ShutdownGame();

	m_bFreeSOResourceOnUnload = true;
	return RSROScene::UnLoad() && bAllSucessfull;
}

bool RSRush::RSRPD1Scene::Render(const double InGameTime, const double InDeltaTime)
{
	bool bAllSucessfull = RSROScene::Render(InGameTime, InDeltaTime);
	RSRush::MVP_DL_Consts matrixes
	{
		.ViewProjMat = XMFLOAT4X4(),
		.ModMat = XMFLOAT4X4(),
		.invProjModMat = XMFLOAT4X4(),
		.CamPos = XMFLOAT4(),
		.lightPos_AmbLight = XMFLOAT4 { 50.0f, 30.0f, 40.0f	,/*AmbLight */ 1.0f },
		.lightCol_SpecLight = XMFLOAT4 { 1.0f, 1.0f, 0.6f		,/*SpecLight*/ 5.5f }
	};
	std::shared_ptr<const RSRPD1GameManager> pD1GameManager = std::static_pointer_cast<const RSRPD1GameManager>(m_gameManager);

	const RSRPD1PlayerController* playerController = pD1GameManager->GetPD1PlayerController();

	//Begin Drawing
	auto* cmdList = DXContext::Get().InitRenderCommandList();
	//Draw to window (Currently presented frame goes back to writing state)
	DXWindow::Get().BeginFrame(cmdList);

	// -- RS --
	D3D12_VIEWPORT vp =
	{
		.TopLeftX = 0.f,
		.TopLeftY = 0.f,
		.Width = (FLOAT)DXWindow::Get().GetWidth(),     //narrow conversion
		.Height = (FLOAT)DXWindow::Get().GetHeight(),   //narrow conversion
		.MinDepth = 1.f,
		.MaxDepth = 0.f
	};
	cmdList->RSSetViewports(1, &vp);
	D3D12_RECT scRect = {
		.left = 0,
		.top = 0,
		.right = (LONG)DXWindow::Get().GetWidth(),   //narrow conversion
		.bottom = (LONG)DXWindow::Get().GetHeight()   //narrow conversion
	};
	cmdList->RSSetScissorRects(1, &scRect);

	cmdList->SetPipelineState(m_programInstance->GetPSO3D().Get());
	cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSig3D().Get());
	cmdList->SetDescriptorHeaps(1, m_srvheap3D.GetAddressOf());
	cmdList->SetGraphicsRootDescriptorTable(1, m_srvheap3D->GetGPUDescriptorHandleForHeapStart());

	const RSRCameraData* CameraData = playerController->GetCameraData();

	DirectX::XMMATRIX CameraViewMatrix = CameraData->ComputeView();
	DirectX::XMMATRIX CameraProjectionMatrix = CameraData->ComputeProjection();
	DirectX::XMMATRIX CameraViewProjectionMatrix = DirectX::XMMatrixMultiply(CameraViewMatrix, CameraProjectionMatrix);


	//Camera position for phong
	matrixes.CamPos = DirectX::XMFLOAT4(CameraData->Position.x, CameraData->Position.y, CameraData->Position.z, 1.0f);
	DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);

	cmdList->SetGraphicsRoot32BitConstants(0, MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

	DrawSOMeshs(cmdList);


#if DEBUG_PHYSIC
	//----------- DrawDebugPhysic ------------//
	cmdList->SetPipelineState(m_programInstance->GetPSODebugPhysic().Get());
	cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSigDebugPhysic().Get());
	DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);
	ModelViewprojectionConstants matrixesDebugPhys = matrixes.ToMVC();
	cmdList->SetGraphicsRoot32BitConstants(0, MVPC::S32B_STRUCT, &matrixesDebugPhys, 0);

	RSRush::RSRPhysicManager::Get().DrawPhysic(cmdList);
#endif

	//(Currently presented frame goes to PresentState)
	DXWindow::Get().EndFrame(cmdList);

	//Force Sync the renderer as ExecuteCommandList Wait for the GPU end of execution before returning
	//This is for simplicity, but of course deafeat the purpuse of DX12 async approch
	DXContext::Get().ExecuteRenderCommandList();

	//Swap the chain (present result)
	DXWindow::Get().Present();

	return bAllSucessfull;
}

bool RSRush::RSRPD1Scene::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	bool bAllSucessfull = RSROScene::PrePassTick(InGameTime, InDeltaTime);
	
	PD1SlideTimelineData& GDs = m_slideTimelineData;
	if (InGameTime > GDs.NextGameTimeBallSpawn)
	{
		static thread_local std::random_device rd;
		static thread_local std::mt19937_64 gen(rd());
		static thread_local std::uniform_real_distribution<double> distTime(0.55, 1.15);
		static thread_local std::uniform_int_distribution<uint64_t> distPoint(0, (BALL_SPAWN_POSITIONS_COUNT > 0) ? (BALL_SPAWN_POSITIONS_COUNT - 1) : 0);

		
		if (GDs.InTickBallSpawned == 0 && GDs.NextGameTimeBallSpawn <= std::numeric_limits<double>::epsilon())
		{	//Special case, first tick, ideally would go to a BeginPlay() function
			GDs.NextGameTimeBallSpawn = InGameTime + distTime(gen);
			GDs.NextBallSpawnIndexLocation = distPoint(gen);
		}
		else
		{

			AddNewSObject(std::make_shared<RSRPD1SphereSObject>
				(
					RSRTransform{
						.Position = BALL_SPAWN_POSITIONS[GDs.NextBallSpawnIndexLocation],
						.Rotation = { 0.f, 0.f,  0.f },
						.Scale = { 1.f, 1.f, 1.f }
					}
				)
			);

			++GDs.InTickBallSpawned;
			GDs.NextGameTimeBallSpawn = InGameTime + distTime(gen);
			GDs.NextBallSpawnIndexLocation = distPoint(gen);
		}
	}
		
	std::shared_ptr<RSRPD1GameManager> pD1GameManager = std::static_pointer_cast<RSRPD1GameManager>(m_gameManager);
	RSRPD1PlayerController* playerController = pD1GameManager->GetPD1PlayerController();
	pD1GameManager->PrePassTick(InGameTime, InDeltaTime);

	return bAllSucessfull;
}

bool RSRush::RSRPD1Scene::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool bAllSucessfull = RSROScene::LateTickSync(InGameTime, InDeltaTime);
	std::shared_ptr<RSRPD1GameManager> pD1GameManager = std::static_pointer_cast<RSRPD1GameManager>(m_gameManager);

	RSRPD1PlayerController* playerController = pD1GameManager->GetPD1PlayerController();

	pD1GameManager->LateTickSync(InGameTime, InDeltaTime);

	return bAllSucessfull;
}
#include "App/Physic/Demo01/LD/LDSlides.h"
std::vector<RSRLDElem> FillLevelData()
{
	return LD::CreateLDSlideslLevelData();
}
