#include "App/TheTrench/Gameplay/RSRTT404Pawn.h"
#include "MDS/Tools/RMath.h"
#include "mds/Tools/System/RTimekeeper.h"

#include "App/Geometry/RSRBasicShapes.h"
#include "App/Managers/RSRAssetManager.h"
#include "App/Managers/RSRPhysicManager.h"

#include <format>
#include <algorithm>
#include <utility>
#include <DirectXMath.h>


#include <string>
//#include <string_view>
#include <format>
#include <App/Tools/RSRLog.h>
#include <Tracy.hpp>
#include "App/TheTrench/RSRTTScene.h"

#define DEBUG_FREECAM 0

using namespace RSRush;
using namespace DirectX;

XMVECTOR RSRTT404Pawn::SCREEN_MOUVEMENT_SPEED = { 0.f, 10.f, 10.f, 0.f };
#if DEBUG_FREECAM
float RSRTT404Pawn::BASE_FORWARD_MOVEMENT_SPEED = 0.f;
float RSRTT404Pawn::BOOST_MOVEMENT_SPEED = 3.f;
float RSRTT404Pawn::BRAKE_MOVEMENT_SPEED = 3.f; //Debug Freecam directions are signed
#else
float RSRTT404Pawn::BASE_FORWARD_MOVEMENT_SPEED = 5.f;
float RSRTT404Pawn::BOOST_MOVEMENT_SPEED = 3.f;
float RSRTT404Pawn::BRAKE_MOVEMENT_SPEED = -2.5f;
#endif

static constexpr float POINTER_SIZE = 0.1f;
static constexpr float POINTER_WIDTH = POINTER_SIZE;
static constexpr float POINTER_HEIGHT = POINTER_SIZE;

static XMVECTOR sm_baseMouvementSpeed;
static XMVECTOR sm_BoostMouvementSpeed;
static XMVECTOR sm_brakeMouvementSpeed;


static constexpr float FRONT_FORWARD = 1.5f;
static constexpr float BACK_FORWARD = -1.f;
static constexpr float FORWARD_LENGTH = FRONT_FORWARD - BACK_FORWARD;

static constexpr float BASE_WIDTH = 1.5f;
static constexpr float BASE_HALF_WIDTH = BASE_WIDTH * 0.5f;


static constexpr float PEAK_HEIGHT = 0.f;
static constexpr float TOP_HEIGHT = 0.45f;
static constexpr float BOTTOM_HEIGHT = -0.25f;
static constexpr float TOTAL_HEIGHT = TOP_HEIGHT - BOTTOM_HEIGHT;


const XMVECTOR& GetMovementSpeed(const XMFLOAT3& InMouvementDirection)
{
#if DEBUG_FREECAM

#define MUL_NORMAL_IF_DEBUG_FREECAM

	XMVECTOR loadedMovementDirection = XMLoadFloat3(&InMouvementDirection);
	float FowardSpeed = XMVectorGetX(XMVector3Length(loadedMovementDirection));
#else

#define MUL_NORMAL_IF_DEBUG_FREECAM 

	float FowardSpeed = InMouvementDirection.x;
#endif
	if (abs(FowardSpeed) < FLT_EPSILON)
	{
		return sm_baseMouvementSpeed MUL_NORMAL_IF_DEBUG_FREECAM;
	}
	else if (FowardSpeed > 0)
	{
		return sm_BoostMouvementSpeed MUL_NORMAL_IF_DEBUG_FREECAM;
	}
	else
	{
		return sm_brakeMouvementSpeed MUL_NORMAL_IF_DEBUG_FREECAM;
	}
#undef MUL_NORMAL_IF_DEBUG_FREECAM
}
static constexpr float DEFAULT_SPEED = 10.f;

void RSRTT404Pawn::SetControlData(const ShipControlData& InControlData)
{
	m_controlData = InControlData;
}

bool RSRTT404Pawn::GetInDamageGraceTime() const
{
	return GetInDamageGraceTime(mds::RTimekeeper::GetFromStartSeconds());
}

