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

		// load the mesh
		mesh = DX12Mesh::LoadMeshObj(filepath.c_str());

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
