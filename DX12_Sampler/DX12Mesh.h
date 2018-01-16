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
class DX12Material;

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

	// To do : impl textures and different PSO management
	// element input defines the element desc in flags
	enum EElementFlags
	{
		eNone = 0,
		eHaveNormal = 1 << 0,
		eHaveTexcoord = 1 << 1,
		eHaveColor = 1 << 2,
	};

	// Factory
	static DX12Mesh *	GeneratePrimitiveMesh(EPrimitiveMesh i_Prim);
	static DX12Mesh *	LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder = nullptr);

	// one shape mesh generation
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount);
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount);
	~DX12Mesh();

	// submeshes
	bool	HaveSubMeshes() const;
	UINT	SubMeshesCount() const;

	const DX12MeshBuffer *					GetRootMesh() const;
	const std::vector<DX12MeshBuffer*>	&	GetSubMeshes() const;

	// these statics are for the primitive meshes
	static const D3D12_INPUT_ELEMENT_DESC	s_PrimitiveElementDesc[];
	static D3D12_INPUT_LAYOUT_DESC			s_PrimitiveLayoutDesc;

	// static helpers
	static UINT		GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static UINT64	CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static void		CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags);

private:
	// private constructor created by LoadMesh static function
	DX12Mesh();

	// mesh buffer (GPU)
	DX12MeshBuffer	*				m_RootMeshBuffer;
	std::vector<DX12MeshBuffer*>	m_SubMeshBuffer;	// if there is multiple shapes per mesh, there are here (sometime there is only submeshes so the root mesh is null)

	// material
	DX12Material *					m_RootMeshMaterial;
	std::vector<DX12Material*>		m_SubMeshMaterial;
};