void RSRTT404Pawn::AttemptDamageNextFrame(uint16_t InDamageAmount)
{
	double timeNow = mds::RTimekeeper::GetFromStartSeconds();
	if (GetIsAlive() && !GetInDamageGraceTime(timeNow))
	{
		m_nextFrameHealth = std::max(m_nextFrameHealth - (int)InDamageAmount, 0);
		m_bInDamageArea = true;
		m_lastInDamageAreaTime = timeNow;
	}
}

void RSRTT404Pawn::OnPawnDeath()
{
	RSRLog::LogDisplay(TEXT("Pawn Death !"));
}

bool RSRTT404Pawn::GetIsDebugFreeCam() const
{
#if DEBUG_FREECAM
	return true;
#else
	return false;
#endif
}

RSRTT404Pawn::RSRTT404Pawn()
	: Pawn(mds::RAssetAuthority::Absolute), RSRIPhysicalEntity(), m_prePhysicNewTransform(RSRush::RSRTransformMatrix(mds::TRS_IDENTITY))
{
	m_prePhysicNewTransform = m_mainTransform;
}

RSRTT404Pawn::~RSRTT404Pawn()
{
}

void RSRTT404Pawn::PrePassRequestActionMovement(const XMFLOAT3& InMouvementDirection, double InDeltaTime)
{
	static bool b_computedSpeeds = false;
	if (!b_computedSpeeds)
	{
		ComputeShipStateSpeed();
		b_computedSpeeds = true;
	}
	XMVECTOR MouvementDirection = XMLoadFloat3(&InMouvementDirection);
	m_frameForwardSpeed = XMVectorGetX(GetMovementSpeed(InMouvementDirection)) * (float)InDeltaTime;

	XMVECTOR loadedMovementDirection = XMLoadFloat3(&InMouvementDirection);
	XMVECTOR loadedMovmtDirNorml = XMVector3Normalize(loadedMovementDirection);
	XMStoreFloat3(&m_frameMouvementInputDirection, loadedMovmtDirNorml);
	m_bMovedFromInput = true;
}


void RSRTT404Pawn::PrePassTargetPosition(const XMFLOAT2& InTargetPosition, double InDeltaTime)
{
	m_nextFramePointerLocation = InTargetPosition;

	bool bAllSucessfull = true;
#if DEBUG_FREECAM
	XMVECTOR rotationQuaternion = XMLoadFloat3(&m_prePhysicNewTransform.GetTransform().Rotation);
	XMVECTOR MouseRot = { 0.f , -m_mouseFrameMvmnt.y, -m_mouseFrameMvmnt.x , 0.f };
	rotationQuaternion = rotationQuaternion + MouseRot;

	rotationQuaternion = XMVectorSetX(rotationQuaternion, 0.f);
	rotationQuaternion = XMVectorSetY(rotationQuaternion, 0.f);
	XMFLOAT3 resultRot;
	XMStoreFloat3(&resultRot, rotationQuaternion);
	m_prePhysicNewTransform.SetRotation(resultRot);


#else
	XMVECTOR rotationQuaternion = mds::RMath::LookAtQuatRad
	(
		XMLoadFloat3(&m_prePhysicNewTransform.GetTransform().Position),
		ComputePointerWorldLocation()
	);

	XMVECTOR EulerDeg = mds::RMath::QuatToEulerDegXMVECTOR(rotationQuaternion);
	XMFLOAT3 EulerDegFLT3;
	XMStoreFloat3(&EulerDegFLT3, EulerDeg);
	m_prePhysicNewTransform.SetRotation(EulerDegFLT3);
#endif

	m_bMovedFromInput = true;
}

void RSRTT404Pawn::SetMouseFrameMouvement(const DirectX::XMFLOAT2& InMouseFrameMouvement, double InDeltaTime)
{
	m_mouseFrameMvmnt = InMouseFrameMouvement;
}

