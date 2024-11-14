#include "App/Physic/Demo01/Gameplay/RSRPD1PlayerController.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1GameManager.h"
#include "App/Physic/Demo01/RSRPD1Scene.h"
#include <App/Tools/RSRLog.h>

using namespace RSRush;
using namespace DirectX;

bool RSRush::RSRPD1PlayerController::GetIsActiveAction(EAction EInAction) const
{
	constexpr size_t I_COUNT_ACTION = std::extent<decltype(RSRPD1PlayerController::m_actionToInputKeyTypes), 0>::value;
	constexpr size_t COUNT_ACTION = std::extent<decltype(RSRPD1PlayerController::m_actionToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < COUNT_ACTION; ++i)
	{
		size_t first = mds::UT_cast(EInAction);
		size_t second = i;
		EMDSInputKeyType keyType = m_actionToInputKeyTypes[first][second];
		if (keyType < EMDSInputKeyType::COUNT)
		{
			auto keyTypeNumerical = mds::UT_cast(keyType);
			if (this->m_keyState[0][keyTypeNumerical])
			{
				return true;
			}
		}
	}
	return false;
}

DirectX::XMFLOAT2 RSRush::RSRPD1PlayerController::GetEAxis2D(EAxis2D EInAxis2D) const
{
	if (m_mainInputDevice == EMainInputDevice::MouseKeyboard)
	{
		return this->m_axis2D[0][mds::UT_cast(m_axis2DToInputKeyTypes[mds::UT_cast(EInAxis2D)][0])];
	}
	else if (m_mainInputDevice == EMainInputDevice::GamePad)
	{
		return this->m_axis2D[0][mds::UT_cast(m_axis2DToInputKeyTypes[mds::UT_cast(EInAxis2D)][1])];
	}
	return DirectX::XMFLOAT2{ 0.f, 0.f };
}

RSRPD1PlayerController::RSRPD1PlayerController()
: PlayerController(), m_bIsInitialized(false)
{
}

bool RSRush::RSRPD1PlayerController::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	constexpr float defaultSpeed = 20.f;
	constexpr XMVECTOR Speed = XMVECTOR{ defaultSpeed, defaultSpeed,  defaultSpeed, 0.f };
	XMFLOAT3 movement = { 0.f, 0.f, 0.f }
	;	//Forward
	if (GetIsActiveAction(EAction::Forward))
	{
		movement.x += 1.f;
	}
	if (GetIsActiveAction(EAction::Back))
	{
		movement.x -= 1.f;
	}

	//Hortizonal
	if (GetIsActiveAction(EAction::Right))
	{
		movement.y += 1.f;
	}
	if (GetIsActiveAction(EAction::Left))
	{
		movement.y -= 1.f;
	}

	//Vertical
	if (GetIsActiveAction(EAction::UP))
	{
		movement.z += 1.f;
	}
	if (GetIsActiveAction(EAction::Down))
	{
		movement.z -= 1.f;
	}
	XMFLOAT2 lookMovement = XMFLOAT2{ 0.f, 0.f };
	XMFLOAT2 mouseAcc = GetEAxis2D(EAxis2D::Mouse2D_FRAME_MVMT);
	if (InGameTime > 0.5f && GetIsActiveAction(EAction::MouseRight))
	{
		XMVECTOR2 lLookMovement = XMVectorZero();
		lLookMovement = XMVectorSetX(lLookMovement, mouseAcc.x);
		lLookMovement = XMVectorSetY(lLookMovement, mouseAcc.y);

		lLookMovement = lLookMovement * 10.f * (float)InDeltaTime;

		XMStoreFloat2(&lookMovement, lLookMovement);
	}
	XMVECTOR lMovement = XMLoadFloat3(&movement);

	lMovement = lMovement * Speed * (float)InDeltaTime;
	XMStoreFloat3(&movement, lMovement);

	
	if (m_bIsInitialized) { if (std::shared_ptr<RSRPD1Pawn> pD1Pawn = m_pD1Pawn.lock())
	{
		pD1Pawn->PrepassRequestMovement(movement);
		pD1Pawn->PrepassRequestMovementLook(lookMovement);
		pD1Pawn->OnInputFrameEnded();
	}}

	bool AllSucessfull = true;
	//if (m_bIsInitialized && m_pD1Pawn && !m_pD1Pawn->GetIsHandledAsSObject())
	//{
	//	AllSucessfull = m_pD1Pawn->PrePassTick(InGameTime, InDeltaTime) && AllSucessfull;
	//}
	return AllSucessfull;
}
bool RSRush::RSRPD1PlayerController::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = true;
	//if (m_bIsInitialized && m_pD1Pawn && !m_pD1Pawn->GetIsHandledAsSObject())
	//{
	//	AllSucessfull = m_pD1Pawn->LateTickSync(InGameTime, InDeltaTime) && AllSucessfull;
	//}
	return AllSucessfull;
}
bool RSRush::RSRPD1PlayerController::Initialize()
{
	if (auto gm = LockGameManager<RSRPD1GameManager>())
	{
		if (auto scene = gm->LockScene<RSRPD1Scene>())
		{
			m_pD1Pawn = scene->AddNewSObject(std::make_shared<RSRPD1Pawn>());
			std::shared_ptr<RSRPD1Pawn> pawn = m_pD1Pawn.lock();
			//pawn->SetSelfReference(m_pD1Pawn);

			PocessPawn(pawn.get());
			m_bIsInitialized = true;
		}
	}
	return m_bIsInitialized;	
}

