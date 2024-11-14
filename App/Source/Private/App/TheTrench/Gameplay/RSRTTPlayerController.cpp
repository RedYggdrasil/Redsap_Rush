#include "App/TheTrench/Gameplay/RSRTTPlayerController.h"
#include <Tracy.hpp>
#include <App/Tools/RSRLog.h>

using namespace RSRush;
using namespace DirectX;
static constexpr float DEFAULT_SPEED = 10.f;

bool RSRush::RSRTTPlayerController::GetIsActiveAction(EAction EInAction) const
{
	constexpr size_t I_COUNT_ACTION = std::extent<decltype(RSRTTPlayerController::m_actionToInputKeyTypes), 0>::value;
	constexpr size_t COUNT_ACTION = std::extent<decltype(RSRTTPlayerController::m_actionToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < COUNT_ACTION; ++i)
	{
		if 
		(
			m_actionToInputKeyTypes[mds::UT_cast(EInAction)][i] < EMDSInputKeyType::COUNT 
			&& 
			this->m_keyState[0][mds::UT_cast(m_actionToInputKeyTypes[mds::UT_cast(EInAction)][i])]
		)
		{
			return true;
		}
	}
	return false;
}

DirectX::XMFLOAT2 RSRush::RSRTTPlayerController::GetEAxis2D(EAxis2D EInAxis2D) const
{
	//constexpr size_t I_COUNT_AXIS2D = std::extent<decltype(PlayerController::m_axis2DToInputKeyTypes), 0>::value;
	//constexpr size_t J_COUNT_AXIS2D = std::extent<decltype(PlayerController::m_axis2DToInputKeyTypes), 1>::value;
	//
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

double RSRush::RSRTTPlayerController::GetPrePassProgression() const
{
	if (!Get404Pawn())
	{
		return 0.;
	}
	//Right now pawn return Camera (last Pass) Progression, 1 Frame 'late'
	return Get404Pawn()->GetPawnProgression();
}


bool RSRush::RSRTTPlayerController::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	ZoneScoped;
	if (!Get404Pawn())
	{
		return 0.;
	}

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

	Get404Pawn()->PrePassRequestActionMovement(movement, InDeltaTime);

	XMFLOAT2 mouseAcc = GetEAxis2D(EAxis2D::Mouse2D_FRAME_MVMT);
	Get404Pawn()->SetMouseFrameMouvement(mouseAcc, InDeltaTime);

	XMFLOAT2 mousePosition = GetEAxis2D(EAxis2D::Mouse2D);
	//RSRLog::Log(LOG_DISPLAY, TEXT("mousePosition : {} {}"), mousePosition.x, mousePosition.y);

	if (m_mainInputDevice == EMainInputDevice::MouseKeyboard)
	{
		mousePosition =
		{
			(mousePosition.x - 0.5f) * 2.f,
			(mousePosition.y - 0.5f) * 2.f
		};
	}
	else if (m_mainInputDevice == EMainInputDevice::GamePad)
	{
		XMFLOAT2 forwardPointer = Get404Pawn()->GetNormalizedForwardPointer();
		forwardPointer.y = -forwardPointer.y;
		mousePosition = { std::clamp(mousePosition.x + forwardPointer.x, -1.f, 1.f), std::clamp(mousePosition.y + forwardPointer.y, -1.f, 1.f) };
	}
	Get404Pawn()->PrePassTargetPosition(mousePosition, InDeltaTime);

	Get404Pawn()->OnInputFrameEnded();

	bool AllSucessfull = true;
	//if (!Get404Pawn()->GetIsHandledAsSObject())
	//{
	//	AllSucessfull = Get404Pawn()->PrePassTick(InGameTime, InDeltaTime) && AllSucessfull;
	//}
	return AllSucessfull;
}

bool RSRush::RSRTTPlayerController::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	bool AllSucessfull = true;
	if (!Get404Pawn())
	{
		return AllSucessfull;
	}
	//if (!Get404Pawn()->GetIsHandledAsSObject())
	//{
	//	AllSucessfull = Get404Pawn()->LateTickSync(InGameTime, InDeltaTime) && AllSucessfull;
	//}
	return AllSucessfull;
}