void RSRush::RSRTT404Pawn::OnInputFrameEnded()
{
	Pawn::OnInputFrameEnded();
	if (m_bMovedFromInput)
	{
		XMVECTOR currentLocation = XMLoadFloat3(&m_prePhysicNewTransform.GetTransform().Position);
#if DEBUG_FREECAM
		XMVECTOR forwardVec = XMLoadFloat3(&m_frameMouvementInputDirection);//{ 1.f, 0.f, 0.f, 0.f };
		XMMATRIX trsMat = XMLoadFloat4x4(&m_prePhysicNewTransform.GetMatrix());
		forwardVec = XMVector4Transform(forwardVec, trsMat);
		forwardVec = forwardVec * std::abs(m_frameForwardSpeed);
#else
		XMVECTOR forwardVec = ComputePointerWorldLocation() - currentLocation;
		forwardVec = XMVector3Normalize(forwardVec);
		forwardVec = forwardVec * m_frameForwardSpeed;
#endif
		m_frameForwardSpeed = 0.f;
		m_prePhysicNewTransform.SetPositon(currentLocation + forwardVec);
		m_bMovedFromInput = false;
	}
	XMStoreFloat2(&m_nrlzForwardPointer, ComputePointerForwardLocation());
	this->OnPhysicalObjectRequestMove(m_prePhysicNewTransform);
}

void RSRush::RSRTT404Pawn::OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	Pawn::OnAddedToScene(InThisWPtr, InScene);
	//At this point RSRTT404Pawn is registered memTree element
	std::shared_ptr<RSRTTScene> InScenePtr = std::static_pointer_cast<RSRTTScene>(InScene.lock());
	if (InScenePtr)
	{
		GenerateMesh(InScenePtr->GetTextureLibrary3D());
		if (!InThisWPtr.expired())
		{
			std::shared_ptr<RSRTT404Pawn> _thisPtr = std::dynamic_pointer_cast<RSRTT404Pawn>(InThisWPtr.lock());
			if (_thisPtr)
			{
				this->SetSelfReference(_thisPtr);
				RSRush::RSRPhysicManager::Get(this)->AddPhysicalEntity(this->GeneratePhysicBody());
			}
		}
	}
}

void RSRush::RSRTT404Pawn::OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene)
{
	if (!InThisWPtr.expired())
	{
		std::shared_ptr<RSRTT404Pawn> _thisPtr = std::dynamic_pointer_cast<RSRTT404Pawn>(InThisWPtr.lock());
		if (_thisPtr)
		{
			RSRush::RSRPhysicManager::Get(this)->RemovePhysicalEntity(this->GetEditKey());
		}
	}
	Pawn::OnRemovedFromScene(InThisWPtr, InScene);
}

bool RSRush::RSRTT404Pawn::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	Pawn::LateTickSync(InGameTime, InDeltaTime);
	if (GetKey().bHasBeenRegistered)
	{
		m_mainTransform = this->GetLastResolvedPhysicBody().Transform;
		OnPositionUpdated();
	}
	m_currentPointerLocation = m_nextFramePointerLocation;
	if (m_currentHealth > 0)
	{
		m_currentHealth = m_nextFrameHealth;

		if (m_currentHealth < 1)
		{
			OnPawnDeath();
		}
	}
	return true;
}

void RSRTT404Pawn::OnPositionUpdated()
{
	m_CameraData.Target = m_mainTransform.GetTransform().Position;
#if DEBUG_FREECAM

	XMVECTOR pos = { -1.f /** std::abs(m_currentPointerLocation.y)*/, 0.f,  m_currentPointerLocation.y * 2.f ,  1.f };
	pos = XMVector3Normalize(pos);
	pos = pos * 5.f;
	m_CameraData.Target.z = m_CameraData.Target.z + 1.f;
	XMMATRIX trsMat = XMLoadFloat4x4(&m_mainTransform.GetMatrix());
	pos = XMVector3Transform(pos, trsMat);
	XMStoreFloat3(&m_CameraData.Position, pos);
#else
	m_CameraData.Target.z = m_CameraData.Target.z + 1.5f;
	XMFLOAT3 CameraPos = m_CameraData.Target;
	CameraPos.x = CameraPos.x - 5.f;
	m_CameraData.Position = CameraPos;
#endif
	m_prePhysicNewTransform = m_mainTransform;
}

