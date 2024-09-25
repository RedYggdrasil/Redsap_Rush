#include "App/System/RSRRandomizer.h"

RSRush::RSRRandomizer::RSRRandomizer()
{
}

void RSRush::RSRRandomizer::Initialize(const uint64_t InSeed)
{
	mds::RRandomizer::Initialize(InSeed);
	this->SetDefaultInstance(this);
}
