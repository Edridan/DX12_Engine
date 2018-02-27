// Mesh management
// this contains all mesh data interesting (pointers to materials, pointers to buffer)
// this is used by editor

#pragma once

#include "Resource.h"
#include <vector>

// class predef : these are all the DX12Resource used for render the model
class DX12Material;
class DX12Texture;
class DX12Mesh;

class Mesh : public Resource
{
public:
	// retreive DX12 data
	DX12Mesh *		GetMeshBuffer(int i_Index = 0) const;
	DX12Material *	GetMaterial(int i_MeshIndex = 0, int i_MaterialIndex = 0) const;

	// informations
	size_t			GetMeshCount() const;
	size_t			GetMaterialCount(int i_Index = 0) const;

	friend class ResourceManager;
protected:
	struct MeshData
	{
		DX12Mesh *						MeshBuffer;	// pointer to the mesh buffer
		std::vector<DX12Material *>		Materials;	// pointer to one or multiple materials
		// CPU mesh data
		BYTE *					VertexData;	// this contains all data for the vertex
		DWORD *					IndexData;	// this contains all data for the index
	};

	// containing all data for the meshes
	std::vector<MeshData>		m_MeshData;
	
	// Inherited via Resource
	virtual void LoadFromFile(const std::string & i_Filepath) override;
	virtual void LoadFromData(const void * i_Data) override;

	Mesh();
	~Mesh();

};