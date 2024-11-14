#pragma once
#include "App/SceneObject/RSRSObjectInstance.h"

namespace RSRush
{
	class RSRBasicShapes;
	class RSRODrawableBackground : public RSRSObjectInstance
	{
	public:
		RSRODrawableBackground(RSRBasicShapes* InBasicShapes, const RSRTransform& InTransform, const uint16_t InTextureID);
		R_VIRTUAL_IMPLICIT ~RSRODrawableBackground() R_OVERRIDE_IMPLICIT;
	};
};
