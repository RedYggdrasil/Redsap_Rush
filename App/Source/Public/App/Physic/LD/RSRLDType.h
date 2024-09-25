#pragma once
#include "MDS/Tools/REnum.h"

namespace RSRush
{
	enum class RSRLDType : uint32_t
	{
		RENUM_MIN_VAL(Unknown, 0),
		Square = 1,
		RENUM_MAX_VAL(Sphere, 2)
	};
	RS_DEFINE_ENUM(RSRLDType);
};