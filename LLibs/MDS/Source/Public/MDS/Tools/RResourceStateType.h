#pragma once

#include "MDS/Tools/REnum.h"
#include <stdint.h>
#include <string>
#include <string_view>

namespace mds
{
	enum class RResourceStateType : uint8_t
	{
		RENUM_MIN_VAL(Unknown, 0),
		NeedUpload = 1,
		WaitingUpload = 2,
		RENUM_MAX_VAL(ResourceLive, 3)
	};
	RS_DEFINE_ENUM(RResourceStateType);

	class RResourceStateType_Func
	{
	public:
		RResourceStateType_Func() = delete;
		RResourceStateType_Func(const RResourceStateType_Func&) = delete;
		RResourceStateType_Func& operator= (const RResourceStateType_Func&) = delete;

	public:
		static const std::string& ToString(const RResourceStateType InType);
	};
};