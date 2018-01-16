#include "DX12RenderEngine.h"

// Include
#include <assert.h>
#include "d3dx12.h"
#include "DX12Mesh.h"
#include "resource.h"	// default icon resource


#if (DEBUG_DX12_ENABLE) && defined(_DEBUG)
#define DX12_DEBUG
#endif

// Static definition implementation
DX12RenderEngine * DX12RenderEngine::s_Instance = nullptr;
const int DX12RenderEngine::ConstantBufferPerObjectAlignedSize = (sizeof(DefaultConstantBuffer) + 255) & ~255;

// Destructor
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }


#define ASSERT_ERROR(i_Text,...)		\
	POPUP_ERROR(i_Text, __VA_ARGS__);	\
	DEBUG_BREAK;						\
	m_Window.Close()

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
	delete s_Instance;
}

HRESULT DX12RenderEngine::InitializeDX12()
{
	HRESULT hr;

	// -- Debug -- //

#ifdef DX12_DEBUG
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController));

	if (FAILED(hr))
	{
		ASSERT_ERROR("Error D3D12GetDebugInterface");
		return E_FAIL;
	}

	m_DebugController->EnableDebugLayer();
#else
	m_DebugController = nullptr;
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
	backBufferDesc.Width = m_Window.GetWidth(); // buffer width
	backBufferDesc.Height = m_Window.GetHeight(); // buffer height
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
	m_SwapChainDesc.OutputWindow = m_Window.GetHWnd(); // handle to our window
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

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = m_FrameBufferCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvDescriptorHeap));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateDescriptorHeap");
		return E_FAIL;
	}

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from m_Device to m_Device, which is why there is no set size and we must ask the 
	// m_Device to give us the size. we will use this size to increment a descriptor handle offset
	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_RtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : m_SwapChain->GetBuffer");
			return E_FAIL;
		}

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		m_Device->CreateRenderTargetView(m_RenderTargets[i], nullptr, m_RtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		m_RtvHandle.Offset(1, m_RtvDescriptorSize);
	}

	// -- Create the Command Allocators -- //

	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[i]));
		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : CreateCommandAllocator");
			return E_FAIL;
		}
	}

	// -- Create a Command List -- //

	// create the command list with the first allocator
	hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[m_FrameIndex], NULL, IID_PPV_ARGS(&m_CommandList));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateCommandList");
		return E_FAIL;
	}

	// -- Create a Fence & Fence Event -- //

	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fences[i]));
		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : Fences -> CreateFence");
			return E_FAIL;
		}
		m_FenceValue[i] = 0; // set the initial m_Fences value to 0
	}

	// create a handle to a m_Fences event
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		ASSERT_ERROR("Error : Fences -> CreateEvent");
		return E_FAIL;
	}

	// -- Create default constant buffer -- //

	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		hr = m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // this heap will be used to upload the constant buffer data
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(m_ConstantBufferHeapSize * 1024 * 64), // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
			D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
			nullptr, // we do not have use an optimized clear value for constant buffers
			IID_PPV_ARGS(&m_ConstantBufferUploadHeap[i]));

		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : CreateCommittedResource");
			return E_FAIL;
		}

		m_ConstantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");

		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)
		hr = m_ConstantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferGPUAdress[i]));

		if (FAILED(hr))
		{
			ASSERT_ERROR("Error : Error during Map");
			return E_FAIL;
		}
	}

	// initialize constant buffer reserved table (internal management)
	for (UINT i = 0; i < m_ConstantBufferHeapSize; i++)
	{
		m_ConstantBufferReservedAddress[i] = false;
	}

	// -- Create default root signature -- //

	// create the root descriptor : where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootDescriptor;
	rootDescriptor.RegisterSpace = 0;
	rootDescriptor.ShaderRegister = 0;

	// create the default root parameter and fill it out
	// this paramater is the model view projection matrix
	D3D12_ROOT_PARAMETER  rootParameters[1]; // only one parameter right now
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our vertex shader will be the only shader accessing this parameter for now

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 1 root parameter
		rootParameters, // a pointer to the beginning of our root parameters array
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	
	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : D3D12SerializeRootSignature");
		return E_FAIL;
	}

	hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_DefaultRootSignature));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateRootSignature");
		return E_FAIL;
	}

	// -- Create default PSO -- //

	// load default shader

	// Load default pso and shader
	//m_DefaultPixelShader = new DX12Shader(DX12Shader::ePixel, L"PixelShader.hlsl");
	//m_DefaultVertexShader = new DX12Shader(DX12Shader::eVertex, L"VertexShader.hlsl");

	//// Create default pso
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPipelineDesc = {};

	//defaultPipelineDesc.InputLayout = DX12Mesh::s_DefaultInputColorLayout; // the structure describing our input layout
	//defaultPipelineDesc.pRootSignature = m_DefaultRootSignature; // the root signature that describes the input data this pso needs
	//defaultPipelineDesc.VS = m_DefaultVertexShader->GetByteCode(); // structure describing where to find the vertex shader bytecode and how large it is	(thx ZELDARCK)
	//defaultPipelineDesc.PS = m_DefaultPixelShader->GetByteCode(); // same as VS but for pixel shader
	//defaultPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	//defaultPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	//defaultPipelineDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	//defaultPipelineDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	//defaultPipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	//defaultPipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	//defaultPipelineDesc.NumRenderTargets = 1; // we are only binding one render target
	//defaultPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//defaultPipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	//// Create the default pipeline state object
	//hr = m_Device->CreateGraphicsPipelineState(&defaultPipelineDesc, IID_PPV_ARGS(&m_DefaultPipelineState));

	//if (FAILED(hr))
	//{
	//	ASSERT_ERROR("Error during default graphics pipeline state creation");
	//	return false;
	//}

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
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Window.GetWidth(), m_Window.GetHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthStencilBuffer)
	);

	m_DepthStencilDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	// create view
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_DepthStencilBuffer->SetName(L"Depth Stencil buffer");

	// generate default pipeline states objects
	GenerateDefaultPipelineState();

	// -- Setup viewport and scissor -- //

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = (FLOAT)m_Window.GetWidth();
	m_Viewport.Height = (FLOAT)m_Window.GetHeight();
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = m_Window.GetWidth();
	m_ScissorRect.bottom = m_Window.GetHeight();

	return S_OK;
}

