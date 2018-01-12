// DX12MeshBuffer contains id to buffers and contains data for rendering
// this is instanced by the DX12Mesh
#pragma once

#include <d3d12.h>

class DX12MeshBuffer
{
public:
	// contructor / destructor
	DX12MeshBuffer(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount);
	DX12MeshBuffer(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount);
	~DX12MeshBuffer();

	const D3D12_INPUT_LAYOUT_DESC & GetInputLayout() const;

	// push vertices buffer on commandlist
	HRESULT PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const;

	// friend class
	friend class DX12Mesh;
private:
	// dx12 helpers
	static HRESULT	CreateBuffer(ID3D12Resource ** i_Buffer, UINT i_BufferSize, const wchar_t * i_Name = L"Default Buffer");
	static HRESULT	UpdateData(ID3D12GraphicsCommandList* i_CommandList, ID3D12Resource * i_Buffer, UINT i_BufferSize, BYTE * i_Data);
	static UINT		GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout);

	// DX12
	D3D12_INPUT_LAYOUT_DESC				m_InputLayoutDesc;
	// Buffer
	ID3D12Resource*						m_VertexBuffer;
	ID3D12Resource*						m_IndexBuffer;
	// Buffer view
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW				m_IndexBufferView;

	const bool		m_HaveIndex;
	const UINT		m_Count;	// vertices/index count for drawing

#ifdef _DEBUG
	static UINT s_MeshInstanciated;
#endif
};