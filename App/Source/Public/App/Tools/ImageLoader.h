#pragma once

#include "App/Libs/WinInclude.h"

#include <vector>
#include <filesystem>



class ImageLoader
{
public:
	struct ImageData
	{
		/*Image buffer*/
		std::vector<char> data;
		/*Image width*/
		uint32_t width;
		/*Image height*/
		uint32_t height;
		/*Bits per pixel*/
		uint32_t bpp;
		/*Chanel count*/
		uint32_t cc;

		/*Pixel format ID for WIC*/
		GUID wicPixelFormat;
		/*Pixel format ID for WIC*/
		DXGI_FORMAT giPixelFormat;
	public:
		inline uint32_t GetBytesPerPixel() const { return ((this->bpp + 7) / 8); };
		/*Row size*/
		inline uint32_t GetBytesPerStride() const { return this->GetBytesPerPixel() * this->width; };
		inline uint32_t GetTotalBytes() const { return this->GetBytesPerPixel() * this->width * this->height; };
	};
	static bool LoadImageFromDisk(const std::filesystem::path& InImagePath, ImageData& outData);
private:
	struct GUID_to_DXGI
	{
		GUID wic;
		DXGI_FORMAT gi;
	};
	static const std::vector<GUID_to_DXGI> s_lookupTable;
private:
	ImageLoader() = default;
	ImageLoader(const ImageLoader&) = default;
	ImageLoader& operator=(const ImageLoader&) = default;
};