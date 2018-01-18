#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <vector>
#include <map>

#include "DX12Shader.h"
#include "DX12ConstantBuffer.h"

// class predef
class DX12MeshBuffer;
class DX12Material;
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
	static DX12Mesh *	LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder = nullptr, const char * i_TextureFolder = nullptr);

	// one shape mesh generation
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount);
	DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount);
	~DX12Mesh();

	// submeshes
	bool	HaveSubMeshes() const;
	UINT	SubMeshesCount() const;

	// get the textures for the meshes (future : will be stocked directly into the materials)
	int		GetTextures(std::vector<DX12Texture*> o_Textures, size_t i_SubMeshId);	// specific for the submeshes
	int		GetTextures(std::vector<DX12Texture*> o_Textures);	// root mesh
	// get material for the meshes
	// to do : implement
	int		GetMaterial(std::vector<DX12Material*> o_Mat, size_t i_SubMeshId);	// specific for the submeshes
	int		GetMaterial(std::vector<DX12Material*> o_Mat);	// root mesh

	const DX12MeshBuffer *		GetRootMesh() const;
	const DX12MeshBuffer*		GetSubMeshes(size_t i_Index) const;

	// these statics are for the primitive meshes
	static const D3D12_INPUT_ELEMENT_DESC	s_PrimitiveElementDesc[];
	static D3D12_INPUT_LAYOUT_DESC			s_PrimitiveLayoutDesc;

	// static helpers
	static UINT		GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static UINT64	CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static void		CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags);

private:
	struct MeshBuffer
	{
		std::vector<DX12Texture*>	Textures;
		std::vector<DX12Material*>	Materials;
		DX12MeshBuffer *			Mesh;
	};

	// private constructor created by LoadMesh static function
	DX12Mesh();

	// mesh buffer (GPU)
	MeshBuffer	*					m_RootMeshBuffer;
	std::vector<MeshBuffer*>		m_SubMeshBuffer;	// if there is multiple shapes per mesh, there are here (sometime there is only submeshes so the root mesh is null)

	// material
	std::vector<DX12Material*>		m_Materials;

	// textures
	std::map<std::string, DX12Texture *>	m_Textures;
};