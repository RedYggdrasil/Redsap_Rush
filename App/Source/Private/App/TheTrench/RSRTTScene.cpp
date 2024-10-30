#include "App/TheTrench/RSRTTScene.h"

#include "MDS/Tools/RMath.h"
#include "App/Game/RSRProgramInstance.h"
#include "App/Game/RSRPlayerPath.h"
#include "App/TheTrench/Gameplay/RSRTTGameManager.h"
#include "App/TheTrench/Gameplay/RSRTTPlayerController.h"
#include "App/Tools/RSRLog.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/D3D/DXContext.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Managers/RSRTrenchManager.h"

#include "App/Data/Trench/RSRTrench.h"
#include <App/Data/Textures/RSRTexture2D.h>
#include "App/GPUCompute/Texture/RSRComputeUVTexture.h"
#include "App/GPUCompute/Texture/RSRComputeGreebleTexture.h"
#include "App/Tools/Window.h"
#include "App/TheTrench/RSRODrawableLightSource.h"
#include "App/TheTrench/RSRODrawableBackground.h"
#include <Tracy.hpp>

using namespace RSRush;

constexpr uint16_t FIRST_TRECH_TOP_GREEBLE_TEXTURE_ID = 3;
constexpr uint16_t TRENCH_TOP_GREEBLE_TEXTURE_NUMBER = 3;

constexpr uint16_t FIRST_TRECH_SIDE_GREEBLE_TEXTURE_ID = 6;
constexpr uint16_t TRENCH_SIDE_GREEBLE_TEXTURE_NUMBER = 3;
using Super = RSROScene;

RSRTTScene::RSRTTScene(Private)
	: Super(), m_trenchManager(nullptr)
{
    m_physicContext = PhysicContext::DEFAULT_MOON;

    std::vector<uint16_t> trenchTopGreebleTextureIDs = std::vector<uint16_t>();
    trenchTopGreebleTextureIDs.reserve(TRENCH_TOP_GREEBLE_TEXTURE_NUMBER);

    constexpr uint16_t pastTopGreebleLastIndex = FIRST_TRECH_TOP_GREEBLE_TEXTURE_ID + TRENCH_TOP_GREEBLE_TEXTURE_NUMBER;
    for (uint16_t i = FIRST_TRECH_TOP_GREEBLE_TEXTURE_ID; i < pastTopGreebleLastIndex; ++i)
    {
        trenchTopGreebleTextureIDs.push_back(i);
    }

    std::vector<uint16_t> trenchSideGreebleTextureIDs = std::vector<uint16_t>();
    trenchSideGreebleTextureIDs.reserve(TRENCH_SIDE_GREEBLE_TEXTURE_NUMBER);

    constexpr uint16_t pastSideGreebleLastIndex = FIRST_TRECH_SIDE_GREEBLE_TEXTURE_ID + TRENCH_SIDE_GREEBLE_TEXTURE_NUMBER;
    for (uint16_t i = FIRST_TRECH_SIDE_GREEBLE_TEXTURE_ID; i < pastSideGreebleLastIndex; ++i)
    {
        trenchSideGreebleTextureIDs.push_back(i);
    }

    m_trenchManager = std::make_shared<RSRTrenchManager>(std::move(trenchSideGreebleTextureIDs), std::move(trenchTopGreebleTextureIDs));
}

