#include "ResourcesManager.h"

#include <locale>
#include <codecvt>
#include <algorithm>

#include "dx12/DX12Mesh.h"
#include "dx12/DX12Texture.h"

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

		std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
		bool isPrimitive = false;
		DX12Mesh::EPrimitiveMesh primitive;

		// generate primitive mesh if necessary
		if (filepath == "plane")
		{
			primitive = DX12Mesh::ePlane;
			isPrimitive = true;
		}
		else if (filepath == "triangle")
		{
			primitive = DX12Mesh::eTriangle;
			isPrimitive = true;
		}
		else if (filepath == "cube")
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
			PRINT_DEBUG("Mesh \"%s\" loaded", filepath.c_str());
			m_Meshes[i_Filepath] = mesh;
		}
	}

	return mesh;
}

DX12Texture * ResourcesManager::GetTexture(const wchar_t * i_Filepath)
{
	DX12Texture * texture = m_Textures[i_Filepath];

	// load the texture if she is not loaded for now
	if (texture == nullptr)
	{
		texture = new DX12Texture(i_Filepath);

		if (!texture->IsLoaded())
		{
			PRINT_DEBUG("Unable to load texture %S", i_Filepath);
			return m_Textures[ERROR_ID];
		}

		PRINT_DEBUG("Texture \"%S\" loaded",i_Filepath);
		m_Textures[i_Filepath] = texture;
	}

	return texture;
}

void ResourcesManager::CleanUpResources()
{
	// delete all resources
	CleanResourcesMap<DX12Mesh*>(m_Meshes);
	CleanResourcesMap<DX12Texture*>(m_Textures);
}

std::map<const std::wstring, DX12Mesh*>::iterator ResourcesManager::GetBeginMesh()
{
	return m_Meshes.begin();
}

std::map<const std::wstring, DX12Mesh*>::iterator ResourcesManager::GetEndMesh()
{
	return m_Meshes.end();
}

std::map<const std::wstring, DX12Texture*>::iterator ResourcesManager::GetBeginTexture()
{
	return m_Textures.begin();
}

std::map<const std::wstring, DX12Texture*>::iterator ResourcesManager::GetEndTexture()
{
	return m_Textures.end();
}

bool ResourcesManager::LoadErrorTexture(const wchar_t * i_Filepath)
{
	DX12Texture * texture = m_Textures[ERROR_ID];

	if (!texture)
	{
		texture = new DX12Texture(i_Filepath);

		if (!texture->IsLoaded())
		{
			return false;
		}

		m_Textures[ERROR_ID] = texture;
	}

	return true;
}
