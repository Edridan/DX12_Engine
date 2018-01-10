#include "DX12Mesh.h"

#include "d3dx12.h"

#include "DX12RenderEngine.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

// Input elements following the vertex struct (DX12Mesh.h)
const D3D12_INPUT_ELEMENT_DESC DX12Mesh::s_DefaultInputElement[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (sizeof(float) * 3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

// fill out the default input layout description structure
D3D12_INPUT_LAYOUT_DESC DX12Mesh::s_DefaultInputLayout =
{
	s_DefaultInputElement,
	sizeof(DX12Mesh::s_DefaultInputElement) / sizeof(D3D12_INPUT_ELEMENT_DESC)
};

/*
 *	Mesh primitive buffers
 */

 // Triangle data
Vertex vTriangle[] = {
	{ { 0.0f, 0.5f, 0.0f },		{ 1.0f, 0.0f, 0.0f, 1.f } },
	{ { 0.5f, -0.5f, 0.0f },	{ 0.0f, 1.0f, 0.0f, 1.f } },
	{ { -0.5f, -0.5f, 0.0f },	{ 0.0f, 0.0f, 1.0f, 1.f } },
};

// Plane data
Vertex vPlane[] = {
	{ { -0.5f,  0.5f, 0.0f },	{ 1.0f, 0.0f, 0.0f, 1.f } },
	{ { 0.5f, -0.5f, 0.0f },	{ 0.0f, 1.0f, 0.0f, 1.f } },
	{ { -0.5f, -0.5f, 0.0f },	{ 0.0f, 0.0f, 1.0f, 1.f } },
	{ { 0.5f,  0.5f, 0.0f },	{ 0.0f, 0.0f, 0.0f, 1.f } }
};

DWORD iPlane[] = {
	0, 1, 2,
	1, 0, 3
};

// Cube mesh
Vertex vCube[] = {
	{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
};

DWORD iCube[] = {
		0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 
		8, 9, 10, 8, 11, 9, 12, 13, 14, 12, 15, 13,
		16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21, 
	};

/// GENERATE PRIMITIVE MESH HERE :
DX12Mesh * DX12Mesh::GeneratePrimitiveMesh(EPrimitiveMesh i_Prim)
{
	DX12Mesh * returnMesh = nullptr;

	switch (i_Prim)
	{
	case ePlane:
		returnMesh = new DX12Mesh(vPlane, 4u, iPlane, 6u);
		break;
	case eTriangle:
		returnMesh = new DX12Mesh(vTriangle, 3u);
		break;
	case eCube:
		returnMesh = new DX12Mesh(vCube, 24u, iCube, 36u);
		break;
	}

	return returnMesh;
}

DX12Mesh * DX12Mesh::LoadMesh(const char * i_Filename)
{
	return nullptr;
}

// Mesh implementation

DX12Mesh::DX12Mesh(Vertex * i_Vertices, UINT i_VerticeCount)
	:m_VerticesCount(0)
	,m_IndexCount(0)
	,m_HaveIndex(false)
{
	HRESULT hr;
	UINT vBufferSize = sizeof(Vertex) * i_VerticeCount;
	m_VerticesCount = i_VerticeCount;

	// Update resources
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();
	ID3D12GraphicsCommandList * commandList = DX12RenderEngine::GetInstance().GetCommandList();
	ID3D12CommandQueue * commandQueue = DX12RenderEngine::GetInstance().GetCommandQueue();

	// Upload data to the GPU
	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_VertexBuffer));

	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap");


	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));

	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(i_Vertices); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UINT64 size = UpdateSubresources(commandList, m_VertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	if (size != vBufferSize)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Now we execute the command list to upload the initial assets (triangle data)
	if (FAILED(commandList->Close()))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during Close the command list (Mesh Initialization)");
	}

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	if (FAILED(DX12RenderEngine::GetInstance().IncrementFence()))
		DX12RenderEngine::GetInstance().PopUpError(L"Error during fence incrementation (Mesh Initialization)");

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vBufferSize;
}

