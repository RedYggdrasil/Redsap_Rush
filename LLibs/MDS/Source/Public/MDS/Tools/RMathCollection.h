#pragma once

#include "DirectXMath.h"
#include <functional>

namespace DirectX
{
    struct XMINT2Hash {
        std::size_t operator()(const DirectX::XMINT2& k) const {
            return std::hash<int32_t>()(k.x) ^ std::hash<int32_t>()(k.y);
        }
    };

    struct XMINT2Equal {
        bool operator()(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs) const {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
    struct XMINT2Different {
        bool operator()(const DirectX::XMINT2& lhs, const DirectX::XMINT2& rhs) const {
            return lhs.x != rhs.x || lhs.y != rhs.y;
        }
    };
}