#pragma once

#include "MDS/Tools/System/RRandomizer.h"
#include <random>
namespace RSRush
{
	class RSRRandomizer : public mds::RRandomizer
	{
	public:
		RSRRandomizer();
		using mds::RRandomizer::Initialize;
		virtual void Initialize(const uint64_t InSeed) override;
	public:
		inline static bool GetIsInitialized() { return mp_inst; }
		inline static const RSRRandomizer& Get() { return *static_cast<RSRRandomizer*>(mp_inst); }
	public:
		inline std::mt19937_64 CreateTrenchManagerGenerator() const
		{
			return std::mt19937_64(uint64_t((312 << 16) ^ m_seed));
		}
		inline std::mt19937_64 GetTrenchGenerator(const uint64_t InTrenchID) const
		{ return std::mt19937_64(uint64_t((InTrenchID << 16) ^ m_seed)); }
	};
};