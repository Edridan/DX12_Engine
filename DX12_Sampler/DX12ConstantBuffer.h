// this manage a constant buffer area for a specific object
#pragma once

#include <d3d12.h>

#include "DX12Utils.h"
#include "DX12RenderEngine.h"

class DX12ConstantBuffer
{
public:
	DX12ConstantBuffer(UINT64 i_BufferSize, UINT64 i_ElementSize);
	~DX12ConstantBuffer();

	// buffer map
	ADDRESS_ID					ReserveVirtualAddress();	// return id
	void						ReleaseVirtualAddress(ADDRESS_ID i_Address);
	UINT8 *						GetGPUAddress(ADDRESS_ID i_Address) const;
	D3D12_GPU_VIRTUAL_ADDRESS	GetUploadVirtualAddress(ADDRESS_ID i_Address) const;

private:
	ID3D12DescriptorHeap *		m_MainDescriptorHeap[FRAME_BUFFER_COUNT];
	ID3D12Resource *			m_ConstantBufferUploadHeap[FRAME_BUFFER_COUNT];	// memory where constant buffers for each frame will be placed
	UINT8 *						m_ConstantBufferGPUAdress[FRAME_BUFFER_COUNT];	// pointer for each of the resource buffer constant heap
	bool *						m_ConstantBufferReservedAddress;	// internal constant buffer management

	// Constant buffer management
	const UINT64		m_ElementSize;	// size of one constant buffer per object
	const UINT64		m_BufferSize;	// number of const buffer elements available
};