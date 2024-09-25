#pragma once
#include "MDS/RTool.h"
#include "App/Gameplay/RSRScene.h"
#include <App/Data/Textures/RSRTexture2D.h>

namespace RSRush
{
	class RSRTT404Pawn;
	class RSRMesh2D;
	class RSRTTScene : public RSRScene
	{
		SCENE_CONSTRUCTOR(RSRTTScene);
	protected:
		std::shared_ptr<RSRush::RSRTT404Pawn> m_playerPawn;
		std::shared_ptr<RSRush::RSRMesh2D> m_topLeftSquare2D;

#pragma region Textures
		std::vector<RSRSharedTexture2DPtr> m_2dTextures;
		std::vector<RSRSharedTexture2DPtr> m_3dTextures;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvheap3D;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvheap2D;
#pragma endregion Textures
	public:
		R_VIRTUAL_IMPLICIT ~RSRTTScene() R_OVERRIDE_IMPLICIT;
	public:
		virtual bool Load() override;
		virtual bool UnLoad() override;
		virtual bool Render(const double InGameTime, const double InDeltaTime) override;
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

	
	public:
		inline std::shared_ptr<RSRush::RSRTT404Pawn> GetPlayerPawn() const { return m_playerPawn; }
		inline void SetPlayerPawnToRender(std::shared_ptr<RSRush::RSRTT404Pawn> InPlayerPawn) { m_playerPawn = InPlayerPawn; };
	};
};