// DX12 render engine
// This is a singleton

#pragma once

#include <d3d12.h>
#include <Windows.h>
#include <DirectXMath.h>

#include "DX12Utils.h"
#include "DX12Window.h"
#include "DX12Shader.h"

// define
#define DEBUG_DX12_ENABLE		1

enum PiplineStateFlags
{
	eDefault			= 0 << 0,
	eTextured			= 1 << 0,
};

// Render engine implementation
class DX12RenderEngine
{
public:
	// Constant buffer definition
	struct DefaultConstantBuffer
	{
		DirectX::XMFLOAT4X4		m_Model;
		DirectX::XMFLOAT4X4		m_View;
		DirectX::XMFLOAT4X4		m_Projection;
	};

	// Singleton
	static DX12RenderEngine &	GetInstance();
	static void					Create(HINSTANCE & i_HInstance);
	static void					Delete();

	// main call for engine
	HRESULT			InitializeDX12();
	HRESULT			PrepareForRender();
	HRESULT			Render();

	//** DX12 Management **//
	// fences
	HRESULT						IncrementFence();
	// constant buffer allocation
	ADDRESS_ID					ReserveConstantBufferVirtualAddress();
	void						ReleaseConstantBufferVirtualAddress(ADDRESS_ID i_Address);
	void						UpdateConstantBuffer(ADDRESS_ID i_Address, DefaultConstantBuffer & i_ConstantBuffer);
	UINT8 *						GetConstantBufferGPUAddress(ADDRESS_ID i_Address) const;
	D3D12_GPU_VIRTUAL_ADDRESS	GetConstantBufferUploadVirtualAddress(ADDRESS_ID i_Address) const;
	// pipeline state management
	ID3D12PipelineState *		GetPipelineState(UINT64 i_Flag);
	ID3D12PipelineState *		GetDefaultPipelineState() const;
	// shader management
	HRESULT						LoadShader(const wchar_t i_Filename);

	// Get/Set
	int								GetFrameIndex() const;
	int								GetFrameBufferCount() const;
	D3D12_RECT &					GetScissor();
	D3D12_VIEWPORT &				GetViewport();
	IDXGISwapChain3 *				SwapChain() const;
	ID3D12Device*					GetDevice() const;
	ID3D12GraphicsCommandList *		GetCommandList() const;
	ID3D12RootSignature*			GetRootSignature() const;
	ID3D12CommandQueue*				GetCommandQueue() const;
	const DXGI_SWAP_CHAIN_DESC &	GetSwapChainDesc() const;
	bool							IsDX12DebugEnabled() const;

	// Engine
	DX12Window &					GetWindow();
	void							UpdateWindow();

private:
	DX12RenderEngine(HINSTANCE & i_HInstance);
	~DX12RenderEngine();


	// Engine implementation
	DX12Window			m_Window;

	// DX12 Internal management
	HRESULT				UpdatePipeline();
	HRESULT				WaitForPreviousFrame();

	// Desc
#define FRAME_BUFFER_COUNT		3
	const int m_FrameBufferCount = FRAME_BUFFER_COUNT; // number of buffers we want, 2 for double buffering, 3 for tripple buffering
	const static int ConstantBufferPerObjectAlignedSize;

	// DX12 Implementation
	ID3D12Device*				m_Device; // direct3d device
	IDXGISwapChain3*			m_SwapChain; // swapchain used to switch between render targets
	DXGI_SWAP_CHAIN_DESC		m_SwapChainDesc;	// swapchain description used for create default pso
	ID3D12CommandQueue*			m_CommandQueue; // container for command lists
	ID3D12DescriptorHeap*		m_RtvDescriptorHeap; // render target view descriptor
	ID3D12Resource*				m_RenderTargets[FRAME_BUFFER_COUNT]; // number of render targets equal to buffer count
	ID3D12CommandAllocator*		m_CommandAllocator[FRAME_BUFFER_COUNT]; // we want enough allocators for each buffer * number of threads (we only have one thread)
	ID3D12GraphicsCommandList*	m_CommandList; // a command list we can record commands into, then execute them to render the frame
	ID3D12Fence*				m_Fences[FRAME_BUFFER_COUNT];		// an object that is locked while our command list is being executed by the gpu. We need as many as we have allocators (more if we want to know when the gpu is finished with an asset)
	HANDLE						m_FenceEvent; // a handle to an event when our fence is unlocked by the gpu
	UINT64						m_FenceValue[FRAME_BUFFER_COUNT]; // this value is incremented each frame. each fence will have its own value
	int							m_FrameIndex; // current render target view we are on
	int							m_RtvDescriptorSize; // size of the rtv descriptor on the device (all front and back buffers will be the same size)

	// Debug
	ID3D12Debug *				m_DebugController;

	// Depth buffer
	ID3D12Resource*				m_DepthStencilBuffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ID3D12DescriptorHeap*		m_DepthStencilDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor

	// Constant buffer
	#define  CONSTANT_BUFFER_HEAP_SIZE			32
	UINT						m_ConstantBufferHeapSize = CONSTANT_BUFFER_HEAP_SIZE;
	ID3D12DescriptorHeap *		m_MainDescriptorHeap[FRAME_BUFFER_COUNT];	// Warning : unused
	ID3D12Resource *			m_ConstantBufferUploadHeap[FRAME_BUFFER_COUNT];	// memory where constant buffers for each frame will be placed
	UINT8 *						m_ConstantBufferGPUAdress[FRAME_BUFFER_COUNT];	// pointer for each of the resource buffer constant heap
	bool						m_ConstantBufferReservedAddress[CONSTANT_BUFFER_HEAP_SIZE];	// internal constant buffer management

	// Shader
	DX12Shader *				m_DefaultPixelShader;
	DX12Shader *				m_DefaultVertexShader;

	// Pipeline state
	ID3D12PipelineState *		m_DefaultPipelineState;	// pipeline state that define all rendering process
	ID3D12RootSignature*		m_DefaultRootSignature; // root signature defines data shaders will access

	// Render
	D3D12_VIEWPORT				m_Viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT					m_ScissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	// Singleton
	static DX12RenderEngine *		s_Instance;
};