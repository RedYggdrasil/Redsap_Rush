#include "App/Data/Textures/RSRTexture2D.h"

#include "MDS/Tools/RMath.h"
#include "Gen_App/Config/AppConfig.h"
#include "App/Tools/RSRLog.h"
#include <format>

using namespace RSRush;
using Microsoft::WRL::ComPtr;
bool RSRTexture2D::FreeUploadBuffer()
{
	bool bSucess = true;
	if (m_uploadBuffer)
	{
		//Not all texture have upload buffers, as some are uploaded in bulk;
		m_uploadBuffer.Reset();
	}

	if (bSucess)
	{
		m_resourceState = (bool)m_textureBuffer ?
			mds::RResourceStateType::ResourceLive :
			mds::RResourceStateType::NeedUpload;
	}
	return bSucess;
}

bool RSRTexture2D::FreeResourcesBuffer()
{
	bool bSucess = true;
	m_textureBuffer.Reset();

	//If we had Upload resources, they woyld now be invalid as the resource toward witch they should upload no longer exist
	//FreeUploadBuffer also update m_resourceState
	bSucess = FreeUploadBuffer() && bSucess;
	return bSucess;
}

bool RSRTexture2D::FreeAllBuffers()
{
	//Also free upload resrouce
	return FreeResourcesBuffer();
}

RSRTexture2D::RSRTexture2D(const std::filesystem::path& InImagePath)
: RAsset()
{
	m_type = mds::RAssetType::Texture2D;
	bool bSucess = ImageLoader::LoadImageFromDisk(InImagePath, /*out*/m_textureData);
	m_resourceState = bSucess ? mds::RResourceStateType::NeedUpload : mds::RResourceStateType::Unknown;
	if (!bSucess)
	{
		
		std::string LogError = std::format("Failed to load texture '{}' !\n", InImagePath.string());
		RSRLog::LogError(LogError);
	}
}

RSRush::RSRTexture2D::RSRTexture2D(Microsoft::WRL::ComPtr<ID3D12Resource2> InLoadedGPUResource, const ImageLoader::ImageData& InData)
	: RAsset(), m_textureData(InData), m_uploadBuffer(), m_textureBuffer(InLoadedGPUResource)
{
	m_resourceState = mds::RResourceStateType::ResourceLive;
}

RSRTexture2D::~RSRTexture2D()
{
	bool bSucess = FreeAllBuffers();
}

bool RSRush::RSRTexture2D::CreateSRVHeapForTextures(UINT InNbTextures, RSRTexture2D** InTextures, ID3D12Device10* InDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& InOutSrvheap)
{
	// -- Descriptor Heap for Texture(s) --
	D3D12_DESCRIPTOR_HEAP_DESC dhd =
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = InNbTextures,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0
	};

	////// Create Descriptor Heap for Table of SRVs //////
	//Create Table for Texture SRVs
	HRESULT result = InDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&InOutSrvheap));
	if (FAILED(result)) { RSRLog::LogError(TEXT("Cannot create texture Descriptor Heaps !"), result); return false; }

	//Create CPU view handles
	D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = InOutSrvheap->GetCPUDescriptorHandleForHeapStart();
	UINT handleSize = InDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT i = 0; i < InNbTextures; ++i)
	{
		RSRTexture2D* texture = *(InTextures + i);


		//Compute Descriptor Handle in srvArray
		texture->m_cpuDescriptorHandle = firstHandle;
		texture->m_cpuDescriptorHandle.ptr += (i * handleSize);

		// --SRVs for textures --
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {
			.Format = texture->m_textureData.giPixelFormat,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MostDetailedMip = 0,
				.MipLevels = 1,
				.PlaneSlice = 0,
				.ResourceMinLODClamp = 0.0f
			}
		};
		InDevice->CreateShaderResourceView(texture->m_textureBuffer.Get(), &srv, texture->m_cpuDescriptorHandle);
	}

	return true;
}

