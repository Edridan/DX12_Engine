// DX12 render engine
// This is a singleton

#pragma once

#include <d3d12.h>
#include <Windows.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include "dx12/DX12Utils.h"
#include "dx12/DX12Shader.h"

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
		// 3D space computing
		DirectX::XMFLOAT4X4		m_Model;
		DirectX::XMFLOAT4X4		m_View;
		DirectX::XMFLOAT4X4		m_Projection;
		// other
		FLOAT					m_Time;
		DirectX::XMFLOAT3		m_CameraForward;
	};

	struct PipelineStateObject
	{
		ID3D12PipelineState *	m_PipelineState;
		ID3D12RootSignature *	m_DefaultRootSignature;
	};

	struct ShaderPipeline
	{
		DX12Shader *		m_Pixel;
		DX12Shader *		m_Vertex;
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
	// this is used for default basic rendering
	// if you need other pipeline states (skinned objects for example) create a pipeline state objects on your side
	PipelineStateObject *		GetPipelineStateObject(UINT64 i_Flag);
	DXGI_SAMPLE_DESC			GetSampleDesc() const;
	// get the shaders for default pipeline state objects
	DX12Shader *				GetShader(UINT64 i_Flags, DX12Shader::EShaderType i_Type);
	// close the render engine
	HRESULT						Close();
	HRESULT						ResizeRenderTargets(const IntVec2 & i_NewSize);
	
	// dx12 helpers
	// For creation of resources in the GPU
	struct HeapProperty
	{
		enum Enum
		{
			Default,
			Upload,
			ReadBack,

			Count
		};

		D3D12_HEAP_PROPERTIES m_properties;
		D3D12_RESOURCE_STATES m_state;
	};

	static const HeapProperty s_HeapProperties[];
	// create comitted resource
	ID3D12Resource *			CreateComittedResource(HeapProperty::Enum i_HeapProperty, uint64_t i_Size, D3D12_RESOURCE_FLAGS i_Flags = D3D12_RESOURCE_FLAG_NONE) const;
	ID3D12Resource *			CreateComittedResource(HeapProperty::Enum i_HeapProperty, D3D12_RESOURCE_DESC * i_ResourceDesc, D3D12_CLEAR_VALUE * i_ClearValue) const;

	// Get/Set
	int								GetFrameIndex() const;
	int								GetFrameBufferCount() const;
	D3D12_RECT &					GetScissor();
	D3D12_VIEWPORT &				GetViewport();
	IDXGISwapChain3 *				SwapChain() const;
	ID3D12Device*					GetDevice() const;
	ID3D12GraphicsCommandList *		GetCommandList() const;;
	ID3D12CommandQueue*				GetCommandQueue() const;
	const DXGI_SWAP_CHAIN_DESC &	GetSwapChainDesc() const;
	D3D12_CPU_DESCRIPTOR_HANDLE		GetRenderTarget() const;
	bool							IsDX12DebugEnabled() const;
	IntVec2							GetRenderSize() const;

private:
	DX12RenderEngine(HINSTANCE & i_HInstance);
	~DX12RenderEngine();

	void		CleanUp();

	// DX12 Internal management
	HRESULT				UpdatePipeline();
	HRESULT				WaitForPreviousFrame();
	// Engine initialization
	void				GenerateDefaultPipelineState();	// load shaders, root and pipeline state for rendering objects
	void				CreatePipelineState(UINT64 i_Flags);
	HRESULT				LoadShader(const wchar_t * i_Filename, DX12Shader::EShaderType i_ShaderType, UINT64 i_Flags);

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

	// size
	IntVec2						m_WindowSize;


	// Shader
	DX12Shader *				m_DefaultPixelShader;
	DX12Shader *				m_DefaultVertexShader;

	// all default pipeline state objects for rendering are stored in this map (this is used only for basic rendering, if it need alpha, rigging and skinning create your own Pipeline State)
	std::unordered_map<UINT64, PipelineStateObject*>	m_PipelineStateObjects;	// this containing pipeline states and root
	std::unordered_map<UINT64, DX12Shader*>				m_PixelShaders, m_VertexShaders;	// this containing pixel and vertices shaders depending the flags

	// Render
	D3D12_VIEWPORT				m_Viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT					m_ScissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	// Singleton
	static DX12RenderEngine *		s_Instance;
};