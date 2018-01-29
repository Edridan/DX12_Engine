#include "dx12/DX12Texture.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"

DX12Texture::DX12Texture(const wchar_t * i_Filename)
	:m_Name(i_Filename)
	,m_IsLoaded(false)
	,m_DescriptorHeap(nullptr)
	,m_TextureBuffer(nullptr)
	,m_TextureBufferUploadHeap(nullptr)
	,m_Desc()
{
	BYTE * data = nullptr;
	ImageDataDesc imageDesc;
	int bytesPerRow = 0;

	int imageSize = LoadImageDataFromFile(&data, m_Desc, imageDesc, m_Name.c_str());

	// load image from file
	if (imageSize <= 0)
	{
		POPUP_ERROR("Unable to load %S\n", m_Name.c_str());
		DEBUG_BREAK;
		return;
	}

	// copy the data to the texture resource
	ID3D12Device * device					= DX12RenderEngine::GetInstance().GetDevice();
	ID3D12GraphicsCommandList * commandList = DX12RenderEngine::GetInstance().GetCommandList();

	// create heap and resource buffer for the texture
	std::wstring heapName(i_Filename);
	heapName.append(L" Buffer Resource Heap");
	CreateTextureBufferResourceHeap(device, heapName);

	std::wstring uploadBufferName(i_Filename);
	uploadBufferName.append(L" Texture Buffer Upload Resource Heap");
	CreateTextureBufferUploadHeap(device, uploadBufferName);

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &data[0]; // pointer to our image data
	textureData.RowPitch = imageDesc.BytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = imageDesc.BytesPerRow * m_Desc.Height; // also the size of our triangle vertex data

	// Now we copy the upload buffer contents to the default heap
	UpdateSubresources(commandList, m_TextureBuffer, m_TextureBufferUploadHeap, 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// now we create a shader resource view (descriptor that points to the texture and describes it)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format						= m_Desc.Format;
	srvDesc.ViewDimension				= D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels			= 1;

	device->CreateShaderResourceView(m_TextureBuffer, &srvDesc, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// delete unused data
	delete data;

	m_IsLoaded = true;
}

DX12Texture::DX12Texture(const ImageDesc & i_Desc)
	:m_Name(i_Desc.Name)
	,m_IsLoaded(false)
	,m_DescriptorHeap(nullptr)
	,m_TextureBuffer(nullptr)
	,m_TextureBufferUploadHeap(nullptr)
	,m_Desc()
{
	// copy the data to the texture resource
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();
	ID3D12GraphicsCommandList * commandList = DX12RenderEngine::GetInstance().GetCommandList();

	m_Desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_Desc.Alignment			= 0;
	m_Desc.Width				= i_Desc.Width;
	m_Desc.Height				= i_Desc.Height;
	m_Desc.DepthOrArraySize		= 1;
	m_Desc.MipLevels			= 1;
	m_Desc.Format				= i_Desc.Format;
	m_Desc.SampleDesc.Count		= 1;
	m_Desc.SampleDesc.Quality	= 0;
	m_Desc.Layout			= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_Desc.Flags			= D3D12_RESOURCE_FLAG_NONE;
	//ID3D12Resource* fontResource = createCommittedResource(_device, HeapProperty::Default, &desc, 0);

	std::wstring heapName(m_Name);
	heapName.append(L" Buffer Resource Heap");
	CreateTextureBufferResourceHeap(device, heapName);

	std::wstring uploadBufferName(m_Name);
	uploadBufferName.append(L" Texture Buffer Upload Resource Heap");
	CreateTextureBufferUploadHeap(device, uploadBufferName);

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(i_Desc.Format); // number of bits per pixel
	int bytesPerRow = (i_Desc.Width * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * i_Desc.Height; // total image size in bytes

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = (i_Desc.Data); // pointer to our image data
	textureData.RowPitch = bytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = bytesPerRow * m_Desc.Height; // also the size of our triangle vertex data

	// Now we copy the upload buffer contents to the default heap
	UpdateSubresources(commandList, m_TextureBuffer, m_TextureBufferUploadHeap, 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// now we create a shader resource view (descriptor that points to the texture and describes it)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Desc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(m_TextureBuffer, &srvDesc, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// the image is loaded on GPU
	m_IsLoaded = true;
}

DX12Texture::~DX12Texture()
{
	// Clean resources
	SAFE_RELEASE(m_TextureBufferUploadHeap);
	SAFE_RELEASE(m_TextureBuffer);
	//SAFE_RELEASE(m_DescriptorHeap);	hack : crash
}

IntVec2 DX12Texture::GetSize() const
{
	return IntVec2(
		(int)m_Desc.Width,
		(int)m_Desc.Height
	);
}

const std::wstring & DX12Texture::GetName() const
{
	return m_Name;
}

bool DX12Texture::IsLoaded() const
{
	return m_IsLoaded;
}

const ID3D12DescriptorHeap * DX12Texture::GetDescriptorHeap() const
{
	return m_DescriptorHeap;
}

HRESULT DX12Texture::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList)
{
	// set the descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap };
	i_CommandList->SetDescriptorHeaps(1, descriptorHeaps);

	i_CommandList->SetGraphicsRootDescriptorTable(1, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	return S_OK;
}

DX12Texture::DX12Texture()
	:m_Name(L"")
	,m_IsLoaded(false)
	,m_DescriptorHeap(nullptr)
	,m_TextureBuffer(nullptr)
	,m_TextureBufferUploadHeap(nullptr)
	,m_Desc()
{
}

// retreived from : https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file
int DX12Texture::LoadImageDataFromFile(BYTE** o_ImageData, D3D12_RESOURCE_DESC & o_ResourceDescription, ImageDataDesc & o_Desc, LPCWSTR i_Filename)
{
	HRESULT hr;

	// initialize the return data
	o_Desc.BitsPerPixel = -1;
	o_Desc.BytesPerRow = -1;
	o_Desc.ImageSize = -1;
	o_Desc.Height = -1;
	o_Desc.Width = -1;

	// we only need one instance of the imaging factory to create decoders and frames
	static IWICImagingFactory *wicFactory;

	// reset decoder, frame and converter since these will be different for each image we load
	IWICBitmapDecoder *wicDecoder = NULL;
	IWICBitmapFrameDecode *wicFrame = NULL;
	IWICFormatConverter *wicConverter = NULL;

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
	*o_ImageData = (BYTE*)malloc(imageSize);

	// copy (decoded) raw image data into the newly allocated memory (o_ImageData)
	if (imageConverted)
	{
		// if image format needed to be converted, the wic converter will contain the converted image
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *o_ImageData);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// no need to convert, just copy data from the wic frame
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *o_ImageData);
		if (FAILED(hr)) return 0;
	}

	// fill up the desc
	o_Desc.BitsPerPixel = bitsPerPixel;
	o_Desc.BytesPerRow = bytesPerRow;
	o_Desc.ImageSize = imageSize;
	o_Desc.Height = textureHeight;
	o_Desc.Width = textureWidth;

	// now describe the texture with the information we have obtained from the image
	o_ResourceDescription = {};
	o_ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	o_ResourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	o_ResourceDescription.Width = textureWidth; // width of the texture
	o_ResourceDescription.Height = textureHeight; // height of the texture
	o_ResourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	o_ResourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	o_ResourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	o_ResourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	o_ResourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	o_ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	o_ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

	// return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
	return imageSize;
}

FORCEINLINE HRESULT DX12Texture::CreateTextureBufferResourceHeap(ID3D12Device * i_Device, const std::wstring & i_BufferName)
{
	HRESULT hr;

	// create the descriptor heap that will store our srv
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	hr = i_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorHeap));

	if (FAILED(hr))	return hr;

	hr = i_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&m_Desc, // the description of our texture
		D3D12_RESOURCE_STATE_COPY_DEST, // We will copy the texture from the upload heap to here, so we start it out in a copy dest state
		nullptr, // used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_TextureBuffer));

	if (FAILED(hr))	return hr;

	// name the heap buffer
	m_TextureBuffer->SetName(i_BufferName.c_str());

	return S_OK;
}

FORCEINLINE HRESULT DX12Texture::CreateTextureBufferUploadHeap(ID3D12Device * i_Device, const std::wstring & i_BufferName)
{
	HRESULT hr;

	UINT64 textureUploadBufferSize;
	// this function gets the size an upload buffer needs to be to upload a texture to the gpu.
	// each row must be 256 byte aligned except for the last row, which can just be the size in bytes of the row
	// eg. textureUploadBufferSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
	//textureUploadBufferSize = (((imageBytesPerRow + 255) & ~255) * (textureDesc.Height - 1)) + imageBytesPerRow;
	i_Device->GetCopyableFootprints(&m_Desc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

	hr = i_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), // resource description for a buffer (storing the image data in this heap just to copy to the default heap)
		D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
		nullptr,
		IID_PPV_ARGS(&m_TextureBufferUploadHeap));

	if (FAILED(hr)) return hr;

	// name the heap buffer
	m_TextureBufferUploadHeap->SetName(i_BufferName.c_str());

	return S_OK;
}
