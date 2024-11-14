#include "MDS/Tools/Assets/RAssetType.h"
#include <iostream>

namespace mds 
{ 
	const std::filesystem::path DYNAMIC_ASSET_PATH		= std::filesystem::path(DYNAMIC_ASSET_FOLDER_NAME);
	const std::string DYNAMIC_ASSET_PATH_NAME			= std::string(DYNAMIC_ASSET_FOLDER_NAME);
	const std::string_view DYNAMIC_ASSET_PATH_NAME_VIEW = std::string_view(mds::DYNAMIC_ASSET_PATH_NAME);
}


const std::string UNKNOWN_NAME = "Unknown";
const std::string MESH_NAME = "Mesh";
const std::string TEXTURE2D_NAME = "Texture2D";

const std::string TYPE_SEPARATOR = "|";

const std::string UNKNOWN_NAME_AND_SEPARATOR = UNKNOWN_NAME + TYPE_SEPARATOR;
const std::string MESH_NAME_AND_SEPARATOR = MESH_NAME + TYPE_SEPARATOR;
const std::string TEXTURE2D_NAME_AND_SEPARATOR = TEXTURE2D_NAME + TYPE_SEPARATOR;

const std::string& mds::RAssetType_Func::ToString(const mds::RAssetType InType)
{
	switch (InType)
	{
	case RAssetType::Unknown :
		return UNKNOWN_NAME;
	case RAssetType::Mesh:
		return MESH_NAME;
	case RAssetType::Texture2D:
		return TEXTURE2D_NAME;
	default:
		std::cout << "[MDSError] Not implemented default case in switch !" << std::endl;
		return UNKNOWN_NAME;
	}
}
const std::string& AssetTypeAndSeparator(const mds::RAssetType InType)
{
	switch (InType)
	{
	case mds::RAssetType::Unknown:
		return UNKNOWN_NAME_AND_SEPARATOR;
	case mds::RAssetType::Mesh:
		return MESH_NAME_AND_SEPARATOR;
	case mds::RAssetType::Texture2D:
		return TEXTURE2D_NAME_AND_SEPARATOR;
	default:
		std::cout << "[MDSError] Not implemented default case in switch !" << std::endl;
		return UNKNOWN_NAME_AND_SEPARATOR;
	}
}

std::string mds::NameAsset(const RAssetType InType, const std::string_view InAssetFilePath)
{
	return AssetTypeAndSeparator(InType) + std::string(InAssetFilePath);
}
std::string mds::NameAsset(const mds::RAssetType InType, const std::string_view InAssetPath, const std::string_view InFileName)
{
	return AssetTypeAndSeparator(InType) + std::string(InAssetPath) + std::string(InFileName);
}
std::string mds::NameDynamicAsset(const mds::RAssetType InType, const std::string_view InFileName)
{
	std::string dynamicPath = (mds::DYNAMIC_ASSET_PATH / InFileName).string();
	return mds::NameAsset(InType, dynamicPath);
}
