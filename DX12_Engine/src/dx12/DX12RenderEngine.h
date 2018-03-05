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

#include "engine/Utils.h"

#ifdef DX12_DEBUG
class DX12Debug;
#include <D3d12sdklayers.h>
#endif

// class predef
class DX12ConstantBuffer;
class DX12RenderTarget;
class DX12RootSignature;
class DX12PipelineState;
class DX12DepthBuffer;
class DX12Mesh;
class DX12Context;

// Render engine implementation
class DX12RenderEngine
{
public:
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
	HRESULT			InitializeRender();	// call this after the DX12Resource manager instanciation
	// To do : clean this part of code, pre load all data and generate dependant as context etc...
	HRESULT			PrepareForRender();
	HRESULT			Render();

	//** DX12 Management **//
	// pipeline state management, this is used for default basic rendering
	// if you need other pipeline states (skinned objects for example) create a pipeline state objects on your side
	DXGI_SAMPLE_DESC			GetSampleDesc() const;
	// close the render engine
	HRESULT						Close();

	// constant buffer management
	enum EConstantBufferId
	{
		eTransform,		// used for transform matrix 3D space
		eGlobal,		// used for global buffer
		eMaterial,		// used for material specs
		eLight,			// used for light specs

		// count
		eConstantBufferCount,
	};

	DX12ConstantBuffer *		GetConstantBuffer(EConstantBufferId i_Id) const;
	
	// deferred render target management
	enum ERenderTargetId
	{
		eNormal,			// normal buffer for pixels
		eDiffuse,			// specular lighting buffer
		eSpecular,			// diffuse color buffer
		ePosition,			// position buffer

		// count
		eRenderTargetCount,
	};

	DX12RenderTarget *			GetRenderTarget(ERenderTargetId i_Id) const;
	DX12DepthBuffer *			GetDepthBuffer() const;

	// deferred contexts
	enum EContextId
	{
		eDeferred,		// deferred context that will render GBuffer
		eImmediate,		// immediate context that will render the frame using G-Buffer

		eUpload,		// upload resources to GPU or change state

		// count
		eContextCount,
	};
	DX12Context *			GetContext(EContextId i_Id) const;	// context management

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
	// create comitted resource To do : use this to create different resources and manage the creation and deletion of the resources in the dx12 render engine
	ID3D12Resource *			CreateComittedResource(HeapProperty::Enum i_HeapProperty, uint64_t i_Size, D3D12_RESOURCE_FLAGS i_Flags = D3D12_RESOURCE_FLAG_NONE) const;
	ID3D12Resource *			CreateComittedResource(HeapProperty::Enum i_HeapProperty, D3D12_RESOURCE_DESC * i_ResourceDesc, D3D12_CLEAR_VALUE * i_ClearValue) const;
	// free create comitted resources
	void						CreateComittedResource(
		const D3D12_HEAP_PROPERTIES *pHeapProperties,
		D3D12_HEAP_FLAGS HeapFlags,
		const D3D12_RESOURCE_DESC *pResourceDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		const D3D12_CLEAR_VALUE *pOptimizedClearValue,
		REFIID riidResource,
		_COM_Outptr_opt_  void **ppvResource);

	// Get/Set
	int								GetFrameIndex() const;
	int								GetFrameBufferCount() const;

#ifdef DX12_DEBUG
	void			EnableDebug(bool i_Enable) const;
	bool			DebugIsEnabled() const;
#endif

	// render primitive 2D
	void							PushRectPrimitive2D(ID3D12GraphicsCommandList * i_CommandList) const;
	D3D12_INPUT_LAYOUT_DESC			GetPrimitiveInputLayout() const;

	// viewport management
	D3D12_VIEWPORT					GetViewportOnRect(const Rect & i_ViewPort) const;	// coordinates are in : 0 to 1 ranges
	D3D12_RECT &					GetScissor();
	D3D12_VIEWPORT &				GetViewport();
	IntVec2							GetRenderSize() const;

	IDXGISwapChain3 *				SwapChain() const;
	ID3D12Device*					GetDevice() const;
	ID3D12CommandQueue*				GetCommandQueue() const;

	D3D12_CPU_DESCRIPTOR_HANDLE		GetBackBufferDesc() const;
	
	bool							IsDX12DebugEnabled() const;
	
	// get primitive mesh
	DX12Mesh *		GetRectMesh() const;
	
private:
	DX12RenderEngine(HINSTANCE & i_HInstance);
	~DX12RenderEngine();

	// internal
	void		CleanUp();

	// helper
	void		WaitForContext(EContextId i_Context, UINT i_FrameIndex, HANDLE & i_Handle) const;

	// DX12 Internal management
	HRESULT				UpdatePipeline();				// called in Render()
	HRESULT				WaitForPreviousFrame();			// called in PrepareForRender()
	HRESULT				GenerateImmediateContext();		// create immediate context, final rendering pipelines(later : post process management)
	HRESULT				GenerateDeferredContext();		// create different deferred context
	void				GeneratePrimitiveShapes();		// create primitive 2D shapes
	void				GenerateRenderTargets();		// create all render target instead of Back Buffer
	HRESULT				GenerateContexts();
	// Initialize contexts to prepare for render
	HRESULT				InitializeImmediateContext();
	HRESULT				InitializeDeferredContext();

	// Desc
#define FRAME_BUFFER_COUNT		3
	const int m_FrameBufferCount = FRAME_BUFFER_COUNT; // number of buffers we want, 2 for double buffering, 3 for tripple buffering

	// dx12
	ID3D12Device*				m_Device; // direct3d device
	IDXGISwapChain3*			m_SwapChain; // swapchain used to switch between render targets
	DXGI_SWAP_CHAIN_DESC		m_SwapChainDesc;	// swapchain description used for create default pso
	ID3D12CommandQueue*			m_CommandQueue; // container for command lists
	ID3D12CommandQueue*			m_DeferredQueue;
	HANDLE						m_FenceEvent; // a handle to an event when our fence is unlocked by the gpu
	int							m_FrameIndex; // current render target view we are on

#ifdef DX12_DEBUG
	ID3D12Debug *				m_DebugController;
	DX12Debug *					m_Debug;
#endif

	// Render target
	DX12RenderTarget *			m_BackBuffer;	// back buffer render target
	ID3D12Resource*				m_BackBufferResource[FRAME_BUFFER_COUNT]; // render target pointer (setup with swap buffer in the engine and then pass to DX12RenderTarget)
	// Deferred rendering
	DX12RenderTarget *			m_RenderTargets[ERenderTargetId::eRenderTargetCount];
	DX12Context *				m_Context[EContextId::eContextCount];

	// Immediate context pipeline state
	DX12RootSignature *		m_ImmediateRootSignature;
	DX12PipelineState *		m_ImmediatePipelineState;
	ADDRESS_ID				m_ImmediateContextBuffer;

	// primitive rectangle mesh
	DX12Mesh *				m_RectMesh;

	// Depth buffer
	DX12DepthBuffer *		m_DepthBuffer;	// Depth buffer handler


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

	// Render
	D3D12_VIEWPORT				m_Viewport; // area that output from rasterizer will be stretched to.
	D3D12_RECT					m_ScissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	// Singleton
	static DX12RenderEngine *		s_Instance;
};