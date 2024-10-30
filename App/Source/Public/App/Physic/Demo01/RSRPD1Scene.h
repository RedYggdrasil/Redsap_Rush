#pragma once
#include "MDS/RTool.h"
#include "App/SceneObject/RSROScene.h"
#include <App/Data/Textures/RSRTexture2D.h>

namespace RSRush
{
	//class RSRTT404Pawn;
	//class RSRMesh2D;
	class RSRPD1Scene : public RSROScene
	{
		SCENE_CONSTRUCTOR(RSRPD1Scene);
	protected:
		struct PD1SlideTimelineData
		{
			size_t InTickBallSpawned = 0;
			double NextGameTimeBallSpawn = 0.0;
			size_t NextBallSpawnIndexLocation = 0;
		};
		PD1SlideTimelineData m_slideTimelineData;
		//std::shared_ptr<RSRush::RSRTT404Pawn> m_playerPawn;
		std::weak_ptr<class RSRODrawableLightSource> m_drawableLightSource;


#pragma region Textures
		std::vector<RSRSharedTexture2DPtr> m_2dTextures;
		std::vector<RSRSharedTexture2DPtr> m_3dTextures;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvheap3D;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvheap2D;
#pragma endregion Textures
	public:
		R_VIRTUAL_IMPLICIT ~RSRPD1Scene() R_OVERRIDE_IMPLICIT;
	public:
		virtual bool Load() override;
		virtual bool UnLoad() override;
		virtual bool Render(const double InGameTime, const double InDeltaTime) override;
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;


	public:
		//inline std::shared_ptr<RSRush::RSRTT404Pawn> GetPlayerPawn() const { return m_playerPawn; }
		//inline void SetPlayerPawnToRender(std::shared_ptr<RSRush::RSRTT404Pawn> InPlayerPawn) { m_playerPawn = InPlayerPawn; };
	};
};