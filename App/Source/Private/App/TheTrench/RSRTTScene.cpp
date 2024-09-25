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
#include <App/Data/Textures/RSRTexture2D.h>
#include "App/Tools/Window.h"
#include <Tracy.hpp>

using namespace RSRush;

RSRTTScene::RSRTTScene(Private)
	: RSRScene()
{
    m_physicContext = PhysicContext::DEFAULT_MOON;
}

bool RSRush::RSRTTScene::Load()
{
	bool bAllSucessfull = RSRScene::Load();
	RSRAssetManager* pAssetManager = &RSRAssetManager::Get();
	m_gameManager = std::make_shared<RSRTTGameManager>();

	m_playerPawn = std::make_shared<RSRTT404Pawn>();
    m_playerPawn->SetSelfReference(m_playerPawn);
	m_playerPawn->GenerateMesh();
	RSRush::RSRPhysicManager::Get().AddPhysicalEntity(m_playerPawn->GeneratePhysicBody());


	m_gameManager->InitializeGame(this->m_thisWPtr);

	if (pAssetManager->Exist(TEXT("TopLeftSquare2D")))
	{
		m_topLeftSquare2D = pAssetManager->GetShared<RSRMesh2D>(TEXT("TopLeftSquare2D"));
	}
	else
	{
		m_topLeftSquare2D = RSRush::RSRBasicShapes::Get().GetRegisterNewPlane2D
		(TEXT("TopLeftSquare2D"),
			DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f),
			DirectX::XMFLOAT2(-1.f, -1.f),
			DirectX::XMFLOAT2(0.f, 0.f),
			DirectX::XMFLOAT2(1.f, 1.f));
	}

	//Copy CPU Resource --> GPU Resource
	auto* verticesCmdList = DXContext::Get().InitRenderCommandList();


	_RF_FALSE(m_topLeftSquare2D.get()->UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));

	_RF_FALSE(RSRush::RSRBasicShapes::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#if DEBUG_PHYSIC
	_RF_FALSE(RSRush::RSRPhysicManager::Get().UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList));
#endif
	if (!m_playerPawn->UploadResources(DXContext::Get().GetDevice().Get(), verticesCmdList))
	{
		RSRLog::LogWarning(TEXT("Fail Upload"));
	}
	//must be after InitializeGame or move 'RSRTrenchManager::BeginNewTrench' Here
	if (!RSRush::RSRTrenchManager::Get().UploadBeingNewResources(DXContext::Get().GetDevice().Get(), verticesCmdList))
	{
		RSRLog::LogWarning(TEXT("TODO : Implement Start Trench special begin and upload prcedure !"));
	}
	DXContext::Get().ExecuteRenderCommandList();
	verticesCmdList = nullptr;

	m_topLeftSquare2D.get()->FreeUploadBuffer();
	RSRush::RSRBasicShapes::Get().FreeUploadBuffers();
#if DEBUG_PHYSIC
	RSRush::RSRPhysicManager::Get().FreeUploadBuffers();
#endif
	_RF_FALSE(m_playerPawn->FreeUploadBuffers());
	_RF_FALSE(RSRush::RSRTrenchManager::Get().FreeUploadBuffers());


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

bool RSRush::RSRTTScene::UnLoad()
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
        RSRush::RSRPhysicManager::Get().RemovePhysicalEntity(m_playerPawn->GetEditKey());
    }

	bool correctlyFreed = m_playerPawn->FreeResourceBuffers();
	correctlyFreed = m_topLeftSquare2D.get()->FreeResourcesBuffer() && correctlyFreed;
	correctlyFreed = RSRush::RSRTrenchManager::Get().FreeResourceBuffers() && correctlyFreed;

	m_playerPawn.reset();
	m_topLeftSquare2D.reset();
	RSRAssetManager::Get().RemoveAsset(TEXT("TopLeftSquare2D"));

	m_gameManager->ShutdownGame();

	return RSRScene::UnLoad() && bAllSucessfull;
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