bool RSRTTScene::Load()
{
	bool bAllSucessfull = Super::Load();
	RSRAssetManager* pAssetManager = &RSRAssetManager::Get();
	m_gameManager = std::make_shared<RSRTTGameManager>();

	m_playerPawn = std::make_shared<RSRTT404Pawn>();
    m_playerPawn->SetSelfReference(m_playerPawn);
	m_playerPawn->GenerateMesh();
	RSRPhysicManager::Get().AddPhysicalEntity(m_playerPawn->GeneratePhysicBody());


	m_gameManager->InitializeGame(this->m_thisWPtr);

	if (pAssetManager->Exist(TEXT("TopLeftSquare2D")))
	{
		m_topLeftSquare2D = pAssetManager->GetShared<RSRMesh2D>(TEXT("TopLeftSquare2D"));
	}
	else
	{
		m_topLeftSquare2D = RSRBasicShapes::Get().GetRegisterNewPlane2D
		(TEXT("TopLeftSquare2D"),
			DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f),
			DirectX::XMFLOAT2(-1.f, -1.f),
			DirectX::XMFLOAT2(0.f, 0.f),
			DirectX::XMFLOAT2(1.f, 1.f));
	}
    AddNewSObject(std::make_shared<RSRODrawableBackground>
        (
            RSRTransform
            {
                .Position = DirectX::XMFLOAT3{ 9900.0f, 0.0f, 0.0f },
                .Rotation = DirectX::XMFLOAT3(0.f, 90.f, 10.f),
                .Scale = DirectX::XMFLOAT3(50000.f, 50000.f, 50000.f),
            },
            1
        ));
    m_drawableLightSource = AddNewSObject(std::make_shared<RSRODrawableLightSource>
        (
            RSRTransform 
            {
                .Position = DirectX::XMFLOAT3{ 7500.0f, 1500.0f, 2500.0f },
                .Rotation = DirectX::XMFLOAT3(0.f, 0.f, 0.f),
                .Scale = DirectX::XMFLOAT3(100.f, 100.f, 100.f),
            },
            /*Handle as SObject*/true,
            2
        ));

	//Copy CPU Resource --> GPU Resource
	auto* verticesCmdList = DXContext::Get().InitRenderCommandList();


	_RF_FALSE(m_topLeftSquare2D.get()->UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));

	_RF_FALSE(RSRBasicShapes::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#if DEBUG_PHYSIC
	_RF_FALSE(RSRPhysicManager::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#endif
	if (!m_playerPawn->UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList))
	{
		RSRLog::LogWarning(TEXT("Fail Upload"));
	}
	//must be after InitializeGame or move 'RSRTrenchManager::BeginNewTrench' Here
	if (!m_trenchManager->UploadBeingNewResources(DXContext::Get().GetDevice().Get(), verticesCmdList))
	{
		RSRLog::LogWarning(TEXT("TODO : Implement Start Trench special begin and upload prcedure !"));
	}

    UploadSOResources(DXContext::Get().GetDevice().Get(), verticesCmdList);

	DXContext::Get().ExecuteRenderCommandList();
	verticesCmdList = nullptr;

	m_topLeftSquare2D.get()->FreeUploadBuffer();
	RSRBasicShapes::Get().FreeUploadBuffers();
#if DEBUG_PHYSIC
	RSRPhysicManager::Get().FreeUploadBuffers();
#endif
	_RF_FALSE(m_playerPawn->FreeUploadBuffers());
	_RF_FALSE(m_trenchManager->FreeUploadBuffers());

    FreeSOUploadBuffers();

    std::shared_ptr<RSRTexture2D> greebleTexture;
    {
        RSRComputeGreebleTexture greebleTask(512, 512, 32, 4, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
        greebleTask.CreateResources(DXContext::Get().GetDevice().Get());
        auto list = DXContext::Get().InitRenderCommandList();
        greebleTask.Compute(list);
        DXContext::Get().ExecuteRenderCommandList();
        list = nullptr;
        greebleTexture = greebleTask.ResultBufferToTexture2D();
    }


	//Textures
    /*01*/ static const std::string space_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Textures")) / TEXT("space_512_512_BGRA_32BPP.png")).string();
    /*01*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(space_512_512_BGRA_32BPP, false));

    /*02*/ static const std::string sun_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Textures")) / TEXT("sun_512_512_BGRA_32BPP.png")).string();
    /*02*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(sun_512_512_BGRA_32BPP, false));
    
    /*03*/ static const std::string GreebleTexture_Simple_00 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_00.png")).string();
	/*03*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_00, false));

    /*04*/ static const std::string GreebleTexture_Simple_01 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_01.png")).string();
	/*04*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_01, false));

    /*05*/ static const std::string GreebleTexture_Simple_02 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_02.png")).string();
	/*05*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_02, false));

    /*06*/ static const std::string GreebleTexture_Outline_00 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_00.png")).string();
	/*06*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_00, false));

    /*07*/ static const std::string GreebleTexture_Outline_01 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_01.png")).string();
	/*07*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_01, false));

    /*08*/ static const std::string GreebleTexture_Outline_02 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_02.png")).string();
    /*08*/ m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_02, false));

    /*00*/ static const std::string target_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Sprites")) / TEXT("target_512_512_BGRA_32BPP.png")).string();
    /*00*/ m_2dTextures.push_back(pAssetManager->AddTextureAsset(target_512_512_BGRA_32BPP, false));

    /*01*/ static const std::string heart_custom_0 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_custom_0.png")).string();
	/*01*/ m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_custom_0, false));

    /*02*/ static const std::string heart_custom_1 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_custom_1.png")).string();
	/*02*/ m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_custom_1, false));

	//Copy CPU Resource --> GPU Resource
	auto* allocationCmdList = DXContext::Get().InitRenderCommandList();

	RSRTexture2D::UploadResources(m_3dTextures, DXContext::Get().GetDevice().Get(), allocationCmdList);

	RSRTexture2D::UploadResources(m_2dTextures, DXContext::Get().GetDevice().Get(), allocationCmdList);

	//EyeTexture->UploadResources(DXContext::Get().GetDevice().Get(), allocationCmdList, srvheap3D);

	//Execute Upload resources CommandList.
	DXContext::Get().ExecuteRenderCommandList();

    m_3dTextures.insert(m_3dTextures.begin(), greebleTexture);
    RSRTexture2D::CreateSRVHeapForTextures(m_3dTextures, DXContext::Get().GetDevice().Get(), m_srvheap3D);
    RSRTexture2D::CreateSRVHeapForTextures(m_2dTextures, DXContext::Get().GetDevice().Get(), m_srvheap2D);

	allocationCmdList = nullptr;

	return  bAllSucessfull;
}

