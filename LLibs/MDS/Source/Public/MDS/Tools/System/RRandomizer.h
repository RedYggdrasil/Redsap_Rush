#pragma once
#include <type_traits>

namespace mds
{
    class RRandomizer
    {
    protected:
        static RRandomizer* mp_inst;
    protected:
        uint64_t m_seed;

    public:
        virtual void Initialize(const uint64_t InSeed);
        void Initialize();
    protected:
        RRandomizer() = default;
        void SetDefaultInstance(RRandomizer* InDefaultInstance);
    };
};