bool RSRTexture2D::UploadResources(UINT InStartIndex, UINT InNbTextures, std::vector<RSRSharedTexture2DPtr> InTextures, ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	RSRTexture2D** texturePtrs = new RSRTexture2D*[InNbTextures];
	UINT effectiveCount = 0;
	for (UINT i = InStartIndex; (i - InStartIndex) < InNbTextures; ++i)
	{
		if (InTextures[i])
		{
			texturePtrs[effectiveCount] = InTextures[i].get();
			++effectiveCount;
		}
	}

	bool bSuccessful = effectiveCount > 0;
	if (bSuccessful)
	{
		bSuccessful = RSRTexture2D::UploadResources(effectiveCount, texturePtrs, InDevice, InUploadCommandList);
	}
	delete[] texturePtrs;
	return bSuccessful;
}

bool RSRush::RSRTexture2D::CreateSRVHeapForTextures(UINT InStartIndex, UINT InNbTextures, std::vector<RSRSharedTexture2DPtr> InTextures, ID3D12Device10* InDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& InOutSrvheap)
{
	RSRTexture2D** texturePtrs = new RSRTexture2D * [InNbTextures];
	UINT effectiveCount = 0;
	for (UINT i = InStartIndex; (i - InStartIndex) < InNbTextures; ++i)
	{
		if (InTextures[i])
		{
			texturePtrs[effectiveCount] = InTextures[i].get();
			++effectiveCount;
		}
	}

	bool bSuccessful = effectiveCount > 0;
	if (bSuccessful)
	{
		bSuccessful = RSRTexture2D::CreateSRVHeapForTextures(effectiveCount, texturePtrs, InDevice, InOutSrvheap);
	}
	delete[] texturePtrs;
	return bSuccessful;
}

bool RSRTexture2D::UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	RSRTexture2D* thisPtr = this;
	return RSRTexture2D::UploadResources(1, { &thisPtr }, InDevice, InUploadCommandList);
}