bool RSRTTScene::UnLoad()
{
	bool bAllSucessfull = true;

	m_srvheap2D.Reset();
	m_srvheap3D.Reset();


	for (size_t i = 0; i < m_3dTextures.size(); ++i)
	{
		bAllSucessfull = m_3dTextures[i]->FreeResourcesBuffer() && bAllSucessfull;
	}
	for (size_t i = 0; i < m_2dTextures.size(); ++i)
	{
		bAllSucessfull = m_2dTextures[i]->FreeResourcesBuffer() && bAllSucessfull;
	}

    if (m_playerPawn->GetHasBeenRegistered())
    {
        RSRPhysicManager::Get().RemovePhysicalEntity(m_playerPawn->GetEditKey());
    }

	bool correctlyFreed = m_playerPawn->FreeResourceBuffers();
	correctlyFreed = m_topLeftSquare2D.get()->FreeResourcesBuffer() && correctlyFreed;
	correctlyFreed = m_trenchManager->FreeResourceBuffers() && correctlyFreed;

	m_playerPawn.reset();
	m_topLeftSquare2D.reset();
    m_drawableLightSource.reset();
	RSRAssetManager::Get().RemoveAsset(TEXT("TopLeftSquare2D"));

	m_gameManager->ShutdownGame();

    m_bFreeSOResourceOnUnload = true;
	return Super::UnLoad() && bAllSucessfull;
}

void pukeColor(float* color)
{
    static int pukeState = 0;
    color[pukeState] += 0.01f;
    if (color[pukeState] > 1.0f)
    {
        pukeState++;
        if (pukeState == 3)
        {
            color[0] = 0.0f;
            color[1] = 0.0f;
            color[2] = 0.0f;
            pukeState = 0;
        }
    }
}

