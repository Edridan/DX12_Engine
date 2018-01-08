#include "DX12ConstantBufferArea.h"

DX12ConstantBufferArea::DX12ConstantBufferArea()
	:m_ConstantBufferGPUAddress((UINT64)-1)
{
}

DX12ConstantBufferArea::~DX12ConstantBufferArea()
{
	// unmap if necessary the address
	UnmapGPUAddress();
}

void DX12ConstantBufferArea::MapGPUAddress()
{
	if (!IsMapped())
	{
		m_ConstantBufferGPUAddress = DX12RenderEngine::GetInstance().ReserveConstantBufferVirtualAddress();

		// The address is not available
		if (m_ConstantBufferGPUAddress == (UINT64)-1)
		{
			DX12RenderEngine::GetInstance().PopUpError(L"Issue when allocating a constant buffer area");
		}
	}
}

void DX12ConstantBufferArea::UnmapGPUAddress()
{
	if (IsMapped())
	{
		DX12RenderEngine::GetInstance().ReleaseConstantBufferVirtualAddress(m_ConstantBufferGPUAddress);
	}
}

bool DX12ConstantBufferArea::IsMapped() const
{
	return m_ConstantBufferGPUAddress != ((UINT64)-1);
}

void DX12ConstantBufferArea::MemCpyToConstantBuffer(UINT8 * i_Src, UINT i_Size)
{
	UINT8 * address = DX12RenderEngine::GetInstance().GetConstantBufferGPUAddress(m_ConstantBufferGPUAddress);
	memcpy(address, i_Src, i_Size);
}

void DX12ConstantBufferArea::CpyConstantBuffer(ConstantBufferPerObject & i_Object)
{
	UINT8 * address = DX12RenderEngine::GetInstance().GetConstantBufferGPUAddress(m_ConstantBufferGPUAddress);
	memcpy(address, &i_Object, sizeof(i_Object));
}

D3D12_GPU_VIRTUAL_ADDRESS DX12ConstantBufferArea::GetVirtualConstantBufferAddress() const
{
	return DX12RenderEngine::GetInstance().GetConstantBufferUploadVirtualAddress(m_ConstantBufferGPUAddress);
}
