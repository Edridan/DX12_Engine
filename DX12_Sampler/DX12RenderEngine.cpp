#include "DX12RenderEngine.h"

// Include
#include <assert.h>
#include "d3dx12.h"
#include "DX12Mesh.h"

// Static definition implementation
DX12RenderEngine * DX12RenderEngine::s_Instance = nullptr;
const int DX12RenderEngine::ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;

// Destructor
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

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

	// -- Create the Device -- //

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error when creating the DXGIFactory1");
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
		DX12RenderEngine::GetInstance().PopUpError(L"Error : Device compatible with DX12 not found");
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
		DX12RenderEngine::GetInstance().PopUpError(L"Error : D3D12CreateDevice");
		return E_FAIL;
	}

	// -- Create command queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_CommandQueue)); // create the command queue
	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateCommandQueue");
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
		DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateSwapChain");
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
		DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateDescriptorHeap");
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
			DX12RenderEngine::GetInstance().PopUpError(L"Error : m_SwapChain->GetBuffer");
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
			DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateCommandAllocator");
			return E_FAIL;
		}
	}

	// -- Create a Command List -- //

	// create the command list with the first allocator
	hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[m_FrameIndex], NULL, IID_PPV_ARGS(&m_CommandList));
	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateCommandList");
		return E_FAIL;
	}

	// -- Create a Fence & Fence Event -- //

	for (int i = 0; i < m_FrameBufferCount; i++)
	{
		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fences[i]));
		if (FAILED(hr))
		{
			DX12RenderEngine::GetInstance().PopUpError(L"Error : Fences -> CreateFence");
			return E_FAIL;
		}
		m_FenceValue[i] = 0; // set the initial m_Fences value to 0
	}

	// create a handle to a m_Fences event
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error : Fences -> CreateEvent");
		return E_FAIL;
	}

	// -- Create default root -- //

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
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	
	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error : D3D12SerializeRootSignature");
		return E_FAIL;
	}

	hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateRootSignature");
		return E_FAIL;
	}



	return true;
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
		PopUpError(L"Error : Failed to reset command allocator");
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
		PopUpError(L"Error in prepare for render");
		return hr;
	}

	// here we start recording commands into the m_CommandList (which all the commands will be stored in the m_CommandAllocator)

	// transition the "m_FrameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);

	// set the render target for the output merger stage (the output of the pipeline)
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Setting up the command list
	m_CommandList->RSSetViewports(1, &DX12RenderEngine::GetInstance().GetViewport());
	m_CommandList->RSSetScissorRects(1, &DX12RenderEngine::GetInstance().GetScissor());
}

HRESULT DX12RenderEngine::Render()
{
	if (FAILED(UpdatePipeline()))
	{
		PopUpError( L"Error during update pipeline");
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
	ConstantBufferPerObject constantBuffer;
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

UINT8 * DX12RenderEngine::GetConstantBufferGPUAddress(ADDRESS_ID i_Address) const
{
	return m_ConstantBufferGPUAdress[m_FrameIndex] + (i_Address * ConstantBufferPerObjectAlignedSize);
}

D3D12_GPU_VIRTUAL_ADDRESS DX12RenderEngine::GetConstantBufferUploadVirtualAddress(ADDRESS_ID i_Address) const
{
	if (m_ConstantBufferReservedAddress[i_Address] == false)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error using a non reserved address for constant buffer");
	}

	return m_ConstantBufferUploadHeap[m_FrameIndex]->GetGPUVirtualAddress() + (i_Address * ConstantBufferPerObjectAlignedSize);
}

ID3D12PipelineState * DX12RenderEngine::GetPipelineState(UINT64 i_Flag)
{
	if (i_Flag == PiplineStateFlags::eDefault)
		return m_DefaultPipelineState;

	// To do : impl
	return nullptr;
}

ID3D12PipelineState * DX12RenderEngine::GetDefaultPipelineState() const
{
	return m_DefaultPipelineState;
}

HRESULT DX12RenderEngine::LoadShader(const wchar_t i_Filename)
{
	// To do : impl
	return E_NOTIMPL;
}

void DX12RenderEngine::PopUpError(const wchar_t * i_Message)
{
	MessageBox(NULL, i_Message,
		L"Error", MB_OK | MB_ICONERROR);
	m_Window.Close();
	DEBUG_BREAK;
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
	return m_RootSignature;
}

ID3D12CommandQueue * DX12RenderEngine::GetCommandQueue() const
{
	return m_CommandQueue;
}

const DXGI_SWAP_CHAIN_DESC & DX12RenderEngine::GetSwapChainDesc() const
{
	return m_SwapChainDesc;
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
	:m_Window(i_HInstance, L"DX12 Engine", L"DX12 Engine", 1600, 900)
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
	};

	SAFE_RELEASE(m_RootSignature);
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
}

// to delete

/*	ZeroMemory(&m_ConstantBufferGPUAdress, sizeof(m_ConstantBufferGPUAdress));

	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_MainDescriptorHeap[i]));
		if (FAILED(hr))
		{
			DX12RenderEngine::GetInstance().PopUpError(L"Error : CreateDescriptorHeap");
		}
	}

	// create resource heap
	for (int i = 0; i < m_FrameBufferCount; ++i)
	{
		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

		// create constant buffer upload heap
		hr = m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // this heap will be used to upload the constant buffer data
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(m_ConstantBufferHeapSize * 64), // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
			D3D12_RESOURCE_STATE_GENERIC_READ, // will be data that is read from so we keep it in the generic read state
			nullptr, // we do not have use an optimized clear value for constant buffers
			IID_PPV_ARGS(&m_ConstantBufferUploadHeap[i]));

		m_ConstantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");
		hr = m_ConstantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferGPUAdress[i]));

		if (FAILED(hr))
		{
			DX12RenderEngine::GetInstance().PopUpError(L"Error Map constant buffer");
		}
	}

	// initialize m_ConstantBufferReservedAddress table
	for (int i = 0; i < m_ConstantBufferHeapSize; i++)
	{
		m_ConstantBufferReservedAddress[i] = false;
	}

	// Fill out the Viewport
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = m_Window.GetWidth();
	m_Viewport.Height = m_Window.GetHeight();
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = m_Window.GetWidth();
	m_ScissorRect.bottom = m_Window.GetHeight();

	// Load default pso and shader
	m_DefaultPixelShader = new DX12Shader(DX12Shader::ePixel, L"PixelShader.hlsl");
	m_DefaultVertexShader = new DX12Shader(DX12Shader::eVertex, L"VertexShader.hlsl");

	// Create default pso
	D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPipelineDesc = {};

	defaultPipelineDesc.InputLayout = DX12Mesh::s_DefaultInputLayout; // the structure describing our input layout
	defaultPipelineDesc.pRootSignature = m_RootSignature; // the root signature that describes the input data this pso needs
	defaultPipelineDesc.VS = m_DefaultPixelShader->GetByteCode(); // structure describing where to find the vertex shader bytecode and how large it is
	defaultPipelineDesc.PS = m_DefaultPixelShader->GetByteCode(); // same as VS but for pixel shader
	defaultPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	defaultPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	defaultPipelineDesc.SampleDesc = m_SwapChainDesc.SampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	defaultPipelineDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	defaultPipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	defaultPipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	defaultPipelineDesc.NumRenderTargets = 1; // we are only binding one render target
	defaultPipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	// Create the default pipeline state object
	hr = m_Device->CreateGraphicsPipelineState(&defaultPipelineDesc, IID_PPV_ARGS(&m_DefaultPipelineState));

	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during default graphics pipeline state creation");
		return false;
		}*/