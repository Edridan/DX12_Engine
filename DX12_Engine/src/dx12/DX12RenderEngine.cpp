#include "dx12/DX12RenderEngine.h"

#include <assert.h>
#include "dx12/d3dx12.h"
#include "dx12/DX12Mesh.h"
#include "dx12/DX12Context.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RenderTarget.h"
#include "engine/Engine.h"



// Static definition implementation
DX12RenderEngine * DX12RenderEngine::s_Instance = nullptr;

// constant buffer size are setupped here
const DX12RenderEngine::ConstantBufferDef			DX12RenderEngine::s_ConstantBufferSize[] =
{
	// {ElementSize, ElementCount}
	{256, 1024},		// transform
	{256, 16},			// global buffer (always pointing on the same)
	{256, 1024},		// materials
};

const DX12RenderEngine::HeapProperty DX12RenderEngine::s_HeapProperties[] =
{
	{ { D3D12_HEAP_TYPE_DEFAULT,  D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 }, D3D12_RESOURCE_STATE_COMMON },
	{ { D3D12_HEAP_TYPE_UPLOAD,   D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 }, D3D12_RESOURCE_STATE_GENERIC_READ },
	{ { D3D12_HEAP_TYPE_READBACK, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 }, D3D12_RESOURCE_STATE_COPY_DEST },
};


DX12RenderEngine & DX12RenderEngine::GetInstance()
{
	assert(s_Instance != nullptr);
	return *s_Instance;
}

void DX12RenderEngine::Create(HINSTANCE & i_HInstance)
{
	assert(s_Instance == nullptr);
	s_Instance = new DX12RenderEngine(i_HInstance);
}

void DX12RenderEngine::Delete()
{
	assert(s_Instance != nullptr);
	s_Instance->CleanUp();
	delete s_Instance;
	s_Instance = nullptr;
}

HRESULT DX12RenderEngine::InitializeDX12()
{
	HRESULT hr;

	// To do : clean this part of code
	Window * window = Engine::GetInstance().GetWindow();
	m_WindowSize = window->GetBackSize();

	// -- Debug -- //


#ifdef DX12_DEBUG
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController));

	if (FAILED(hr))
	{
		ASSERT_ERROR("Error D3D12GetDebugInterface");
		return E_FAIL;
	}

	m_DebugController->EnableDebugLayer();