HRESULT DX12RenderEngine::PrepareForRender()
{
	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = m_CommandAllocator[m_FrameIndex]->Reset();
	if (FAILED(hr))
	{
		POPUP_ERROR("Error : Failed to reset command allocator");
		return hr;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = m_CommandList->Reset(m_CommandAllocator[m_FrameIndex], nullptr);
	if (FAILED(hr))
	{
		POPUP_ERROR("Error in prepare for render");
		return hr;
	}

	// here we start recording commands into the m_CommandList (which all the commands will be stored in the m_CommandAllocator)

	// transition the "m_FrameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Setting up the command list
	m_CommandList->RSSetViewports(1, &DX12RenderEngine::GetInstance().GetViewport());
	m_CommandList->RSSetScissorRects(1, &DX12RenderEngine::GetInstance().GetScissor());

	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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
		HRESULT hr;

		// create an array of command lists (only one command list here)
		ID3D12CommandList* ppCommandLists[] = { m_CommandList };

		// execute the array of command lists
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// this command goes in at the end of our command queue. we will know when our command queue 
		// has finished because the m_Fences value will be set to "m_FenceValue" from the GPU since the command
		// queue is being executed on the GPU
		hr = m_CommandQueue->Signal(m_Fences[m_FrameIndex], m_FenceValue[m_FrameIndex]);
		if (FAILED(hr))
		{
			m_Window.Close();
		}

		// present the current back buffer
		hr = m_SwapChain->Present(0, 0);
		if (FAILED(hr))
		{
			m_Window.Close();
		}
	}

	return S_OK;
}

HRESULT DX12RenderEngine::IncrementFence()
{
	++(m_FenceValue[m_FrameIndex]);
	return m_CommandQueue->Signal(m_Fences[m_FrameIndex], m_FenceValue[m_FrameIndex]);
}

ADDRESS_ID DX12RenderEngine::ReserveConstantBufferVirtualAddress()
{
	ADDRESS_ID address = 0;

	// retreive the first address available
	while (address < m_ConstantBufferHeapSize)
	{
		if (m_ConstantBufferReservedAddress[address] == false)
		{
			m_ConstantBufferReservedAddress[address] = true;
			break;	// exit the loop, we have found an available address
		}
		++address;
	}

	// we didn't found a available address
	if (address == m_ConstantBufferHeapSize)
		return -1;	// error address

	// erase before used data and push null data to the constant buffer
	DefaultConstantBuffer constantBuffer;
	ZeroMemory(&constantBuffer, sizeof(constantBuffer));

	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		// constant buffer new data
		memcpy(m_ConstantBufferGPUAdress[i] + (address * ConstantBufferPerObjectAlignedSize), &constantBuffer, sizeof(constantBuffer));
	}

	return address;
}

