#pragma once
#include <cstdint>

namespace mds
{
    class RTimekeeper
    {
    protected:

        static double sm_lastCpuDeltaSecond;
        static double sm_fromStartCpuSecond;

    public:
        /// <summary>
        /// Initialize timer and sleep for the duration of the given time to initialize first frame
        /// </summary>
        /// <param name="nbSleepFirstFrameMilisecondTime"></param>
        static void Initialize(unsigned int nbSleepFirstFrameMilisecondTime = 16);

        /// <summary>
        /// report a new Frame and return last tick
        /// </summary>
        /// <returns>Computed GetLastTick() in second</returns>
        static double BeginNewFrame();

        static inline double GetLastTick() { return sm_lastCpuDeltaSecond; }
        static inline double GetFromStartSeconds() { return sm_fromStartCpuSecond; }

        static void SleepFor(unsigned int nbMilliseconds);
    protected:
    };

};