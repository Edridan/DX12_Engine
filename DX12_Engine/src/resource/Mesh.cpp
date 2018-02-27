#include "Mesh.h"

#include "engine/Debug.h"
#include "engine/Utils.h"

#include "resource/DX12Mesh.h"

///////////////////////////////////////////////////////////////////
// primitive mesh manager
// Triangle data
FLOAT vTriangle[] = {
	0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
};

// Plane data
FLOAT vPlane[] = {
	-0.5f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	0.5f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f
};

DWORD iPlane[] = {
	0, 1, 2,
	1, 0, 3
};

// Cube mesh
FLOAT vCube[] = {
	-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
};

DWORD iCube[] = {
	0, 1, 2, 0, 3, 1,
	4, 5, 6, 4, 7, 5,
	8, 9, 10, 8, 11, 9,
	12, 13, 14, 12, 15, 13,
	16, 17, 18, 16, 19, 17,
	20, 21, 22, 20, 23, 21,
};

///////////////////////////////////////////////////////////////////
// Mesh implementation

DX12Mesh * Mesh::GetMeshBuffer(int i_Index) const
{
	ASSERT(i_Index < m_MeshData.size());
	return m_MeshData[i_Index].MeshBuffer;
}

DX12Material * Mesh::GetMaterial(int i_MeshIndex, int i_MaterialIndex) const
{
	ASSERT(i_MeshIndex < m_MeshData.size());
	ASSERT(i_MaterialIndex < m_MeshData[i_MeshIndex].Materials.size());

	return m_MeshData[i_MeshIndex].Materials[i_MaterialIndex];
}

size_t Mesh::GetMeshCount() const
{
	return m_MeshData.size();
}

size_t Mesh::GetMaterialCount(int i_Index) const
{
	ASSERT(i_Index < m_MeshData.size());
	return m_MeshData[i_Index].Materials.size();
}

void Mesh::LoadFromFile(const std::string & i_Filepath)
{
	if (String::StartWith(i_Filepath, "Primitive:"))
	{
		DX12Mesh::DX12MeshData meshData;

		// name
		meshData.FileName	= i_Filepath;
		meshData.Name		= i_Filepath.substr(10, 10 - i_Filepath.size());

		// input description management
		const D3D12_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		const D3D12_INPUT_LAYOUT_DESC layoutDesc =
		{
			inputDesc,
			sizeof(inputDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC)
		};

		meshData.InputLayout = layoutDesc;

		// generate mesh depending type
		if (i_Filepath == "Primitive:Cube")
		{
			meshData.VerticesBuffer = reinterpret_cast<BYTE*>(vCube);
			meshData.IndexBuffer = iCube;
		}
		else if (i_Filepath == "Primitive:Plane")
		{
			meshData.VerticesBuffer = reinterpret_cast<BYTE*>(vPlane);
			meshData.IndexBuffer = iPlane;
		}
		else if (i_Filepath == "Primitive:Triangle")
		{
			meshData.VerticesBuffer = reinterpret_cast<BYTE*>(vTriangle);
		}
	}
	else
	{
		
	}
}

void Mesh::LoadFromData(const void * i_Data)
{
	TO_DO;
}

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	// release resource
	
}
