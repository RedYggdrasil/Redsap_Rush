#pragma once

#include "MDS/Tools/Assets/RAssetType.h"
#include "MDS/Tools/RResourceStateType.h"

namespace mds
{
	enum class RAssetAuthority : uint8_t
	{
		RENUM_MIN_VAL(Unknown, 0),
		None = 1,
		Shared = 2,
		RENUM_MAX_VAL(Absolute, 3)
	};
	RS_DEFINE_ENUM(RAssetAuthority);
	inline constexpr bool HasAssetAssetAuthority(const RAssetAuthority InAuthorityType)
	{ return InAuthorityType >= RAssetAuthority::Shared; }

	class RAsset
	{
	protected:
		RAssetType m_type = RAssetType::Unknown;
		RResourceStateType m_resourceState = RResourceStateType::Unknown;
	public:
		inline RAssetType GetType() const { return m_type; }
		inline RResourceStateType GetResourceState() const { return m_resourceState; }
		virtual ~RAsset() {};
	protected:
		RAsset() = default;
	};
};