bool RSRTTScene::Render(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = Super::Render(InGameTime, InDeltaTime);

    std::shared_ptr<const RSRTTGameManager> ttGameManager = std::static_pointer_cast<const RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    const RSRCameraData* CameraData = playerController->GetCameraData();
    if (!CameraData)
    {
        RSRLog::LogError(TEXT("Error no pawn or camera from player Controller !"));
        return 0;
    }

#pragma region Init Matrixes

    float diffuseStrength = 0.15f;
    float anbiantStrength = 0.35f;
    float specularStrength = 5.5f;
    anbiantStrength = (1.f / diffuseStrength) * anbiantStrength;
    MVP_DL_Consts matrixes
    {
        .ViewProjMat = DirectX::XMFLOAT4X4(),
        .ModMat = DirectX::XMFLOAT4X4(),
        .invProjModMat = DirectX::XMFLOAT4X4(),
        .CamPos = DirectX::XMFLOAT4()
        //.lightPos_AmbLight    //Handled by SetLightDatas
        //.lightCol_SpecLight   //Handled by SetLightDatas
    };
    
    matrixes.SetLightDatas
    (
        /*Color*/ DirectX::XMFLOAT3{ 1.f, 1.f, 0.6f },
        /*DiffStr*/ 0.15f, /*AmbStr*/ 0.35f, /*SpecStr*/ 0.2f,
        /*LightPos*/ /*mds::unoptimized_add3(*/m_drawableLightSource.lock()->GetTrsMat().GetTransform().Position/*, DirectX::XMFLOAT3{-150.f, 0.f, 0.f})*/
    );

    DirectX::XMMATRIX CameraViewMatrix = CameraData->ComputeView();
    DirectX::XMMATRIX CameraProjectionMatrix = CameraData->ComputeProjection();
    DirectX::XMMATRIX CameraViewProjectionMatrix = DirectX::XMMatrixMultiply(CameraViewMatrix, CameraProjectionMatrix);

    DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);

    //Camera position for phong
    matrixes.CamPos = DirectX::XMFLOAT4(CameraData->Position.x, CameraData->Position.y, CameraData->Position.z, 1.0f);


#pragma endregion Init Matrixes

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

    //----------- Draw3D ------------//

    //Setup
    cmdList->SetPipelineState(m_programInstance->GetPSO3D().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSig3D().Get());
    cmdList->SetDescriptorHeaps(1, m_srvheap3D.GetAddressOf());

    cmdList->SetGraphicsRootDescriptorTable(1, m_srvheap3D->GetGPUDescriptorHandleForHeapStart());

    cmdList->SetGraphicsRoot32BitConstants(0, MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    //Draw
    m_trenchManager->DrawTrench(matrixes.CamPos, cmdList);
    m_playerPawn->DrawGeometry(cmdList);

    DrawSOMeshs(cmdList);
    //m_drawableLightSource.lock()->DrawGeometry(cmdList);

    //----------- DrawInstanced ------------//
    // 
    //Setup
    cmdList->SetPipelineState(m_programInstance->GetPSO3DInstanced().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSig3DInstanced().Get());
    //cmdList->SetDescriptorHeaps(1, m_srvheap3D.GetAddressOf()); //already set
    cmdList->SetGraphicsRootDescriptorTable(1, m_srvheap3D->GetGPUDescriptorHandleForHeapStart());

    RSRush::MVP_DL_ConstsInstanced matrixesInst
    {
        .ViewProjMat = matrixes.ViewProjMat,
        .CamPos = matrixes.CamPos,
        .lightPos_AmbLight = matrixes.lightPos_AmbLight,
        .lightCol_SpecLight = matrixes.lightCol_SpecLight
    };
    cmdList->SetGraphicsRoot32BitConstants(0, MVP_DL_ConstsInstanced::S32B_STRUCT, &matrixesInst, 0);


    //Draw
    m_instancedMeshes.DrawInstances(cmdList);

#if DEBUG_PHYSIC
    //----------- DrawDebugPhysic ------------//

    //Setup
    cmdList->SetPipelineState(m_programInstance->GetPSODebugPhysic().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSigDebugPhysic().Get());
    DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);
    ModelViewprojectionConstants matrixesDebugPhys = matrixes.ToMVC();
    cmdList->SetGraphicsRoot32BitConstants(0, MVPC::S32B_STRUCT, &matrixesDebugPhys, 0);

    //Draw
    RSRPhysicManager::Get().DrawPhysic(cmdList);