void RSRTT404Pawn::ComputeShipStateSpeed()
{
#if DEBUG_FREECAM	
	sm_baseMouvementSpeed = (XYZ_VECTOR_MASK * BASE_FORWARD_MOVEMENT_SPEED);
	sm_BoostMouvementSpeed = (XYZ_VECTOR_MASK * (BASE_FORWARD_MOVEMENT_SPEED + BOOST_MOVEMENT_SPEED));
	sm_brakeMouvementSpeed = (XYZ_VECTOR_MASK * (BASE_FORWARD_MOVEMENT_SPEED + BRAKE_MOVEMENT_SPEED));

#else
	//Update speeds, later handle in a specific functions
	sm_baseMouvementSpeed = SCREEN_MOUVEMENT_SPEED + (X_VECTOR_MASK * BASE_FORWARD_MOVEMENT_SPEED);
	sm_BoostMouvementSpeed = SCREEN_MOUVEMENT_SPEED + (X_VECTOR_MASK * (BASE_FORWARD_MOVEMENT_SPEED + BOOST_MOVEMENT_SPEED));
	sm_brakeMouvementSpeed = SCREEN_MOUVEMENT_SPEED + (X_VECTOR_MASK * (BASE_FORWARD_MOVEMENT_SPEED + BRAKE_MOVEMENT_SPEED));
#endif
}

XMVECTOR XM_CALLCONV RSRTT404Pawn::ComputePointerForwardLocation(const float InShipDepth) const
{
	//Not consistant implementation between ComputePointerForwardLocation and ComputePointerWorldLocation
	//Should set a depth based on a 2D plane forward form the ship and use this as distance for depth.

	const RSRCameraData* camera = GetCameraData();
	XMMATRIX cView = camera->ComputeView();
	XMMATRIX cProj = camera->ComputeProjection();

	XMVECTOR shipPosition = XMLoadFloat3(&m_mainTransform.GetTransform().Position);

	//This doesn't take into account current level orientation, camera position should be some kind of local and farward should be the somewhat level forward
	XMVECTOR worldForwardPointerLocation = shipPosition + (X_VECTOR_MASK * InShipDepth);

	XMVECTOR viewSpacePos = XMVector3TransformCoord(worldForwardPointerLocation, cView);

	XMVECTOR clipSpacePos = XMVector3TransformCoord(viewSpacePos, cProj);

	XMVECTOR ndcPos = XMVectorDivide(clipSpacePos, XMVectorSplatW(clipSpacePos));
	return ndcPos;
}


XMVECTOR XM_CALLCONV RSRTT404Pawn::ComputePointerWorldLocation(const float InDepth) const
{
	//Camera may not take PrepassLocation into account, possibly a rework needed
	const RSRCameraData* camera = GetCameraData();

	XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, camera->ComputeView());
	XMMATRIX invProj = DirectX::XMMatrixInverse(nullptr, camera->ComputeProjection());

	//RSRLog::Log(LOG_DISPLAY, TEXT("m_nextFramePointerLocation : {} {}"), m_nextFramePointerLocation.x, m_nextFramePointerLocation.y);
	XMVECTOR rayClip = DirectX::XMVectorSet(m_nextFramePointerLocation.x, -m_nextFramePointerLocation.y, -1.0f, 1.0f);

	XMVECTOR rayView = DirectX::XMVector4Transform(rayClip, invProj);
	rayView = DirectX::XMVectorSetW(rayView, 0.0f); // Reset w for a direction vector

	// Transform the ray to world space
	DirectX::XMVECTOR rayWorld = DirectX::XMVector4Transform(rayView, invView);
	rayWorld = DirectX::XMVector3Normalize(rayWorld);

	rayWorld = rayWorld * /*Distance*/InDepth;
	rayWorld = XMLoadFloat3(&camera->Position) + rayWorld;
	// Log the result
	// XMFLOAT4 projPos;
	// XMStoreFloat4(&projPos, rayWorld);
	// RSRLog::Log(LOG_DISPLAY, TEXT("POSITION [{}, {}, {}, {}]"), projPos.x, projPos.y, projPos.z, projPos.w);
	return rayWorld;
}

