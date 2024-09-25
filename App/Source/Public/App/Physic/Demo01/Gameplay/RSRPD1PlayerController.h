#pragma once
#include "MDS/Defines/MDS.h"
#include "App/Gameplay/PlayerController.h"
#include "App/Physic/Demo01/Gameplay/RSRPD1Pawn.h"

namespace RSRush
{
	class RSRPD1PlayerController : public PlayerController
	{
	private:
		bool m_bIsInitialized;
		std::shared_ptr<RSRPD1Pawn> m_pD1Pawn = nullptr;
		inline std::shared_ptr<RSRPD1Pawn> GetPD1Pawn() const { return m_pD1Pawn; }
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
			MouseRight = 6,
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
	//	double Update(double InDeltaSeconds);
		RSRPD1PlayerController();
		R_VIRTUAL_IMPLICIT ~RSRPD1PlayerController() R_OVERRIDE_IMPLICIT;

		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

		void Initialize();
		void Shutdown();
	public:
		//Interfaces IInputListener
		virtual bool ReceiveKey(bool bIsDown, WPARAM InKey) override;
		//End Interfaces IInputListener

		virtual void PocessPawn(Pawn* InPawn) override;
		virtual void UnpocessPawn() override;


	public:
		void ApplyDefaultActionBinding();
	};
}