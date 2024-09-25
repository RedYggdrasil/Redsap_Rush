#include "mds/Tools/System/RTimekeeper.h"
#include <assert.h>
#include <windows.h>

#include <chrono>
#include <thread>

//avoid inclusion of windows in .h
//static member are inclosed in TU
static LARGE_INTEGER sm_Frequency;
static LARGE_INTEGER sm_StartTime;

static LARGE_INTEGER sm_PreviousTime;
static LARGE_INTEGER sm_CurrentTime;

static LARGE_INTEGER sm_LastElapsedTicks;
static LARGE_INTEGER sm_TotalElapsedTicks;

double mds::RTimekeeper::sm_lastCpuDeltaSecond = 0.0;
double mds::RTimekeeper::sm_fromStartCpuSecond = 0.0;

static double ComputeSecondTimeFromElapsedTicks(const LARGE_INTEGER& InElaspedTicks) 
{ 
	return static_cast<double>(InElaspedTicks.QuadPart) / sm_Frequency.QuadPart; 
}

void mds::RTimekeeper::Initialize(unsigned int nbSleepFirstFrameMilisecondTime)
{
	assert(TRUE == QueryPerformanceFrequency(&sm_Frequency) && "System incompatible with performance counter");

	QueryPerformanceFrequency(&sm_Frequency);

	QueryPerformanceCounter(&sm_StartTime);
	sm_PreviousTime = sm_StartTime;

	sm_CurrentTime = sm_PreviousTime;

	//force first frame to be nb miliseconds
	SleepFor(nbSleepFirstFrameMilisecondTime);

	double _ = BeginNewFrame();

}

double mds::RTimekeeper::BeginNewFrame()
{
	sm_PreviousTime = sm_CurrentTime;
	QueryPerformanceCounter(&sm_CurrentTime);

	sm_LastElapsedTicks.QuadPart = sm_CurrentTime.QuadPart - sm_PreviousTime.QuadPart;
	sm_TotalElapsedTicks.QuadPart = sm_CurrentTime.QuadPart - sm_StartTime.QuadPart;

	sm_lastCpuDeltaSecond = ComputeSecondTimeFromElapsedTicks(sm_LastElapsedTicks);
	sm_fromStartCpuSecond = ComputeSecondTimeFromElapsedTicks(sm_TotalElapsedTicks);

	return sm_lastCpuDeltaSecond;
}

void mds::RTimekeeper::SleepFor(unsigned int nbMilliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(nbMilliseconds));
}
