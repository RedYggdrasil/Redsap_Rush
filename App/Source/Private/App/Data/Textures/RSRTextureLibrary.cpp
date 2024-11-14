#include "App/Data/Textures/RSRTextureLibrary.h"
#include "App/Data/Textures/RSRTexture2D.h"
#include "App/Managers/RSRAssetManager.h"

using namespace RSRush;

inline uint16_t RSRush::RSRTextureLibrary::GetIndexFromPath(const std::string_view InTexturePath, bool& bOutExist) const
{
	const auto it = m_pathToTextureIndex.find(InTexturePath);
	if (it != m_pathToTextureIndex.end())
	{
		bOutExist = true;
		return it->second;
	}
	bOutExist = false;
	return std::numeric_limits<uint16_t>::max();
}

inline uint16_t RSRush::RSRTextureLibrary::GetIndexFromName(const std::string_view InTextureName, bool& bOutExist) const
{
	const auto it = m_nameToTextureIndex.find(InTextureName);
	if (it != m_nameToTextureIndex.end())
	{
		bOutExist = true;
		return it->second;
	}
	bOutExist = false;
	return std::numeric_limits<uint16_t>::max();
}

std::shared_ptr<RSRTexture2D> RSRTextureLibrary::AddGetByPath(const std::string_view InTexturePath)
{
	bool bExist = false;
	uint16_t textureIndex = GetIndexFromPath(InTexturePath, /*Out*/bExist);
	if (!bExist)
	{
		std::string assetName;
		std::shared_ptr<RSRTexture2D> addedTexture = RSRAssetManager::Get(this)->AddTextureAsset(InTexturePath, false, /*Out*/assetName);
		return INTERNAL_Add(InTexturePath, assetName, std::move(addedTexture), textureIndex);
	}
	return m_textures[textureIndex];
}

std::shared_ptr<RSRush::RSRTexture2D> RSRush::RSRTextureLibrary::AddGetDynamicByPath(const std::string_view InTexturePath, std::shared_ptr<RSRush::RSRTexture2D>&& InTexture)
{
	bool bExist = false;
	uint16_t textureIndex = GetIndexFromPath(InTexturePath, /*Out*/bExist);
	if (!bExist)
	{
		std::string assetName = mds::NameDynamicAsset(mds::RAssetType::Texture2D, InTexturePath);
		std::shared_ptr<RSRTexture2D> existingAsset = INTERNAL_Add(InTexturePath, assetName, std::move(InTexture), /*Out*/textureIndex);
		if (!RSRAssetManager::Get(this)->Exist(assetName))
		{
			RSRAssetManager::Get(this)->AddAsset(assetName, existingAsset, false);
		}
	}
	return m_textures[textureIndex];
}


bool RSRTextureLibrary::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	bool bSuccessful = true;
	RSRTexture2D** uploadTextureArray = new RSRTexture2D* [m_textures.size()];
	UINT effectiveCount = 0;
	for (UINT i = 0; i < m_textures.size(); ++i)
	{
		std::shared_ptr<RSRTexture2D> texture = m_textures[i];
		if (texture->GetResourceState()  == mds::RResourceStateType::NeedUpload)
		{
			uploadTextureArray[effectiveCount] = texture.get();
			++effectiveCount;
		}
	}

	if (effectiveCount > 0)
	{
		bSuccessful = RSRTexture2D::UploadResources(effectiveCount, uploadTextureArray, InDevice, InUploadCommandList) && bSuccessful;
	}

	delete[] uploadTextureArray;
	return bSuccessful;
}

bool RSRush::RSRTextureLibrary::FreeUploadBuffer()
{
	bool bAllSucessfull = true;
	for (std::shared_ptr<RSRTexture2D> texture : m_textures)
	{
		if (texture->GetResourceState() == mds::RResourceStateType::WaitingUpload)
		{
			bAllSucessfull = texture->FreeUploadBuffer() && bAllSucessfull;
		}
	}
	return bAllSucessfull;
}

bool RSRush::RSRTextureLibrary::CreateSRVHeapForTextures(ID3D12Device10* InDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& InOutSrvheap)
{
	return RSRTexture2D::CreateSRVHeapForTextures(m_textures, InDevice, InOutSrvheap);
}

bool RSRush::RSRTextureLibrary::FreeResourcesBuffer()
{
	bool bAllSucessfull = true;
	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		bAllSucessfull = m_textures[i]->FreeResourcesBuffer() && bAllSucessfull;
	}
	return bAllSucessfull;
}

bool RSRush::RSRTextureLibrary::UnLoad()
{
	bool bAllSucessfull = FreeResourcesBuffer();

	RSRAssetManager* assetManager = RSRAssetManager::Get(this);
	for (auto& pair : m_nameToTextureIndex)
	{
		assetManager->RemoveAsset(pair.first);
	}
	m_pathToTextureIndex.clear();
	m_nameToTextureIndex.clear();
	m_textures.clear();
	return bAllSucessfull;
}

std::shared_ptr<RSRTextureLibrary> RSRush::RSRTextureLibrary::Create(mds::IRProgramMemElem* InParentMemElem)
{
	std::shared_ptr<RSRTextureLibrary> textureLibrary = std::make_shared<RSRTextureLibrary>();
	textureLibrary->InitMemNode(InParentMemElem, textureLibrary);
	return textureLibrary;
}

RSRTextureLibrary::RSRTextureLibrary()
: mds::IRProgramMemNode(), m_textures(), m_pathToTextureIndex(), m_nameToTextureIndex()
{
}

RSRTextureLibrary::~RSRTextureLibrary()
{
}

std::shared_ptr<RSRTexture2D> RSRTextureLibrary::INTERNAL_Add(const std::string_view InTexturePath, const std::string_view InTextureName, std::shared_ptr<RSRTexture2D>&& InTexture, uint16_t& OutIndex)
{
	OutIndex = (uint16_t)m_textures.size();
	m_textures.push_back(std::move(InTexture));
	m_pathToTextureIndex.emplace(InTexturePath, OutIndex);
	m_nameToTextureIndex.emplace(InTextureName, OutIndex);

	m_textures[OutIndex]->SetTextureIndex(OutIndex);
	return m_textures[OutIndex];
}

