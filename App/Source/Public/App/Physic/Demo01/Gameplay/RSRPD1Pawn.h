#pragma once
#include "MDS/Tools/REnum.h"
#include "App/Gameplay/Pawn.h"
#include "App/Data/Physic/RSRPhysicBody.h"
#include "App/Data/Physic/RSRIPhysicalEntity.h"

namespace RSRush
{
	class RSRPD1Pawn : public Pawn, public RSRIPhysicalEntity
	{
	protected:
		/// <summary>
		/// Used for input movement before physical pass
		/// </summary>
		RSRush::RSRTransformMatrix m_prePhysicNewTransform;

		DirectX::XMFLOAT2 m_prePhysicLookDir = { 0.f, 0.f };;
		/// <summary>
		/// pointer location on screen, in normalized space (-1, 1)
		/// </summary>
		DirectX::XMFLOAT2 m_currentPointerLocation = { 0.f, 0.f };

		/// <summary>
		/// Used for freecam
		/// </summary>
		DirectX::XMFLOAT2 m_mouseFrameMvmnt = { 0.f, 0.f };
#pragma region PlayerState

		bool m_bMovedFromInput = false;
		float m_frameForwardSpeed = 0.f;
		DirectX::XMFLOAT3 m_frameMouvementInputDirection = { 0.f, 0.f, 0.f };
#pragma endregion
	public:
		RSRPD1Pawn();
		/*virtual*/ ~RSRPD1Pawn() /*override*/;
	public:
		void PrepassRequestMovement(const DirectX::XMFLOAT3& InAddMovement);
		void PrepassRequestMovementLook(const DirectX::XMFLOAT2& InAddLookMovement);
		virtual void OnInputFrameEnded() override;

		inline float GetPawnProgression() const { return m_CameraData.Position.x; }
		void SetCameraData(const RSRush::RSRCameraData& InCameraData) { m_CameraData = InCameraData; };

		void OnPositionUpdated();

		virtual void OnAddedToScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual void OnRemovedFromScene(std::weak_ptr<RSRSObject> InThisWPtr, std::weak_ptr<RSROScene> InScene);
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;
	
	public:

		RSRPhysicBody GeneratePhysicBody() const;
	public:
		//Implemented via RSRIPhysicalEntity
		virtual void OnPhysicalPrePass(RSRPhysicManager* InPhysicManager, double InDeltaTime) override;
		virtual void OnOverlapWith(RSRIPhysicalEntity* InOther) override;
	};
}