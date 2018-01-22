#include "dx12/DX12ConstantBuffer.h"

DX12ConstantBuffer::DX12ConstantBuffer(UINT64 i_BufferSize, UINT64 i_ElementSize)
	:m_BufferSize(i_ElementSize)
	,m_ElementSize(i_BufferSize)
{
	// allocate and intialize available blocks
	m_ConstantBufferReservedAddress = new bool[i_BufferSize];
	for (int i = 0; i < i_BufferSize; ++i)
	{
		m_ConstantBufferReservedAddress[i] = false;
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

ADDRESS_ID DX12ConstantBuffer::ReserveVirtualAddress()
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

					// erase before used data and push null data to the constant buffer
	//ConstantBuffer constantBuffer;
	//ZeroMemory(&constantBuffer, sizeof(constantBuffer));

	//for (int i = 0; i < frameBufferCount; ++i)
	//{
	//	// constant buffer new data
	//	memcpy(m_ConstantBufferGPUAdress[i] + (address * m_ElementSize), &constantBuffer, sizeof(constantBuffer));
	//}

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
		POPUP_ERROR("Error using a non reserved address for constant buffer");
	}

	return m_ConstantBufferUploadHeap[frameIndex]->GetGPUVirtualAddress() + (i_Address * m_ElementSize);
}
