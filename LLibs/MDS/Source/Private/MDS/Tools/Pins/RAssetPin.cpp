#include "MDS/Tools/Pins/RAssetPin.h"
#include <utility>

mds::RPin* mds::RAssetPin::Clone()
{
    return new RAssetPin(*this);
}

bool mds::RAssetPin::Release()
{
    return
        RAssetPin::InternalRelease()
        /* &&
        RPin::Release()*/; //Release is pure virtual
}


mds::RAssetPin::RAssetPin(RAsset* InAsset)
    : m_asset(InAsset)
{
}

mds::RAssetPin::RAssetPin(const RAssetPin& InOther)
    : m_asset(InOther.m_asset)
{
}

mds::RAssetPin::RAssetPin(RAssetPin&& RVOther) noexcept
: m_asset()
{
    *this = std::move(RVOther);
}

mds::RAssetPin& mds::RAssetPin::operator=(const RAssetPin& InOther)
{
    this->m_asset = InOther.m_asset;
    return *this;
}

mds::RAssetPin& mds::RAssetPin::operator=(RAssetPin&& RVOther) noexcept
{
    if (this != &RVOther)
    {
        //Reset probably not needed
        m_asset.reset();
        m_asset = std::move(RVOther.m_asset);
    }
    return *this;
}

mds::RAssetPin::~RAssetPin()
{
    RAssetPin::InternalRelease();
    //Now Base destructor will be called
}

bool mds::RAssetPin::InternalRelease()
{
    m_asset.reset();
    return true;
}
