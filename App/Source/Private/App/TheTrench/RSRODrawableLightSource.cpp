#include "App/TheTrench/RSRODrawableLightSource.h"
#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"
#include "App/Geometry/RSRBasicShapes.h"

using namespace RSRush;
using namespace DirectX;

RSRODrawableLightSource::RSRODrawableLightSource(RSRBasicShapes* BasicShapes, const RSRTransform& InTransform, const uint16_t InTextureID)
	:RSRSObject(mds::RAssetAuthority::Absolute, false)
{
	m_mainTransform.SetTransform(InTransform);
	
	m_mainMesh = BasicShapes->GetRegisterNewSphere(TEXT("SunSphere"), XMFLOAT3(1.f, 1.f, 1.f), InTextureID);
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