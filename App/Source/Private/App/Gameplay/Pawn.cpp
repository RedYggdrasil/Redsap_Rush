#include "App/Gameplay/Pawn.h"
#include "App/Tools/RSRLog.h"

void RSRush::Pawn::OnPocess(PlayerController* InController)
{
	m_controller = InController;
}

void RSRush::Pawn::OnUnpocess(PlayerController* InController)
{
	if (m_controller == InController)
	{
		m_controller = nullptr;
	}
}


RSRush::Pawn::Pawn(const mds::RAssetAuthority InMainMeshAuthority)
: RSRSObject(InMainMeshAuthority)
{
}

RSRush::Pawn::~Pawn()
{
}
