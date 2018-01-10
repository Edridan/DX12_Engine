#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "DX12Shader.h"
#include "DX12ConstantBuffer.h"

// Struct
struct Vertex
{
	// Vertice definition
	DirectX::XMFLOAT3 m_Pos;	// float x, float y, float z
	DirectX::XMFLOAT4 m_Color;	// float x, float y, float z, float a
};

class DX12Mesh
{
public:
	/*
	 *	Primitive mesh generator
	 */
	enum EPrimitiveMesh
	{
		eTriangle,
		ePlane,
		eCube,
	};
	// Factory
	static DX12Mesh *	GeneratePrimitiveMesh(EPrimitiveMesh i_Prim);
	static DX12Mesh *	LoadMesh(const char * i_Filename, const char * i_Folder = nullptr);

	// Mesh
	DX12Mesh(Vertex * i_Vertices, UINT i_VerticeCount);
	DX12Mesh(Vertex * i_Vertices, UINT i_VerticeCount, DWORD * i_Indices, UINT i_IndiceCount);
	~DX12Mesh();

	// Management
	void		Draw(ID3D12GraphicsCommandList* i_CommandList, ID3D12PipelineState*  i_Pso);
	void		Draw(ID3D12GraphicsCommandList* i_CommandList, ID3D12PipelineState*  i_Pso, D3D12_GPU_VIRTUAL_ADDRESS i_ConstantBufferAddress);

	// Get/Set
	const D3D12_INPUT_LAYOUT_DESC & GetInputLayoutDesc() const;
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC & GetPipelineStateDesc() const;

	// Input layout defined for rendering meshes (Used by PSO)
	static const D3D12_INPUT_ELEMENT_DESC	s_DefaultInputElement[];
	static D3D12_INPUT_LAYOUT_DESC			s_DefaultInputLayout;

private:
	// Mesh data
	UINT		m_VerticesCount;
	UINT		m_IndexCount;
	const bool	m_HaveIndex;

	// DX12
	ID3D12Resource*						m_VertexBuffer;
	ID3D12Resource*						m_IndexBuffer;

	// Helpers
	HRESULT		PushDX12ResourceBuffer(ID3D12Resource ** i_Buffer, UINT i_BufferSize, DWORD * i_BufferPtr, ID3D12GraphicsCommandList* i_CommandList, ID3D12Device * i_Device);

	// Buffer view
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW				m_IndexBufferView;

	// To do : implement pso management for mesh rendering
	D3D12_INPUT_LAYOUT_DESC						m_InputLayoutDesc;
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC *	m_PipelineStateDesc;	// External
};