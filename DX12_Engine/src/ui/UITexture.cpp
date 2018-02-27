#include "UITexture.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12Context.h"

UITexture::UITexture(const RECT & i_Rect, ID3D12DescriptorHeap * i_Desc)
	:m_Descriptor(i_Desc)
{
	// create buffer
	// retreive informations
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12GraphicsCommandList * commandList = render.GetContext(DX12RenderEngine::eImmediate)->GetCommandList();
	ID3D12CommandQueue * commandQueue = render.GetCommandQueue();
	ID3D12Device * device = render.GetDevice();

	static const UINT stride = 6;
	static const UINT vBufferSize = 4 * sizeof(float) * stride;
	static const UINT iBufferSize = sizeof(DWORD) * 6;
	
	// initialize buffer
	m_VData[0] = UITexVertex(0.f, 0.f, 1.f, 0.f);
	m_VData[1] = UITexVertex(0.f, 0.f, 1.f, 0.f);
	m_VData[2] = UITexVertex(0.f, 0.f, 1.f, 0.f);
	m_VData[3] = UITexVertex(0.f, 0.f, 1.f, 0.f);

	// create the vertex buffer
	 device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE, 
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST, 
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer));

	 m_VertexBuffer->SetName(L"UITex_Vertex");

	 // create index buffer
	 device->CreateCommittedResource(
		 &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		 D3D12_HEAP_FLAG_NONE, // no flags
		 &CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), 
		 D3D12_RESOURCE_STATE_COPY_DEST, 
		 nullptr, 
		 IID_PPV_ARGS(&m_IndexBuffer));

	 m_IndexBuffer->SetName(L"UITex_Index");

	// set rectangle data for the resources
	SetRect(i_Rect);
}

UITexture::~UITexture()
{
}

void UITexture::Render(ID3D12GraphicsCommandList * i_CommandList)
{
	ID3D12DescriptorHeap * descriptors[] = { m_Descriptor };

	// update the descriptor for the resources
	i_CommandList->SetDescriptorHeaps(_countof(descriptors), descriptors);
	i_CommandList->SetGraphicsRootDescriptorTable(0, m_Descriptor->GetGPUDescriptorHandleForHeapStart());
}

void UITexture::SetTextureDescriptor(ID3D12DescriptorHeap * i_Desc)
{
	m_Descriptor = i_Desc;
}

void UITexture::SetRect(const RECT & i_Rect)
{
	HRESULT hr;
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12Device * device = render.GetDevice();
	ID3D12GraphicsCommandList * commandList = render.GetContext(DX12RenderEngine::eImmediate)->GetCommandList();

	static const UINT stride = 6;
	static const UINT vBufferSize = 4 * sizeof(float) * stride;

	

	ID3D12Resource* bufferUploadHeap;
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&bufferUploadHeap));

	// return if failed
	if (FAILED(hr)) return;

	bufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store buffer in upload heap
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData			= (void*)m_VData; // pointer to our vertex array
	data.RowPitch		= vBufferSize; // size of all our triangle vertex data
	data.SlicePitch		= vBufferSize; // also the size of our triangle vertex data

	UINT64 size = UpdateSubresources(commandList, m_VertexBuffer, bufferUploadHeap, 0, 0, 1, &data);

	// return if failed
	ASSERT(size == vBufferSize);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
}

void UITexture::UpdateData(const RECT & i_Rect) const
{
	// Plane data
	FLOAT vPlane[] = {
		-0.5f,  0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		0.5f,  0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f
	};
}
