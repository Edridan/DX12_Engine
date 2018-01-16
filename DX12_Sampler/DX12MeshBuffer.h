// DX12MeshBuffer contains id to buffers and contains data for rendering
// this is instanced by the DX12Mesh
#pragma once

#include <d3d12.h>
#include <string>

class DX12MeshBuffer
{
public:
	// contructor / destructor
	DX12MeshBuffer(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, const std::wstring & i_Name = L"Unknown");
	DX12MeshBuffer(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount, const std::wstring & i_Name = L"Unknown");
	~DX12MeshBuffer();

	const D3D12_INPUT_LAYOUT_DESC & GetInputLayout() const;

	// push vertices buffer on commandlist
	HRESULT PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const;
	UINT64	GetElementFlags() const;	// retreive flags to render the mesh buffer

	// friend class
	friend class DX12Mesh;
private:
	// dx12 helpers
	static HRESULT	CreateBuffer(ID3D12Resource ** i_Buffer, UINT i_BufferSize, const wchar_t * i_Name = L"Default Buffer");
	static HRESULT	UpdateData(ID3D12GraphicsCommandList* i_CommandList, ID3D12Resource * i_Buffer, UINT i_BufferSize, BYTE * i_Data);


	// DX12
	D3D12_INPUT_LAYOUT_DESC				m_InputLayoutDesc;
	// Buffer
	ID3D12Resource*						m_VertexBuffer;
	ID3D12Resource*						m_IndexBuffer;
	// Buffer view
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW				m_IndexBufferView;
	// Other
	const std::wstring 					m_Name;

	UINT64								m_ElementFlags;

	const bool		m_HaveIndex;
	const UINT		m_Count;	// vertices/index count for drawing

#ifdef _DEBUG
	static UINT s_MeshInstanciated;
#endif
};