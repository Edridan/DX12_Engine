#include "DX12Mesh.h"
#include "DX12RenderEngine.h"
#include "DX12MeshBuffer.h"

#include "d3dx12.h"
#include "Clock.h"

#include <sstream>
#include <algorithm>
#include <codecvt>

// mesh loader
#include "lib/tinyobjloader/tiny_obj_loader.h"

// default input element using the input color
const D3D12_INPUT_ELEMENT_DESC DX12Mesh::s_PrimitiveElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (sizeof(float) * 3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

// fill out the default input layout description structure
D3D12_INPUT_LAYOUT_DESC DX12Mesh::s_PrimitiveLayoutDesc =
{
	s_PrimitiveElementDesc,
	sizeof(DX12Mesh::s_PrimitiveElementDesc) / sizeof(D3D12_INPUT_ELEMENT_DESC)
};
//
//// default input element using the input normal
//const D3D12_INPUT_ELEMENT_DESC DX12Mesh::s_DefaultInputNormal[] =
//{
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (sizeof(float) * 3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//};
//
//// fill out the default input layout description structure
//D3D12_INPUT_LAYOUT_DESC DX12Mesh::s_DefaultInputNormalLayout =
//{
//	s_DefaultInputNormal,
//	sizeof(DX12Mesh::s_DefaultInputNormal) / sizeof(D3D12_INPUT_ELEMENT_DESC)
//};


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
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
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
		DX12Mesh::EElementFlags::eHaveNormal |
		DX12Mesh::EElementFlags::eHaveTexcoord;

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

DX12Mesh * DX12Mesh::LoadMeshObj(const char * i_Filename, const char * i_MaterialFolder)
{
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	material;

	std::string error;
	Clock timer;
	timer.Restart();

	// load the model
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &material, &error, i_Filename, i_MaterialFolder);
	float loadTime = timer.GetElaspedTime().ToSeconds();

#ifdef _DEBUG
	// display debug message
	std::replace(error.begin(), error.end(), '\n', ' ');
	std::ostringstream stream;
	stream << i_Filename << " : load time (" << loadTime << ")"
		<< std::endl << (ret ? "[No Error]" : error) << std::endl;
	
	std::string message(stream.str());
	PRINT_DEBUG(message.c_str());

#else
	if (!ret)
	{
		// display error message
		std::replace(error.begin(), error.end(), '\n', ' ');
		DX12RenderEngine::GetInstance().PrintMessage(error.c_str());
		return nullptr;
	}
#endif

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
		UINT64 flags = DX12Mesh::EElementFlags::eNone;

		// compute stride and flags for the mesh
		if (origin.normal_index != -1)
		{
			flags |= DX12Mesh::EElementFlags::eHaveNormal;
			stride += 3;
		}
		if (origin.texcoord_index != -1)
		{
			flags |= DX12Mesh::EElementFlags::eHaveTexcoord;
			stride += 2;
		}
		if (attrib.colors.size() != 0 && (attrib.colors[0] != -1.0f) /* verify that color is really present in the model */)
		{
			flags |= DX12Mesh::EElementFlags::eHaveColor;
			stride += 3;
		}

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
			if (flags & DX12Mesh::EElementFlags::eHaveNormal)
			{
				// copy the normal
				memcpy(bufferItr, &attrib.normals[3 * index.normal_index], 3 * sizeof(FLOAT));
				bufferItr += 3;
			}
			if (flags & DX12Mesh::EElementFlags::eHaveTexcoord)
			{
				// copy the uv
				memcpy(bufferItr, &attrib.texcoords[2 * index.texcoord_index], 2 * sizeof(FLOAT));
				bufferItr += 2;
			}
			if (flags & DX12Mesh::EElementFlags::eHaveColor)
			{
				// copy the color
				memcpy(bufferItr, &attrib.colors[3 * index.vertex_index], 3 * sizeof(FLOAT));
				bufferItr += 3;
			}
		}

		// Get the name of the shape
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
		std::wstring wname = stringConverter.from_bytes(shape->name);

		D3D12_INPUT_LAYOUT_DESC layout;
		CreateInputLayoutFromFlags(layout, flags);

		// create mesh and initialize it
		mesh->m_SubMeshBuffer.push_back(new DX12MeshBuffer(
			layout,	// the generated layout depending on the flags
			reinterpret_cast<BYTE*>(verticeBuffer),
			(UINT)verticeCount,
			wname.c_str()
		));
		
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
	m_RootMeshBuffer = new DX12MeshBuffer(i_InputLayout, i_VerticesBuffer, i_VerticesCount);
}

