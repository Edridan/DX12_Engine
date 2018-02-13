#include "dx12/DX12Mesh.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12MeshBuffer.h"
#include "dx12/DX12Texture.h"
#include "dx12/DX12Material.h"
#include "dx12/DX12PipelineState.h"

#include "d3dx12.h"

#include "engine/Clock.h"
#include "engine/Engine.h"
#include "engine/ResourcesManager.h"

#include <sstream>
#include <algorithm>
#include <codecvt>

// mesh loader
#include "../lib/tinyobjloader/tiny_obj_loader.h"

// default input element using the input color
const D3D12_INPUT_ELEMENT_DESC DX12Mesh::s_PrimitiveElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

// fill out the default input layout description structure
D3D12_INPUT_LAYOUT_DESC DX12Mesh::s_PrimitiveLayoutDesc =
{
	s_PrimitiveElementDesc,
	sizeof(DX12Mesh::s_PrimitiveElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC)
};

/*
 *	Mesh primitive buffers
 */

 // Triangle data
FLOAT vTriangle[] = {
	 0.0f, 0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
	 -0.5f, -0.5f, 0.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
};

// Plane data
FLOAT vPlane[] = {
	-0.5f,  0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
	 0.5f,  0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f
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
	-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
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


/// GENERATE PRIMITIVE MESH HERE :
DX12Mesh * DX12Mesh::GeneratePrimitiveMesh(EPrimitiveMesh i_Prim)
{
	DX12Mesh * returnMesh = nullptr;
	// default primitive mesh flags
	static const UINT64 flags = 
		DX12PipelineState::EElementFlags::eHaveNormal |
		DX12PipelineState::EElementFlags::eHaveTexcoord;

	switch (i_Prim)
	{
	case ePlane:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vPlane), 4u, iPlane, 6u);
		break;
	case eTriangle:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vTriangle), 3u);
		break;
	case eCube:
		returnMesh = new DX12Mesh(s_PrimitiveLayoutDesc,
			reinterpret_cast<BYTE*>(vCube), 24u, iCube, 36u);
		break;
	}

	return returnMesh;
}

// helpers
inline DX12Texture * LoadTexture(const std::string & i_TexName, const std::string & i_Folder, ResourcesManager * i_Manager)
{
	if (i_TexName == "")
		return nullptr;

	const std::string filepath = i_Folder + i_TexName;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
	const std::wstring wFilepath = stringConverter.from_bytes(filepath.c_str());

	DX12Texture * texture = i_Manager->GetTexture(wFilepath.c_str());
	if (texture != nullptr && texture->IsLoaded())
	{
		return texture;
	}

	PRINT_DEBUG("Error when loading %s", i_TexName.c_str());
	DEBUG_BREAK;
	return nullptr;
}

DX12Mesh * DX12Mesh::LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder, const char * i_TextureFolder)
{
	ResourcesManager * resourcesManager = Engine::GetInstance().GetResourcesManager();

	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	material;

	std::string error;
	Clock timer;
	timer.Restart();

	// create load directory
	std::string textureFolder;
	std::string materialFolder;
	std::string objFilepath(i_Filename);

	if (i_TextureFolder == nullptr)
	{
		if (i_MaterialFolder != nullptr)
		{
			textureFolder.append(i_MaterialFolder);
		}
		else
		{
			// default texture folder
			textureFolder.append("resources/tex/");
		}
	}

	if (i_MaterialFolder == nullptr)
	{
		// retreive the same folder as the obj
		size_t i = objFilepath.find_last_of("/");

		if (i != std::string::npos)
		{
			materialFolder.append(objFilepath, 0, (i + 1));
		}
		else
		{
			// default material folder
			materialFolder.append("resources/mat/");
		}
	}

	// load the model
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &material, &error, objFilepath.c_str(), materialFolder.c_str());
	float loadTime = timer.GetElaspedTime().ToSeconds();

#ifdef _DEBUG
	// display debug message
	std::replace(error.begin(), error.end(), '\n', ' ');
	std::ostringstream stream;
	stream << "load obj : " << i_Filename << " : load time (" << loadTime << " sec)" 
		<< std::endl << (ret ? "[No Error]" : error);
	
	std::string message(stream.str());
	PRINT_DEBUG(message.c_str());
