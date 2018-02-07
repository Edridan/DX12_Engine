#pragma once

#include "dx12/DX12Texture.h"
#include "dx12/DX12DescriptorHeap.h"
#include <dxgi1_4.h>
#include "d3dx12.h"


class DX12RenderTarget
{
public:

	struct RenderTargetDesc
	{
		IntVec2				BufferSize;						// size of the render texture buffer
		UINT				BufferCount			= 0;		// if 0 : use the frame index from render engine
		ID3D12Resource **	Resource			= nullptr;	// if not nullptr : the resources pointer are already created
		std::wstring		Name				= L"RenderTarget";
		bool				IsShaderResource	= false;	// if true : the render target can also be binded as texture
		DXGI_FORMAT			Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;	// format of the resource
	};


	DX12RenderTarget(const RenderTargetDesc & i_Desc);
	~DX12RenderTarget();

	// dx12
	D3D12_CPU_DESCRIPTOR_HANDLE		GetRenderTargetDescriptor(UINT i_Index = ((UINT)-1)) const;	// get the descriptor as render target (used for drawing in buffer)
	D3D12_CPU_DESCRIPTOR_HANDLE		GetTextureDescriptor(UINT i_Index = 0) const;		// get the descriptor as texture (used for reading buffer)

private:
	// render target 
	DX12DescriptorHeap *			m_RenderTargetDesc;	// descriptor heap for render target
	DX12DescriptorHeap *			m_ShaderResourceDesc;	// descriptor heap for shader resource
	ID3D12Resource **				m_RenderTarget;		// render target resources

	// internal
	UINT				m_FrameCount;
	const std::wstring	m_Name;
	// resource management
	bool		m_CustomAmount;
	const bool	m_IsAllocator;
	const bool	m_IsResourceView;
};