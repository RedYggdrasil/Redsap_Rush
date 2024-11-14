#include "App/Managers/RSRTrenchManager.h"

#include <format>

#include "MDS/Tools/RResourceStateType.h"

#include "App/D3D/DXContext.h"
#include "App/Data/Trench/RSRTrench.h"
//#include "App/Data/Trench/RSRBasicTrench.h"
#include "App/Data/Trench/RSRVoxalTrench.h"
#include "App/Game/RSRPlayerPath.h"
#include "App/Managers/RSRPhysicManager.h"
#include "App/TheTrench/RSRTTScene.h"
#include "App/Tools/RSRLog.h"
#include "App/System/RSRRandomizer.h"
#include <Tracy.hpp>

using namespace DirectX;
using namespace RSRush;

static const size_t UPLOAD_SEGMENT_COUNT = 5;
static const double GENERATE_EVERY_SECONDS = 0.1;

//The distance remaining at witch we start to generate the next segments
static const double GENERATE_PROGRESSION_DISTANCE = 30.;

const XMFLOAT3 START_LOCATION = XMFLOAT3(0.f, 0.f, 0.f);
const XMFLOAT3 START_DIRECTION = XMFLOAT3(1.f, 0.f, 0.f);


RSRProgramInstance* RSRush::RSRTrenchManager::GetProgramInstance() const
{
	return GetRoot<RSRProgramInstance>();
}

void RSRTrenchManager::BeginNewTrench(double InCurrentProgression, std::weak_ptr<RSRPlayerPath> InPlayerPath)
{
	this->m_pPlayerPath = InPlayerPath;
	return;
	//m_segments.push_back(std::make_unique<RSRBasicTrench>());
	//
	//m_segments[0]->GenerateMesh();
	//
	//m_segments.push_back(std::make_unique<RSRBasicTrench>());
	//m_segments[1]->SetPositon( { 10.f, 0.f , 0.f } );
	//m_segments[1]->SetRotation({ 00.f, 0.f , 90.f });
	//m_segments[1]->GenerateMesh();
}

void RSRTrenchManager::UpdateTrench(double InDeltaTime, double InCurrentProgression, UINT64 InCurrentUploadFence)
{
	ZoneScoped;
	// Clean passed trenchs but always keep at least 1 Trench to keep progression
	RSRPhysicManager* physicManager = RSRPhysicManager::Get(this);
	while(m_segments.size() > 1 && m_segments.front()->GetEndProgression() < InCurrentProgression)
	{
		std::shared_ptr<RSRTrench>& front = m_segments.front();
		front->FreeResourceBuffers();
		physicManager->RemovePhysicalEntity(front->GetEditKey());
		m_segments.pop_front();
	}

	if (!m_currentlyGeneratingSegments)
	{
		if (!this->IsNeedToGenerateSegments(InCurrentProgression))
		{
			//No need to do anything.
			return;
		}
		m_currentlyGeneratingSegments = true;
	}
	if (m_generatingSegments.size() < UPLOAD_SEGMENT_COUNT)
	{
		m_nextGenerateTime -= InDeltaTime;
		if (m_nextGenerateTime <= 0.0)
		{
			if (!IsTrenchGenerationTaskOnging())
			{
				this->GenerateNewTrench();
			}
		}
		return;
	}
	if (IsTrenchGenerationTaskOnging())
	{
		//We wait for last trench generation to complete...
		return;
	}
	if (!m_currentlyUploading)
	{
		this->StartUploadingTrenchMesh();
		return;
	}
	if (InCurrentUploadFence >= m_waitingForUploadFence)
	{
		this->OnUploadingTrenchMeshEnded();
	}
	return;
}

bool RSRTrenchManager::UploadBeingNewResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	RSRLog::LogError(TEXT(L_PATH "Unimplemented"));
	return false;
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRTrench>& Trench : m_segments)
	{
		if (Trench->GetResourceState() == mds::RResourceStateType::NeedUpload)
		{
			if (!Trench->UploadResources(InDevice, InUploadCommandList))
			{
				bCumulativeSucess = false;
			}
		}
	}
	return bCumulativeSucess;
}

bool RSRTrenchManager::FreeUploadBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRTrench>& Trench : m_segments)
	{
		if (Trench->GetResourceState() == mds::RResourceStateType::WaitingUpload)
		{
			bCumulativeSucess = Trench->FreeUploadBuffers() && bCumulativeSucess;
		}
	}
	return bCumulativeSucess;
}

bool RSRTrenchManager::FreeResourceBuffers()
{
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRTrench>& Trench : m_segments)
	{
		bCumulativeSucess = Trench->FreeResourceBuffers() && bCumulativeSucess;
	}

	for (std::unique_ptr<RSRTrench>& Trench : m_generatingSegments)
	{
		bCumulativeSucess = Trench->FreeResourceBuffers() && bCumulativeSucess;
	}
	return bCumulativeSucess;
}

