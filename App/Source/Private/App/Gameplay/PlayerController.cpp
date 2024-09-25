#include "App/Gameplay/PlayerController.h"

#include <algorithm>
#include <iostream>
#include <Tracy.hpp>
#include <App/Tools/RSRLog.h>



using namespace RSRush;

RSRush::PlayerController::PlayerController()
{
}

RSRush::PlayerController::~PlayerController()
{
}

bool PlayerController::ReceiveKey(bool bIsDown, WPARAM InKey)
{
	bool bSucess = IInputListener::ReceiveKey(bIsDown, InKey);
	return bSucess;
}

void PlayerController::PocessPawn(Pawn* InPawn)
{
	if (m_pawn)
	{
		if (m_pawn != InPawn)
		{
			UnpocessPawn();
		}
		else { return; }
	}
	m_pawn = InPawn;
}

void PlayerController::UnpocessPawn()
{
	if (m_pawn)
	{
		m_pawn->OnUnpocess(this);
		m_pawn = nullptr;
	}
}