bool RSRush::RSRTTScene::Render(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSRScene::Render(InGameTime, InDeltaTime);

    std::shared_ptr<const RSRTTGameManager> ttGameManager = std::static_pointer_cast<const RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    RSRush::MVP_DL_Consts matrixes
    {
        .ViewProjMat = DirectX::XMFLOAT4X4(),
        .ModMat = DirectX::XMFLOAT4X4(),
        .invProjModMat = DirectX::XMFLOAT4X4(),
        .CamPos = DirectX::XMFLOAT4(),
        .lightPos_AmbLight = DirectX::XMFLOAT4 { 50.0f, 30.0f, 40.0f                       ,/*AmbLight */ 0.1f },
        .lightCol_SpecLight = DirectX::XMFLOAT4 { 1.0f * 0.25f, 1.0f * 0.25f, 0.6f * 0.25f  ,/*SpecLight*/ 5.5f }
    };

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

    //myPC->GetCameraData().FOV = vp.Width / vp.Height;
    cmdList->RSSetScissorRects(1, &scRect);

    //static float CameraAngle = 0;
    //CameraAngle += 0.5f * (float)(LastDeltaTimeSecond * (2 * mds::RMath::PI));

    //----------- Draw3D ------------//
    // 
    //std::cout << "X : " << CameraData.Position.x << ", Y : " << CameraData.Position.y << ", Z : " << CameraData.Position.z <<  std::endl;

    const RSRush::RSRCameraData* CameraData = playerController->GetCameraData();
    if (!CameraData)
    {
        RSRLog::LogError(TEXT("Error no pawn or camera from player Controller !"));
        return 0;
    }
    DirectX::XMMATRIX CameraViewMatrix = CameraData->ComputeView();
    DirectX::XMMATRIX CameraProjectionMatrix = CameraData->ComputeProjection();
    DirectX::XMMATRIX CameraViewProjectionMatrix = DirectX::XMMatrixMultiply(CameraViewMatrix, CameraProjectionMatrix);

    static double lastLog = 0.;
    if (false && lastLog > 1.)
    {

        std::cout
            << "Position : { x : " << CameraData->Position.x << ", y : " << CameraData->Position.y << ", z : " << CameraData->Position.z <<
            " } \nTarget : { x : " << CameraData->Target.x << ", y : " << CameraData->Target.y << ", z : " << CameraData->Target.z << " }" <<
            " } \nUp : { x : " << CameraData->Up.x << ", y : " << CameraData->Up.y << ", z : " << CameraData->Up.z << " }" <<
            " } \nFOV : " << CameraData->FOV << ", AspectRatio : " << CameraData->AspectRatio << ", NearClip : " << CameraData->NearClip << ", FarClip : " << CameraData->FarClip <<
            "\nCameraViewMatrix :\n" << mds::RLog::XMMatrixToString(CameraViewMatrix) <<
            "\nCameraProjectionMatrix :\n" << mds::RLog::XMMatrixToString(CameraProjectionMatrix) <<
            "\nCameraViewProjectionMatrix :\n" << mds::RLog::XMMatrixToString(CameraViewProjectionMatrix) <<
            std::endl;
        lastLog = 0.;
    }
    else
    {
        lastLog += InDeltaTime;
    }



    DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);

    //std::cout<<Context3dCBV.viewProjectionMatrix << std::endl;

    //DirectX::XMMATRIX IdentityMatrix = DirectX::XMMatrixIdentity();
    //DirectX::XMStoreFloat4x4(&matrixes.ModMat, IdentityMatrix);
    //-- PS --

    cmdList->SetPipelineState(m_programInstance->GetPSO3D().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSig3D().Get());
    cmdList->SetDescriptorHeaps(1, m_srvheap3D.GetAddressOf());
    //a draw

    //Context b0
    //Mesh b1 depend per mesh


    cmdList->SetGraphicsRootDescriptorTable(1, m_srvheap3D->GetGPUDescriptorHandleForHeapStart());
    //Draw

    DirectX::XMVECTOR vScale = { 2.f, 1.f, 1.f, 1.f };

    DirectX::XMVECTOR vRotation = { DirectX::XMConvertToRadians(0.f),DirectX::XMConvertToRadians(0.f), DirectX::XMConvertToRadians(90.f), 0.f };
    DirectX::XMVECTOR vTranslation = { 10.f, 0.f, 2.f, 1.f };


    // Create the transformation matrix
    DirectX::XMMATRIX mat = DirectX::XMMatrixTransformation(
        DirectX::XMVectorZero(), // scaling origin
        DirectX::XMQuaternionIdentity(), // rotation origin
        vScale, // scale vector
        DirectX::XMVectorZero(), // rotation origin
        DirectX::XMQuaternionRotationRollPitchYawFromVector(vRotation), // rotation quaternion
        vTranslation // translation vector
    );

    DirectX::XMStoreFloat4x4(&matrixes.ModMat, mat);

    //Camera position for phong
    matrixes.CamPos = DirectX::XMFLOAT4(CameraData->Position.x, CameraData->Position.y, CameraData->Position.z, 1.0f);
    cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    DirectX::XMVECTOR ldedCameraPos = DirectX::XMLoadFloat4(&matrixes.CamPos);

    //RSRush::RSRBasicShapes::Get().GetDefSquare().get()->DrawMesh(cmdList);

    matrixes.ModMat =
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        -0.5f, 0.f, -4.f, 1.f,
    };
    //cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    //RSRush::RSRBasicShapes::Get().GetDefPlane().get()->DrawMesh(cmdList);

    matrixes.ModMat =
    {
        10.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 10.f, 0.f,
        0.f, 10.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
    //Context b0
    //cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    //RSRush::RSRBasicShapes::Get().GetDefPlane().get()->DrawMesh(cmdList);

    matrixes.ModMat =
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
    //texture t0
    //cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);
    //RSRush::RSRBasicShapes::Get().GetDefPlane().get()->DrawMesh(cmdList);

    matrixes.ModMat =
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.5f, 1.f, 1.f, 1.f,
    };
    //cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVP_DL_Consts::S32B_STRUCT, &matrixes, 0);

    //RSRush::RSRBasicShapes::Get().GetDefPlane().get()->DrawMesh(cmdList);

    RSRush::RSRTrenchManager::Get().DrawTrench(ldedCameraPos, cmdList);
    m_playerPawn->DrawGeometry(cmdList);

