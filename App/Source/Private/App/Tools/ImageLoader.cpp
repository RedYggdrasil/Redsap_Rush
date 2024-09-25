#include "App/Tools/ImageLoader.h"
#include <wrl/client.h>
#include <iostream>

#include "App/Tools/RSRLog.h"

using Microsoft::WRL::ComPtr;


const std::vector<ImageLoader::GUID_to_DXGI> ImageLoader::s_lookupTable =
{
	{ GUID_WICPixelFormat32bppBGRA , DXGI_FORMAT_B8G8R8A8_UNORM },
	{ GUID_WICPixelFormat32bppRGBA , DXGI_FORMAT_R8G8B8A8_UNORM }
};

bool ImageLoader::LoadImageFromDisk(const std::filesystem::path& InImagePath, ImageData& outData)
{
	ComPtr<IWICImagingFactory> wicFactory;
	_RF_FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)));
	// Load the image
	ComPtr<IWICStream> wicFileStream;

	_RF_FAILED(wicFactory->CreateStream(&wicFileStream));
	std::cout << InImagePath << std::endl;
	_RF_FAILED(wicFileStream->InitializeFromFilename(InImagePath.wstring().c_str(), GENERIC_READ));

	// Bitmap decoder
	ComPtr<IWICBitmapDecoder> wicDecoder;
	_RF_FAILED(wicFactory->CreateDecoderFromStream(wicFileStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &wicDecoder));

	// Bitmap decoder
	ComPtr<IWICBitmapFrameDecode> wicFrameDecoder;
	_RF_FAILED(wicDecoder->GetFrame(0, &wicFrameDecoder));

	// Trivial Metadata
	_RF_FAILED(wicFrameDecoder->GetSize(&outData.width, &outData.height));
	_RF_FAILED(wicFrameDecoder->GetPixelFormat(&outData.wicPixelFormat));

	bool bCompatibleFormat = false;
	for (const auto& Pair : s_lookupTable)
	{
		if (Pair.wic == outData.wicPixelFormat)
		{
			bCompatibleFormat = true;
			break;
		}
	}
	ComPtr<IWICFormatConverter> wicConverter;
	if (!bCompatibleFormat)
	{
		// Format convert to 32bppPBGRA which D2D expects
		_RF_FAILED(wicFactory->CreateFormatConverter(&wicConverter));
		BOOL bCanConver = false;
		_RF_FAILED(wicConverter->CanConvert(outData.wicPixelFormat, GUID_WICPixelFormat32bppRGBA, &bCanConver));
		if (!bCanConver)
		{
			RSRLog::Log(LOG_ERROR, TEXT("Cannot load texture format for file : '{}'"), InImagePath.string());
			return false;
		}
		wicConverter->Initialize(
			wicFrameDecoder.Get(),
			GUID_WICPixelFormat32bppRGBA,//GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
		outData.wicPixelFormat = GUID_WICPixelFormat32bppRGBA;
	}
	// Metadata of pixel format

	ComPtr<IWICComponentInfo> wicComponentInfo;
	_RF_FAILED(wicFactory->CreateComponentInfo(outData.wicPixelFormat, &wicComponentInfo));

	ComPtr<IWICPixelFormatInfo> wicPixelFormatInfo;
	_RF_FAILED(wicComponentInfo->QueryInterface(IID_PPV_ARGS(&wicPixelFormatInfo)));

	_RF_FAILED(wicPixelFormatInfo->GetBitsPerPixel(&outData.bpp));
	_RF_FAILED(wicPixelFormatInfo->GetChannelCount(&outData.cc));

	//DXGI Pixel format
	auto findIt = std::find_if
	(
		s_lookupTable.begin(), s_lookupTable.end(),
		[&](const GUID_to_DXGI& entry)
		{
			return memcmp(&entry.wic, &outData.wicPixelFormat, sizeof(GUID)) == 0;
		}
	);

	if (findIt == s_lookupTable.end())
	{
		return false;
	}
	outData.giPixelFormat = findIt->gi;


	//Image loading

	/* Stride is bytes per row. +7 force align bytes, probably work as a modulo because uint */
	uint32_t stride = /*Bytes Per Pixel*/((outData.bpp + 7) / 8) * /*Pixel per Row*/outData.width;

	/* total Bytes size */
	uint32_t size = stride * outData.height;

	outData.data.resize(size);

	WICRect copyRect = 
	{
		.X = 0,
		.Y = 0,
		.Width	= (INT)outData.width, //Narrow conversion
		.Height	= (INT)outData.height //Narrow conversion
	};
	if (bCompatibleFormat)
	{
		_RF_FAILED(wicFrameDecoder->CopyPixels(&copyRect, stride, size, (BYTE*)outData.data.data()));
	}
	else
	{
		_RF_FAILED(wicConverter->CopyPixels(&copyRect, stride, size, (BYTE*)outData.data.data()));
	}

	return true;
}
