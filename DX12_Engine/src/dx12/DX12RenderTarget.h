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
		float				ClearValue[4]		= {0.f, 0.f, 0.f, 1.f};				// optimized clear value
	};

	DX12RenderTarget(const RenderTargetDesc & i_Desc);
	~DX12RenderTarget();

	// dx12
	DX12DescriptorHeap *			GetRenderTargetDescriptorHeap() const;
	DX12DescriptorHeap *			GetShaderResourceDescriptorHeap() const;
	// transition between 2 resources types
	void			ResourceBarrier(ID3D12GraphicsCommandList * i_CommandList, D3D12_RESOURCE_STATES i_From, D3D12_RESOURCE_STATES i_To, UINT i_Index = ((UINT)-1)) const;

	// handle (render target)
	D3D12_CPU_DESCRIPTOR_HANDLE		GetRenderTargetCPUDescriptorHandle(UINT i_Index = ((UINT)-1)) const;	// get the descriptor as render target (used for drawing in buffer)
	
																											// handle (shader resource)
	D3D12_CPU_DESCRIPTOR_HANDLE		GetShaderResourceCPUDescriptorHandle(UINT i_Index = ((UINT)-1)) const;		// get the descriptor as texture (used for reading buffer)

	// buffer management
	HRESULT							ResizeBuffer(const IntVec2 & i_Size);
	// barrier management
	CD3DX12_RESOURCE_BARRIER		GetResourceBarrier(D3D12_RESOURCE_STATES i_StateBefore, D3D12_RESOURCE_STATES i_StateAfter, UINT i_Index = ((UINT)-1)) const;

	// information
	DXGI_FORMAT						GetFormat() const;
	const float *					GetClearValue() const;
private:
	// helper
	UINT			GetIndex(UINT i_Index) const;

	// render target 
	DX12DescriptorHeap *			m_RenderTargetDesc;	// descriptor heap for render target
	DX12DescriptorHeap *			m_ShaderResourceDesc;	// descriptor heap for shader resource
	ID3D12Resource **				m_RenderTarget;		// render target resources

	// information
	DXGI_FORMAT		m_Format;

	// internal
	UINT				m_FrameCount;
	const std::wstring	m_Name;

	// resource management
	bool		m_CustomAmount;
	const bool	m_IsAllocator;
	const bool	m_IsResourceView;
	float 		m_ClearColor[4];
};