#pragma once
#include "App/Data/RSRCameraData.h"
#include "MDS/Defines/MDS.h"
#include "MDS/Tools/RCoordinate.h"

#include "App/Data/RSRTransform.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/SceneObject/RSRSObject.h"
#include "MDS/Tools/RResourceStateType.h"

namespace RSRush
{
	class Pawn : public RSRSObject
	{
	protected:
		class PlayerController* m_controller = nullptr;
	protected:
		RSRush::RSRCameraData m_CameraData;

	public:
		PlayerController* GetController() const { return m_controller; }
	public:
		inline const RSRush::RSRCameraData* GetCameraData() const { return &m_CameraData; };
		inline RSRush::RSRCameraData* GetCameraData() { return &m_CameraData; };

	public:
		virtual void OnPocess(class PlayerController* InController);
		virtual void OnUnpocess(PlayerController* InController);


		virtual void OnInputFrameEnded() {};

		virtual bool DrawUIs(ID3D12GraphicsCommandList7* InDraw2DCommandList)  const { return true; };

		Pawn(const mds::RAssetAuthority InMainMeshAuthority);
		Pawn(const mds::RAssetAuthority InMainMeshAuthority, const bool bInIsHandledAsSObject);
		R_VIRTUAL_IMPLICIT ~Pawn() R_OVERRIDE_IMPLICIT;
	};
}