// Depth buffer management

#pragma once

#include "d3dx12.h"
#include "engine/Utils.h"
#include <string>

class DX12DepthBuffer
{
public:
	struct DepthBufferDesc
	{
		DXGI_FORMAT		Format	= DXGI_FORMAT_D32_FLOAT;
		D3D12_DSV_FLAGS	Flags	= D3D12_DSV_FLAG_NONE;
		D3D12_CLEAR_VALUE	DepthOptimizedClearValue = { DXGI_FORMAT_D32_FLOAT , { 1.f, 0.f } };
		IntVec2				BufferSize;

		std::wstring			Name = L"Depth Stencil";

	};

	// constructor/destructor
	DX12DepthBuffer(const DepthBufferDesc & i_Desc);
	~DX12DepthBuffer();

	DXGI_FORMAT			GetFormat() const;

	ID3D12DescriptorHeap *		GetDepthStencilDescriptorHeap() const;

private:
	// dx12
	ID3D12Resource*				m_DepthStencilBuffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ID3D12DescriptorHeap*		m_DepthStencilDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor

	// informations
	DXGI_FORMAT			m_Format;
};