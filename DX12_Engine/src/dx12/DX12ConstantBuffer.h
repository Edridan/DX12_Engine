// this manage a constant buffer area for a specific object
#pragma once

#include <d3d12.h>

#include "dx12/DX12Utils.h"

class DX12ConstantBuffer
{
public:
	DX12ConstantBuffer(UINT64 i_BufferSize, UINT64 i_ElementSize);
	~DX12ConstantBuffer();

	// buffer map
	ADDRESS_ID					ReserveVirtualAddress(bool i_Initialize = false);	// return id
	void						ReleaseVirtualAddress(ADDRESS_ID i_Address);
	// dx12 management (internal)
	UINT8 *						GetGPUAddress(ADDRESS_ID i_Address) const;
	D3D12_GPU_VIRTUAL_ADDRESS	GetUploadVirtualAddress(ADDRESS_ID i_Address) const;

	// information
	UINT64						GetConstantElementSize() const;

	// update buffer
	void						UpdateConstantBuffer(ADDRESS_ID i_Address, void * i_Data, UINT i_Size);

private:
	// dx12
	ID3D12DescriptorHeap **		m_MainDescriptorHeap;
	ID3D12Resource **			m_ConstantBufferUploadHeap;	// memory where constant buffers for each frame will be placed
	UINT8 **					m_ConstantBufferGPUAdress;	// pointer for each of the resource buffer constant heap
	bool *						m_ConstantBufferReservedAddress;	// internal constant buffer management
	UINT						m_ConstantBufferHeapSize = 32;
	// internal management
	UINT					m_FrameCount;

	// Constant buffer management
	const UINT64		m_ElementSize;	// size of one constant buffer per object
	const UINT64		m_BufferSize;	// number of const buffer elements available
};