#endif


	// -- Create the Device -- //

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error when creating the DXGIFactory1");
		return E_FAIL;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapterFound = false; // set this to true when a good one was found
							   // find first hardware gpu that supports d3d 12

	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;	// we dont want a software m_Device
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			adapterFound = true;
			break;
		}

		++adapterIndex;
	}

	if (!adapterFound)
	{
		ASSERT_ERROR("Error : Device compatible with DX12 not found");
		DEBUG_BREAK;
		return E_FAIL;
	}

	// Create the m_Device
	hr = D3D12CreateDevice(
		adapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_Device)
	);
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : D3D12CreateDevice");
		return E_FAIL;
	}

	// -- Create command queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_CommandQueue)); // create the command queue
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateCommandQueue");
		return E_FAIL;
	}

	// -- Create the Swap Chain (double/tripple buffering) -- //

	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = m_WindowSize.x; // buffer width
	backBufferDesc.Height = m_WindowSize.y; // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

														// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Describe and create the swap chain.
	m_SwapChainDesc = {};
	m_SwapChainDesc.BufferCount = m_FrameBufferCount; // number of buffers we have
	m_SwapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	m_SwapChainDesc.OutputWindow = window->GetHWnd(); // handle to our window
	m_SwapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	m_SwapChainDesc.Windowed = true; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;

	hr = dxgiFactory->CreateSwapChain(
		m_CommandQueue, // the queue will be flushed once the swap chain is created
		&m_SwapChainDesc, // give it the swap chain description we created above
		&tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
	);

	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateSwapChain");
		return E_FAIL;
	}

	// retreive the swapchain
	m_SwapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();


	// -- Create Context -- //

	const std::wstring contextNames[eContextCount] = {
		L"Deferred",			// the GBuffer update context
		L"Immediate",			// the Immediate (final rendering)
		L"Copy",				// copy commandlist for resources
	};

	DX12Context::ContextDesc desc[eContextCount];

	// setup for somes context
	desc[EContextId::eUpload].CommandListType = D3D12_COMMAND_LIST_TYPE_COPY;

	for (UINT i = 0; i < eContextCount; ++i)
	{
		desc[i].Name = contextNames[i];
		m_Context[i] = new DX12Context(desc[i]);
	}

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBufferResource[i]));
		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : m_SwapChain->GetBuffer");
			return E_FAIL;
		}
	}

	DX12RenderTarget::RenderTargetDesc backRTVDesc;
	backRTVDesc.Format				= backBufferDesc.Format;
	backRTVDesc.Name				= L"Back Buffer";
	backRTVDesc.IsShaderResource	= false;
	backRTVDesc.Resource			= m_BackBufferResource;
	
	m_BackBuffer = new DX12RenderTarget(backRTVDesc);

	// create a handle to a m_Fences event
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		ASSERT_ERROR("Error : Fences -> CreateEvent");
		return E_FAIL;
	}

	// -- Create different render targets for the deferred rendering -- //

	const std::wstring rtName[eRenderTargetCount] =
	{
		L"Normal",
		L"Specular",
		L"Diffuse",
	};

	// same description for each render target
	DX12RenderTarget::RenderTargetDesc rtDesc;
	rtDesc.BufferSize = m_WindowSize;
	rtDesc.IsShaderResource = true;

	for (UINT i = 0; i < eRenderTargetCount; ++i)
	{
		rtDesc.Name = rtName[i];
		m_RenderTargets[i] = new DX12RenderTarget(rtDesc);
	}

	// -- Create constant buffer -- //

	for (size_t i = 0; i < EConstantBufferId::eConstantBufferCount; ++i)
	{
		// create constant buffer of 256 slots of 256 bytes
		m_ConstantBuffer[i] = new DX12ConstantBuffer(
			s_ConstantBufferSize[i].ElementCount, 
			s_ConstantBufferSize[i].ElementSize
		);
	}

	// -- Create multiple default Pipeline State and Root Signature for predifined -- //

	// generate default pipeline states objects
	// this is going to create default pipeline state for drawing default objects
	// this features : vertex coloring, one texture handling
	GenerateImmediatePipelineState();

	// -- Create depth/stencil buffer -- //

	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DepthStencilDescriptorHeap));

	if (FAILED(hr))
	{
		ASSERT_ERROR("Error during CreateDescriptorHeap Depth/Stencil creation");
		return hr;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format			= DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension	= D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags			= D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format					= DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth		= 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil	= 0;

	m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_WindowSize.x, m_WindowSize.y, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthStencilBuffer)
	);

	m_DepthStencilDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	// create view
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_DepthStencilBuffer->SetName(L"Depth Stencil buffer");


	// -- Setup viewport and scissor -- //

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = (FLOAT)m_WindowSize.x;
	m_Viewport.Height = (FLOAT)m_WindowSize.y;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = m_WindowSize.x;
	m_ScissorRect.bottom = m_WindowSize.y;

	return S_OK;
}

HRESULT DX12RenderEngine::PrepareForRender()
{
	
	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();
	
	// initialize contexts
	InitializeDeferredContext();
	InitializeImmediateContext();

	return S_OK;
}

HRESULT DX12RenderEngine::Render()
{
	if (FAILED(UpdatePipeline()))
	{
		ASSERT_ERROR("Error during update pipeline");
		return E_FAIL;
	}
	else
	{
		DX12Context * context = GetContext(eImmediate);

		// create an array of command lists (only one command list here)
		ID3D12CommandList* ppCommandLists[] = { context->GetCommandList() };
		UINT64 fenceValue = context->GetFenceValue(m_FrameIndex);
		ID3D12Fence * fence = context->GetFence(m_FrameIndex);

		// execute the array of command lists
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// this command goes in at the end of our command queue. we will know when our command queue 
		// has finished because the m_Fences value will be set to "m_FenceValue" from the GPU since the command
		// queue is being executed on the GPU
		DX12_ASSERT(m_CommandQueue->Signal(fence, fenceValue));

		// present the current back buffer
		DX12_ASSERT(m_SwapChain->Present(0, 0));
	}

	return S_OK;
}

DXGI_SAMPLE_DESC DX12RenderEngine::GetSampleDesc() const
{
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	return sampleDesc;
}

HRESULT DX12RenderEngine::Close()
{
	HRESULT hr;

	CloseHandle(m_FenceEvent);
	hr = WaitForPreviousFrame();

	return hr;
}

DX12ConstantBuffer * DX12RenderEngine::GetConstantBuffer(EConstantBufferId i_Id) const
{
	ASSERT(i_Id < eConstantBufferCount);
	return m_ConstantBuffer[i_Id];
}

DX12RenderTarget * DX12RenderEngine::GetRenderTarget(ERenderTargetId i_Id) const
{
	ASSERT(i_Id < eRenderTargetCount);
	return m_RenderTargets[i_Id];
}

DX12Context * DX12RenderEngine::GetContext(EContextId i_Id) const
{
	ASSERT(i_Id < eContextCount);
	return m_Context[i_Id];
}

