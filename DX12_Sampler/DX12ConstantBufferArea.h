// this manage a constant buffer area for a specific object
#pragma once

#include <d3d12.h>
#include "DX12RenderEngine.h"

class DX12ConstantBufferArea
{
public:
	DX12ConstantBufferArea();
	~DX12ConstantBufferArea();

	void	MapGPUAddress();
	void	UnmapGPUAddress();

	bool	IsMapped() const;

	void	MemCpyToConstantBuffer(UINT8 * i_Src, UINT i_Size);
	void	CpyConstantBuffer(ConstantBufferPerObject & i_Object);
	D3D12_GPU_VIRTUAL_ADDRESS	GetVirtualConstantBufferAddress() const;

private:
	// DX12 
	ADDRESS_ID				m_ConstantBufferGPUAddress;
};