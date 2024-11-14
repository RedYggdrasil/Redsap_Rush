#pragma once
#include "App/SceneObject/RSROScene.h"
#include "App/Game/RSRProgramInstance.h"

namespace RSRush
{
	class RSRTT404Pawn;
	class RSRMesh2D;
	class RSRTexture2D;
	class RSRTextureLibrary;
	class RSRTTScene : public RSROScene
	{
		SCENE_CONSTRUCTOR(RSRTTScene);
	protected:
		std::weak_ptr<RSRush::RSRTT404Pawn> m_playerPawn;
		std::shared_ptr<RSRush::RSRMesh2D> m_topLeftSquare2D;
		std::weak_ptr<class RSRODrawableLightSource> m_drawableLightSource;
		std::shared_ptr<class RSRTrenchManager> m_trenchManager;

#pragma region Textures
		std::shared_ptr<RSRTextureLibrary> m_2dTextures;
		std::shared_ptr<RSRTextureLibrary> m_3dTextures;

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

		RSRTrenchManager* GetTrenchManager() { return m_trenchManager.get(); }
		RSRTextureLibrary* GetTextureLibrary3D() { return m_3dTextures.get(); }
		RSRTextureLibrary* GetTextureLibrary2D() { return m_2dTextures.get(); }
	public:
		inline std::shared_ptr<RSRush::RSRTT404Pawn> GetPlayerPawn() const { return m_playerPawn.lock(); }
	};
};