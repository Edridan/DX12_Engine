#include "Texture.h"

#include "DX12Texture.h"
#include "engine/Engine.h"
#include "resource/DX12ResourceManager.h"

DX12Texture * Texture::GetDX12Texture() const
{
	return m_Texture;
}

Texture::Texture()
	:Resource()
	,m_Data(nullptr)
{
}

Texture::~Texture()
{
	// delete resources
	if (m_Data != nullptr)	delete [] m_Data;
	if (m_Texture != nullptr) delete m_Texture;
}

void Texture::LoadFromFile(const std::string & i_Filepath)
{
	ImageDataDesc imageDesc = {};
	std::wstring filepath;
	String::Utf8ToUtf16(filepath, i_Filepath);

	// load image
	int imageSize = LoadImageDataFromFile(&m_Data, imageDesc, filepath.c_str());

	// load image from file
	if (imageSize <= 0)
	{
		PRINT_DEBUG_VS("Unable to load %S", m_Name.c_str());
		DEBUG_BREAK;
		return;
	}

	DX12Texture::DX12TextureData * tData = new DX12Texture::DX12TextureData;

	// fill resource information
	tData->Filepath	= i_Filepath;
	tData->Name		= ExtractFileName(i_Filepath);

	// fill image data for loading
	tData->Format	= imageDesc.Format;
	tData->Height	= imageDesc.Height;
	tData->Width	= imageDesc.Width;
	// pixels data
	tData->ImageData = m_Data;

	// push the texture to be loaded on the GPU
	m_Texture = Engine::GetInstance().GetRenderResourceManager()->PushTexture(&tData);

	ASSERT(m_Texture != nullptr);
}

void Texture::LoadFromData(const void * i_Data)
{
	// to do : implement
	TO_DO;
}

FORCEINLINE int Texture::LoadImageDataFromFile(BYTE ** o_Data, ImageDataDesc & o_ImageDesc, LPCWSTR i_Filename)
{
	HRESULT hr;

	// initialize the return data
	o_ImageDesc.BitsPerPixel	= -1;
	o_ImageDesc.BytesPerRow		= -1;
	o_ImageDesc.ImageSize		= -1;
	o_ImageDesc.Height			= -1;
	o_ImageDesc.Width			= -1;
	o_ImageDesc.Format			= DXGI_FORMAT_UNKNOWN;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory *wicFactory;
	// reset decoder, frame and converter since these will be different for each image we load
	IWICBitmapDecoder *	wicDecoder		= NULL;
	IWICBitmapFrameDecode * wicFrame	= NULL;
	IWICFormatConverter * wicConverter	= NULL;

	bool imageConverted = false;

	if (wicFactory == NULL)
	{
		// Initialize the COM library
		CoInitialize(NULL);

		// create the WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) return 0;
	}

	// load a decoder for the image
	hr = wicFactory->CreateDecoderFromFilename(
		i_Filename,                        // Image we want to load in
		NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
		GENERIC_READ,                    // We want to read from this file
		WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
		&wicDecoder                      // the wic decoder to be created
	);

	if (FAILED(hr)) return 0;

	// get image from decoder (this will decode the "frame")
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return 0;

	// get wic pixel format of image
	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	// get size of image
	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return 0;

	// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
	// out how to implement the support yourself

	// convert wic pixel format to dxgi pixel format
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);


	// if the format of the image is not a supported dxgi format, try to convert it
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		// get a dxgi compatible wic format from the current image format
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

		// return if no dxgi compatible format was found
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

		// set the dxgi format
		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

		// create the format converter
		hr = wicFactory->CreateFormatConverter(&wicConverter);
		if (FAILED(hr)) return 0;

		// make sure we can convert to the dxgi compatible format
		BOOL canConvert = FALSE;
		hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert) return 0;

		// do the conversion (wicConverter will contain the converted image)
		hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return 0;

		// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
		imageConverted = true;
	}

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
	int bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * textureHeight; // total image size in bytes

	// allocate enough memory for the raw image data, and set o_ImageData to point to that memory
	*o_Data = (BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (o_ImageData)
	if (imageConverted)
	{
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *o_Data);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// no need to convert, just copy data from the wic frame
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *o_Data);
		if (FAILED(hr)) return 0;
	}

	// fill up the desc
	o_ImageDesc.BitsPerPixel	= bitsPerPixel;
	o_ImageDesc.BytesPerRow		= bytesPerRow;
	o_ImageDesc.ImageSize		= imageSize;
	o_ImageDesc.Height			= textureHeight;
	o_ImageDesc.Width			= textureWidth;
	o_ImageDesc.Format			= dxgiFormat;

	return imageSize;
}
