#include "ResourcesManager.h"

#include <locale>
#include <codecvt>
#include "dx12/DX12Mesh.h"

ResourcesManager::ResourcesManager()
{
}

ResourcesManager::~ResourcesManager()
{
}

DX12Mesh * ResourcesManager::GetMesh(const wchar_t * i_Filepath)
{
	DX12Mesh * mesh = m_Meshes[i_Filepath];

	if (mesh == nullptr)
	{
		std::wstring fp = i_Filepath;
		std::string filepath(fp.begin(), fp.end());
		bool isPrimitive = false;
		DX12Mesh::EPrimitiveMesh primitive;

		// generate primitive mesh if necessary
		if (filepath == "Plane")
		{
			primitive = DX12Mesh::ePlane;
			isPrimitive = true;
		}
		else if (filepath == "Triangle")
		{
			primitive = DX12Mesh::eTriangle;
			isPrimitive = true;
		}
		else if (filepath == "Cube")
		{
			primitive = DX12Mesh::eCube;
			isPrimitive = true;
		}


		// load the mesh
		if (!isPrimitive)
		{
			mesh = DX12Mesh::LoadMeshObj(filepath.c_str());
		}
		else
		{
			mesh = DX12Mesh::GeneratePrimitiveMesh(primitive);
		}

		if (mesh != nullptr)
		{
			m_Meshes[i_Filepath] = mesh;
		}
	}

	return mesh;
}

DX12Font * ResourcesManager::GetFont(const wchar_t * i_Filepath)
{
	return nullptr;
}

void ResourcesManager::CleanUpResources()
{
	// delete all resources
	auto itr = m_Meshes.begin();

	while (itr != m_Meshes.end())
	{
		// delete meshes
		delete (*itr).second;
		++itr;
	}
}
