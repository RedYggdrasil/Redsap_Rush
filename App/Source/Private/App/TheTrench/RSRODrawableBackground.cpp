#include "App/TheTrench/RSRODrawableBackground.h"
#include "MDS/Tools/RMath.h"
#include "App/Tools/RSRLog.h"
#include "App/Geometry/RSRBasicShapes.h"
#include "App/Data/Shaders/ShaderStructures.h"

using namespace RSRush;
using namespace DirectX;

RSRODrawableBackground::RSRODrawableBackground(const RSRTransform& InTransform, const uint16_t InTextureID)
	:RSRSObjectInstance
	(
		/*Mesh*/RSRBasicShapes::Get().GetRegisterNewPlane(TEXT("BackgroundPlane"), XMFLOAT3(1.f, 1.f, 1.f), InTextureID, XMFLOAT2{ 5.f, 5.f }),
		InTransform,
		std::move(INIT_INSTANCE_DATA_EMISSIVE((uint8_t)InTextureID, 0.6f)),
		mds::RAssetAuthority::Absolute,
		/*bInIsHandledAsSObject*/true,
		/*bInIsDrawnAsInstance*/true
	)
{
	auto& vertices = m_mainMesh->GetVertices();
	XMVECTOR3 lNormal;
	for (auto& vertice : vertices)
	{
		lNormal = XMLoadFloat3(&vertice.normal);
		XMStoreFloat3(&vertice.normal, XMVectorScale(lNormal, -1.f));
	}
}

RSRODrawableBackground::~RSRODrawableBackground()
{
}