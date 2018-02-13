#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <vector>
#include <map>

#include "dx12/DX12Shader.h"
#include "dx12/DX12ConstantBuffer.h"
#include "dx12/DX12Material.h"

// class predef
class DX12MeshBuffer;
class DX12Texture;


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
	static DX12Mesh *	LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder = nullptr, const char * i_TextureFolder = nullptr);

	// one shape procedural mesh generation
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount);
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount);
	~DX12Mesh();

	// submeshes
	bool	HaveSubMeshes() const;
	UINT	SubMeshesCount() const;

	// get material for the meshes
	// to do : implement
	DX12Material::DX12MaterialDesc		GetMaterial(size_t i_SubMeshId);	// specific for the submeshes (one material per mesh
	DX12Material::DX12MaterialDesc		GetMaterial();	// root mesh

	const DX12MeshBuffer *		GetRootMesh() const;
	const DX12MeshBuffer*		GetSubMeshes(size_t i_Index) const;

	// these statics are for the primitive meshes
	static const D3D12_INPUT_ELEMENT_DESC	s_PrimitiveElementDesc[];
	static D3D12_INPUT_LAYOUT_DESC			s_PrimitiveLayoutDesc;

private:
	// private constructor created by LoadMesh static function
	DX12Mesh();

	// mesh buffer (GPU)
	DX12MeshBuffer	*				m_RootMeshBuffer;
	std::vector<DX12MeshBuffer*>	m_SubMeshBuffer;	// if there is multiple shapes per mesh, there are here (sometime there is only submeshes so the root mesh is null)
};