DX12Mesh::DX12Mesh(D3D12_INPUT_LAYOUT_DESC i_InputLayout, BYTE * i_VerticesBuffer, UINT i_VerticesCount, DWORD * i_IndexBuffer, UINT i_IndexCount)
	:m_RootMeshBuffer(nullptr)
{
	m_RootMeshBuffer = new DX12MeshBuffer(i_InputLayout, i_VerticesBuffer, i_VerticesCount, i_IndexBuffer, i_IndexCount);
}

DX12Mesh::~DX12Mesh()
{
}

bool DX12Mesh::HaveSubMeshes() const
{
	return (m_SubMeshBuffer.size() != 0);
}

UINT DX12Mesh::SubMeshesCount() const
{
	return (UINT)(m_SubMeshBuffer.size());
}

const DX12MeshBuffer * DX12Mesh::GetRootMesh() const
{
	return m_RootMeshBuffer;
}

const std::vector<DX12MeshBuffer*>& DX12Mesh::GetSubMeshes() const
{
	return m_SubMeshBuffer;
}

UINT DX12Mesh::GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout)
{
	// size of one element
	UINT elementSize = 0;

	// go into the structure and get the size of the buffer
	for (UINT i = 0; i < i_InputLayout.NumElements; ++i)
	{
		D3D12_INPUT_ELEMENT_DESC element = i_InputLayout.pInputElementDescs[i];

		if ((element.AlignedByteOffset != D3D12_APPEND_ALIGNED_ELEMENT) && (element.AlignedByteOffset != elementSize))
		{
			elementSize = element.AlignedByteOffset;
		}

		// update the size of the current buffer
		elementSize += SizeOfFormatElement(element.Format);
	}

	return elementSize;
}

UINT64 DX12Mesh::CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout)
{
	UINT64 flags = EElementFlags::eNone;
	// go into the structure and get the size of the buffer
	for (UINT i = 0; i < i_InputLayout.NumElements; ++i)
	{
		D3D12_INPUT_ELEMENT_DESC element = i_InputLayout.pInputElementDescs[i];

		if (strcmp(element.SemanticName, "TEXCOORD") == 0)	flags |= EElementFlags::eHaveTexcoord;
		else if (strcmp(element.SemanticName, "COLOR") == 0)	flags |= EElementFlags::eHaveColor;
		else if (strcmp(element.SemanticName, "NORMAL") == 0)	flags |= EElementFlags::eHaveNormal;
	}

	return flags;
}

void DX12Mesh::CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags)
{
	// compute size of elements
	D3D12_INPUT_ELEMENT_DESC * elements;
	UINT size = 1;
	UINT index = 0;
	UINT offset = 0;

	if (i_Flags & EElementFlags::eHaveColor)			++size;
	if (i_Flags & EElementFlags::eHaveTexcoord)			++size;
	if (i_Flags & EElementFlags::eHaveNormal)			++size;

	elements = new D3D12_INPUT_ELEMENT_DESC[size];
	o_InputLayout.NumElements = size;
	o_InputLayout.pInputElementDescs = elements;

	// layout order definition depending flags : 
	// 1 - Position
	// 2 - Normal
	// 3 - Texcoord
	// 4 - Color

	// default position
	elements[index++] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	offset += 3 * sizeof(float);

	if (i_Flags & EElementFlags::eHaveNormal)
	{
		elements[index++] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 3 * sizeof(float);
	}
	if (i_Flags & EElementFlags::eHaveTexcoord)
	{
		elements[index++] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 2 * sizeof(float);
	}
	if (i_Flags & EElementFlags::eHaveColor)
	{
		elements[index++] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 3 * sizeof(float);
	}
}