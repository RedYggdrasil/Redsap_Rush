#pragma once
#include <stdint.h>

namespace RSRush
{
	using RSRPhysicBodyID = size_t;
	constexpr RSRPhysicBodyID NONE = 0;
	struct RSRPBIDProvider
	{
	private:
		static RSRPhysicBodyID s_nextPhysicBodyID;
	public:
		static RSRPhysicBodyID NewBodyID();
	public:
		RSRPBIDProvider() = delete;
	};

};