#if DEBUG_PHYSIC
    //----------- DrawDebugPhysic ------------//
    cmdList->SetPipelineState(m_programInstance->GetPSODebugPhysic().Get());
    cmdList->SetGraphicsRootSignature(m_programInstance->GetRootSigDebugPhysic().Get());
    DirectX::XMStoreFloat4x4(&matrixes.ViewProjMat, CameraViewProjectionMatrix);
    ModelViewprojectionConstants matrixesDebugPhys = matrixes.ToMVC();
    cmdList->SetGraphicsRoot32BitConstants(0, RSRush::MVPC::S32B_STRUCT, &matrixesDebugPhys, 0);
    RSRush::RSRPhysicManager::Get().DrawPhysic(cmdList);
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
    RSRush::UICBV uiCBV
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
    //RSRush::RSRBasicShapes::Get().GetDefPlane2D().get()->DrawMesh(cmdList);

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

bool RSRush::RSRTTScene::PrePassTick(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSRScene::PrePassTick(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    ttGameManager->PrePassTick(InGameTime, InDeltaTime);
    return bAllSucessfull;
}

bool RSRush::RSRTTScene::LateTickSync(const double InGameTime, const double InDeltaTime)
{
    bool bAllSucessfull = RSRScene::LateTickSync(InGameTime, InDeltaTime);

    std::shared_ptr<RSRTTGameManager> ttGameManager = std::static_pointer_cast<RSRTTGameManager>(m_gameManager);
    auto playerController = ttGameManager->GetPlayerController(0);

    UINT64 UploadFence = DXContext::Get().UpdateUploadCommandQueueState(InDeltaTime);

    ttGameManager->LateTickSync(InGameTime, InDeltaTime);

    double CurrentProgression = ttGameManager->GetPrePassProgression();
    RSRush::RSRTrenchManager::Get().UpdateTrench(InDeltaTime, CurrentProgression, UploadFence);

    return bAllSucessfull;
}

RSRTTScene::~RSRTTScene()
{
}