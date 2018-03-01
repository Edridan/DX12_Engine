#include "DX12Mesh.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12PipelineState.h"
#include "engine/Debug.h"
#include "engine/Utils.h"

const D3D12_VERTEX_BUFFER_VIEW & DX12Mesh::GetVertexBufferView() const
{
	return m_VertexBufferView;
}

const D3D12_INDEX_BUFFER_VIEW & DX12Mesh::GetIndexBufferView() const
{
	return m_IndexBufferView;
}

HRESULT DX12Mesh::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList, UINT i_Instance) const
{
	i_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView); // set the vertex buffer (using the vertex buffer view)

	if (HaveIndexBuffer())
	{
		i_CommandList->IASetIndexBuffer(&m_IndexBufferView);	// push the index buffer into the command list
		i_CommandList->DrawIndexedInstanced(m_Count, i_Instance, 0, 0, 0);	// draw indexed vertices
	}
	else
	{
		i_CommandList->DrawInstanced(m_Count, i_Instance, 0, 0);	// draw triangles
	}

	return S_OK;
}

UINT DX12Mesh::GetVerticeCount() const
{
	return m_VertexCount;
}

UINT DX12Mesh::GetIndexCount() const
{
	return m_IndexCount;
}

FORCEINLINE bool DX12Mesh::HaveIndexBuffer() const
{
	return (m_IndexCount != 0);
}

const D3D12_INPUT_LAYOUT_DESC & DX12Mesh::GetInputLayoutDesc() const
{
	return m_InputLayoutDesc;
}

DX12Mesh::DX12Mesh(DX12MeshData * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device)
	:DX12Resource(true)	// the data is loaded on different path than the resource manager
	,m_IndexBuffer(nullptr)
	,m_VertexBuffer(nullptr)
	,m_Count(0)
	,m_IndexCount(0)
	,m_VertexCount(0)
{
	PreloadData(i_Data);
	LoadFromData(i_Data, i_CommandList, i_Device);
}

DX12Mesh::DX12Mesh()
	:m_IndexBuffer(nullptr)
	,m_VertexBuffer(nullptr)
	,m_Count(0)
	,m_IndexCount(0)
	,m_VertexCount(0)
{
}

DX12Mesh::~DX12Mesh()
{
}


void DX12Mesh::LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device)
{
	// we are retreiving data
	const DX12MeshData * data = (const DX12MeshData*)i_Data;

	// vertices count
	const UINT stride = DX12PipelineState::GetElementSize(m_InputLayoutDesc);
	const UINT vBufferSize = m_VertexCount * stride;
	// index count
	const UINT iBufferSize = sizeof(DWORD) * m_IndexCount;

	std::wstring name;
	String::Utf8ToUtf16(name, m_Name);

	// create vertex buffer
	CreateBuffer(i_Device, &m_VertexBuffer, vBufferSize, name.c_str());
	UpdateData(i_Device, i_CommandList, m_VertexBuffer, vBufferSize, (data->VerticesBuffer));

	// create a vertex buffer view for the triangle. We get the GPU memory address to the pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = stride;
	m_VertexBufferView.SizeInBytes = vBufferSize;

	// create index buffer if necessary
	if (m_IndexCount != 0)
	{
		CreateBuffer(i_Device, &m_IndexBuffer, iBufferSize, name.c_str());
		UpdateData(i_Device, i_CommandList, m_IndexBuffer, iBufferSize, reinterpret_cast<const BYTE*>(data->IndexBuffer));

		// create a index buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
		m_IndexBufferView.SizeInBytes = iBufferSize;
	}

	// delete the data
	delete data;
}

void DX12Mesh::PreloadData(const void * i_Data)
{
	// we are retreiving data
	const DX12MeshData * data = (const DX12MeshData*)i_Data;

	// information
	m_Name = data->Name;
	m_Filepath = data->Filepath;

	m_IndexCount = data->IndexCount;
	m_VertexCount = data->VerticesCount;
	m_Count = (m_IndexCount != 0) ? m_IndexCount : m_VertexCount;

	ASSERT(m_IndexCount != 0 || m_VertexCount != 0);
	ASSERT(m_Count != 0);

	// retreive the input layout
	DX12PipelineState::CopyInputLayout(m_InputLayoutDesc, data->InputLayout);
}

void DX12Mesh::Release()
{
	SAFE_RELEASE(m_VertexBuffer);

	if (HaveIndexBuffer())
	{
		SAFE_RELEASE(m_IndexBuffer);
	}

	// call for release of the resource
	DX12Resource::Release();
}

HRESULT DX12Mesh::CreateBuffer(ID3D12Device * i_Device, ID3D12Resource ** i_Buffer, UINT i_BufferSize, const wchar_t * i_Name)
{
	HRESULT hr;

	hr = i_Device->CreateCommittedResource(
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

HRESULT DX12Mesh::UpdateData(ID3D12Device * i_Device, ID3D12GraphicsCommandList * i_CommandList, ID3D12Resource * i_Buffer, UINT i_BufferSize, const BYTE * i_Data)
{
	HRESULT hr;
	ID3D12Resource* bufferUploadHeap;
	hr = i_Device->CreateCommittedResource(
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