void RSRush::RSRPD1PlayerController::Shutdown()
{
	if (m_pawn)
	{
		auto pd1Pawn = m_pD1Pawn;
		UnpocessPawn();

		if (!pd1Pawn.expired())
		{
			if (std::shared_ptr<RSRPD1GameManager> gm = LockGameManager<RSRPD1GameManager>())
			{
				if (auto scene = gm->LockScene<RSRPD1Scene>())
				{
					scene->RemoveSObject(pd1Pawn);
				}
			}
		}
	}
	m_bIsInitialized = false;
}
bool RSRush::RSRPD1PlayerController::ReceiveKey(bool bIsDown, WPARAM InKey)
{
	//bool bSucess = IInputListener::ReceiveKey(bIsDown, InKey);
	return false;
}
void RSRush::RSRPD1PlayerController::PocessPawn(Pawn* InPawn)
{
	ApplyDefaultActionBinding();
	if (InPawn && (m_pD1Pawn.expired() ||  m_pD1Pawn.lock().get() != InPawn))
	{
		RSRLog::Log(LOG_EXCEPTION, TEXT("RSRPD1PlayerController Only handle self created PD1 Pawn !"));
		return;
	}
	PlayerController::PocessPawn(InPawn);
}
void RSRush::RSRPD1PlayerController::UnpocessPawn()
{
	PlayerController::UnpocessPawn();
	m_pD1Pawn.reset();
}

RSRPD1PlayerController::~RSRPD1PlayerController()
{
	if (m_bIsInitialized)
	{	//Do not handle potencial child class
		Shutdown();
	}
}

void RSRush::RSRPD1PlayerController::ApplyDefaultActionBinding()
{
	constexpr size_t I_COUNT_ACTION = std::extent<decltype(RSRPD1PlayerController::m_actionToInputKeyTypes), 0>::value;
	constexpr size_t J_COUNT_ACTION = std::extent<decltype(RSRPD1PlayerController::m_actionToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < I_COUNT_ACTION; ++i)
	{
		for (size_t j = 0; j < J_COUNT_ACTION; ++j)
		{
			m_actionToInputKeyTypes[i][j] = EMDSInputKeyType::NONE;
		}
	}

	constexpr size_t I_COUNT_AXIS2D = std::extent<decltype(RSRPD1PlayerController::m_axis2DToInputKeyTypes), 0>::value;
	constexpr size_t J_COUNT_AXIS2D = std::extent<decltype(RSRPD1PlayerController::m_axis2DToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < I_COUNT_AXIS2D; ++i)
	{
		for (size_t j = 0; j < J_COUNT_AXIS2D; ++j)
		{
			m_axis2DToInputKeyTypes[i][j] = EMDSAxis2DType::NONE;
		}
	}
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][0] = EMDSInputKeyType::MDS_IK_Z;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][1] = EMDSInputKeyType::MDS_IK_W;
	//m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][2] = EMDSInputKeyType::MDS_IK_LBUTTON;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][3] = EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD;


	m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][0] = EMDSInputKeyType::MDS_IK_S;
	//m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][1] = EMDSInputKeyType::MDS_IK_RBUTTON;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][2] = EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::Left)][0] = EMDSInputKeyType::MDS_IK_Q;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::Right)][0] = EMDSInputKeyType::MDS_IK_D;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::UP)][0] = EMDSInputKeyType::MDS_IK_A;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Down)][0] = EMDSInputKeyType::MDS_IK_E;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::MouseRight)][0] = EMDSInputKeyType::MDS_IK_RBUTTON;
	

	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D)][0] = EMDSAxis2DType::MDS_A2D_MS_WND_NRML;
	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D)][1] = EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK;

	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D_FRAME_MVMT)][0] = EMDSAxis2DType::MDS_A2D_MS_FRAME;
	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D_FRAME_MVMT)][1] = EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK;
}