bool RSRush::RSRTrenchManager::DrawTrench(DirectX::XMFLOAT4 InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList)
{
	DirectX::XMVECTOR4 lCameraPosition = XMLoadFloat4(&InCameraPosition);
	bool bCumulativeSucess = true;
	for (std::shared_ptr<RSRTrench>& Trench : m_segments)
	{
		if (Trench->GetResourceState() == mds::RResourceStateType::ResourceLive)
		{
			if (!Trench->DrawTrench(lCameraPosition, InDrawCommandList))
			{
				bCumulativeSucess = false;
			}
		}
	}
	return bCumulativeSucess;
}


RSRush::RSRTrenchManager::RSRTrenchManager(RSRTTScene* InOwnerScene, std::vector<uint16_t>&& InSideGreedbleTextureIDs, std::vector<uint16_t>&& InTopGreedbleTextureIDs)
: mds::IRProgramMemNode(), m_owningScene(InOwnerScene), m_segments(), m_generatingSegments(), gen(RSRRandomizer::Get().CreateTrenchManagerGenerator()),
	m_sideGreedbleTextureIDs(std::move(InSideGreedbleTextureIDs)), m_topGreedbleTextureIDs(std::move(InTopGreedbleTextureIDs))
{
}


bool RSRTrenchManager::IsNeedToGenerateSegments(double InCurrentProgression) const
{
	if (m_segments.size() < 1)
	{
		return true;
	}
	return (InCurrentProgression + GENERATE_PROGRESSION_DISTANCE) > m_segments.back()->GetEndProgression();
}

std::unique_ptr<RSRTrench> RSRTrenchManager::GetNewTrenchInstance()
{
	//TODO : Randomly choose a new Trench class and instanciate it
	return std::make_unique<RSRVoxalTrench>(this, m_sideGreedbleTextureIDs, m_topGreedbleTextureIDs);
}

bool RSRush::RSRTrenchManager::IsTrenchGenerationTaskOnging() const
{
	if (m_generatingSegments.size() < 1)
	{
		return false;
	}
	return !m_generatingSegments.back().get()->IsGenerationCompleted();
}

void RSRTrenchManager::GenerateNewTrench()
{
	int32_t nbNeededTrench = ((int32_t)UPLOAD_SEGMENT_COUNT - (int32_t)m_generatingSegments.size());

	const RSRTrench* previousTrench = (m_generatingSegments.size() > 0) ?
		(m_generatingSegments.back().get())
		:
		((m_segments.size() > 0) ? m_segments.back().get() : nullptr);
	double previousProgression = (previousTrench != nullptr) ? previousTrench->GetEndProgression() : 0.0;
	XMFLOAT3 previousLocation = (previousTrench != nullptr) ? previousTrench->GetEndLocation() : START_LOCATION;
	XMFLOAT3 previousDirection = (previousTrench != nullptr) ? previousTrench->GetEndDirection() : START_DIRECTION;
	m_generatingSegments.push_back(GetNewTrenchInstance());
	m_generatingSegments.back()->SetStartPositionData(previousProgression, previousLocation, previousDirection, m_pPlayerPath);
	m_generatingSegments.back()->StartGeneratingMesh();
	//Next Mesh Generation
	if (nbNeededTrench > 1)
	{
		m_nextGenerateTime = GENERATE_EVERY_SECONDS;
	}
	else
	{
		m_nextGenerateTime = 0.0;
	}
}

void RSRTrenchManager::StartUploadingTrenchMesh()
{
	// TODO : Start uploading m_generatingSegments
	m_currentlyUploading = true;
	bool bCumulativeSucess = true;
	ID3D12Device10* device = DXContext::Get(this)->GetDevice().Get();
	ID3D12GraphicsCommandList7* uploadCommandList = DXContext::Get(this)->GetAsyncUploadList();

	for (const auto& generatedSegment : m_generatingSegments)
	{
		/*Should always evaluate to true*/
		if (generatedSegment->GetResourceState() == mds::RResourceStateType::NeedUpload)
		{
			bCumulativeSucess = generatedSegment->UploadResources(device, uploadCommandList) && bCumulativeSucess;
		}
		else
		{
			RSRLog::LogWarning(TEXT(L_PATH "Unexepected behavior, elements of m_generatingSegments in RSRTrenchManager should always be in NeedUpload State !"));
		}
	}
	m_waitingForUploadFence = DXContext::Get(this)->ReportAddedAsyncUploadTask();
}

void RSRTrenchManager::OnUploadingTrenchMeshEnded()
{
	RSRPhysicManager* physicManager = RSRPhysicManager::Get(this);
	for (size_t i = 0; i < m_generatingSegments.size(); ++i)
	{
		if (!m_generatingSegments[i]->FreeUploadBuffers())
		{
			std::string logStr = std::format(TEXT("Generated Segment {} cannot free buffers !"), i);
			RSRLog::LogError(logStr);
		}
		const std::shared_ptr<RSRTrench>& ptr = m_segments.emplace_back(m_generatingSegments[i].release());
		ptr->SetSelfReference(ptr);
		physicManager->AddPhysicalEntity(ptr->GeneratePhysicBody());

	}
	m_generatingSegments.clear();
	m_waitingForUploadFence = 0;
	m_currentlyUploading = false;
	m_currentlyGeneratingSegments = false;
}