DX12Mesh::DX12Mesh(Vertex * i_Vertices, UINT i_VerticeCount, DWORD * i_Indices, UINT i_IndiceCount)
	:m_VerticesCount(0)
	,m_IndexCount(0)
	,m_HaveIndex(true)
{
	HRESULT hr;
	UINT vBufferSize = sizeof(Vertex) * i_VerticeCount;
	UINT iBufferSize = sizeof(DWORD) * i_IndiceCount;
	m_VerticesCount = i_IndiceCount;
	m_IndexCount = i_IndiceCount;

	// Update resources
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();
	ID3D12GraphicsCommandList * commandList = DX12RenderEngine::GetInstance().GetCommandList();
	ID3D12CommandQueue * commandQueue = DX12RenderEngine::GetInstance().GetCommandQueue();

	// Upload data to the GPU

	// push vertices resources

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_VertexBuffer));

	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));

	if (FAILED(hr))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(i_Vertices); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

										 // we are now creating a command with the command list to copy the data from
										 // the upload heap to the default heap
	UINT64 size = UpdateSubresources(commandList, m_VertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	if (size != vBufferSize)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during CreateCommittedResource (Mesh Initialization)");
	}

	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// push indices resources
	// create default heap to hold index buffer
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
		nullptr, // optimized clear value must be null for this type of resource
		IID_PPV_ARGS(&m_IndexBuffer));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_IndexBuffer->SetName(L"Index Buffer Resource Heap");

	// create upload heap to upload index buffer
	ID3D12Resource* iBufferUploadHeap;
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&iBufferUploadHeap));
	vBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(i_Indices); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(commandList, m_IndexBuffer, iBufferUploadHeap, 0, 0, 1, &indexData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_IndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


	// push data to the GPU using the commandlist

	// Now we execute the command list to upload the initial assets (triangle data)
	if (FAILED(commandList->Close()))
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Error during Close the command list (Mesh Initialization)");
	}

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	if (FAILED(DX12RenderEngine::GetInstance().IncrementFence()))
		DX12RenderEngine::GetInstance().PopUpError(L"Error during fence incrementation (Mesh Initialization)");

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vBufferSize;

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IndexBufferView.SizeInBytes = iBufferSize;
}

DX12Mesh::~DX12Mesh()
{
	// Cleanup resources
	SAFE_RELEASE(m_VertexBuffer);

	// release if needed the index buffer
	if (m_HaveIndex)
	{
		SAFE_RELEASE(m_IndexBuffer);
	}
}

inline void DX12Mesh::Draw(ID3D12GraphicsCommandList* i_CommandList, ID3D12PipelineState * i_Pso)
{
	if (i_Pso == nullptr)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Try to draw a mesh without specifying Pipeline State Object");
		return;
	}

	const UINT count = m_HaveIndex ? m_IndexCount : m_VerticesCount;

	//i_CommandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);
	i_CommandList->SetGraphicsRootSignature(DX12RenderEngine::GetInstance().GetRootSignature()); // set the root signature
	i_CommandList->SetPipelineState(i_Pso);

	i_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	i_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView); // set the vertex buffer (using the vertex buffer view)

	if (m_HaveIndex)
	{
		i_CommandList->IASetIndexBuffer(&m_IndexBufferView);
		i_CommandList->DrawIndexedInstanced(count, 1, 0, 0, 0);
	}
	else
	{
		i_CommandList->DrawInstanced(count, 1, 0, 0); 
	}
}

void DX12Mesh::Draw(ID3D12GraphicsCommandList * i_CommandList, ID3D12PipelineState * i_Pso, D3D12_GPU_VIRTUAL_ADDRESS i_ConstantBufferAddress)
{
	// setup the graphic root constant buffer

	if (i_Pso == nullptr)
	{
		DX12RenderEngine::GetInstance().PopUpError(L"Try to draw a mesh without specifying Pipeline State Object");
		return;
	}

	const UINT count = m_HaveIndex ? m_IndexCount : m_VerticesCount;

	//i_CommandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);
	i_CommandList->SetGraphicsRootSignature(DX12RenderEngine::GetInstance().GetRootSignature()); // set the root signature
	i_CommandList->SetGraphicsRootConstantBufferView(0, i_ConstantBufferAddress);

	i_CommandList->SetPipelineState(i_Pso);

	i_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	i_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView); // set the vertex buffer (using the vertex buffer view)

	if (m_HaveIndex)
	{
		i_CommandList->IASetIndexBuffer(&m_IndexBufferView);
		i_CommandList->DrawIndexedInstanced(count, 1, 0, 0, 0);
	}
	else
	{
		i_CommandList->DrawInstanced(count, 1, 0, 0);
	}
}

const D3D12_INPUT_LAYOUT_DESC & DX12Mesh::GetInputLayoutDesc() const
{
	return m_InputLayoutDesc;
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC & DX12Mesh::GetPipelineStateDesc() const
{
	return *m_PipelineStateDesc;
}

HRESULT DX12Mesh::PushDX12ResourceBuffer(ID3D12Resource ** i_Buffer, UINT i_BufferSize, DWORD * i_BufferPtr, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device)
{
	return E_NOTIMPL;
}