void RSRTT404Pawn::GenerateMesh(RSRTextureLibrary* InTextureLibrary)
{
	RSRBasicShapes* BasicShapes = RSRBasicShapes::Get(this);
	RSRAssetManager* AssetManager = RSRAssetManager::Get(this);

	static const std::string mesh404Ship = (std::filesystem::path(TEXT("Meshes")) / TEXT("404Ship.obj")).string();
	std::shared_ptr<RSRush::RSRMesh3D> shipMesh = AssetManager->AddMesh3DAsset(mesh404Ship, false, InTextureLibrary);


	XMFLOAT3 DEFAULT_COLOR = XMFLOAT3(1.f, 0.f, 0.f);
	XMFLOAT4 DEFAULT_COLOR_4D = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	m_mainMesh = shipMesh;
	if (false)
	{
		XMFLOAT3 DEFAULT_COLOR = XMFLOAT3(1.f, 0.f, 0.f);
		XMFLOAT4 DEFAULT_COLOR_4D = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		m_mainMesh = AssetManager->AddAsset<RSRMesh3D>
			(
				mds::NameDynamicAsset(mds::RAssetType::Mesh, TEXT("Ship404")),
				false,
				std::vector<VertexPositionUVColor>
		{
			{   //00 Front
				.pos = XMFLOAT3(FRONT_FORWARD, 0.f, PEAK_HEIGHT),
					.normal = XMFLOAT3(1.f, 0.f, 0.f),
					.color = DEFAULT_COLOR,
					.uv0 = XMFLOAT2(0.f, 0.f)
			},
				{   //01 Back Left
					.pos = XMFLOAT3(BACK_FORWARD, -BASE_HALF_WIDTH, BOTTOM_HEIGHT),
					.normal = XMFLOAT3(0., -1.f, 0.f),
					.color = DEFAULT_COLOR,
					.uv0 = XMFLOAT2(1.f, 0.0f)
				},
				{   //02 Back Right
					.pos = XMFLOAT3(BACK_FORWARD, BASE_HALF_WIDTH, BOTTOM_HEIGHT),
					.normal = XMFLOAT3(0., 1.f, 0.f),
					.color = DEFAULT_COLOR,
					.uv0 = XMFLOAT2(1.f, 1.0f)
				},
				{   //03 Back Center
					.pos = XMFLOAT3(BACK_FORWARD, 0.f, TOP_HEIGHT),
					.normal = XMFLOAT3(0., 0.f, 1.f),
					.color = DEFAULT_COLOR,
					.uv0 = XMFLOAT2(0.0f, 1.0f)
				}
		},
				std::vector<unsigned short>
		{
			//F0
			0, 1, 2,
				0, 3, 1,
				0, 2, 3,
				1, 3, 2
		}
		//{
		//	//F0
		//	2, 1, 0,
		//	1, 3, 0,
		//	3, 2, 0,
		//	2, 3, 1
		//}
			);
	}

	GeneratePhysicBody();

	m_pointerUI = BasicShapes->GetRegisterNewPlane2D
	(
		TEXT("Ship404_PointerUI"),
		DEFAULT_COLOR_4D,
		{ 0.f, 0.f }, //Pivot Center
			{  //Point Top Left
				-(POINTER_WIDTH * 0.5f),
				-(POINTER_HEIGHT * 0.5f)
			},
			{  //Point BottomRight
				(POINTER_WIDTH * 0.5f),
				(POINTER_HEIGHT * 0.5f)
			},
		0, //TextureIndex
		AssetManager
	);
	//static const std::string lifepointUIName = TEXT("")
	for (size_t i = 0; i < _countof(m_lifepointOnUIs); ++i)
	{
		std::string lifepointOnName = std::format(TEXT("RSRTT404Pawn_LifepointOn_{}"), i);
		std::string lifepointOffName = std::format(TEXT("RSRTT404Pawn_LifepointOff_{}"), i);
		static constexpr float upperMargin = 0.02f;
		static constexpr float leftMargin = 0.02f;
		static constexpr float UISize = 0.2f;
		static constexpr float UIWidth = UISize;
		static constexpr float UIWidthMargin = 0.05f;
		static constexpr float UIHeight = UISize;
		m_lifepointOnUIs[i] = BasicShapes->GetRegisterNewPlane2D
		(
			lifepointOnName,
			DEFAULT_COLOR_4D,
			{ -1.f, -1.f }, //Pivot Top Left
			{  //Point Top Left
				leftMargin + (i * (UIWidth + UIWidthMargin)) ,
				upperMargin
			},
			{  //Point BottomRight
				leftMargin + UIWidth + (i * (UIWidth + UIWidthMargin)) ,
				upperMargin + UIHeight
			},
			1, //TextureIndex
			AssetManager
		);
		m_lifepointOffUIs[i] = BasicShapes->GetRegisterNewPlane2D
		(
			lifepointOffName,
			DEFAULT_COLOR_4D,
			{ -1.f, -1.f }, //Pivot Top Left
			{  //Point Top Left
				leftMargin + (i * (UIWidth + UIWidthMargin)) ,
				upperMargin
			},
			{  //Point BottomRight
				leftMargin + UIWidth + (i * (UIWidth + UIWidthMargin)) ,
				upperMargin + UIHeight
			},
			2, //TextureIndex
			AssetManager
		);
	}
}