bool RSRush::RSRTexture2D::UploadResources(UINT InNbTextures, RSRTexture2D** InTextures, ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList)
{
	UINT spaceIndex = 0;

	if (InNbTextures < 1) { return true; }

	RSRTexture2D* firstTextureOfArray = *InTextures;

	//Upload Heap Properties
	D3D12_HEAP_PROPERTIES hpUpload{};
	hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpUpload.CreationNodeMask = 0;
	hpUpload.VisibleNodeMask = 0;

	//Device Resource Heap Properties
	D3D12_HEAP_PROPERTIES hpDefault{};
	hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
	hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpDefault.CreationNodeMask = 0;
	hpDefault.VisibleNodeMask = 0;



	HRESULT result;
	uint32_t totalUploadSize = 0;

	//////Compute total upload size and create one Texture D3D12Resource per textures//////
	for (UINT i = 0; i < InNbTextures; ++i)
	{
		RSRTexture2D* texture = *(InTextures + i);

		if (texture->GetTextureIndex() != (uint16_t)i)
		{
			RSRLog::Log(LOG_EXCEPTION, TEXT("Unexpected texture index '{}' for texture, expected index '{}' !"), i, texture->GetTextureIndex());
		}
		//texture->m_indexInSRVTable = i;
		texture->m_spaceIndex = spaceIndex;

		totalUploadSize += texture->m_textureData.GetTotalBytes();


		//Texture Buffer Description
		D3D12_RESOURCE_DESC rdt{};
		rdt.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rdt.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		rdt.Width = texture->m_textureData.width;
		rdt.Height = texture->m_textureData.height;
		rdt.DepthOrArraySize = 1;
		rdt.MipLevels = 1;
		rdt.Format = texture->m_textureData.giPixelFormat;
		rdt.SampleDesc = { .Count = 1, .Quality = 0 };
		rdt.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		rdt.Flags = D3D12_RESOURCE_FLAG_NONE;

		result = InDevice->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &rdt, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture->m_textureBuffer));
		if (FAILED(result)) { RSRLog::LogError(TEXT("Cannot upload current Texture !"), result); return false; }
	}


	////// Create Upload Buffer Resources //////

	//Upload Buffer Description
	D3D12_RESOURCE_DESC rdUpload{};
	rdUpload.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rdUpload.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	rdUpload.Width = totalUploadSize; //Size of the uploaded data
	rdUpload.Height = 1; //Size for 2nd dimension of array
	rdUpload.DepthOrArraySize = 1; //Size for 3rd dimension of array
	rdUpload.MipLevels = 1; //Number of Mipmap, buffer is 1, non-duplicated data
	rdUpload.Format = DXGI_FORMAT_UNKNOWN; //Buffer does not have any special format
	rdUpload.SampleDesc = { .Count = 1, .Quality = 0 }; //For Super sampling
	rdUpload.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //Buffer is Row Major
	rdUpload.Flags = D3D12_RESOURCE_FLAG_NONE;

	/*Our Upload Buffer will be layout as { [0] : Textures } */
	result = InDevice->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &rdUpload, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&firstTextureOfArray->m_uploadBuffer));
	if (FAILED(result)) { RSRLog::LogError(TEXT("Cannot create texture Upload Buffer !"), result); return false; }





	////// Copy Texture to Upload Buffer | Create one SRV per texture and place it in reserved DescriptorHeap table //////

	/*Adress on the Upload buffer for the textures data*/
	char* uploadBufferAdress;
	D3D12_RANGE uploadRange = { .Begin = 0, .End = totalUploadSize }; //Adress is begin of our buffer + size of all our textures
	firstTextureOfArray->m_uploadBuffer->Map(0, &uploadRange, (void**)&uploadBufferAdress);
	size_t currentTextureStartCopyPosition = 0;


	for (UINT i = 0; i < InNbTextures; ++i)
	{
		RSRTexture2D* texture = *(InTextures + i);

		//Copy texture to upload buffer
		uint32_t textureSize = texture->m_textureData.GetTotalBytes();
		memcpy(&uploadBufferAdress[currentTextureStartCopyPosition], texture->m_textureData.data.data(), textureSize);
		currentTextureStartCopyPosition += textureSize;

	};
	firstTextureOfArray->m_uploadBuffer->Unmap(0, &uploadRange);


	////// Upload textures form Upload Buffer to Texture Resources //////

	currentTextureStartCopyPosition = 0;
	for (UINT i = 0; i < InNbTextures; ++i)
	{
		RSRTexture2D* texture = *(InTextures + i);

		uint32_t textureSize = texture->m_textureData.GetTotalBytes();

		D3D12_BOX textureSizeAsBox =
		{
			.left = 0,
			.top = 0,
			.front = 0,
			.right = texture->m_textureData.width,
			.bottom = texture->m_textureData.height,
			.back = 1
		};
		D3D12_TEXTURE_COPY_LOCATION txtcSrc, txtcDst;
		//Texture Source
		txtcSrc = {
			.pResource = firstTextureOfArray->m_uploadBuffer.Get(),
			.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint = {
				.Offset = currentTextureStartCopyPosition,
				.Footprint = {
					.Format = texture->m_textureData.giPixelFormat,
					.Width = texture->m_textureData.width,
					.Height = texture->m_textureData.height,
					.Depth = 1,
					.RowPitch = texture->m_textureData.GetBytesPerStride()   //Distance between rows
				}
			}
		};
		//Texture Destinaton
		txtcDst = {
			.pResource = texture->m_textureBuffer.Get(),
			.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = 0
		};
		InUploadCommandList->CopyTextureRegion(&txtcDst, 0, 0, 0, &txtcSrc, &textureSizeAsBox);

		currentTextureStartCopyPosition += textureSize;

		texture->m_resourceState = mds::RResourceStateType::WaitingUpload;
	}

	return true;
}