ID3D12Resource * DX12RenderEngine::CreateComittedResource(HeapProperty::Enum i_HeapProperty, uint64_t i_Size, D3D12_RESOURCE_FLAGS i_Flags) const
{
	if (i_HeapProperty >= HeapProperty::Enum::Count)	return nullptr;

	// create the resource desc based on parmaters
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension	= D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment	= 0;
	resourceDesc.Width		= i_Size;
	resourceDesc.Height		= 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels	= 1;
	resourceDesc.Format		= DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count	= 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout		= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags		= i_Flags;

	return CreateComittedResource(i_HeapProperty, &resourceDesc, nullptr);
}

ID3D12Resource * DX12RenderEngine::CreateComittedResource(HeapProperty::Enum i_HeapProperty, D3D12_RESOURCE_DESC * i_ResourceDesc, D3D12_CLEAR_VALUE * i_ClearValue) const
{
	const HeapProperty & heapProperty = s_HeapProperties[i_HeapProperty];
	ID3D12Resource * resource;

	m_Device->CreateCommittedResource(&heapProperty.m_properties
		, D3D12_HEAP_FLAG_NONE
		, i_ResourceDesc
		, heapProperty.m_state
		, i_ClearValue
		, __uuidof(ID3D12Resource)
		, (void**)&resource
	);

	return resource;
}

int DX12RenderEngine::GetFrameIndex() const
{
	return m_FrameIndex;
}

int DX12RenderEngine::GetFrameBufferCount() const
{
	return m_FrameBufferCount;
}

D3D12_RECT & DX12RenderEngine::GetScissor()
{
	return m_ScissorRect;
}

D3D12_VIEWPORT & DX12RenderEngine::GetViewport()
{
	return m_Viewport;
}

IDXGISwapChain3 * DX12RenderEngine::SwapChain() const
{
	return m_SwapChain;
}

ID3D12Device * DX12RenderEngine::GetDevice() const
{
	return m_Device;
}

ID3D12CommandQueue * DX12RenderEngine::GetCommandQueue() const
{
	return m_CommandQueue;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderEngine::GetBackBufferDesc() const
{
	return m_BackBuffer->GetRenderTargetDescriptor(m_FrameIndex);
}

bool DX12RenderEngine::IsDX12DebugEnabled() const
{
#ifdef DX12_DEBUG
	return true;
#else
	return false;
#endif
}

IntVec2 DX12RenderEngine::GetRenderSize() const
{
	return m_WindowSize;
}

DX12RenderEngine::DX12RenderEngine(HINSTANCE & i_HInstance)
{
}

DX12RenderEngine::~DX12RenderEngine()
{
}

void DX12RenderEngine::CleanUp()
{
	// Cleanup resources
	// wait for the gpu to finish all frames
	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		m_FrameIndex = i;
		WaitForPreviousFrame();
	}

	// get swapchain out of full screen before exiting
	BOOL fs = false;
	if (m_SwapChain->GetFullscreenState(&fs, NULL))
		m_SwapChain->SetFullscreenState(false, NULL);

	// delete render targets
	delete m_BackBuffer;

	for (UINT i = 0; i < eRenderTargetCount; ++i)
	{
		delete m_RenderTargets[i];
	}

	// To do : release properly data : might have some random crash here


	// delete context
	for (UINT i = 0; i < eContextCount; ++i)
	{
		delete m_Context[i];
	}

	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_BackBufferResource[i]);
	};

	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_CommandQueue);

	SAFE_RELEASE(m_DepthStencilBuffer);
	SAFE_RELEASE(m_DepthStencilDescriptorHeap);

	// delete resources
	for (int i = 0; i < EConstantBufferId::eConstantBufferCount; ++i)
	{
		// delete the constant buffer
		delete (m_ConstantBuffer[i]);
	}

	// release debug resources
#ifdef DX12_DEBUG
	SAFE_RELEASE(m_DebugController);
#endif

	SAFE_RELEASE(m_Device);
}

FORCEINLINE void DX12RenderEngine::WaitForContext(EContextId i_Context, UINT i_FrameIndex, HANDLE & i_Handle) const
{
	HRESULT hr;
	DX12Context * context = GetContext(i_Context);

	ID3D12Fence * fence = context->GetFence(i_FrameIndex);
	UINT64 fenceValue = context->GetFenceValue(i_FrameIndex);

	// if the current m_Fences value is still less than "m_FenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "m_CommandQueue->Signal(m_Fences, m_FenceValue)" command
	if (fence->GetCompletedValue() < fenceValue)
	{
		// we have the m_Fences create an event which is signaled once the m_Fences's current value is "m_FenceValue"
		hr = fence->SetEventOnCompletion(fenceValue, i_Handle);

		// We will wait until the m_Fences has triggered the event that it's current value has reached "m_FenceValue". once it's value
		// has reached "m_FenceValue", we know the command queue has finished executing
		WaitForSingleObject(i_Handle, INFINITE);
	}

	// increment m_FenceValue for next frame
	context->IncrementFenceValue(m_FrameIndex);
}

