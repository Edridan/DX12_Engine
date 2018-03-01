// Mesh management
// this contains all mesh data interesting (pointers to materials, pointers to buffer)
// this is used by editor

#pragma once

#include "Resource.h"
#include "resource/DX12Mesh.h"
#include <vector>

// class predef : these are all the DX12Resource used for render the model
class DX12Material;
class DX12Texture;
class DX12Mesh;

class Mesh : public Resource
{
public:
	// retreive DX12 data
	DX12Mesh *		GetMeshBuffer(size_t i_Index = 0) const;
	DX12Mesh *		GetMeshBuffer(const std::string & i_Name) const;
	DX12Material *	GetMaterial(int i_MeshIndex = 0, int i_MaterialIndex = 0) const;
	DX12Material *	GetMaterial(const std::string & i_Name, int i_MaterialIndex = 0) const;

	// informations
	size_t			GetMeshCount() const;
	size_t			GetMaterialCount(int i_Index = 0) const;
	bool			IsMultiMesh() const;	// mesh have multi shapes

	friend class ResourceManager;
protected:
	// constructor
	Mesh();
	~Mesh();

	struct MeshData
	{
		DX12Mesh *						MeshBuffer;	// pointer to the mesh buffer
		std::vector<DX12Material *>		Materials;	// pointer to one or multiple materials
		// CPU mesh data
		const BYTE *					VertexData;	// this contains all data for the vertex
		const DWORD *					IndexData;	// this contains all data for the index
	};

	// containing all data for the meshes
	std::vector<MeshData>		m_MeshData;
	
	// Inherited via Resource
	virtual void LoadFromFile(const std::string & i_Filepath) override;
	virtual void LoadFromData(const void * i_Data) override;

	// internal helpers
	void	LoadPrimitiveMesh(const std::string & i_PrimitiveName);
	void	LoadMeshFromFile(const std::string & i_Filepath);
};