bool RSRTT404Pawn::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool bAllSucessfull = true;
	bAllSucessfull = m_pointerUI->UploadResources(InDevice, InUploadCommandList) && bAllSucessfull;
	for (size_t i = 0; i < _countof(m_lifepointOnUIs); ++i)
	{
		if (m_lifepointOnUIs[i])
		{
			bAllSucessfull = m_lifepointOnUIs[i]->UploadResources(InDevice, InUploadCommandList) && bAllSucessfull;
		}
		else
		{
			RSRLog::LogError(TEXT("UploadResources m_lifepointOnUIs nullptr"));
			bAllSucessfull = false;
		}
		if (m_lifepointOffUIs[i])
		{
			bAllSucessfull = m_lifepointOffUIs[i]->UploadResources(InDevice, InUploadCommandList) && bAllSucessfull;
		}
		else
		{
			RSRLog::LogError(TEXT("UploadResources m_lifepointOffUIs nullptr"));
			bAllSucessfull = false;
		}
	}
	return Pawn::UploadResources(InDevice, InUploadCommandList) && bAllSucessfull;
}

bool RSRTT404Pawn::FreeUploadBuffers()
{
	bool bAllSucessfull = true;
	for (size_t i = 0; i < _countof(m_lifepointOnUIs); ++i)
	{
		if (m_lifepointOnUIs[i])
		{
			bAllSucessfull = m_lifepointOnUIs[i]->FreeUploadBuffer() && bAllSucessfull;
		}
		if (m_lifepointOffUIs[i])
		{
			bAllSucessfull = m_lifepointOffUIs[i]->FreeUploadBuffer() && bAllSucessfull;
		}
	}
	bAllSucessfull = m_pointerUI->FreeUploadBuffer() && bAllSucessfull;
	return Pawn::FreeUploadBuffers() && bAllSucessfull;
}

bool RSRTT404Pawn::FreeResourceBuffers()
{
	bool bAllSucessfull = true;
	for (size_t i = 0; i < _countof(m_lifepointOnUIs); ++i)
	{
		if (m_lifepointOnUIs[i])
		{
			bAllSucessfull = m_lifepointOnUIs[i]->FreeResourcesBuffer() && bAllSucessfull;
		}
		if (m_lifepointOffUIs[i])
		{
			bAllSucessfull = m_lifepointOffUIs[i]->FreeResourcesBuffer() && bAllSucessfull;
		}
	}
	bAllSucessfull = m_pointerUI->FreeResourcesBuffer() && bAllSucessfull;
	return Pawn::FreeResourceBuffers() && bAllSucessfull;
}