bool RSRush::RSRTTPlayerController::ReceiveKey(bool bIsDown, WPARAM InKey)
{
	return false;
}

void RSRush::RSRTTPlayerController::PocessPawn(Pawn* InPawn)
{
	ApplyDefaultActionBinding();
	bool bIsNewPawn = InPawn && InPawn != m_pawn;
	PlayerController::PocessPawn(InPawn);
	if (bIsNewPawn)
	{
		RSRTT404Pawn* shipPawn = Get404Pawn();
		if (shipPawn)//Should alway be true
		{
			shipPawn->SetCameraData({
			.Position = { -5.f,  0.f, 0.f},
			.Target = { 0.f, 0.f,  0.f },
			.Up = { 0.f, 0.f,  1.f },

			.FOV = (5.f / 12.f) * DirectX::XM_PI, //75°
			.AspectRatio = 16.f / 9.f,
			.NearClip = 0.1f,
			.FarClip = 10000.f
				});
		}
	}
}

void RSRush::RSRTTPlayerController::ApplyDefaultActionBinding()
{
	constexpr size_t I_COUNT_ACTION = std::extent<decltype(RSRTTPlayerController::m_actionToInputKeyTypes), 0>::value;
	constexpr size_t J_COUNT_ACTION = std::extent<decltype(RSRTTPlayerController::m_actionToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < I_COUNT_ACTION; ++i)
	{
		for (size_t j = 0; j < J_COUNT_ACTION; ++j)
		{
			m_actionToInputKeyTypes[i][j] = EMDSInputKeyType::NONE;
		}
	}

	constexpr size_t I_COUNT_AXIS2D = std::extent<decltype(RSRTTPlayerController::m_axis2DToInputKeyTypes), 0>::value;
	constexpr size_t J_COUNT_AXIS2D = std::extent<decltype(RSRTTPlayerController::m_axis2DToInputKeyTypes), 1>::value;

	for (size_t i = 0; i < I_COUNT_AXIS2D; ++i)
	{
		for (size_t j = 0; j < J_COUNT_AXIS2D; ++j)
		{
			m_axis2DToInputKeyTypes[i][j] = EMDSAxis2DType::NONE;
		}
	}
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][0] = EMDSInputKeyType::MDS_IK_Z;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][1] = EMDSInputKeyType::MDS_IK_W;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][2] = EMDSInputKeyType::MDS_IK_LBUTTON;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Forward)][3] = EMDSInputKeyType::MDS_IK_GMPD_DPAD_LEFT_SHOULD;


	m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][0] = EMDSInputKeyType::MDS_IK_S;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][1] = EMDSInputKeyType::MDS_IK_RBUTTON;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Back)][2] = EMDSInputKeyType::MDS_IK_GMPD_DPAD_RIGHT_SHOULD;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::Left)][0] = EMDSInputKeyType::MDS_IK_Q;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::Right)][0] = EMDSInputKeyType::MDS_IK_D;

	m_actionToInputKeyTypes[mds::UT_cast(EAction::UP)][0] = EMDSInputKeyType::MDS_IK_A;
	m_actionToInputKeyTypes[mds::UT_cast(EAction::Down)][0] = EMDSInputKeyType::MDS_IK_E;

	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D)][0] = EMDSAxis2DType::MDS_A2D_MS_WND_NRML;
	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D)][1] = EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK;

	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D_FRAME_MVMT)][0] = EMDSAxis2DType::MDS_A2D_MS_FRAME;
	m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::Mouse2D_FRAME_MVMT)][1] = EMDSAxis2DType::MDS_A2D_GMPD_LEFT_STICK;
}

RSRTTPlayerController::RSRTTPlayerController()
: PlayerController()
{
}

RSRTTPlayerController::~RSRTTPlayerController()
{
}
