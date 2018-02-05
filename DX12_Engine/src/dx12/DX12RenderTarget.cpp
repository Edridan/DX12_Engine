#include "DX12RenderTarget.h"

DX12RenderTarget::DX12RenderTarget()
	:DX12Texture()
{
}

DX12RenderTarget::~DX12RenderTarget()
{
}

HRESULT DX12RenderTarget::BindToCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	return E_NOTIMPL;
}
