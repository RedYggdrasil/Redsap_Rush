#pragma once
#include "App/SceneObject/RSRSObjectInstance.h"

namespace RSRush
{
	class RSRODrawableBackground : public RSRSObjectInstance
	{
	public:
		RSRODrawableBackground(const RSRTransform& InTransform, const uint16_t InTextureID);
		R_VIRTUAL_IMPLICIT ~RSRODrawableBackground() R_OVERRIDE_IMPLICIT;
	};
};
