#include "DX12DepthBuffer.h"

#include "dx12/DX12RenderEngine.h"

DX12DepthBuffer::DX12DepthBuffer(const DepthBufferDesc & i_Desc)
	:m_DepthStencilBuffer(nullptr)
	,m_DepthStencilDescriptorHeap(nullptr)
{
	// retreive the device to create resource
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	DX12_ASSERT(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DepthStencilDescriptorHeap)));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = i_Desc.Format;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = i_Desc.Flags;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = i_Desc.Format;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, (UINT)i_Desc.BufferSize.x, (UINT)i_Desc.BufferSize.y, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthStencilBuffer)
	);

	m_DepthStencilDescriptorHeap->SetName((i_Desc.Name + L" Resource Heap").c_str());
	device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	m_DepthStencilBuffer->SetName((i_Desc.Name + L" Buffer").c_str());
}

DX12DepthBuffer::~DX12DepthBuffer()
{
	SAFE_RELEASE(m_DepthStencilBuffer);
	SAFE_RELEASE(m_DepthStencilDescriptorHeap);
}

ID3D12DescriptorHeap * DX12DepthBuffer::GetDepthStencilDescriptorHeap() const
{
	return m_DepthStencilDescriptorHeap;
}
