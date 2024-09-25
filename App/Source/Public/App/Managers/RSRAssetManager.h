#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "MDS/Tools/Assets/RAsset.h"
#include "MDS/Tools/Pins/RAssetPin.h"
#include "MDS/Tools/RCollection.h"

#include "App/Game/RSRPaths.h"

namespace RSRush
{
	class RSRTexture2D;
	typedef std::weak_ptr<mds::RAsset> RSRWeakAssetPtr;
	typedef std::shared_ptr<mds::RAsset> RSRSharedAssetPtr;
	class RSRAssetManager
	{
	private:
		static RSRAssetManager Instance;
	protected:
		mds::RUnorderedStringMap<RSRSharedAssetPtr> m_pinnedAssets;
		mds::RUnorderedStringMap<RSRWeakAssetPtr> m_assets;

	public:
#pragma region Instance
		RSRAssetManager(const RSRAssetManager&) = delete;
		RSRAssetManager& operator=(const RSRAssetManager&) = delete;
		static RSRAssetManager& Get() { return Instance; }
#pragma endregion
	public:
#pragma region Asset
		bool Exist(const std::string_view InKey) const;
		RSRSharedAssetPtr GetShared(const std::string_view InKey);
		
		//Play with fire, get burned
		template<class AssetType>
		std::shared_ptr<AssetType> GetShared(const std::string_view InKey)
		{
			return std::static_pointer_cast<AssetType>(GetShared(InKey));
		}

		/// <summary>
		/// Try adding Asset
		/// </summary>
		/// <param name="InKey">Key, add or exist</param>
		/// <param name="InAsset">Asset pointer, add or same exist</param>
		/// <param name="bInPin">Pin asset, regardless of alread present status</param>
		/// <returns>false if another asset is present with the given key</returns>
		bool AddAsset(const std::string_view InKey, RSRush::RSRSharedAssetPtr InAsset, bool bInPin = false);
		
		template<class AssetType, typename... Args>
		std::shared_ptr<AssetType> AddAsset(const std::string_view InKey, bool bInPin, Args&&... args)
		{
			std::shared_ptr<AssetType> TypedAssetPtr = std::make_shared<AssetType>(std::forward<Args>(args)...);
			if (AddAsset(InKey, (RSRush::RSRSharedAssetPtr)TypedAssetPtr, bInPin))
			{
				return TypedAssetPtr;
			}
			return nullptr;
		}

		std::shared_ptr<RSRTexture2D> AddTextureAsset(const std::string_view InTextureAssetPath, bool bInPin);

		/// <summary>
		/// Remove asset associated with the given key, Unpin if pinned
		/// </summary>
		/// <param name="InKey">The key to remove</param>
		/// <returns>number of existing shared Ptr or -1 if Asset not found</returns>
		long RemoveAsset(const std::string_view InKey);

		bool SetPinAsset(const std::string_view InKey, bool bInIsPin);
		bool PinAsset(const std::string_view InKey);
		void UnpinAsset(const std::string_view InKey);
		bool IsPinAsset(const std::string_view InKey) const { return m_pinnedAssets.contains(InKey); }

#pragma endregion

	private:
		RSRAssetManager();
	};
}