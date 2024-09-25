#pragma once

#include "App/Libs/WinInclude.h"

//#include "App/Data/Shaders/ShaderStructures.h"
#include "MDS/Tools/Assets/RAsset.h"
#include "App/Tools/ImageLoader.h"
#include "MDS/Tools/Pins/RID3D12RPin.h"

#include <wrl/client.h>
#include <memory>
#include <vector>

namespace RSRush
{
	class RSRTexture2D;
	typedef std::weak_ptr<RSRush::RSRTexture2D> RSRWeakTexture2DPtr;
	typedef std::shared_ptr<RSRush::RSRTexture2D> RSRSharedTexture2DPtr;

#define l_ComPtr	Microsoft::WRL::ComPtr
	class RSRTexture2D : public mds::RAsset
	{
	protected:

		ImageLoader::ImageData m_textureData;


		//CPU Resources
		l_ComPtr<ID3D12Resource2> m_uploadBuffer;
		//GPU Resources
		l_ComPtr<ID3D12Resource2> m_textureBuffer;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptorHandle;
		UINT m_indexInSRVTable = 0;
		UINT m_spaceIndex = 0;

	public:
		UINT64 GetTextureBufferSize() const;
		UINT64 GetAlignedTextureBufferSize() const;


		inline const l_ComPtr<ID3D12Resource2>& GetTextureBuffer() const { return m_textureBuffer; }

	public:
		bool FreeUploadBuffer();
		bool FreeResourcesBuffer();
		bool FreeAllBuffers();

	public:
		RSRTexture2D(const std::filesystem::path& InImagePath);

	public:
		/*virtual*/~RSRTexture2D()/*override*/;
	public:
		virtual bool UploadResources(ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, l_ComPtr<ID3D12DescriptorHeap>& InOutSrvheap);
		static bool UploadResources(UINT InNbTextures, RSRTexture2D** InTextures, ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, l_ComPtr<ID3D12DescriptorHeap>& InOutSrvheap);
		static bool UploadResources(std::vector<RSRSharedTexture2DPtr> InTextures, ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, l_ComPtr<ID3D12DescriptorHeap>& InOutSrvheap)
		{ return UploadResources(0, (UINT)InTextures.size(), InTextures, InDevice, InUploadCommandList, InOutSrvheap); }
		static bool UploadResources(UINT InStartIndex, UINT InNbTextures, std::vector<RSRSharedTexture2DPtr> InTextures, ID3D12Device10* InDevice, ID3D12GraphicsCommandList7* InUploadCommandList, l_ComPtr<ID3D12DescriptorHeap>& InOutSrvheap);
	};
#undef l_ComPtr
}