void DX12RenderEngine::ReleaseConstantBufferVirtualAddress(ADDRESS_ID i_Address)
{
	if (i_Address < m_ConstantBufferHeapSize)
	{
		// release the constant buffer address
		// we let the buffer as is, we don't need to clear or release on gpu side (it's done when the engine is killed)
		m_ConstantBufferReservedAddress[i_Address] = false;
	}
}

void DX12RenderEngine::UpdateConstantBuffer(ADDRESS_ID i_Address, DefaultConstantBuffer & i_ConstantBuffer)
{
	if (m_ConstantBufferReservedAddress[i_Address] == true)
	{
		// copy data to the constant buffer
		memcpy(m_ConstantBufferGPUAdress[m_FrameIndex] + (i_Address * ConstantBufferPerObjectAlignedSize), &i_ConstantBuffer, sizeof(DefaultConstantBuffer));
	}
	else
	{
		ASSERT_ERROR("Error trying to update non reserved address");
	}
}

UINT8 * DX12RenderEngine::GetConstantBufferGPUAddress(ADDRESS_ID i_Address) const
{
	return m_ConstantBufferGPUAdress[m_FrameIndex] + (i_Address * ConstantBufferPerObjectAlignedSize);
}

D3D12_GPU_VIRTUAL_ADDRESS DX12RenderEngine::GetConstantBufferUploadVirtualAddress(ADDRESS_ID i_Address) const
{
	if (m_ConstantBufferReservedAddress[i_Address] == false || (i_Address == UnavailableAdressId))
	{
		POPUP_ERROR("Error using a non reserved address for constant buffer");
		DEBUG_BREAK;
	}

	return m_ConstantBufferUploadHeap[m_FrameIndex]->GetGPUVirtualAddress() + (i_Address * ConstantBufferPerObjectAlignedSize);
}

DX12RenderEngine::PipelineStateObject * DX12RenderEngine::GetPipelineStateObject(UINT64 i_Flag)
{
	return m_PipelineStateObjects[i_Flag];
}

DX12Shader * DX12RenderEngine::GetShader(UINT64 i_Flags, DX12Shader::EShaderType i_Type)
{
	// return the shader depending the flag
	std::unordered_map<UINT64, DX12Shader*> & shaders = (i_Type == DX12Shader::ePixel ? m_PixelShaders : m_VertexShaders);
	DX12Shader * shader = shaders[i_Flags];
	return shader;
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

ID3D12GraphicsCommandList * DX12RenderEngine::GetCommandList() const
{
	return m_CommandList;
}

IDXGISwapChain3 * DX12RenderEngine::SwapChain() const
{
	return m_SwapChain;
}

ID3D12Device * DX12RenderEngine::GetDevice() const
{
	return m_Device;
}

ID3D12RootSignature * DX12RenderEngine::GetRootSignature() const
{
	return m_DefaultRootSignature;
}

ID3D12CommandQueue * DX12RenderEngine::GetCommandQueue() const
{
	return m_CommandQueue;
}

const DXGI_SWAP_CHAIN_DESC & DX12RenderEngine::GetSwapChainDesc() const
{
	return m_SwapChainDesc;
}

bool DX12RenderEngine::IsDX12DebugEnabled() const
{
#ifdef DX12_DEBUG
	return true;
#else
	return false;
#endif
}

DX12Window & DX12RenderEngine::GetWindow()
{
	return m_Window;
}

void DX12RenderEngine::UpdateWindow()
{
	m_Window.Update();
}

DX12RenderEngine::DX12RenderEngine(HINSTANCE & i_HInstance)
	:m_Window(i_HInstance, L"DX12 Engine", L"DX12 Engine", 1600, 900, DX12Window::Icon((LPWSTR)IDI_ICON1, 32, 32))
{
}

DX12RenderEngine::~DX12RenderEngine()
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

	// To do : release unordered map


	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_CommandQueue);
	SAFE_RELEASE(m_RtvDescriptorHeap);
	SAFE_RELEASE(m_CommandList);

	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		SAFE_RELEASE(m_RenderTargets[i]);
		SAFE_RELEASE(m_CommandAllocator[i]);
		SAFE_RELEASE(m_Fences[i]);

		SAFE_RELEASE(m_ConstantBufferUploadHeap[i]);
	};

	SAFE_RELEASE(m_DefaultRootSignature);
	//SAFE_RELEASE(m_DefaultPipelineState);

	SAFE_RELEASE(m_DepthStencilBuffer);
	SAFE_RELEASE(m_DepthStencilDescriptorHeap);
}

