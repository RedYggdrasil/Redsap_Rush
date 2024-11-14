#pragma once

#include <stdint.h>
#include <filesystem>
#include <string>
#include <string_view>
namespace mds
{
	constexpr const char* DYNAMIC_ASSET_FOLDER_NAME = "ResevedDynamicAsset";

	extern const std::filesystem::path DYNAMIC_ASSET_PATH;
	extern const std::string DYNAMIC_ASSET_PATH_NAME;
	extern const std::string_view DYNAMIC_ASSET_PATH_NAME_VIEW;

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