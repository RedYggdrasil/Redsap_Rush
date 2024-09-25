#include "MDS/Tools/Assets/RAssetType.h"
#include <iostream>

const std::string UNKNOWN_NAME = "Unknown";
const std::string MESH_NAME = "Mesh";
const std::string TEXTURE2D_NAME = "Texture2D";
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
std::string mds::NameAsset(const RAssetType InType, const std::string_view InAssetFilePath)
{
	return mds::RAssetType_Func::ToString(InType) + std::string(InAssetFilePath);
}
std::string mds::NameAsset(const mds::RAssetType InType, const std::string_view InAssetPath, const std::string_view InFileName)
{
	return mds::RAssetType_Func::ToString(InType) + std::string(InAssetPath) + std::string(InFileName);
}
std::string mds::NameDynamicAsset(const mds::RAssetType InType, const std::string_view InFileName)
{
	return mds::NameAsset(InType, mds::DYNAMIC_ASSET_PATH, InFileName);
}
