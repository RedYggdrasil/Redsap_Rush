#pragma once

#include "App/Data/Physic/RSRPhysicContext.h"
#include "App/Gameplay/Scene/RSRSceneInstancedMeshes.h"
#include <memory>
struct ID3D12GraphicsCommandList7;
namespace RSRush
{
	class GameManager;
	class RSRProgramInstance;
	class RSRScene
	{
	protected:
		bool m_bIsLoaded;
		std::weak_ptr<RSRScene> m_thisWPtr;
		RSRProgramInstance* m_programInstance;
		std::shared_ptr<GameManager> m_gameManager;
		RSRPhysicContext m_physicContext = PhysicContext::DEFAULT_EARTH;

		RSRSceneInstancedMeshes m_instancedMeshes;

	public:
		inline std::shared_ptr<GameManager> GetGameManager() const { return m_gameManager; }
		inline bool GetIsLoaded() const { return m_bIsLoaded; }
		inline const RSRPhysicContext& GetPhysicContext() const { return m_physicContext; }

	public:
		virtual bool Load();
		virtual bool UnLoad();
		/// <summary>
		/// Render handle rendering only of the last computed frame
		/// </summary>
		virtual bool Render(const double InGameTime, const double InDeltaTime) { return true; };
		/// <summary>
		/// Not executed on main tread, Compute new input and request new position
		/// new application state is to be validated by Physic Pass
		/// </summary>
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) { return true; };
		/// <summary>
		/// Read the result of PrepassTick and Physic Pass, and apply them to object before rendering
		/// </summary>
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) { return true; };

		bool UpdateMeshInstanceBuffers(const double InGameTime, const double InDeltaTime, ID3D12GraphicsCommandList7* InFrameUploadCommandList);
		bool ClearUnusedMeshInstanceBuffers();

		void RegisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh);
		void UnregisterInstancedMesheHolder(RSRIInstancedMesheHolder* InMeshHolder, std::shared_ptr<RSRMesh> InMesh);

	public:
		virtual ~RSRScene();
	protected:
		RSRScene();
	private:
		friend class RSRProgramInstance;
	};
};

#define SCENE_CONSTRUCTOR(SceneClass)\
private:\
	struct Private { explicit Private() = default; };\
public:\
	struct Constructor\
	{\
	private:\
		static std::shared_ptr<SceneClass>Create(RSRush::RSRProgramInstance* InProgramInstance)\
		{\
			std::shared_ptr<SceneClass> scene = std::make_shared<SceneClass>(Private());\
			scene->m_programInstance = InProgramInstance;\
			scene->m_thisWPtr = scene;\
			return scene;\
		};\
		friend class RSRProgramInstance;\
	};\
	SceneClass(Private);