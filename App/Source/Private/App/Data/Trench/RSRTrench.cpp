#include "App/Data/Trench/RSRTrench.h"
#include "App/Tools/RSRLog.h"

#include "App/Game/RSRPlayerPath.h"

using namespace RSRush;

UINT64 RSRTrench::s_nextInstanceID = 0;


RSRPhysicBody RSRTrench::GeneratePhysicBody()
{
	assert(!m_physicalBodyKey.SelfEntity.expired() && TEXT("Expired Trench reference given, has ptr been initialized in TrenchManager ?"));
	DirectX::XMMATRIX lTrsMatrix = DirectX::XMLoadFloat4x4(&m_mainTransform.GetMatrix());
	RSRPhysicBody result =
	{
		.Transform = m_mainTransform,
		.Colliders = RSRCollidersBodyLWPair::FromLocalCollidersBody(m_localColliders, lTrsMatrix),
		.Dynamics = NoDynamics(10000.f),
		.Entity = m_physicalBodyKey.SelfEntity
	};
	return result;
}

mds::RResourceStateType RSRush::RSRTrench::GetResourceState(const size_t InLODIndex) const
{
	if (HasMesh(InLODIndex))
	{
		return m_mainMesh.GetMeshForLOD(InLODIndex)->GetResourceState();
	}
	return mds::RResourceStateType::Unknown;
}

bool RSRush::RSRTrench::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	if (HasMesh())
	{
		return m_mainMesh.UploadResources(InDevice, InUploadCommandList);
	}
	return false;
}

bool RSRush::RSRTrench::FreeUploadBuffers()
{
	if (HasMesh())
	{
		return m_mainMesh.FreeUploadBuffers();
	}
	return false;
}

bool RSRush::RSRTrench::FreeResourceBuffers()
{
	if (HasMesh())
	{
		return m_mainMesh.FreeResourceBuffers();
	}
	return true;
}

bool RSRush::RSRTrench::DrawTrench(DirectX::FXMVECTOR InCameraPosition, ID3D12GraphicsCommandList7* InDrawCommandList)
{
	if (HasMesh())
	{
		RSRush::RSRTransformMatrix trs = m_mainTransform;
		return m_mainMesh.DrawMesh(InCameraPosition, InDrawCommandList, m_mainTransform);
	}
	return false;
}

RSRush::RSRTrench::~RSRTrench()
{
	//No strictly necessary, RAII should ensure this anyway
	if (HasMesh())
	{
		bool bSucess = m_mainMesh.FreeAllBuffers();
	}
}

void RSRush::RSRTrench::SetTransform(const RSRush::RSRTransformMatrix& InTransform)
{
	m_mainTransform = InTransform;
}

void RSRush::RSRTrench::SetPositon(const DirectX::XMFLOAT3& InPosition)
{
	m_mainTransform.SetPositon(InPosition);
}

void RSRush::RSRTrench::SetRotation(const DirectX::XMFLOAT3& InRotation)
{
	m_mainTransform.SetRotation(InRotation);
}

void RSRush::RSRTrench::SetScale(const DirectX::XMFLOAT3& InScale)
{
	m_mainTransform.SetScale(InScale);
}

void RSRush::RSRTrench::SetStartPositionData(double InStartProgression, const DirectX::XMFLOAT3& InStartLocation, const DirectX::XMFLOAT3& InStartDirection, std::weak_ptr<RSRush::RSRPlayerPath> InPlayerPath)
{
	this->m_pPlayerPath = InPlayerPath;
	m_startProgression = InStartProgression;
	m_startLocation = InStartLocation;
	m_startDirection = InStartDirection;
}

RSRush::RSRTrench::RSRTrench()
: RSRIPhysicalEntity(), m_bThreadedInitializationCompleted(false), m_mainMesh(), m_instanceID(s_nextInstanceID)
{
	++s_nextInstanceID;
}

void RSRush::RSRTrench::SetEndPositionData(double InEndProgression, const DirectX::XMFLOAT3& InEndLocation, const DirectX::XMFLOAT3& InEndDirection)
{
	m_endProgression = InEndProgression;
	m_endLocation  = InEndLocation;
	m_endDirection = InEndDirection;
}