#endif

	if (!ret)
	{
		DEBUG_BREAK;
		return nullptr;
	}

	// load mesh information and create the stream
	DX12Mesh * mesh = new DX12Mesh;

	for (size_t sh = 0; sh < shapes.size(); ++sh)
	{
		// create buffer and initialize data
		UINT stride					= 3;	// default stride in float (3 float for positions)
		tinyobj::shape_t * shape	= &shapes[sh];
		const size_t verticeCount	= shape->mesh.indices.size();

		// compute the flag :
		// by default the mesh always have normals
		const tinyobj::index_t origin = shape->mesh.indices[0];
		UINT64 flags = DX12PipelineState::EElementFlags::eNone;

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
		// do not support vertex color for now

		FLOAT * const verticeBuffer = new FLOAT[verticeCount * stride];
		FLOAT * bufferItr = verticeBuffer;

		for (size_t id = 0; id < shape->mesh.indices.size(); ++id)
		{
			const tinyobj::index_t index = shape->mesh.indices[id];

			// layout order definition depending flags : 
			// 1 - Position
			// 2 - Normal
			// 3 - Texcoord
			// 4 - Color

			// copy the position to the buffer
			memcpy(bufferItr, & attrib.vertices[3 * index.vertex_index], 3 * sizeof(FLOAT));
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
			//if (flags & DX12Mesh::EElementFlags::eHaveColor)
			//{
			//	// copy the color
			//	memcpy(bufferItr, &attrib.colors[3 * index.vertex_index], 3 * sizeof(FLOAT));
			//	bufferItr += 3;
			//}
		}

		// Get the name of the shape
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
		std::wstring wname = stringConverter.from_bytes(shape->name);

		D3D12_INPUT_LAYOUT_DESC layout;
		DX12PipelineState::CreateInputLayoutFromFlags(layout, flags);

		DX12MeshBuffer * meshBuffer = new DX12MeshBuffer(
			layout,	// the generated layout depending on the flags
			reinterpret_cast<BYTE*>(verticeBuffer),
			(UINT)verticeCount,
			wname.c_str());

		// retreive materials of the mesh
		std::vector<int> meshMaterials(shape->mesh.material_ids);
		meshMaterials.erase(std::unique(meshMaterials.begin(), meshMaterials.end()), meshMaterials.end());

		if (meshMaterials.size() > 1)
		{
			// To do : create other mesh with multiple material
		}

		// load materials
		for (size_t i = 0; i < meshMaterials.size(); ++i)
		{
			tinyobj::material_t mat = material[meshMaterials[i]];

			DX12Material::DX12MaterialDesc desc;

			// setup the name of the material
			desc.Name = mat.name;

			// load textures
			desc.map_Ka = LoadTexture(mat.ambient_texname, textureFolder, resourcesManager);
			desc.map_Kd = LoadTexture(mat.diffuse_texname, textureFolder, resourcesManager);
			desc.map_Ks = LoadTexture(mat.specular_texname, textureFolder, resourcesManager);
			// to do : load other textures

			// retreive other data
			desc.Ka = mat.ambient;
			desc.Kd = mat.diffuse;
			desc.Ke = mat.emission;
			desc.Ks = mat.specular;

			meshBuffer->SetDefaultMaterial(desc);
		}

		// create mesh and initialize it
		mesh->m_SubMeshBuffer.push_back(meshBuffer);
		
		// cleanup the resources
		delete[] verticeBuffer;
	}

	if (shapes.size() == 1)
	{
		// we put the only one submesh from the submeshes to the root mesh
		mesh->m_RootMeshBuffer = mesh->m_SubMeshBuffer[0];
		mesh->m_SubMeshBuffer.clear();
	}

	return mesh;
}

DX12Mesh::DX12Mesh()
	:m_RootMeshBuffer(nullptr)
{
}

// Mesh implementation

DX12Mesh::DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount)
	:m_RootMeshBuffer(nullptr)
{
	// create default material

	m_RootMeshBuffer = new DX12MeshBuffer(i_InputLayout, i_VerticesBuffer, i_VerticesCount);
}

DX12Mesh::DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount)
	:m_RootMeshBuffer(nullptr)
{
	m_RootMeshBuffer = new DX12MeshBuffer(i_InputLayout, i_VerticesBuffer, i_VerticesCount, i_IndexBuffer, i_IndexCount);
}

DX12Mesh::~DX12Mesh()
{
	if (m_RootMeshBuffer != nullptr)
	{
		delete m_RootMeshBuffer;
	}
	
	// delete submeshes
	for (size_t i = 0; i < m_SubMeshBuffer.size(); ++i)
	{
		delete m_SubMeshBuffer[i];
	}
}

bool DX12Mesh::HaveSubMeshes() const
{
	return (m_SubMeshBuffer.size() != 0);
}

UINT DX12Mesh::SubMeshesCount() const
{
	return (UINT)(m_SubMeshBuffer.size());
}

DX12Material::DX12MaterialDesc DX12Mesh::GetMaterial(size_t i_SubMeshId)
{
	if (i_SubMeshId < m_SubMeshBuffer.size())
	{
		return m_SubMeshBuffer[i_SubMeshId]->GetDefaultMaterialDesc();
	}

	// return default desc for material (pink one)
	DX12Material::DX12MaterialDesc desc;
	return desc;
}

DX12Material::DX12MaterialDesc DX12Mesh::GetMaterial()
{
	return m_RootMeshBuffer->GetDefaultMaterialDesc();
}

const DX12MeshBuffer * DX12Mesh::GetRootMesh() const
{
	return m_RootMeshBuffer;
}

const DX12MeshBuffer* DX12Mesh::GetSubMeshes(size_t i_Index) const
{
	if (i_Index < m_SubMeshBuffer.size())
	{
		return m_SubMeshBuffer[i_Index];
	}
	
	return nullptr;
}