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

#ifdef _DEBUG
#include <D3d12sdklayers.h>
#endif

// class predef
class DX12ConstantBuffer;
class DX12RenderTarget;

// define
#define DEBUG_DX12_ENABLE		1

// debug management
#if (DEBUG_DX12_ENABLE) && defined(_DEBUG)
#define DX12_DEBUG
#endif

// Render engine implementation
class DX12RenderEngine
{
public:
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
	// pipeline state management
	// this is used for default basic rendering
	// if you need other pipeline states (skinned objects for example) create a pipeline state objects on your side
	PipelineStateObject *		GetPipelineStateObject(UINT64 i_Flag);
	DXGI_SAMPLE_DESC			GetSampleDesc() const;
	// get the shaders for default pipeline state objects
	DX12Shader *				GetShader(UINT64 i_Flags, DX12Shader::EShaderType i_Type);
	// close the render engine
	HRESULT						Close();

	// constant buffer management
	enum EConstantBufferId
	{
		eTransform,		// used for transform matrix 3D space
		eGlobal,		// used for global buffer
		eMaterial,		// used for material specs

		// count
		eConstantBufferCount,
	};

	DX12ConstantBuffer *		GetConstantBuffer(EConstantBufferId i_Id);
	
	// deferred render target management
	enum ERenderTargetId
	{
		eNormal,			// normal buffer for pixels
		eSpecular,			// specular lighting buffer
		eDiffuse,			// diffuse color buffer

		// count
		eRenderTargetCount,
	};

	DX12RenderTarget *			GetRenderTarget(ERenderTargetId i_Id);

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

	// DX12 Implementation
	ID3D12Device*				m_Device; // direct3d device
	IDXGISwapChain3*			m_SwapChain; // swapchain used to switch between render targets
	DXGI_SWAP_CHAIN_DESC		m_SwapChainDesc;	// swapchain description used for create default pso
	ID3D12CommandQueue*			m_CommandQueue; // container for command lists
	ID3D12CommandAllocator*		m_CommandAllocator[FRAME_BUFFER_COUNT]; // we want enough allocators for each buffer * number of threads (we only have one thread)
	ID3D12GraphicsCommandList*	m_CommandList; // a command list we can record commands into, then execute them to render the frame
	ID3D12Fence*				m_Fences[FRAME_BUFFER_COUNT];		// an object that is locked while our command list is being executed by the gpu. We need as many as we have allocators (more if we want to know when the gpu is finished with an asset)
	HANDLE						m_FenceEvent; // a handle to an event when our fence is unlocked by the gpu
	UINT64						m_FenceValue[FRAME_BUFFER_COUNT]; // this value is incremented each frame. each fence will have its own value
	int							m_FrameIndex; // current render target view we are on
#ifdef DX12_DEBUG
	// Debug
	ID3D12Debug *				m_DebugController;
#endif

	// Render target
	DX12RenderTarget *			m_BackBuffer;	// back buffer render target
	ID3D12Resource*				m_BackBufferResource[FRAME_BUFFER_COUNT]; // render target pointer (setup with swap buffer in the engine and then pass to DX12RenderTarget)
	// Deferred rendering
	DX12RenderTarget *			m_RenderTargets[ERenderTargetId::eRenderTargetCount];


	// Depth buffer
	ID3D12Resource*				m_DepthStencilBuffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ID3D12DescriptorHeap*		m_DepthStencilDescriptorHeap; // This is a heap for our depth/stencil buffer descriptor

	// Constant buffer
	struct ConstantBufferDef
	{
		UINT		ElementSize;
		UINT		ElementCount;
	};
	static const ConstantBufferDef	s_ConstantBufferSize[EConstantBufferId::eConstantBufferCount];	// setup this array to manage the size of the constant buffer
	DX12ConstantBuffer *			m_ConstantBuffer[EConstantBufferId::eConstantBufferCount];	// constant buffer are created here and used/managed from other space

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