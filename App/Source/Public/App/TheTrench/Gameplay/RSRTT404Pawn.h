#pragma once

#include "MDS/Tools/REnum.h"
#include "App/Gameplay/Pawn.h"
#include "App/Data/Physic/RSRPhysicBody.h"
#include "App/Data/Physic/RSRIPhysicalEntity.h"
#include "App/Data/Meshes/RSRMesh2D.h"

namespace RSRush
{
	class RSRTT404Pawn : public Pawn, public RSRIPhysicalEntity
	{
		static constexpr INT32	PLAYER_DEFAULT_MAX_HEALTH = 3;
		static constexpr double PLAYER_DAMAGE_GRACE_TIME = 1.0;

		static DirectX::XMVECTOR SCREEN_MOUVEMENT_SPEED;
		static float BASE_FORWARD_MOVEMENT_SPEED;
		static float BOOST_MOVEMENT_SPEED;
		static float BRAKE_MOVEMENT_SPEED;
	public:
		enum class EShipPointerType : uint8_t
		{
			RelativeScreen = 0,
			AccNormalizedAxis = 1,
			RENUM_COUNT
		};
		struct ShipControlData
		{
		public:
			EShipPointerType ShipPointerType;
		};
	protected:
		/// <summary>
		/// Used for input movement before physical pass
		/// </summary>
		RSRush::RSRTransformMatrix m_prePhysicNewTransform;


		RSRush::RSRSharedMesh2DPtr m_pointerUI;
		RSRush::RSRSharedMesh2DPtr m_lifepointOnUIs[PLAYER_DEFAULT_MAX_HEALTH];
		RSRush::RSRSharedMesh2DPtr m_lifepointOffUIs[PLAYER_DEFAULT_MAX_HEALTH];

		/// <summary>
		/// pointer location on screen, in normalized space (-1, 1)
		/// </summary>
		DirectX::XMFLOAT2 m_currentPointerLocation = { 0.f, 0.f };
		/// <summary>
		/// pointer location on screen, in normalized space (-1, 1)
		/// </summary>
		DirectX::XMFLOAT2 m_nextFramePointerLocation = { 0.f, 0.f };

		/// <summary>
		/// Used for freecam
		/// </summary>
		DirectX::XMFLOAT2 m_mouseFrameMvmnt = { 0.f, 0.f };
#pragma region PlayerState
		////////////State////////////
	protected:
		//Control 
		ShipControlData m_controlData;
		DirectX::XMFLOAT2 m_nrlzForwardPointer;

		//Health
		INT32 m_maxHealth = RSRTT404Pawn::PLAYER_DEFAULT_MAX_HEALTH;
		INT32 m_currentHealth = RSRTT404Pawn::PLAYER_DEFAULT_MAX_HEALTH;
		INT32 m_nextFrameHealth = RSRTT404Pawn::PLAYER_DEFAULT_MAX_HEALTH;

		//Damage
		double m_damageGraceTime = RSRTT404Pawn::PLAYER_DAMAGE_GRACE_TIME;
		double m_lastInDamageAreaTime = DBL_MIN;
		bool m_bInDamageArea = false;

		bool m_bMovedFromInput = false;
		float m_frameForwardSpeed = 0.f;
		DirectX::XMFLOAT3 m_frameMouvementInputDirection = { 0.f, 0.f, 0.f };
	public:
		//Control
		inline const ShipControlData& GetControlData() const { return m_controlData; }
		void SetControlData(const ShipControlData& InControlData);
		inline const DirectX::XMFLOAT2& GetNormalizedForwardPointer() const { return m_nrlzForwardPointer; }

		//Health
		inline INT32 GetMaxHealth() const { return m_maxHealth; }
		inline INT32 GetCurrentHealth() const { return m_currentHealth; }

		inline bool GetIsAlive() const { return m_currentHealth > 0; };
		//Damage
		bool GetInDamageGraceTime() const;
		inline bool GetInDamageGraceTime(double InCurrentGameTime) const { return (m_lastInDamageAreaTime + m_damageGraceTime) > InCurrentGameTime; }

		void AttemptDamageNextFrame(uint16_t InDamageAmount = 1);

		void OnPawnDeath();
		bool GetIsDebugFreeCam() const;
#pragma endregion





	public:
		RSRTT404Pawn();
		/*virtual*/ ~RSRTT404Pawn() /*override*/;
	public:
		/// <summary>
		/// YZ mouvement on screen, in X is packed forward data ie : negative is brake, 0 is default, 1 is boost
		/// </summary>
		/// <param name="InMouvementDirection"></param>
		/// <param name="InDeltaTime"></param>
		void PrePassRequestActionMovement(const DirectX::XMFLOAT3& InMouvementDirection, double InDeltaTime);

		/// <summary>
		/// XY Mouse position, in [-1.f, 1.f] normalized space
		/// </summary>
		/// <param name="InTargetPosition">In normalized space</param>
		/// <param name="InDeltaTime"></param>
		void PrePassTargetPosition(const DirectX::XMFLOAT2& InTargetPosition, double InDeltaTime);

		void SetMouseFrameMouvement(const DirectX::XMFLOAT2& InMouseFrameMouvement, double InDeltaTime);

		virtual void OnInputFrameEnded() override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

		inline float GetPawnProgression() const { return m_CameraData.Position.x; }
		void SetCameraData(const RSRush::RSRCameraData& InCameraData) { m_CameraData = InCameraData; };

		void OnPositionUpdated();
	protected:
		static void ComputeShipStateSpeed();

		DirectX::XMVECTOR XM_CALLCONV ComputePointerForwardLocation(const float InShipDepth = 5.f) const;
		DirectX::XMVECTOR XM_CALLCONV ComputePointerWorldLocation(const float InDepth = 10.f) const;

	public:
		void GenerateMesh();
		virtual bool UploadResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList) override;
		virtual bool FreeUploadBuffers() override;
		virtual bool FreeResourceBuffers() override;

		virtual bool DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const override;

		virtual bool DrawUIs(ID3D12GraphicsCommandList7* InDraw2DCommandList) const override;

		RSRPhysicBody GeneratePhysicBody() const;
	public:
		//Implemented via RSRIPhysicalEntity
		virtual void OnPhysicalPrePass(double InDeltaTime) override;
		virtual void OnOverlapWith(RSRIPhysicalEntity* InOther) override;
	};
}