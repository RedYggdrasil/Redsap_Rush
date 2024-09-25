#pragma once
#include "App/Gameplay/PlayerController.h"
#include "App/TheTrench/Gameplay/RSRTT404Pawn.h"

namespace RSRush
{
	class RSRTTPlayerController : public PlayerController
	{
	private:
		inline RSRTT404Pawn* Get404Pawn() const { return static_cast<RSRTT404Pawn*>(m_pawn); }
	public:
		static constexpr size_t MAX_ACTION_BOUND_KEYS = 8;
	protected:
		enum class EAction : uint32_t
		{
			Forward = 0,
			Back = 1,
			Left = 2,
			Right = 3,
			UP = 4,
			Down = 5,
			RENUM_COUNT
		};
		enum class EAxis2D : uint32_t
		{
			Mouse2D = 0,
			Mouse2D_FRAME_MVMT = 1,
			RENUM_COUNT
		};

		EMDSInputKeyType m_actionToInputKeyTypes[mds::UT_cast(EAction::COUNT)][MAX_ACTION_BOUND_KEYS];
		EMDSAxis2DType m_axis2DToInputKeyTypes[mds::UT_cast(EAxis2D::COUNT)][MAX_ACTION_BOUND_KEYS];

	protected:
		bool GetIsActiveAction(EAction EInAction) const;
		DirectX::XMFLOAT2 GetEAxis2D(EAxis2D EInAxis2D) const;
	public:
		double GetPrePassProgression() const;

		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;
	public:
		//Interfaces IInputListener
		virtual bool ReceiveKey(bool bIsDown, WPARAM InKey) override;
		//End Interfaces IInputListener

		virtual void PocessPawn(Pawn* InPawn) override;

	public:
		void ApplyDefaultActionBinding();
	};
}