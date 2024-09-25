#pragma once
#include "MDS/Tools/REnum.h"

namespace RSRush
{
	enum class RSRCollisionType : uint8_t
	{
		RENUM_MIN_VAL(None, 0),
		Overlap = 1,
		RENUM_MAX_VAL(Collide, 2)
	};
	RS_DEFINE_ENUM(RSRCollisionType);
};