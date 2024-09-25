#include "MDS/Tools/RLog.h"
#include <format>
#include <winnt.rh>

std::string mds::RLog::XMMatrixToString(const DirectX::XMMATRIX& InMatrix)
{
    std::string result;
    DirectX::XMFLOAT4X4 elements;
    DirectX::XMStoreFloat4x4(&elements, InMatrix);
    return XMFloat4x4ToString(elements);
}

std::string mds::RLog::XMFloat4x4ToString(const DirectX::XMFLOAT4X4& InFloat4x4)
{
    std::string result;
    result.reserve(120);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result += std::to_string(InFloat4x4.m[i][j]) + "\t";
        }
        result += "\n";
    }

    return result;
}

std::string mds::RLog::XMFloat4ToString(const DirectX::XMFLOAT4& InFloat4)
{
    return std::format(TEXT("[ x : {}, y : {}, z : {}, w : {}]"), InFloat4.x, InFloat4.y, InFloat4.z, InFloat4.w);
}

std::string mds::RLog::XMFloat3ToString(const DirectX::XMFLOAT3& InFloat3)
{
    return std::format(TEXT("[ x : {}, y : {}, z : {}]"), InFloat3.x, InFloat3.y, InFloat3.z);
}

std::string mds::RLog::XMFloat2ToString(const DirectX::XMFLOAT2& InFloat2)
{
    return std::format(TEXT("[ x : {}, y : {}]"), InFloat2.x, InFloat2.y);
}

std::string mds::RLog::XMInt4ToString(const DirectX::XMINT4& InInt4)
{
    return std::format(TEXT("[ x : {}, y : {}, z : {}, w : {}]"), InInt4.x, InInt4.y, InInt4.z, InInt4.w);
}

std::string mds::RLog::XMInt3ToString(const DirectX::XMINT3& InInt3)
{
    return std::format(TEXT("[ x : {}, y : {}, z : {}]"), InInt3.x, InInt3.y, InInt3.z);
}

std::string mds::RLog::XMInt2ToString(const DirectX::XMINT2& InInt2)
{
    return std::format(TEXT("[ x : {}, y : {}]"), InInt2.x, InInt2.y);
}

std::string mds::RLog::XMVECTORToString(DirectX::FXMVECTOR InVector, const uint8_t InVectorSize)
{
    std::string returnValue;
    DirectX::XMFLOAT4 flt;
    DirectX::XMStoreFloat4(&flt, InVector);

    switch (InVectorSize)
    {
    case 4ui8:
        return std::format(TEXT("( X : {}, Y : {}, Z : {}, W : {}) "), flt.x, flt.y, flt.z, flt.w);
    case 3ui8:
        return std::format(TEXT("( X : {}, Y : {}, Z : {}) "), flt.x, flt.y, flt.z);
    case 2ui8:
        return std::format(TEXT("( X : {}, Y : {}) "), flt.x, flt.y);
    case 1ui8 : 
        return std::format(TEXT("( X : {} ) "), flt.x);
    default:
        return std::format(TEXT("( invalid vector size {} ) "), InVectorSize);
    }
}