#include "Mesh.h"

#include "engine/Debug.h"
#include "engine/Engine.h"
#include "engine/Utils.h"
#include "dx12/DX12PipelineState.h"
#include "resource/Material.h"
#include "resource/Texture.h"
#include "resource/ResourceManager.h"
#include "resource/DX12ResourceManager.h"
#include <algorithm>

// tinyobj loader
#include "../lib/tinyobjloader/tiny_obj_loader.h"

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

DX12Mesh * Mesh::GetMeshBuffer(const std::string & i_Name) const
{
	for (size_t i = 0; i < m_MeshData.size(); ++i)
	{
		if (m_MeshData[i].MeshBuffer->GetName() == i_Name)
			return m_MeshData[i].MeshBuffer;
	}

	// didn't find
	return nullptr;
}

DX12Material * Mesh::GetMaterial(int i_MeshIndex, int i_MaterialIndex) const
{
	ASSERT(i_MeshIndex < m_MeshData.size());
	ASSERT(i_MaterialIndex < m_MeshData[i_MeshIndex].Materials.size());

	return m_MeshData[i_MeshIndex].Materials[i_MaterialIndex];
}

DX12Material * Mesh::GetMaterial(const std::string & i_Name,  int i_MaterialIndex) const
{
	for (size_t i = 0; i < m_MeshData.size(); ++i)
	{
		if (m_MeshData[i].MeshBuffer->GetName() == i_Name)
		{
			ASSERT(i_MaterialIndex < m_MeshData[i].Materials.size());
			return m_MeshData[i].Materials[i_MaterialIndex];
		}
	}

	// didn't find the material
	return nullptr;
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

bool Mesh::IsMultiMesh() const
{
	return (m_MeshData.size() > 1);
}

void Mesh::LoadFromFile(const std::string & i_Filepath)
{
	Engine & engine = Engine::GetInstance();

	m_Filepath = i_Filepath;

	if (String::StartWith(i_Filepath, "Primitive:"))
	{
		// generate name for the resource
		m_Name = i_Filepath.substr(11);	// remove "Primitive:" from the file path 
		LoadPrimitiveMesh(i_Filepath);
	}
	else
	{
		// generate name for the resource
		m_Name = ExtractFileName(i_Filepath);	// name for the resource (will displayed on the editor)
		LoadMeshFromFile(i_Filepath);
	}
}

void Mesh::LoadFromData(const void * i_Data)
{
	TO_DO;
}

FORCEINLINE void Mesh::LoadPrimitiveMesh(const std::string & i_PrimitiveName)
{
	DX12Mesh::DX12MeshData mData;

	// name
	mData.Filepath	= i_PrimitiveName;
	mData.Name		= m_Name;

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

	mData.InputLayout = layoutDesc;

	// generate mesh depending type
	if (i_PrimitiveName == "Primitive:Cube")
	{
		mData.VerticesBuffer	= reinterpret_cast<BYTE*>(vCube);
		mData.VerticesCount		= 24u;
		mData.IndexBuffer		= iCube;
		mData.IndexCount		= _countof(iCube);
	}
	else if (i_PrimitiveName == "Primitive:Plane")
	{
		mData.VerticesBuffer	= reinterpret_cast<BYTE*>(vPlane);
		mData.VerticesCount		= 4u;
		mData.IndexBuffer		= iPlane;
		mData.IndexCount		= _countof(iPlane);
	}
	else if (i_PrimitiveName == "Primitive:Triangle")
	{
		mData.VerticesBuffer	= reinterpret_cast<BYTE*>(vTriangle);
		mData.VerticesCount		= 3u;
	}

	MeshData meshData;
	meshData.VertexData = mData.VerticesBuffer;
	meshData.IndexData = mData.IndexBuffer;

	// load mesh here
	meshData.MeshBuffer = Engine::GetInstance().GetRenderResourceManager()->PushMesh(&mData);

	if (meshData.MeshBuffer == nullptr)
	{
		ASSERT_ERROR("Error during creation of DX12Mesh");
		DEBUG_BREAK;
		return;
	}

	m_MeshData.push_back(meshData);

	NotifyFinishLoad();
}

FORCEINLINE void Mesh::LoadMeshFromFile(const std::string & i_Filepath)
{
	ResourceManager * const resourceManager			= Engine::GetInstance().GetResourceManager();
	DX12ResourceManager * const dx12ResourceManager = Engine::GetInstance().GetRenderResourceManager();

	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;

	// create load directory
	std::string error;
	std::string textureFolderDefault = "resources/tex/";	// default path for textures (tested if texture not found in the base path)
	std::string textureFolder = ExtractFilePath(i_Filepath);
	std::string materialFolder = ExtractFilePath(i_Filepath);
	std::string objFilepath(i_Filepath);

	// load the mesh and materials
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, objFilepath.c_str(), materialFolder.c_str());

	if (!ret)
	{
		// end loading
		ASSERT_ERROR(error.c_str());
		DEBUG_BREAK;
		return;
	}

	// for each shapes
	for (size_t sh = 0; sh < shapes.size(); ++sh)
	{
		MeshData mData;	// create a new mesh data that will be contains
		UINT stride = 3;	// default stride in float (3 float for positions)
		tinyobj::shape_t * shape = &shapes[sh];
		const size_t verticeCount = shape->mesh.indices.size();
		std::string shapeName = m_Name + ":" + shape->name;	// this will be : Resource name : 

		// compute the flag :
		// by default the mesh always have normals
		const tinyobj::index_t origin = shape->mesh.indices[0];
		UINT64 flags = DX12PipelineState::EElementFlags::eNone;

		// generate the materials for the current shape
		// retreive materials per id and sort them to 
		std::vector<int> meshMaterials(shape->mesh.material_ids);
		meshMaterials.erase(std::unique(meshMaterials.begin(), meshMaterials.end()), meshMaterials.end());

		// compute stride and flags for the mesh
		if (origin.normal_index != -1)
		{
			flags |= DX12PipelineState::EElementFlags::eHaveNormal;
			stride += 3;
		}
		if (origin.texcoord_index != -1)
		{
			flags |= DX12PipelineState::EElementFlags::eHaveTexcoord;
			stride += 2;
		}
		if (meshMaterials.size() > 1)
		{
			// To do : add a flags for material index
			// the materials will be filled into the buffer needed
			TO_DO;
		}

		// To do : support other mesh types
		// only one supported type for now
		if (!(flags & DX12PipelineState::EElementFlags::eHaveNormal) || !(flags & DX12PipelineState::EElementFlags::eHaveTexcoord))
		{
			TO_DO;
			return;
		}

		// generate vertex buffer
		FLOAT * const verticeBuffer = new FLOAT[verticeCount * stride];
		FLOAT * bufferItr = verticeBuffer;

		// fill the vertex buffer that will contains vertex data
		// this manage to fill only needed data as position, UV, normals etc...
		// To do : fill other data that is needed but not existing in the mesh file
		// To do : fill index and vertex buffer into 2 different buffer to avoid
		for (size_t id = 0; id < shape->mesh.indices.size(); ++id)
		{
			// generate the mesh vertex operations
			const tinyobj::index_t index = shape->mesh.indices[id];
			memcpy(bufferItr, &attrib.vertices[3 * index.vertex_index], 3 * sizeof(FLOAT));
			bufferItr += 3;

			if (flags & DX12PipelineState::EElementFlags::eHaveNormal)
			{
				// copy the normal
				memcpy(bufferItr, &attrib.normals[3 * index.normal_index], 3 * sizeof(FLOAT));
				bufferItr += 3;
			}
			if (flags & DX12PipelineState::EElementFlags::eHaveTexcoord)
			{
				// copy the uv
				memcpy(bufferItr, &attrib.texcoords[2 * index.texcoord_index], 2 * sizeof(FLOAT));
				bufferItr += 2;
			}
		}

		std::string materialName = "Generated:" + m_Filepath + "_" + m_Name;

		// To do : search before and 

		Material::MaterialData matData;
		matData.Filepath = materialName;	// put the identifier
		matData.MaterialCount = meshMaterials.size();
		matData.Materials = new Material::MaterialSpec[matData.MaterialCount];

		for (size_t i = 0; i < meshMaterials.size(); ++i)
		{
			tinyobj::material_t mat = materials[meshMaterials[i]];
			Material::MaterialSpec & m = matData.Materials[i];

			// Name
			m.Name = mat.name;

			// To do : load textures
			/*desc.map_Ka = LoadTexture(mat.ambient_texname, textureFolder, resourcesManager);
			desc.map_Kd = LoadTexture(mat.diffuse_texname, textureFolder, resourcesManager);
			desc.map_Ks = LoadTexture(mat.specular_texname, textureFolder, resourcesManager);*/

			// retreive other data
			m.Ka = mat.ambient;
			m.Kd = mat.diffuse;
			m.Ke = mat.emission;
			m.Ks = mat.specular;
		}

		Material * material = resourceManager->LoadMaterialWithData(&matData);
		
		if (material->IsLoaded())
		{
			for (size_t i = 0; i < meshMaterials.size(); ++i)
			{
				tinyobj::material_t mat = materials[meshMaterials[i]];
				DX12Material * m = material->GetDX12Material(mat.name);

				if (m != nullptr)
				{
					mData.Materials.push_back(m);
				}
			}
		}
		else
		{
			ASSERT_ERROR("Error when loading materials");
			return;
		}
		
		// generate layout for the shape
		D3D12_INPUT_LAYOUT_DESC layout;
		DX12PipelineState::CreateInputLayoutFromFlags(layout, flags);
		
		// generate mesh data for mesh loading
		DX12Mesh::DX12MeshData * meshData = new DX12Mesh::DX12MeshData;
		DX12PipelineState::CopyInputLayout(meshData->InputLayout, layout);

		// fill buffers into the data
		meshData->VerticesBuffer	= reinterpret_cast<BYTE*>(verticeBuffer);
		meshData->VerticesCount		= verticeCount;

		// generate the mesh (will be uploaded onto the GPU later)
		mData.MeshBuffer = dx12ResourceManager->PushMesh(meshData);
		ASSERT(mData.MeshBuffer != nullptr);

		m_MeshData.push_back(mData);
	}

	NotifyFinishLoad();
}

Mesh::Mesh()
	:Resource()
	,m_MeshData()
{
}

Mesh::~Mesh()
{
	// release resource
}
