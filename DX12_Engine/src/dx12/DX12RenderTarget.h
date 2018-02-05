#pragma once

#include "d3dx12.h"

#include "dx12/DX12Texture.h"

class DX12RenderTarget : public DX12Texture
{
public:
	DX12RenderTarget();
	~DX12RenderTarget();

	// dx12
	HRESULT		BindToCommandList(ID3D12GraphicsCommandList * i_CommandList) const;	// bind the render target to the command list



private:

};