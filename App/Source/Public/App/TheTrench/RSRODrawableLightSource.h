#pragma once
#include "App/SceneObject/RSRSObject.h"

namespace RSRush
{
	class RSRODrawableLightSource : public RSRSObject
	{
	public:
		RSRODrawableLightSource(const RSRTransform& InTransform, const bool bInHandleAsSObject, const uint16_t InTextureID);
		R_VIRTUAL_IMPLICIT ~RSRODrawableLightSource() R_OVERRIDE_IMPLICIT;

		////Handled as instanced now
		//virtual bool DrawGeometry(ID3D12GraphicsCommandList7* InDraw3DCommandList) const override { return true; }

	};
};
