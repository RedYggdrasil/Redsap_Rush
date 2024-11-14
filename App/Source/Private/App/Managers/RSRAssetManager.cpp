#include "App/Managers/RSRAssetManager.h"

#ifndef RSRPI_INCLUDED
#include "App/Game/RSRProgramInstance.h"

#endif

#include <Gen_App/Config/AppConfig.h>
#include "App/Data/Textures/RSRTexture2D.h"
#include "App/Geometry/RSRMeshLoader.h"
#include <string>
#include <string_view>

using namespace RSRush;

RSRAssetManager::RSRAssetManager(RSRProgramInstance* InProgramInstance)
: mds::IRProgramMemElem(InProgramInstance)
{
}

void RSRush::RSRAssetManager::Init()
{
	m_pinnedAssets.clear();
	m_assets.clear();
}

void RSRush::RSRAssetManager::ShutDown()
{
	m_pinnedAssets.clear();
	m_assets.clear();
}


RSRAssetManager* RSRush::RSRAssetManager::Get(const mds::IRProgramMemElem* InProgramMemElem)
{
	return InProgramMemElem->GetRoot<RSRProgramInstance>()->GetAssetManager();
}

RSRAssetManager* RSRush::RSRAssetManager::Get(RSRProgramInstance* InProgramInstance)
{
	return InProgramInstance->GetAssetManager();
}

bool RSRAssetManager::Exist(const std::string_view InKey) const
{
	auto pair = m_assets.find(InKey);
	if (pair != m_assets.end())
	{
		return !pair->second.expired();
	}
	return false;
}

RSRSharedAssetPtr RSRAssetManager::GetShared(const std::string_view InKey)
{
	auto pair = m_assets.find(InKey);
	if (pair != m_assets.end())
	{
		if (!pair->second.expired())
		{
			return pair->second.lock();
		}
	}
	return nullptr;
}

bool RSRAssetManager::AddAsset(const std::string_view InKey, RSRSharedAssetPtr InAsset, bool bInPin)
{
	if (!InAsset)
	{
		return false;
	}
	if (Exist(InKey))
	{
		RSRSharedAssetPtr ExistingAsset = GetShared(InKey);
		if (ExistingAsset != InAsset)
		{
			return false;
		}
		if (bInPin != IsPinAsset(InKey))
		{
			SetPinAsset(InKey, bInPin);
		}
		return true;
	}
	m_assets[std::string(InKey)] = RSRSharedAssetPtr(InAsset);
	if (bInPin)
	{
		PinAsset(InKey);
	}
	return true;
}

std::shared_ptr<RSRTexture2D> RSRAssetManager::AddTextureAsset(const std::string_view InTextureAssetPath, bool bInPin)
{
	std::string _;
	return AddTextureAsset(InTextureAssetPath, bInPin, /*Out*/_);
}

std::shared_ptr<RSRTexture2D> RSRush::RSRAssetManager::AddTextureAsset(const std::string_view InTextureAssetPath, bool bInPin, std::string& OutResultName)
{
	OutResultName = mds::NameAsset(mds::RAssetType::Texture2D, InTextureAssetPath);
	std::string assetPath = (RSRPaths::AssetsDiskPath() / InTextureAssetPath).string();
	return AddAsset<RSRTexture2D>(
		OutResultName,
		bInPin,
		assetPath
	);
}

std::shared_ptr<RSRMesh3D> RSRush::RSRAssetManager::AddMesh3DAsset(const std::string_view InMeshAssetPath, bool bInPin, RSRTextureLibrary* InTextureLibrary)
{
	std::string _;
	return AddMesh3DAsset(InMeshAssetPath, bInPin, InTextureLibrary, /*Out*/_);
}

std::shared_ptr<RSRMesh3D> RSRush::RSRAssetManager::AddMesh3DAsset(const std::string_view InMeshAssetPath, bool bInPin, RSRTextureLibrary* InTextureLibrary, std::string& OutResultName)
{
	OutResultName = mds::NameAsset(mds::RAssetType::Mesh, InMeshAssetPath);
	std::string assetPath = (RSRPaths::AssetsDiskPath() / InMeshAssetPath).string();
	std::shared_ptr<RSRMesh3D> loadedMesh = RSRMeshLoader::Get(this)->LoadMesh3DFromPath(assetPath, InTextureLibrary);
	if (AddAsset(OutResultName, loadedMesh, bInPin))
	{
		return loadedMesh;
	}
	return nullptr;
}

long RSRAssetManager::RemoveAsset(const std::string_view InKey)
{
	if (Exist(InKey))
	{
		UnpinAsset(InKey);
		RSRWeakAssetPtr AssetWPtr = m_assets.find(InKey)->second;
		m_assets.erase(std::string(InKey));
		return AssetWPtr.use_count();
	}
	else
	{
		auto pair = m_assets.find(InKey);
		if (pair != m_assets.end())
		{
			m_assets.erase(std::string(InKey));
			return 0;
		}
	}
	return -1;
}

bool RSRAssetManager::SetPinAsset(const std::string_view InKey, bool bInIsPin)
{
	if (bInIsPin)
	{
		return PinAsset(InKey);
	}
	UnpinAsset(InKey);
	return true;
}

bool RSRAssetManager::PinAsset(const std::string_view InKey)
{
	RSRSharedAssetPtr AssetPtr = GetShared(InKey);
	if (AssetPtr)
	{
		m_pinnedAssets[std::string(InKey)] = AssetPtr;
		return true;
	}
	return false;
}

void RSRAssetManager::UnpinAsset(const std::string_view InKey)
{
	if (IsPinAsset(InKey))
	{
		m_pinnedAssets.erase(std::string(InKey));
	}
}