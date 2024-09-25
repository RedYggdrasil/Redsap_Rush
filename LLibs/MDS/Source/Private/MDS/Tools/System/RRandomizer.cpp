#include "MDS/Tools/System/RRandomizer.h"
#include <random>

mds::RRandomizer* mds::RRandomizer::mp_inst = nullptr;

void mds::RRandomizer::Initialize(const uint64_t InSeed)
{
	m_seed = InSeed;
}

void mds::RRandomizer::Initialize()
{
	std::random_device rd;   // non-deterministic generator
	std::mt19937_64 gen(rd());  // to seed mersenne twister.
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
	return Initialize(dist(gen));
}

void mds::RRandomizer::SetDefaultInstance(RRandomizer* InDefaultInstance)
{
	mds::RRandomizer::mp_inst = InDefaultInstance;
}
