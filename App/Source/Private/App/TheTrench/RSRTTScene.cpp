#include "App/TheTrench/RSRTTScene.h"

#include "MDS/Tools/RMath.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include "App/Data/Textures/RSRTextureLibrary.h"
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


}

bool RSRTTScene::Load()
{
	bool bAllSucessfull = Super::Load();

    m_3dTextures = RSRTextureLibrary::Create(this);
    m_2dTextures = RSRTextureLibrary::Create(this);

    {
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

        m_trenchManager = std::make_shared<RSRTrenchManager>(this, std::move(trenchSideGreebleTextureIDs), std::move(trenchTopGreebleTextureIDs));
        m_trenchManager->InitMemNode(m_WPtrSelfNode.lock(), m_trenchManager);
    }

	RSRAssetManager* pAssetManager = RSRAssetManager::Get(this);

    //Declaring In Order Textures

    DXContext* context = DXContext::Get(this);
    std::shared_ptr<RSRTexture2D> noiseTexture;
    {
        RSRComputeGreebleTexture greebleTask(512, 512, 32, 4, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
        greebleTask.CreateResources(context->GetDevice().Get());
        auto list = context->InitRenderCommandList();
        greebleTask.Compute(list);
        context->ExecuteRenderCommandList();
        list = nullptr;
        noiseTexture = greebleTask.ResultBufferToTexture2D();
    }

    std::string greeblePath = TEXT("NoiseTexture.gpu");
    m_3dTextures->AddGetDynamicByPath(greeblePath, std::move(noiseTexture));

    //Textures
    /*01*/ static const std::string space_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Textures")) / TEXT("space_512_512_BGRA_32BPP.png")).string();
    /*01*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(space_512_512_BGRA_32BPP, false));
    m_3dTextures->AddGetByPath(space_512_512_BGRA_32BPP);

    /*02*/ static const std::string sun_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Textures")) / TEXT("sun_512_512_BGRA_32BPP.png")).string();
    /*02*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(sun_512_512_BGRA_32BPP, false));
    m_3dTextures->AddGetByPath(sun_512_512_BGRA_32BPP);

    /*03*/ static const std::string GreebleTexture_Simple_00 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_00.png")).string();
    /*03*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_00, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Simple_00);

    /*04*/ static const std::string GreebleTexture_Simple_01 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_01.png")).string();
    /*04*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_01, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Simple_01);

    /*05*/ static const std::string GreebleTexture_Simple_02 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Simple_02.png")).string();
    /*05*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Simple_02, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Simple_02);

    /*06*/ static const std::string GreebleTexture_Outline_00 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_00.png")).string();
    /*06*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_00, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Outline_00);

    /*07*/ static const std::string GreebleTexture_Outline_01 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_01.png")).string();
    /*07*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_01, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Outline_01);

    /*08*/ static const std::string GreebleTexture_Outline_02 = (std::filesystem::path(TEXT("Textures")) / TEXT("GreebleTexture_Outline_02.png")).string();
    /*08*/ //m_3dTextures.push_back(pAssetManager->AddTextureAsset(GreebleTexture_Outline_02, false));
    m_3dTextures->AddGetByPath(GreebleTexture_Outline_02);

    /*00*/ static const std::string target_512_512_BGRA_32BPP = (std::filesystem::path(TEXT("Sprites")) / TEXT("target_512_512_BGRA_32BPP.png")).string();
    /*00*/ //m_2dTextures.push_back(pAssetManager->AddTextureAsset(target_512_512_BGRA_32BPP, false));
    m_2dTextures->AddGetByPath(target_512_512_BGRA_32BPP);

    /*01*/ static const std::string heart_custom_0 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_custom_0.png")).string();
    /*01*/ //m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_custom_0, false));
    m_2dTextures->AddGetByPath(heart_custom_0);

    /*02*/ static const std::string heart_custom_1 = (std::filesystem::path(TEXT("Sprites")) / TEXT("heart_custom_1.png")).string();
    /*02*/ //m_2dTextures.push_back(pAssetManager->AddTextureAsset(heart_custom_1, false));
    m_2dTextures->AddGetByPath(heart_custom_1);

    //End declaring In Order Textures


	m_gameManager = std::make_shared<RSRTTGameManager>();

    m_playerPawn = AddNewSObject<RSRTT404Pawn>(std::make_shared<RSRTT404Pawn>());
    //std::shared_ptr<RSRush::RSRTT404Pawn> playerPawn = m_playerPawn.lock();
    //playerPawn->SetSelfReference(playerPawn);
    //playerPawn->GenerateMesh();


    RSRPhysicManager* physicManager = RSRPhysicManager::Get(this);
    DXWindow* window = DXWindow::Get(this);
    RSRBasicShapes* basicShapes = RSRBasicShapes::Get(this);

	m_gameManager->InitializeGame(LockSelf<RSRTTScene>(), m_gameManager);

	if (pAssetManager->Exist(TEXT("TopLeftSquare2D")))
	{
		m_topLeftSquare2D = pAssetManager->GetShared<RSRMesh2D>(TEXT("TopLeftSquare2D"));
	}
	else
	{
		m_topLeftSquare2D = basicShapes->GetRegisterNewPlane2D
		(TEXT("TopLeftSquare2D"),
			DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f),
			DirectX::XMFLOAT2(-1.f, -1.f),
			DirectX::XMFLOAT2(0.f, 0.f),
			DirectX::XMFLOAT2(1.f, 1.f));
	}
    AddNewSObject(std::make_shared<RSRODrawableBackground>
        (
            RSRBasicShapes::Get(this),
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
            RSRBasicShapes::Get(this),
            RSRTransform 
            {
                .Position = DirectX::XMFLOAT3{ 7500.0f, 1500.0f, 2500.0f },
                .Rotation = DirectX::XMFLOAT3(0.f, 0.f, 0.f),
                .Scale = DirectX::XMFLOAT3(100.f, 100.f, 100.f),
            },
            2
        ));

	//Copy CPU Resource --> GPU Resource
	auto* verticesCmdList = context->InitRenderCommandList();


	_RF_FALSE(m_topLeftSquare2D.get()->UploadResources(context->GetDevice().Get(), verticesCmdList));

	_RF_FALSE(basicShapes->UploadResources(context->GetDevice().Get(), verticesCmdList));
#if DEBUG_PHYSIC
	_RF_FALSE(physicManager->UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#endif
    //Now SOResource
	//if (!playerPawn->UploadResources(context->GetDevice().Get(), verticesCmdList))
	//{
	//	RSRLog::LogWarning(TEXT("Fail Upload"));
	//}
    // 
	//must be after InitializeGame or move 'RSRTrenchManager::BeginNewTrench' Here
	if (!m_trenchManager->UploadBeingNewResources(context->GetDevice().Get(), verticesCmdList))
	{
		RSRLog::LogWarning(TEXT("TODO : Implement Start Trench special begin and upload prcedure !"));
	}

    UploadSOResources(context->GetDevice().Get(), verticesCmdList);

	context->ExecuteRenderCommandList();
	verticesCmdList = nullptr;

	m_topLeftSquare2D.get()->FreeUploadBuffer();
	basicShapes->FreeUploadBuffers();
#if DEBUG_PHYSIC
	physicManager->FreeUploadBuffers();
#endif
    //Now SOResource
	//_RF_FALSE(playerPawn->FreeUploadBuffers());
	_RF_FALSE(m_trenchManager->FreeUploadBuffers());

    FreeSOUploadBuffers();


	//Copy CPU Resource --> GPU Resource
	auto* allocationCmdList = context->InitRenderCommandList();

    bAllSucessfull = m_3dTextures->UploadResources(context->GetDevice().Get(), allocationCmdList) && bAllSucessfull;
    bAllSucessfull = m_2dTextures->UploadResources(context->GetDevice().Get(), allocationCmdList) && bAllSucessfull;


	//Execute Upload resources CommandList.
    context->ExecuteRenderCommandList();
	allocationCmdList = nullptr;


    m_3dTextures->CreateSRVHeapForTextures(context->GetDevice().Get(), m_srvheap3D);
    m_2dTextures->CreateSRVHeapForTextures(context->GetDevice().Get(), m_srvheap2D);

	return  bAllSucessfull;
}

bool RSRTTScene::UnLoad()
{
	bool bAllSucessfull = true;

	m_srvheap2D.Reset();
	m_srvheap3D.Reset();

    bAllSucessfull = m_3dTextures->UnLoad() && bAllSucessfull;
    bAllSucessfull = m_2dTextures->UnLoad() && bAllSucessfull;

    m_3dTextures.reset();
    m_2dTextures.reset();
    //if (m_playerPawn->GetHasBeenRegistered())
    //{
    //    RSRPhysicManager::Get().RemovePhysicalEntity(m_playerPawn->GetEditKey());
    //}

	//bool correctlyFreed = m_playerPawn->FreeResourceBuffers();
    bool correctlyFreed = m_topLeftSquare2D.get()->FreeResourcesBuffer();
	correctlyFreed = m_trenchManager->FreeResourceBuffers() && correctlyFreed;

	m_playerPawn.reset();
	m_topLeftSquare2D.reset();
    m_drawableLightSource.reset();
	RSRAssetManager::Get(this)->RemoveAsset(TEXT("TopLeftSquare2D"));

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

    DXContext* context = DXContext::Get(this);
    DXWindow* window = DXWindow::Get(this);
    RSRProgramInstance* programInstance = GetRoot<RSRProgramInstance>();
    RSRTT404Pawn* playerPawn = m_playerPawn.lock().get();
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
    auto* cmdList = context->InitRenderCommandList();
    //Draw to window (Currently presented frame goes back to writing state)
    window->BeginFrame(cmdList);

    // -- RS --
    D3D12_VIEWPORT vp =
    {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = (FLOAT)window->GetWidth(),     //narrow conversion
        .Height = (FLOAT)window->GetHeight(),   //narrow conversion
        .MinDepth = 1.f,
        .MaxDepth = 0.f
    };
    cmdList->RSSetViewports(1, &vp);
    D3D12_RECT scRect = {
        .left = 0,
        .top = 0,
        .right = (LONG)window->GetWidth(),   //narrow conversion
        .bottom = (LONG)window->GetHeight()   //narrow conversion
    };

    cmdList->RSSetScissorRects(1, &scRect);

    //----------- Draw3D ------------//

    //Setup
    cmdList->SetPipelineState(programInstance->GetPSO3D().Get());
    cmdList->SetGraphicsRootSignature(programInstance->GetRootSig3D().Get());
    cmdList->SetDescriptorHeaps(1, m_srvheap3D.GetAddressOf());

    cmdList->SetGraphicsRootDescriptorTable(1, m_srvheap3D->GetGPUDescriptorHandleForHeapStart());

    cmdList->SetGraphicsRoot32BitConstants(0, MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    //Draw
    m_trenchManager->DrawTrench(matrixes.CamPos, cmdList);
    //m_playerPawn->DrawGeometry(cmdList);

    DrawSOMeshs(cmdList);
    //m_drawableLightSource.lock()->DrawGeometry(cmdList);

    //----------- DrawInstanced ------------//
    // 
    //Setup
    cmdList->SetPipelineState(programInstance->GetPSO3DInstanced().Get());
    cmdList->SetGraphicsRootSignature(programInstance->GetRootSig3DInstanced().Get());
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
        .aspectRatio = window->ComputeAspectRatio(),
        .zoom = 0.1f,
        .sinAngle = sinf(angle),
        .cosAngle = cosf(angle)
    };

    DirectX::XMFLOAT2 twoDScale = window->Compute2DScale();
    UICBV uiCBV
    {
        //Center Extends
        .rect = DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f),
        .ratio = DirectX::XMFLOAT4(twoDScale.x, twoDScale.y, window->ComputeAspectRatio(), 1.f),
        .color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f)
    };

    //-- PS --
    //Changing SetPipelineState and SetGraphicsRootSignature multiple time per frame is okay, SetDescriptorHeaps is not
    cmdList->SetPipelineState(programInstance->GetPSO2D().Get());
    cmdList->SetGraphicsRootSignature(programInstance->GetRootSig2D().Get());
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

    playerPawn->DrawUIs(cmdList);

    //Finish Drawing



    //(Currently presented frame goes to PresentState)
    window->EndFrame(cmdList);

    //Force Sync the renderer as ExecuteCommandList Wait for the GPU end of execution before returning
    //This is for simplicity, but of course deafeat the purpuse of DX12 async approch
    context->ExecuteRenderCommandList();

    //Swap the chain (present result)
    window->Present();


    return bAllSucessfull;
}

bool RSRTTScene::PrePassTick(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSROScene::PrePassTick(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    ttGameManager->PrePassTick(InGameTime, InDeltaTime);
    return bAllSucessfull;
}

bool RSRTTScene::LateTickSync(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSROScene::LateTickSync(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    UINT64 UploadFence = DXContext::Get(this)->UpdateAsyncUploadCommandQueueState(InDeltaTime);

    ttGameManager->LateTickSync(InGameTime, InDeltaTime);

    double CurrentProgression = ttGameManager->GetPrePassProgression();
    m_trenchManager->UpdateTrench(InDeltaTime, CurrentProgression, UploadFence);

    return bAllSucessfull;
}

RSRTTScene::~RSRTTScene()
{
    m_trenchManager.reset();
}