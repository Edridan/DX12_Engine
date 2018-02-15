#include "dx12/DX12MeshBuffer.h"

#include "d3dx12.h"
#include "dx12/DX12Utils.h"
#include "dx12/DX12Mesh.h"
#include "dx12/DX12Context.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RenderEngine.h"


#ifdef _DEBUG
UINT DX12MeshBuffer::s_MeshInstanciated = 0;
#endif

DX12MeshBuffer::DX12MeshBuffer(const D3D12_INPUT_LAYOUT_DESC & i_InputLayout, const BYTE * i_VerticesBuffer, UINT i_VerticesCount, const std::wstring & i_Name)
	:m_Count(i_VerticesCount)
	,m_IndexBuffer(nullptr)
	,m_IndexBufferView()
	,m_HaveIndex(false)
	,m_Name(i_Name.c_str())
	,m_ElementFlags(DX12PipelineState::CreateFlagsFromInputLayout(i_InputLayout))
{
	// get size of the input layout
	D3D12_INPUT_LAYOUT_DESC elem = i_InputLayout;
	i_InputLayout.NumElements;

	// retreive informations
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12GraphicsCommandList * commandList = render.GetContext(DX12RenderEngine::eImmediate)->GetCommandList();
	ID3D12CommandQueue * commandQueue = DX12RenderEngine::GetInstance().GetCommandQueue();

	const UINT stride = DX12PipelineState::GetElementSize(i_InputLayout);
	const UINT vBufferSize = i_VerticesCount * stride;

	// create vertex buffer
	CreateBuffer(&m_VertexBuffer, vBufferSize, i_Name.c_str());
	UpdateData(commandList, m_VertexBuffer, vBufferSize, (i_VerticesBuffer));

	// create a vertex buffer view for the triangle. We get the GPU memory address to the pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = stride;
	m_VertexBufferView.SizeInBytes = vBufferSize;
}

DX12MeshBuffer::DX12MeshBuffer(const D3D12_INPUT_LAYOUT_DESC & i_InputLayout, const BYTE * i_VerticesBuffer, UINT i_VerticesCount, const DWORD * i_IndexBuffer, UINT i_IndexCount, const std::wstring & i_Name)
	:m_Count(i_IndexCount)
	,m_HaveIndex(true)
	,m_Name(i_Name.c_str())
	,m_ElementFlags(DX12PipelineState::CreateFlagsFromInputLayout(i_InputLayout))
{
	// get size of the input layout
	D3D12_INPUT_LAYOUT_DESC elem = i_InputLayout;
	i_InputLayout.NumElements;

	// retreive informations
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12GraphicsCommandList * commandList = render.GetContext(DX12RenderEngine::eImmediate)->GetCommandList();
	ID3D12CommandQueue * commandQueue = render.GetCommandQueue();
	// vertices count
	const UINT stride = DX12PipelineState::GetElementSize(i_InputLayout);
	const UINT vBufferSize = i_VerticesCount * stride;
	// index count
	const UINT iBufferSize = sizeof(DWORD) * i_IndexCount;

	// create vertex buffer
	CreateBuffer(&m_VertexBuffer, vBufferSize, i_Name.c_str());
	UpdateData(commandList, m_VertexBuffer, vBufferSize, (i_VerticesBuffer));

	CreateBuffer(&m_IndexBuffer, iBufferSize, i_Name.c_str());
	UpdateData(commandList, m_IndexBuffer, iBufferSize, reinterpret_cast<const BYTE*>(i_IndexBuffer));

	// create a vertex buffer view for the triangle. We get the GPU memory address to the pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = stride;
	m_VertexBufferView.SizeInBytes = vBufferSize;

	// create a index buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IndexBufferView.SizeInBytes = iBufferSize;
}

DX12MeshBuffer::~DX12MeshBuffer()
{
	// cleanup resources
	//SAFE_RELEASE(m_VertexBuffer);

	// release if needed the index buffer
	if (m_HaveIndex)
	{
		/*SAFE_RELEASE(m_IndexBuffer);*/
	}
}

const D3D12_INPUT_LAYOUT_DESC & DX12MeshBuffer::GetInputLayout() const
{
	return m_InputLayoutDesc;
}

HRESULT DX12MeshBuffer::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	i_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView); // set the vertex buffer (using the vertex buffer view)

	if (m_HaveIndex)
	{
		i_CommandList->IASetIndexBuffer(&m_IndexBufferView);	// push the index buffer into the command list
		i_CommandList->DrawIndexedInstanced(m_Count, 1, 0, 0, 0);	// draw indexed vertices
	}
	else
	{
		i_CommandList->DrawInstanced(m_Count, 1, 0, 0);	// draw triangles
	}

	return S_OK;
}

UINT64 DX12MeshBuffer::GetElementFlags() const
{
	return m_ElementFlags;
}

const std::wstring & DX12MeshBuffer::GetName() const
{
	return m_Name;
}

DX12Material::DX12MaterialDesc DX12MeshBuffer::GetDefaultMaterialDesc() const
{
	return m_DefaultMaterial;
}

void DX12MeshBuffer::SetDefaultMaterial(const DX12Material::DX12MaterialDesc & i_Desc)
{
	// update default material
	m_DefaultMaterial = i_Desc;
}

bool DX12MeshBuffer::IsCompatible(const DX12Material::DX12MaterialDesc & i_Desc) const
{
	return false;
}

bool DX12MeshBuffer::IsCompatible(const DX12Material & i_Desc) const
{
	return false;
}

inline HRESULT DX12MeshBuffer::CreateBuffer(ID3D12Resource ** i_Buffer, UINT i_BufferSize, const wchar_t * i_Name)
{
	HRESULT hr;
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(i_BufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(i_Buffer));

	// return if failed
	if (FAILED(hr)) return hr;

	(*i_Buffer)->SetName(i_Name);
	return hr;
}

inline HRESULT DX12MeshBuffer::UpdateData(ID3D12GraphicsCommandList* i_CommandList, ID3D12Resource * i_Buffer, UINT i_BufferSize, const BYTE * i_Data)
{
	HRESULT hr;
	ID3D12Device * device						= DX12RenderEngine::GetInstance().GetDevice();

	ID3D12Resource* bufferUploadHeap;
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(i_BufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&bufferUploadHeap));

	// return if failed
	if (FAILED(hr)) return hr;

	bufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store buffer in upload heap
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData = i_Data; // pointer to our vertex array
	data.RowPitch = i_BufferSize; // size of all our triangle vertex data
	data.SlicePitch = i_BufferSize; // also the size of our triangle vertex data

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UINT64 size = UpdateSubresources(i_CommandList, i_Buffer, bufferUploadHeap, 0, 0, 1, &data);
	
	// return if failed
	if (size != i_BufferSize) return E_UNEXPECTED;

	// transition the vertex buffer data from copy destination state to vertex buffer state
	i_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(i_Buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	return hr;
}
