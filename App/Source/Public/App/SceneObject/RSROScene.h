#pragma once

#include "MDS/RTool.h"
#include "App/Gameplay/RSRScene.h"
#include "App/SceneObject/RSRSObject.h"
#include <vector>


namespace RSRush
{
	class RSROScene : public RSRScene
	{
	protected:
		bool m_bFreeSOResourceOnUnload;/*true*/
		std::vector<std::shared_ptr<RSRush::RSRSObject>> m_sObjects;
		
		std::shared_ptr<RSRush::RSROScene> ThisOScenePtr()
		{
			if (!m_thisWPtr.expired())
			{
				return std::static_pointer_cast<RSRush::RSROScene>(m_thisWPtr.lock());
			}
			return nullptr;
		}

	public:
		RSROScene();
		R_VIRTUAL_IMPLICIT ~RSROScene() R_OVERRIDE_IMPLICIT;

	public:
		virtual bool PrePassTick(const double InGameTime, const double InDeltaTime) override;
		virtual bool LateTickSync(const double InGameTime, const double InDeltaTime) override;

	public:
		std::weak_ptr<RSRush::RSRSObject> AddNewSObject(std::shared_ptr<RSRush::RSRSObject>&& InNewSObject);
		void RemoveSObject(std::weak_ptr<RSRush::RSRSObject> InSObjectToRemove);
		void ClearSObjects();

		//Handle all SO currently in m_sObjects list
		bool UploadSOResources(struct ID3D12Device10* InDevice, struct ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeSOUploadBuffers();
		bool FreeSOResourceBuffers();
		virtual bool DrawSOMeshs(ID3D12GraphicsCommandList7* InRenderCommandList) const;
		virtual bool UnLoad() override;

	private:
		void RemoveSObject(std::shared_ptr<RSRush::RSRSObject> InSObjectToRemove);
	};
};