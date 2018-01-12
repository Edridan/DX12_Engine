#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <vector>

#include "DX12Shader.h"
#include "DX12ConstantBuffer.h"

// class predef
class DX12MeshBuffer;

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

	// To do : impl textures and different PSO management
	// element input defines the element desc in flags
	enum EElementInput
	{
		eNone			= 0,
		eHaveNormal		= 1 << 0,
		eHaveTexcoord	= 1 << 1,
		eHaveColor		= 1 << 2,
	};

	// one shape mesh generation
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount);
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount);
	~DX12Mesh();

	// submeshes
	bool	HaveSubMeshes() const;
	UINT	SubMeshesCount() const;

	const DX12MeshBuffer *					GetRootMesh() const;
	const std::vector<DX12MeshBuffer*>	&	GetSubMeshes() const;

	UINT64		GetElementFlags() const;

	// Get/Set
	const D3D12_INPUT_LAYOUT_DESC & GetInputLayoutDesc() const;
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC & GetPipelineStateDesc() const;

	// Input layout defined for rendering meshes (Used by PSO)
	static const D3D12_INPUT_ELEMENT_DESC	s_DefaultInputElement[];
	static D3D12_INPUT_LAYOUT_DESC			s_DefaultInputLayout;

private:
	// private constructor created by LoadMesh static function
	DX12Mesh();

	// mesh buffer (GPU)
	DX12MeshBuffer	*				m_RootMeshBuffer;
	std::vector<DX12MeshBuffer*>	m_SubMeshBuffer;	// if there is multiple shapes per mesh, there are here (sometime there is only submeshes so the root mesh is null)

	UINT64							m_ElementFlags;

	// To do : implement pso management for mesh rendering
	D3D12_INPUT_LAYOUT_DESC						m_InputLayoutDesc;
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC *	m_PipelineStateDesc;	// External
};