bool RSRTT404Pawn::DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const
{
	bool bAllSucessfull = true;
	bAllSucessfull = Pawn::DrawGeometry(InDraw3DCommandList) && bAllSucessfull;
	return bAllSucessfull;
}

bool RSRTT404Pawn::DrawUIs(ID3D12GraphicsCommandList7* InDraw2DCommandList) const
{
	ZoneScoped;
	bool bAllSucessfull = true;
	XMFLOAT4 PointerRectPosition =
	{
		m_currentPointerLocation.x,
		m_currentPointerLocation.y,
		1.f,
		1.f
	};
	InDraw2DCommandList->SetGraphicsRoot32BitConstants(0, 4, &PointerRectPosition, 0);
	bAllSucessfull = m_pointerUI->DrawMesh(InDraw2DCommandList) && bAllSucessfull;

	PointerRectPosition =
	{
		0.f,
		0.f,
		1.f,
		1.f
	};
	InDraw2DCommandList->SetGraphicsRoot32BitConstants(0, 4, &PointerRectPosition, 0);
	size_t lifepoint = (size_t)std::max(std::min((int)_countof(m_lifepointOnUIs), m_currentHealth), 0);
	size_t totalLifepoints = m_maxHealth;
	for (size_t i = 0; i < totalLifepoints; ++i)
	{
		if (i < lifepoint && m_lifepointOnUIs[i])
		{
			bAllSucessfull = m_lifepointOnUIs[i]->DrawMesh(InDraw2DCommandList) && bAllSucessfull;
		}
		else if (m_lifepointOffUIs[i])
		{
			bAllSucessfull = m_lifepointOffUIs[i]->DrawMesh(InDraw2DCommandList) && bAllSucessfull;
		}
	}
	bAllSucessfull = Pawn::DrawUIs(InDraw2DCommandList) && bAllSucessfull;

	return bAllSucessfull;
}


RSRPhysicBody RSRTT404Pawn::GeneratePhysicBody() const
{
	RSRPhysicBody result =
	{
		.Transform = m_mainTransform,
		.Colliders = RSRCollidersBodyLWPair(RSRush::PLAYER),
		.Dynamics = NoDynamics(100.f),
		.Entity = m_physicalBodyKey.SelfEntity
	};

	RSRCollidersBody& localBody = result.Colliders.DirtyGetLocal();
	localBody.Colliders.resize(1);
	RSRCollider* bodyCol = &localBody.Colliders[0];

	XMFLOAT4 QuatIdentity = { 0.f, 0.f, 0.f, 1.f };
	//XMMatrixTransformation
	bodyCol->Type = RSRColliderType::RectCollider;
	bodyCol->RectData =
	{
		/*.Center = */      {(FRONT_FORWARD + BACK_FORWARD) * 0.5f, 0.f, (TOP_HEIGHT + BOTTOM_HEIGHT) * 0.5f},
		/*.Extents = */     { FORWARD_LENGTH * 0.5f, BASE_HALF_WIDTH, TOTAL_HEIGHT * 0.5f },
		/*.Orientation = */ QuatIdentity
	};

	localBody.ComputeAABB();

	XMMATRIX worldTransform = XMLoadFloat4x4(&result.Transform.GetMatrix());
	result.Colliders.RecomputeWorld(worldTransform, true);
	return result;
}

void RSRTT404Pawn::OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime)
{
	RSRIPhysicalEntity::OnPhysicalPrePass(InPhysicManager, InDeltaTime);
	m_bInDamageArea = false;
}

void RSRTT404Pawn::OnOverlapWith(RSRIPhysicalEntity* InOther)
{
	AttemptDamageNextFrame(1);
}

#ifdef DEBUG_FREECAM
#undef DEBUG_FREECAM
#endif // DEBUG_FREECAM
