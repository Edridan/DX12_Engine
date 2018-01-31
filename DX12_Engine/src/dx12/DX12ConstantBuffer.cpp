#include "dx12/DX12ConstantBuffer.h"
#include "dx12/DX12RenderEngine.h"


DX12ConstantBuffer::DX12ConstantBuffer(UINT64 i_BufferSize, UINT64 i_ElementSize)
	:m_ElementSize((i_ElementSize + 255) & ~255)	// align element size on 256 bytes
	,m_BufferSize(i_BufferSize)
{
	// retreive device
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12Device * device = render.GetDevice();
	m_FrameCount = render.GetFrameBufferCount();

	// initialize arrays
	m_MainDescriptorHeap		= new ID3D12DescriptorHeap *[m_FrameCount];
	m_ConstantBufferUploadHeap	= new ID3D12Resource *[m_FrameCount];	// memory where constant buffers for each frame will be placed
	m_ConstantBufferGPUAdress	= new UINT8 *[m_FrameCount];	// pointer for each of the resource buffer constant heap

	// allocate and intialize available blocks
	m_ConstantBufferReservedAddress = new bool[i_BufferSize];
	for (int i = 0; i < i_BufferSize; ++i)
	{
		m_ConstantBufferReservedAddress[i] = false;
	}

	// create constant buffer
	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		DX12_ASSERT(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // this heap will be used to upload the constant buffer data
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(m_ConstantBufferHeapSize * m_ElementSize * 64), // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
			D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
			nullptr, // we do not have use an optimized clear value for constant buffers
			IID_PPV_ARGS(&m_ConstantBufferUploadHeap[i])));

		m_ConstantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");

		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)
		DX12_ASSERT(m_ConstantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferGPUAdress[i])));
	}
}

DX12ConstantBuffer::~DX12ConstantBuffer()
{
	for (int i = 0; i < DX12RenderEngine::GetInstance().GetFrameBufferCount(); ++i)
	{
		SAFE_RELEASE(m_MainDescriptorHeap[i]);
	}

	// Delete the array
	delete[] m_ConstantBufferReservedAddress;
}

ADDRESS_ID DX12ConstantBuffer::ReserveVirtualAddress(bool i_Initialize /* = false */)
{
	ADDRESS_ID address = 0;
	const int frameBufferCount = DX12RenderEngine::GetInstance().GetFrameBufferCount();
	
	// retreive the first address available
	while (address < m_BufferSize)
	{
		if (m_ConstantBufferReservedAddress[address] == false)
		{
			m_ConstantBufferReservedAddress[address] = true;
			break;	// exit the loop, we have found an available address
		}
		++address;
	}

	// we didn't found a available address
	if (address == m_BufferSize)
		return -1;	// error address

	// erase old memory from const buffer
	if (i_Initialize)
	{
		for (size_t i = 0; i < m_FrameCount; ++i)
		{
			// zero memory on the constant buffer position
			ZeroMemory(m_ConstantBufferGPUAdress[i] + (address * m_ElementSize), m_ElementSize);
		}
	}

	return address;
}

void DX12ConstantBuffer::ReleaseVirtualAddress(ADDRESS_ID i_Address)
{
	if (i_Address < m_BufferSize)
	{
		// release the constant buffer address
		// we let the buffer as is, we don't need to clear or release on gpu side (it's done when the engine is killed)
		m_ConstantBufferReservedAddress[i_Address] = false;
	}
}

UINT8 * DX12ConstantBuffer::GetGPUAddress(ADDRESS_ID i_Address) const
{
	const int frameIndex = DX12RenderEngine::GetInstance().GetFrameIndex();
	return m_ConstantBufferGPUAdress[frameIndex] + (i_Address * m_ElementSize);
}

D3D12_GPU_VIRTUAL_ADDRESS DX12ConstantBuffer::GetUploadVirtualAddress(ADDRESS_ID i_Address) const
{
	const int frameIndex = DX12RenderEngine::GetInstance().GetFrameIndex();

	if (m_ConstantBufferReservedAddress[i_Address] == false)
	{
		PRINT_DEBUG("Error using a non reserved address for constant buffer");
		DEBUG_BREAK;
	}

	return m_ConstantBufferUploadHeap[frameIndex]->GetGPUVirtualAddress() + (i_Address * m_ElementSize);
}

UINT64 DX12ConstantBuffer::GetConstantElementSize() const
{
	return m_ElementSize;
}

void DX12ConstantBuffer::UpdateConstantBuffer(ADDRESS_ID i_Address, void * i_Data, UINT i_Size)
{
	int frameIndex = DX12RenderEngine::GetInstance().GetFrameIndex();
	if (m_ConstantBufferReservedAddress[i_Address] == true)
	{
		// copy data to the constant buffer
		memcpy(m_ConstantBufferGPUAdress[frameIndex] + (i_Address * m_ElementSize), i_Data, i_Size);
	}
	else
	{
		PRINT_DEBUG("Error, trying to update non reserved constant buffer address");
		DEBUG_BREAK;
	}
}

void DX12ConstantBuffer::UpdateConstantBufferForEachFrame(ADDRESS_ID i_Address, void * i_Data, UINT i_Size)
{
	if (m_ConstantBufferReservedAddress[i_Address] == true)
	{
		for (UINT i = 0; i < m_FrameCount; ++i)
		{
			// copy data to the constant buffer
			memcpy(m_ConstantBufferGPUAdress[i] + (i_Address * m_ElementSize), i_Data, i_Size);
		}
	}
	else
	{
		PRINT_DEBUG("Error, trying to update non reserved constant buffer address");
		DEBUG_BREAK;
	}
}
