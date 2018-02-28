#include "ResourceManager.h"

// include
#include "resource/Mesh.h"
#include "resource/Material.h"
#include "resource/Texture.h"

Mesh * ResourceManager::GetMesh(const std::string & i_File)
{
	Mesh * mesh = m_Meshes[i_File];

	if (mesh == nullptr)
	{
		// load the mesh with the file
		mesh = new Mesh;
		mesh->LoadFromFile(i_File);
		if (mesh->IsLoaded())	m_Meshes[i_File] = mesh;	// update the container
		else
		{
			// the mesh is not loaded correctly : we destroy it
			ASSERT_ERROR("Unable to load mesh %s", i_File.c_str());
			delete mesh;
			mesh = nullptr;
		}
	}

	return mesh;
}

Material * ResourceManager::GetMaterial(const std::string & i_File)
{
	Material * material = m_Materials[i_File];

	if (material == nullptr)
	{
		// load the material
		material = new Material;
		material->LoadFromFile(i_File);

		if (material->IsLoaded())	m_Materials[i_File] = material;
		else
		{
			ASSERT_ERROR("Unable to load material %s", i_File.c_str());
			delete material;
			material = nullptr;
		}
	}

	return material;
}

Texture * ResourceManager::GetTexture(const std::string & i_File)
{
	Texture * texture = m_Textures[i_File];

	if (texture == nullptr)
	{
		// load the texture
		texture = new Texture;
		texture->LoadFromFile(i_File);

		if (texture->IsLoaded())		m_Textures[i_File] = texture;
		else
		{
			ASSERT_ERROR("Unable to load texture %s", i_File.c_str());
			delete texture;
			texture = nullptr;
		}
	}

	return texture;
}

Mesh * ResourceManager::GetMeshByName(const std::string & i_Name) const
{
	auto itr = m_Meshes.begin();
	while (itr != m_Meshes.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Material * ResourceManager::GetMaterialByName(const std::string & i_Name) const
{
	auto itr = m_Materials.begin();
	while (itr != m_Materials.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Texture * ResourceManager::GetTextureByName(const std::string & i_Name) const
{
	auto itr = m_Textures.begin();
	while (itr != m_Textures.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

void ResourceManager::GetAllMeshByName(std::vector<Mesh*> o_Out, const std::string & i_Name) const
{
	auto itr = m_Meshes.begin();
	while (itr != m_Meshes.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

void ResourceManager::GetAllTexturesByName(std::vector<Texture*> o_Out, const std::string & i_Name) const
{
	auto itr = m_Textures.begin();
	while (itr != m_Textures.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

void ResourceManager::GetAllMaterialsByName(std::vector<Material*> o_Out, const std::string & i_Name) const
{
	auto itr = m_Materials.begin();
	while (itr != m_Materials.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}
