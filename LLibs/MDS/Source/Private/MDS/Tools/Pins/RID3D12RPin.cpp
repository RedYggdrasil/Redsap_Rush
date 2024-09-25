#include "MDS/Tools/Pins/RID3D12RPin.h"
#include <utility>

mds::RPin* mds::RID3D12RPin::Clone()
{
    return new RID3D12RPin(*this);
}

bool mds::RID3D12RPin::Release()
{
    return
        RID3D12RPin::InternalRelease()
        /* &&
        RPin::Release()*/; //Release is pure virtual
}


mds::RID3D12RPin::RID3D12RPin(ID3D12Resource2* InResource)
: m_d3d12Resource(InResource)
{
}

mds::RID3D12RPin::RID3D12RPin(const RID3D12RPin& InOther)
: m_d3d12Resource(InOther.m_d3d12Resource)
{
}

mds::RID3D12RPin::RID3D12RPin(RID3D12RPin&& RVOther) noexcept
: m_d3d12Resource()
{
    *this = std::move(RVOther);
}

mds::RID3D12RPin& mds::RID3D12RPin::operator=(const RID3D12RPin& InOther)
{
    InOther.m_d3d12Resource.CopyTo(IID_PPV_ARGS(&this->m_d3d12Resource));
    return *this;
}

mds::RID3D12RPin& mds::RID3D12RPin::operator=(RID3D12RPin&& RVOther) noexcept
{
    if (this != &RVOther)
    {
        //Reset probably not needed
        m_d3d12Resource.Reset();
        m_d3d12Resource = std::move(RVOther.m_d3d12Resource);
    }
    return *this;
}

mds::RID3D12RPin::~RID3D12RPin()
{
    RID3D12RPin::InternalRelease();
    //Now Base destructor will be called
}

bool mds::RID3D12RPin::InternalRelease()
{
    m_d3d12Resource.Reset();
    return true;
}
