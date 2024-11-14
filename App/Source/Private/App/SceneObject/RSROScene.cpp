#include "App/SceneObject/RSROScene.h"

using namespace RSRush;

RSROScene::RSROScene()
	:RSRScene(), m_bFreeSOResourceOnUnload(true)
{
}
RSROScene::~RSROScene()
{
	ClearSObjects();
}

bool RSRush::RSROScene::PrePassTick(const double InGameTime, const double InDeltaTime)
{
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		sObject->PrePassTick(InGameTime, InDeltaTime);
	}
	return true;
}

bool RSRush::RSROScene::LateTickSync(const double InGameTime, const double InDeltaTime)
{
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		sObject->LateTickSync(InGameTime, InDeltaTime);
	}
	return true;
}

std::weak_ptr<RSRush::RSRSObject> RSRush::RSROScene::AddNewSObject_inner(std::shared_ptr<RSRush::RSRSObject>&& InNewSObject)
{
	std::weak_ptr<RSRush::RSRSObject> returnValue;
	if (!m_WPtrSelfNode.expired() && InNewSObject)
	{
		m_sObjects.push_back(std::move(InNewSObject));
		std::shared_ptr<RSRush::RSRSObject>& addedObject = m_sObjects.back();
		addedObject->OnAddedToScene(addedObject, ThisOScenePtr());
		returnValue = addedObject;
	}
	return returnValue;
}

void RSRush::RSROScene::RemoveSObject(std::weak_ptr<RSRush::RSRSObject> InSObjectToRemove)
{
	if (std::shared_ptr<RSRush::RSRSObject> sptr = InSObjectToRemove.lock())
	{
		RemoveSObject(sptr);
	}
}

void RSRush::RSROScene::RemoveSObject(std::shared_ptr<RSRush::RSRSObject> InSObjectToRemove)
{
	auto it = std::find(m_sObjects.begin(), m_sObjects.end(), InSObjectToRemove);
	if (it != m_sObjects.end())
	{
		if (InSObjectToRemove)
		{
			std::weak_ptr<RSROScene> _thisOScenePtr;
			if (!m_WPtrSelfNode.expired())
			{
				_thisOScenePtr = ThisOScenePtr();
			}
			InSObjectToRemove->OnRemovedFromScene(InSObjectToRemove, _thisOScenePtr);
		}
		m_sObjects.erase(it);
	}
}

void RSRush::RSROScene::ClearSObjects()
{
	while (m_sObjects.size() > 0)
	{
		RemoveSObject(m_sObjects.back());
	}
}

bool RSRush::RSROScene::UploadSOResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		if (sObject->GetResourceState() == mds::RResourceStateType::NeedUpload)
		{
			if (!sObject->UploadResources(InDevice, InUploadCommandList))
			{
				bCumulativeSucess = false;
			}
		}
	}
	return bCumulativeSucess;
}

bool RSRush::RSROScene::FreeSOUploadBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		if (sObject->GetResourceState() == mds::RResourceStateType::WaitingUpload)
		{
			bCumulativeSucess = sObject->FreeUploadBuffers() && bCumulativeSucess;
		}
	}
	return bCumulativeSucess;
}

bool RSRush::RSROScene::FreeSOResourceBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		bCumulativeSucess = sObject->FreeResourceBuffers() && bCumulativeSucess;
	}
	return bCumulativeSucess;
}

bool RSRush::RSROScene::DrawSOMeshs(ID3D12GraphicsCommandList7* InRenderCommandList) const
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRSObject> sObject : m_sObjects)
	{
		if (!sObject->GetIsDrawnAsInstance())
		{
			bCumulativeSucess = sObject->DrawGeometry(InRenderCommandList) && bCumulativeSucess;
		}
	}
	return bCumulativeSucess;
}

bool RSRush::RSROScene::UnLoad()
{
	bool bAllSucessfull = true;
	if (m_bFreeSOResourceOnUnload)
	{
		bAllSucessfull = FreeSOResourceBuffers() && bAllSucessfull;
	}
	return RSRScene::UnLoad() && bAllSucessfull;
}