HRESULT DX12RenderEngine::UpdatePipeline()
{
	// transition the "m_FrameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	GetContext(eImmediate)->GetCommandList()->ResourceBarrier(1, &m_BackBuffer->GetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	DX12_ASSERT(GetContext(eImmediate)->GetCommandList()->Close());

	// setup the resource barriere for each render textures
	for (UINT i = 0; i < eRenderTargetCount; ++i)
	{
		GetContext(eDeferred)->GetCommandList()->ResourceBarrier(1, &m_RenderTargets[i]->GetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	// we close the command list
	DX12_ASSERT(GetContext(eDeferred)->GetCommandList()->Close());

	return S_OK;
}

HRESULT DX12RenderEngine::WaitForPreviousFrame()
{
	HRESULT hr;
	DX12Context * context = GetContext(eImmediate);

	// swap the current rtv buffer index so we draw on the correct buffer
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	ID3D12Fence * fence		= context->GetFence(m_FrameIndex);
	UINT64 fenceValue		= context->GetFenceValue(m_FrameIndex);

	// if the current m_Fences value is still less than "m_FenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "m_CommandQueue->Signal(m_Fences, m_FenceValue)" command
	if (fence->GetCompletedValue() < fenceValue)
	{
		// we have the m_Fences create an event which is signaled once the m_Fences's current value is "m_FenceValue"
		hr = fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
		if (FAILED(hr))
		{
			return hr;
		}

		// We will wait until the m_Fences has triggered the event that it's current value has reached "m_FenceValue". once it's value
		// has reached "m_FenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	// increment m_FenceValue for next frame
	context->IncrementFenceValue(m_FrameIndex);

	return S_OK;
}

FORCEINLINE HRESULT DX12RenderEngine::InitializeImmediateContext()
{
	DX12Context * context = GetContext(eImmediate);

	// reset the immediate context
	context->ResetContext();

	// here we start recording commands into the m_CommandList (which all the commands will be stored in the m_CommandAllocator)

	// transition the "m_FrameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	context->GetCommandList()->ResourceBarrier(1, &m_BackBuffer->GetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_BackBuffer->GetRenderTargetDescriptor();

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	context->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	//const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	static const float clearColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	context->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Setting up the command list
	context->GetCommandList()->RSSetViewports(1, &DX12RenderEngine::GetInstance().GetViewport());
	context->GetCommandList()->RSSetScissorRects(1, &DX12RenderEngine::GetInstance().GetScissor());

	context->GetCommandList()->ClearDepthStencilView(m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// setup primitive topology
	context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	return S_OK;
}

FORCEINLINE HRESULT DX12RenderEngine::InitializeDeferredContext()
{
	// create deferred handle for render targets
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[ERenderTargetId::eRenderTargetCount] =
	{
		m_RenderTargets[ERenderTargetId::eDiffuse]->GetRenderTargetDescriptor(),
		m_RenderTargets[ERenderTargetId::eNormal]->GetRenderTargetDescriptor(),
		m_RenderTargets[ERenderTargetId::eSpecular]->GetRenderTargetDescriptor(),
	};

	DX12Context * context = GetContext(eDeferred);
	context->ResetContext();

	for (UINT i = 0; i < eRenderTargetCount; ++i)
	{
		// transition the "m_FrameIndex" render target from the present state to the render target state so the command list draws to it starting from here
		context->GetCommandList()->ResourceBarrier(1, &m_RenderTargets[i]->GetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	context->GetCommandList()->OMSetRenderTargets(eRenderTargetCount, rtvHandle, FALSE, &dsvHandle);

	for (UINT i = 0; i < eRenderTargetCount; ++i)
	{
		context->GetCommandList()->ClearRenderTargetView(rtvHandle[i], m_RenderTargets[i]->GetClearValue(), 0, nullptr);
	}

	// Setting up the command list
	context->GetCommandList()->RSSetViewports(1, &DX12RenderEngine::GetInstance().GetViewport());
	context->GetCommandList()->RSSetScissorRects(1, &DX12RenderEngine::GetInstance().GetScissor());

	// setup primitive topology
	context->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	return S_OK;
}

void DX12RenderEngine::GenerateImmediatePipelineState()
{
	
}