#endif

    //----------- Draw2D ------------//
    // 
    //-- ROOT AGRS --

    static float color[] = { 0.0f, 0.0f, 0.0f };
    pukeColor(color);

    struct Correction
    {
        float aspectRatio;
        float zoom;
        float sinAngle;
        float cosAngle;
    };
    static float angle = 0.f;
    angle += 0.01f;
    Correction correction = {
        .aspectRatio = DXWindow::Get().ComputeAspectRatio(),
        .zoom = 0.1f,
        .sinAngle = sinf(angle),
        .cosAngle = cosf(angle)
    };

    DirectX::XMFLOAT2 twoDScale = DXWindow::Get().Compute2DScale();
    UICBV uiCBV
    {
        //Center Extends
        .rect = DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f),
        .ratio = DirectX::XMFLOAT4(twoDScale.x, twoDScale.y, DXWindow::Get().ComputeAspectRatio(), 1.f),
        .color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f)
    };

    //-- PS --
    //Changing SetPipelineState and SetGraphicsRootSignature multiple time per frame is okay, SetDescriptorHeaps is not
    cmdList->SetPipelineState(m_programInstance->GetPSO2D().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSig2D().Get());
    //RSRLog::LogWarning(TEXT("SetDescriptorHeaps is a costly operation, can't set 1 for 3D & 1 for 2D, need to be done only once a frame !"));
    cmdList->SetDescriptorHeaps(1, m_srvheap2D.GetAddressOf());
    //a draw

    cmdList->SetGraphicsRoot32BitConstants(1, 4, &correction, 0);
    cmdList->SetGraphicsRoot32BitConstants(0, 12, &uiCBV, 0);
    cmdList->SetGraphicsRootDescriptorTable(2, m_srvheap2D->GetGPUDescriptorHandleForHeapStart());

    // -- IA --
    //TriangleMesh.get()->DrawMesh(cmdList);
    //TriangleMesh2.get()->DrawMesh(cmdList);
    //RSRBasicShapes::Get().GetDefPlane2D().get()->DrawMesh(cmdList);

    //TopLeftSquare2D.get()->DrawMesh(cmdList);

    m_playerPawn->DrawUIs(cmdList);

    //Finish Drawing



    //(Currently presented frame goes to PresentState)
    DXWindow::Get().EndFrame(cmdList);

    //Force Sync the renderer as ExecuteCommandList Wait for the GPU end of execution before returning
    //This is for simplicity, but of course deafeat the purpuse of DX12 async approch
    DXContext::Get().ExecuteRenderCommandList();

    //Swap the chain (present result)
    DXWindow::Get().Present();


    return bAllSucessfull;
}

bool RSRTTScene::PrePassTick(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSRScene::PrePassTick(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    ttGameManager->PrePassTick(InGameTime, InDeltaTime);
    return bAllSucessfull;
}

bool RSRTTScene::LateTickSync(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSRScene::LateTickSync(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    UINT64 UploadFence = DXContext::Get().UpdateAsyncUploadCommandQueueState(InDeltaTime);

    ttGameManager->LateTickSync(InGameTime, InDeltaTime);

    double CurrentProgression = ttGameManager->GetPrePassProgression();
    m_trenchManager->UpdateTrench(InDeltaTime, CurrentProgression, UploadFence);

    return bAllSucessfull;
}

RSRTTScene::~RSRTTScene()
{
    m_trenchManager.reset();
}