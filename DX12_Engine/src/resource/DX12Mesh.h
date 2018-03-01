#pragma once

#include "DX12Resource.h"
#include <d3d12.h>
#include <DirectXMath.h>


class DX12Mesh : public DX12Resource
{
public:
	struct DX12MeshData
	{
		// DX12 data
		D3D12_INPUT_LAYOUT_DESC		InputLayout;	// to compute the stride, also to manage compatibilities between materials and meshes
		// raw data
		UINT						VerticesCount = 0;	// vertices count
		const BYTE *				VerticesBuffer = nullptr;	// must be filled
		UINT						IndexCount = 0;	// indices : can be 0 if no indexes
		const DWORD *				IndexBuffer = nullptr;	// null if no Index buffer	
		// other
		std::string					Name, Filepath;

	};

	// external management
	const D3D12_VERTEX_BUFFER_VIEW &		GetVertexBufferView() const;
	const D3D12_INDEX_BUFFER_VIEW &			GetIndexBufferView() const;

	// helper
	HRESULT							PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList, UINT i_Instance = 1) const;

	// infomations
	UINT							GetVerticeCount() const;
	UINT							GetIndexCount() const;
	bool							HaveIndexBuffer() const;
	const D3D12_INPUT_LAYOUT_DESC &	GetInputLayoutDesc() const;

	// friend class
	friend class DX12ResourceManager;

private:
	DX12Mesh();
	~DX12Mesh();

	// Inherited via DX12Resource
	virtual void LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) override;
	virtual void Release() override;

	// dx12 helpers
	static HRESULT	CreateBuffer(ID3D12Device * i_Device, ID3D12Resource ** i_Buffer, UINT i_BufferSize, const wchar_t * i_Name = L"Default Buffer");
	static HRESULT	UpdateData(ID3D12Device * i_Device, ID3D12GraphicsCommandList* i_CommandList, ID3D12Resource * i_Buffer, UINT i_BufferSize, const BYTE * i_Data);

	// Mesh data
	D3D12_INPUT_LAYOUT_DESC			m_InputLayoutDesc;
	// Buffer
	ID3D12Resource*					m_VertexBuffer;
	ID3D12Resource*					m_IndexBuffer;
	// Descriptors
	D3D12_VERTEX_BUFFER_VIEW		m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_IndexBufferView;

	// mesh management
	bool		m_HaveIndex;
	UINT		m_VertexCount;
	UINT		m_IndexCount;
	UINT		m_Count;	// vertices/index count for drawing
};