HRESULT DX12RenderEngine::UpdatePipeline()
{
	HRESULT hr;

	// transition the "m_FrameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	hr = m_CommandList->Close();

	return hr;
}

HRESULT DX12RenderEngine::WaitForPreviousFrame()
{
	HRESULT hr;

	if (!m_Window.IsOpen())
	{
		return S_OK;
	}

	// swap the current rtv buffer index so we draw on the correct buffer
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// if the current m_Fences value is still less than "m_FenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "m_CommandQueue->Signal(m_Fences, m_FenceValue)" command
	if (m_Fences[m_FrameIndex]->GetCompletedValue() < m_FenceValue[m_FrameIndex])
	{
		// we have the m_Fences create an event which is signaled once the m_Fences's current value is "m_FenceValue"
		hr = m_Fences[m_FrameIndex]->SetEventOnCompletion(m_FenceValue[m_FrameIndex], m_FenceEvent);
		if (FAILED(hr))
		{
			return hr;
		}

		// We will wait until the m_Fences has triggered the event that it's current value has reached "m_FenceValue". once it's value
		// has reached "m_FenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	// increment m_FenceValue for next frame
	m_FenceValue[m_FrameIndex]++;

	return S_OK;
}

inline void DX12RenderEngine::GenerateDefaultPipelineState()
{
	// preload shaders
	LoadShader(L"DefaultPixel.hlsl", DX12Shader::ePixel, 
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal);
	LoadShader(L"NormalVertex.hlsl", DX12Shader::eVertex, 
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal);

	LoadShader(L"NormalTexPixel.hlsl", DX12Shader::ePixel, 
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal 
		| DX12Mesh::EElementFlags::eHaveTexcoord);
	LoadShader(L"NormalTexVertex.hlsl", DX12Shader::eVertex, 
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal 
		| DX12Mesh::EElementFlags::eHaveTexcoord);

	LoadShader(L"DefaultPixel.hlsl", DX12Shader::ePixel, 
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal 
		| DX12Mesh::EElementFlags::eHaveColor);
	LoadShader(L"NormalColorVertex.hlsl", DX12Shader::eVertex,
		DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal 
		| DX12Mesh::EElementFlags::eHaveColor);

	// here we are going to build each pipeline state for rendering
	// this is a default rendering pipeline and root signatures
	// if a mesh need other pipeline state object to be rendered don't use them
	CreatePipelineState(DX12Mesh::EElementFlags::eNone
		| DX12Mesh::EElementFlags::eHaveNormal);

	CreatePipelineState(DX12Mesh::EElementFlags::eNone
		| DX12Mesh::EElementFlags::eHaveNormal
		| DX12Mesh::EElementFlags::eHaveTexcoord);

	CreatePipelineState(DX12Mesh::EElementFlags::eNone 
		| DX12Mesh::EElementFlags::eHaveNormal 
		| DX12Mesh::EElementFlags::eHaveColor);
}

inline void DX12RenderEngine::CreatePipelineState(UINT64 i_Flags)
{
	// The pipeline state is already created
	if (m_PipelineStateObjects[i_Flags] != nullptr)
		return;

	// each elements are rendered need a position
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC desc;
	DX12Shader * pixelShader = nullptr, *vertexShader = nullptr;
	UINT textureCount = 0;

	// sampler for textures
	D3D12_STATIC_SAMPLER_DESC	* sampler			= nullptr;
	D3D12_ROOT_DESCRIPTOR_TABLE * descriptorTable	= nullptr;

	// create input layout
	DX12Mesh::CreateInputLayoutFromFlags(desc, i_Flags);

	// layout order definition depending flags : 
	// 1 - Position
	// 2 - Normal
	// 3 - Texcoord
	// 4 - Color
	pixelShader		= GetShader(i_Flags, DX12Shader::ePixel);
	vertexShader	= GetShader(i_Flags, DX12Shader::eVertex);
	
	if (pixelShader == nullptr || vertexShader == nullptr)
	{
		PopUpWindow(PopUpIcon::eWarning, "Warning", "Trying to create a pipeline state but pixel or vertex shaders are not loaded");
		DEBUG_BREAK;
		return;
	}

	// create sampler for mesh buffer with tex coord
	// we are going to use static samplers that are samplers that we can't change when they are set to the pipeline state
	// this mean we have to save samplers in files and read them when loading the mesh
	// To do : multiple textures count
	if (i_Flags & DX12Mesh::EElementFlags::eHaveTexcoord)
	{
		// at least one texture
		textureCount = 1;

		// create descriptor table ranges
		D3D12_DESCRIPTOR_RANGE*  descriptorTableRanges = new D3D12_DESCRIPTOR_RANGE[textureCount];
		descriptorTableRanges[0].RangeType			= D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
		descriptorTableRanges[0].NumDescriptors		= 1; // we only have one texture right now, so the range is only 1
		descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
		descriptorTableRanges[0].RegisterSpace		= 0; // space 0. can usually be zero

		descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

		descriptorTable = new D3D12_ROOT_DESCRIPTOR_TABLE[1];	// create a descriptor table
		descriptorTable[0].NumDescriptorRanges	= textureCount;	// one range per texture
		descriptorTable[0].pDescriptorRanges	= descriptorTableRanges; // the pointer to the beginning of our ranges array

		sampler = new D3D12_STATIC_SAMPLER_DESC[textureCount]; // create a descriptor table

		for (UINT i = 0; i < textureCount; ++i)
		{
			sampler[0].Filter			= D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler[0].AddressU			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].AddressV			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].AddressW			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].MipLODBias		= 0;
			sampler[0].MaxAnisotropy	= 0;
			sampler[0].ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
			sampler[0].BorderColor		= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler[0].MinLOD			= 0.0f;
			sampler[0].MaxLOD			= D3D12_FLOAT32_MAX;
			sampler[0].ShaderRegister	= 0;
			sampler[0].RegisterSpace	= 0;
			sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}
	}

	// -- Create root signature -- //

	// create the root descriptor : where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// create the default root parameter and fill it out
	// this paramater is the model view projection matrix
	D3D12_ROOT_PARAMETER *  rootParameters = new D3D12_ROOT_PARAMETER[1 + textureCount]; // only one parameter right now

	// first parameter is always the CBV
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our vertex shader will be the only shader accessing this parameter for now

	// setup the root parameters for textures
	for (UINT i = 0; i < textureCount; ++i) 
	{
		rootParameters[1 + i].ParameterType		= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[1 + i].DescriptorTable	= descriptorTable[i];
		rootParameters[1 + i].ShaderVisibility	= D3D12_SHADER_VISIBILITY_PIXEL;	// for now only the pixel shader will going to use the textures
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	if (!(i_Flags & DX12Mesh::EElementFlags::eHaveTexcoord))
	{
		rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(1 + textureCount, // we have 1 root parameter
		rootParameters,			// a pointer to the beginning of our root parameters array
		textureCount,			// static samplers count
		sampler,				// static samplers pointer
		rootSignatureFlags);	// flags

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : D3D12SerializeRootSignature");
	}

	ID3D12RootSignature * rootSignature = nullptr;
	hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateRootSignature");
	}

	// -- Create Pipeline state -- //

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Create default pso
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

	pipelineDesc.InputLayout = desc; // the structure describing our input layout
	pipelineDesc.pRootSignature = rootSignature; // the root signature that describes the input data this pso needs
	pipelineDesc.VS = vertexShader->GetByteCode(); // structure describing where to find the vertex shader bytecode and how large it is	(thx ZELDARCK)
	pipelineDesc.PS = pixelShader->GetByteCode(); // same as VS but for pixel shader
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	pipelineDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	pipelineDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	pipelineDesc.NumRenderTargets = 1; // we are only binding one render target
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	// Create the default pipeline state object
	ID3D12PipelineState * pipelineState = nullptr;
	hr = m_Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));

	if (FAILED(hr))
	{
		DEBUG_BREAK;
	}

	// Create the pipeline state object
	m_PipelineStateObjects[i_Flags] = new PipelineStateObject;
	m_PipelineStateObjects[i_Flags]->m_PipelineState = pipelineState;
	m_PipelineStateObjects[i_Flags]->m_DefaultRootSignature = rootSignature;
}

HRESULT DX12RenderEngine::LoadShader(const wchar_t * i_Filename, DX12Shader::EShaderType i_ShaderType, UINT64 i_Flags)
{
	std::unordered_map<UINT64, DX12Shader*> & shaders = (i_ShaderType == DX12Shader::ePixel ? m_PixelShaders : m_VertexShaders);
	DX12Shader * shader = shaders[i_Flags];

	if (shader == nullptr)
	{
		// push the shader into the map
		shader = new DX12Shader(i_ShaderType, i_Filename);

		if (!shader->IsLoaded())
		{
			return E_FAIL;
		}

		shaders[i_Flags] = shader;
	}
	
	return S_OK;
}
