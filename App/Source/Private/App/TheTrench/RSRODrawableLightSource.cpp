#include "App/TheTrench/RSRODrawableLightSource.h"
#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"
#include "App/Geometry/RSRBasicShapes.h"

using namespace RSRush;
using namespace DirectX;

RSRODrawableLightSource::RSRODrawableLightSource(const RSRTransform& InTransform, const bool bInHandleAsSObject, const uint16_t InTextureID)
	:RSRSObject(mds::RAssetAuthority::Absolute, bInHandleAsSObject)
{
	m_mainTransform.SetTransform(InTransform);
	
	m_mainMesh = RSRBasicShapes::Get().GetRegisterNewSphere(TEXT("SunSphere"), XMFLOAT3(1.f, 1.f, 1.f), InTextureID);
	auto& vertices = m_mainMesh->GetVertices();
	XMVECTOR3 lNormal;
	for (auto& vertice : vertices)
	{
		lNormal = XMLoadFloat3(&vertice.normal);
		XMStoreFloat3(&vertice.normal, XMVectorScale(lNormal, -1.f));
	}
}

RSRODrawableLightSource::~RSRODrawableLightSource()
{
}