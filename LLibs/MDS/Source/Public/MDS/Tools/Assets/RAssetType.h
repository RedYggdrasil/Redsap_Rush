#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
namespace mds
{
	constexpr const char* DYNAMIC_ASSET_PATH = "ResevedDynamicAsset/";
	enum class RAssetType : uint8_t
	{
		Unknown = 0,
		Mesh = 1,
		Texture2D = 2
	};
	class RAssetType_Func
	{
	public:
		RAssetType_Func() = delete;
		RAssetType_Func(const RAssetType_Func&) = delete;
		RAssetType_Func& operator= (const RAssetType_Func&) = delete;

	public:
		static const std::string& ToString(const RAssetType InType);
	};
	std::string NameAsset(const RAssetType InType, const std::string_view InAssetFilePath);
	std::string NameAsset(const RAssetType InType, const std::string_view InAssetPath, const std::string_view InFileName);
	std::string NameDynamicAsset(const RAssetType InType, const std::string_view InFileName);
};