#include "App/Managers/RSRAssetManager.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include <Gen_App/Config/AppConfig.h>
#include <string>
#include <string_view>

RSRush::RSRAssetManager RSRush::RSRAssetManager::Instance = RSRush::RSRAssetManager();

bool RSRush::RSRAssetManager::Exist(const std::string_view InKey) const
{
	auto pair = m_assets.find(InKey);
	if (pair != m_assets.end())
	{
		return !pair->second.expired();
	}
	return false;
}

RSRush::RSRSharedAssetPtr RSRush::RSRAssetManager::GetShared(const std::string_view InKey)
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

bool RSRush::RSRAssetManager::AddAsset(const std::string_view InKey, RSRush::RSRSharedAssetPtr InAsset, bool bInPin)
{
	if (!InAsset)
	{
		return false;
	}
	if (Exist(InKey))
	{
		RSRush::RSRSharedAssetPtr ExistingAsset = GetShared(InKey);
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

std::shared_ptr<RSRush::RSRTexture2D> RSRush::RSRAssetManager::AddTextureAsset(const std::string_view InTextureAssetPath, bool bInPin)
{
	std::string assetPath = (RSRPaths::AssetsDiskPath() / InTextureAssetPath).string();
	return AddAsset<RSRush::RSRTexture2D>(
		mds::NameAsset(mds::RAssetType::Texture2D, assetPath),
		bInPin,
		assetPath
	);

}

long RSRush::RSRAssetManager::RemoveAsset(const std::string_view InKey)
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

bool RSRush::RSRAssetManager::SetPinAsset(const std::string_view InKey, bool bInIsPin)
{
	if (bInIsPin)
	{
		return PinAsset(InKey);
	}
	UnpinAsset(InKey);
	return true;
}

bool RSRush::RSRAssetManager::PinAsset(const std::string_view InKey)
{
	RSRSharedAssetPtr AssetPtr = GetShared(InKey);
	if (AssetPtr)
	{
		m_pinnedAssets[std::string(InKey)] = AssetPtr;
		return true;
	}
	return false;
}

void RSRush::RSRAssetManager::UnpinAsset(const std::string_view InKey)
{
	if (IsPinAsset(InKey))
	{
		m_pinnedAssets.erase(std::string(InKey));
	}
}

RSRush::RSRAssetManager::RSRAssetManager()
{
}
