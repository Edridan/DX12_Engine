#include "DX12RenderTarget.h"

#include "dx12/DX12RenderEngine.h"
#include "engine/Debug.h"

DX12RenderTarget::DX12RenderTarget(const RenderTargetDesc & i_Desc)
	:m_FrameCount(i_Desc.BufferCount)
	,m_CustomAmount(i_Desc.BufferCount != 0)
	,m_Name(i_Desc.Name)
	,m_IsAllocator(i_Desc.Resource == nullptr)
	,m_IsResourceView(i_Desc.IsShaderResource)
	// descriptors
	,m_ShaderResourceDesc(nullptr)
	,m_RenderTargetDesc(nullptr)
	// resources
	,m_RenderTarget(nullptr)
{
	// retreive dx12 utils
	DX12RenderEngine & render				= DX12RenderEngine::GetInstance();
	ID3D12GraphicsCommandList * commandList = render.GetCommandList();
	ID3D12Device * device					= render.GetDevice();
	HRESULT hr;

	// retreive buffer count if needed
	if (m_FrameCount == 0)	m_FrameCount = render.GetFrameBufferCount();
	
	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = m_FrameCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	std::wstring rtvDescName = m_Name;
	rtvDescName.append(L" Render Target Descriptor");
	m_RenderTargetDesc = new DX12DescriptorHeap(rtvHeapDesc, rtvDescName.c_str());

	// create render target if the render target need to be allocated
	if (m_IsAllocator)
	{
		m_RenderTarget = new ID3D12Resource *[m_FrameCount];

		D3D12_RESOURCE_DESC resourceDesc;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = i_Desc.BufferSize.x;
		resourceDesc.Height = i_Desc.BufferSize.y;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = i_Desc.Format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		for (size_t i = 0; i < m_FrameCount; ++i)
		{
			// create resources here
			hr = device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap (used for render target too)
				D3D12_HEAP_FLAG_NONE, // no flags
				&resourceDesc, // the description of our texture
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, // We will copy the texture from the upload heap to here, so we start it out in a copy dest state
				nullptr, // used for render targets and depth/stencil buffers
				IID_PPV_ARGS(&m_RenderTarget[i]));

			std::wstring renderTargetName = m_Name;
			renderTargetName.append(L" Resources");
			m_RenderTarget[i]->SetName(renderTargetName.c_str());
		}
	}


	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from m_Device to m_Device, which is why there is no set size and we must ask the 
	// m_Device to give us the size. we will use this size to increment a descriptor handle offset
	int RTVDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RenderTargetDesc->GetDescriptorHandle();

	for (size_t i = 0; i < m_FrameCount; ++i)
	{
		device->CreateRenderTargetView(m_RenderTarget[i], nullptr, rtvHandle);
		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, m_RenderTargetDesc->GetDescriptorSize());
	}

	// create the shader resource view for viewing the render target
	if (m_IsResourceView)
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = m_FrameCount; // number of descriptors for this heap.
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; // this heap is a shader resource view
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		// To do : make the possibily to add flags on some pipeline state

		m_ShaderResourceDesc = new DX12DescriptorHeap(srvHeapDesc);

		// now we create a shader resource view (descriptor that points to the texture and describes it)
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = i_Desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = m_ShaderResourceDesc->GetDescriptorHandle();

		for (size_t i = 0; i < m_FrameCount; ++i)
		{
			device->CreateShaderResourceView(m_RenderTarget[i], &srvDesc, srvHandle);
			srvHandle.Offset(1, m_ShaderResourceDesc->GetDescriptorSize());
		}
	}
}

DX12RenderTarget::~DX12RenderTarget()
{
	if (m_IsAllocator)
	{
		// clean resource internally if we allocate them
		for (size_t i = 0; i < m_FrameCount; ++i)
		{
			SAFE_RELEASE(m_RenderTarget[i]);
		}
	}
	
	// release the descriptor heap
	delete m_RenderTargetDesc;

	if (m_ShaderResourceDesc != nullptr) delete m_ShaderResourceDesc;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTarget::GetRenderTargetDescriptor(UINT i_Index /* = ((UINT)-1) */) const
{
	if (i_Index == ((UINT)-1))
	{
		if (!m_CustomAmount)
		{
			const UINT currentFrame = DX12RenderEngine::GetInstance().GetFrameIndex();
			return m_RenderTargetDesc->GetDescriptorHandle(currentFrame);
		}
	}
	else
	{
		if (i_Index < m_FrameCount)
			return m_RenderTargetDesc->GetDescriptorHandle(i_Index);
	}

	// default return
	return m_RenderTargetDesc->GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTarget::GetTextureDescriptor(UINT i_Index /* = 0 */) const
{
	if (i_Index == ((UINT)-1))
	{
		if (!m_CustomAmount)
		{
			const UINT currentFrame = DX12RenderEngine::GetInstance().GetFrameIndex();
			return m_ShaderResourceDesc->GetDescriptorHandle(currentFrame);
		}
	}
	else
	{
		if (i_Index < m_FrameCount)
			return m_ShaderResourceDesc->GetDescriptorHandle(i_Index);
	}

	// default return
	return m_ShaderResourceDesc->GetDescriptorHandle();
}
