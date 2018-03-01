#include "DX12Texture.h"

#include "dx12/DX12Utils.h"

DXGI_FORMAT DX12Texture::GetFormat() const
{
	return m_Desc.Format;
}

IntVec2 DX12Texture::GetSize() const
{
	return IntVec2((int)m_Desc.Width, (int)m_Desc.Height);
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12Texture::GetGPUDescriptorHandle() const
{
	return m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Texture::GetCPUDescriptorHandle() const
{
	return m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

DX12Texture::DX12Texture()
	:m_DescriptorHeap(nullptr)
	,m_ResourceBuffer(nullptr)
	,m_UploadBuffer(nullptr)
{
}

DX12Texture::~DX12Texture()
{
	Release();
}

void DX12Texture::LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device)
{
	const DX12TextureData * data = (const DX12TextureData*)i_Data;

	// other
	m_Name		= data->Name;
	m_Filepath	= data->Filepath;

	// retreive information to loading the texture
	m_Desc = {};
	m_Desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_Desc.Alignment			= 0;
	m_Desc.Width				= data->Width;
	m_Desc.Height				= data->Height;
	m_Desc.DepthOrArraySize		= 1;
	m_Desc.MipLevels			= 1;
	m_Desc.Format				= data->Format;
	m_Desc.SampleDesc.Count		= 1;
	m_Desc.SampleDesc.Quality	= 0;
	m_Desc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_Desc.Flags				= D3D12_RESOURCE_FLAG_NONE;

	std::wstring heapName, uploadName;
	String::Utf8ToUtf16(heapName, m_Name);
	heapName += L" Resource Buffer";
	CreateResourceBuffer(i_Device, heapName);

	String::Utf8ToUtf16(uploadName, m_Name);
	uploadName += L" Upload Buffer";
	CreateUploadBuffer(i_Device, uploadName);

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(m_Desc.Format); // number of bits per pixel
	int bytesPerRow = (m_Desc.Width * bitsPerPixel) / 8; // number of bytes in each row of the image data
	int imageSize = bytesPerRow * m_Desc.Height; // total image size in bytes

	// update texture data from upload to final resource buffer
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = (data->ImageData); // pointer to our image data
	textureData.RowPitch = bytesPerRow; // size of all our triangle vertex data
	textureData.SlicePitch = bytesPerRow * m_Desc.Height; // also the size of our triangle vertex data

	UpdateSubresources(i_CommandList, m_ResourceBuffer, m_UploadBuffer, 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	i_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ResourceBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// now we create a shader resource view (descriptor that points to the texture and describes it)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Desc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	i_Device->CreateShaderResourceView(m_ResourceBuffer, &srvDesc, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// delete the data
	delete data;
}

void DX12Texture::Release()
{
	SAFE_RELEASE(m_UploadBuffer);
	SAFE_RELEASE(m_ResourceBuffer);
	SAFE_RELEASE(m_DescriptorHeap);

	DX12Resource::Release();
}

FORCEINLINE HRESULT DX12Texture::CreateResourceBuffer(ID3D12Device * i_Device, const std::wstring & i_BufferName)
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
		IID_PPV_ARGS(&m_ResourceBuffer));

	if (FAILED(hr))	return hr;

	// name the heap buffer
	m_ResourceBuffer->SetName(i_BufferName.c_str());

	return S_OK;
}

FORCEINLINE HRESULT DX12Texture::CreateUploadBuffer(ID3D12Device * i_Device, const std::wstring & i_BufferName)
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
		IID_PPV_ARGS(&m_UploadBuffer));

	if (FAILED(hr)) return hr;

	// name the heap buffer
	m_UploadBuffer->SetName(i_BufferName.c_str());

	return S_OK;
}
