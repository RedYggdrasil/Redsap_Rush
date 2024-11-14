#pragma once

#include "MDS/Tools/System/IRProgramMemNode.h"
#include "MDS/Tools/RCollection.h"

#include "App/Libs/WinInclude.h"

#include <memory>
#include <unordered_set>
#include <vector>
#include <wrl/client.h>

namespace RSRush
{
	class RSRTexture2D;
	class RSRTextureLibrary : public mds::IRProgramMemNode
	{
	private:
		std::vector<std::shared_ptr<RSRush::RSRTexture2D>> m_textures;
		mds::RUnorderedStringMap<uint16_t> m_pathToTextureIndex;
		mds::RUnorderedStringMap<uint16_t> m_nameToTextureIndex;
	public:
		inline bool ExistPath(const std::string_view InTexturePath) const { return m_pathToTextureIndex.contains(InTexturePath); }
		inline bool ExistName(const std::string_view InTextureName) const { return m_nameToTextureIndex.contains(InTextureName); }

		inline uint16_t GetIndexFromPath(const std::string_view InTexturePath, bool& bOutExist) const;
		uint16_t GetIndexFromPath(const std::string_view InTexturePath) const { bool _; return GetIndexFromPath(InTexturePath, _); }

		inline uint16_t GetIndexFromName(const std::string_view InTextureName, bool& bOutExist) const;
		uint16_t GetIndexFromName(const std::string_view InTextureName) const { bool _; return GetIndexFromName(InTextureName, _); }

		std::shared_ptr<RSRush::RSRTexture2D> AddGetByPath(const std::string_view InTexturePath);
		std::shared_ptr<RSRush::RSRTexture2D> AddGetDynamicByPath(const std::string_view InTexturePath, std::shared_ptr<RSRush::RSRTexture2D>&& InTexture);


	public:
		bool UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList);
		bool FreeUploadBuffer();
		bool CreateSRVHeapForTextures(ID3D12Device10* InDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& InOutSrvheap);
		bool FreeResourcesBuffer();
		bool UnLoad();


	public:
		static std::shared_ptr<RSRTextureLibrary> Create(mds::IRProgramMemElem* InParentMemElem);
		RSRTextureLibrary();
	
	public:
		R_VIRTUAL_IMPLICIT ~RSRTextureLibrary() R_OVERRIDE_IMPLICIT;
	private:
		std::shared_ptr<RSRush::RSRTexture2D> INTERNAL_Add(const std::string_view InTexturePath, const std::string_view InTextureName, std::shared_ptr<RSRush::RSRTexture2D>&& InTexture, uint16_t& OutIndex);
	};
}