#include "ResourceManager.h"

// include
#include "resource/Mesh.h"
#include "resource/Material.h"
#include "resource/Texture.h"

Mesh * ResourceManager::LoadMesh(const std::string & i_File)
{
	Mesh * mesh = m_Meshes[i_File];

	if (mesh == nullptr)
	{
		// load the mesh with the file
		mesh = new Mesh;
		mesh->LoadFromFile(i_File);

		if (mesh->IsLoaded())
		{
			m_Meshes[i_File]				= mesh;	// update the container
			m_MeshesId[mesh->GetId()]		= mesh;
			m_AllResources[mesh->GetId()]	= mesh;
		}
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

Material * ResourceManager::LoadMaterial(const std::string & i_File)
{
	Material * material = m_Materials[i_File];

	if (material == nullptr)
	{
		// load the material
		material = new Material;
		material->LoadFromFile(i_File);

		if (material->IsLoaded())
		{
			m_Materials[i_File]					= material;
			m_MaterialsId[material->GetId()]	= material;
			m_AllResources[material->GetId()]	= material;
		}
		else
		{
			ASSERT_ERROR("Unable to load material %s", i_File.c_str());
			delete material;
			material = nullptr;
		}
	}

	return material;
}

Texture * ResourceManager::LoadTexture(const std::string & i_File)
{
	Texture * texture = m_Textures[i_File];

	if (texture == nullptr)
	{
		// load the texture
		texture = new Texture;
		texture->LoadFromFile(i_File);

		if (texture->IsLoaded())
		{
			m_Textures[i_File]					= texture;
			m_TexturesId[texture->GetId()]		= texture;
			m_AllResources[texture->GetId()]	= texture;
		}
		else
		{
			ASSERT_ERROR("Unable to load texture %s", i_File.c_str());
			delete texture;
			texture = nullptr;
		}
	}

	return texture;
}

Material * ResourceManager::LoadMaterialWithData(const void * i_Data)
{
	Material * material = new Material;
	material->LoadFromData(i_Data);
	
	if (material->IsLoaded())
	{
		m_AllResources[material->GetId()] = material;
		m_MaterialsId[material->GetId()] = material;
	}
	else
	{
		ASSERT_ERROR("Unable to load material");
		delete material;
		material = nullptr;
	}

	return material;
}

Mesh * ResourceManager::GetMeshByName(const std::string & i_Name) const
{
	auto itr = m_MeshesId.begin();
	while (itr != m_MeshesId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Material * ResourceManager::GetMaterialByName(const std::string & i_Name) const
{
	auto itr = m_MaterialsId.begin();
	while (itr != m_MaterialsId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Texture * ResourceManager::GetTextureByName(const std::string & i_Name) const
{
	auto itr = m_TexturesId.begin();
	while (itr != m_TexturesId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

void ResourceManager::GetAllMeshByName(std::vector<Mesh*> o_Out, const std::string & i_Name) const
{
	auto itr = m_MeshesId.begin();
	while (itr != m_MeshesId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

void ResourceManager::GetAllTexturesByName(std::vector<Texture*> o_Out, const std::string & i_Name) const
{
	auto itr = m_TexturesId.begin();
	while (itr != m_TexturesId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

void ResourceManager::GetAllMaterialsByName(std::vector<Material*> o_Out, const std::string & i_Name) const
{
	auto itr = m_MaterialsId.begin();
	while (itr != m_MaterialsId.end())
	{
		if ((*itr).second->GetName() == i_Name)
			o_Out.push_back((*itr).second);
		++itr;
	}
}

Mesh * ResourceManager::GetMeshByFilename(const std::string & i_Filename) const
{
	auto itr = m_Meshes.find(i_Filename);
	if (itr != m_Meshes.end())
		return (*itr).second;

	return nullptr;
}

Material * ResourceManager::GetMaterialByFilename(const std::string & i_Filename) const
{
	auto itr = m_Materials.find(i_Filename);
	if (itr != m_Materials.end())
		return (*itr).second;

	return nullptr;
}

Texture * ResourceManager::GetTextureByFilename(const std::string & i_Filename) const
{
	auto itr = m_Textures.find(i_Filename);
	if (itr != m_Textures.end())
		return (*itr).second;

	return nullptr;
}

Mesh * ResourceManager::GetGeneratedMeshByFilename(const std::string & i_Filename)
{
	auto itr = m_MeshesId.begin();
	while (itr != m_MeshesId.end())
	{
		if ((*itr).second->GetFilepath() == i_Filename)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Material * ResourceManager::GetGeneratedMaterialByFilename(const std::string & i_Filename)
{
	auto itr = m_MaterialsId.begin();
	while (itr != m_MaterialsId.end())
	{
		if ((*itr).second->GetFilepath() == i_Filename)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

Texture * ResourceManager::GetGeneratedTextureByFilename(const std::string & i_Filename)
{
	auto itr = m_TexturesId.begin();
	while (itr != m_TexturesId.end())
	{
		if ((*itr).second->GetFilepath() == i_Filename)
			return (*itr).second;
		++itr;
	}

	return nullptr;
}

FORCEINLINE Mesh * ResourceManager::GetMeshById(UINT64 i_Id) const
{
	auto itr = m_MeshesId.find(i_Id);
	if (itr != m_MeshesId.end())
	{
		return (*itr).second;
	}

	return nullptr;
}

FORCEINLINE Texture * ResourceManager::GetTextureById(UINT64 i_Id) const
{
	auto itr = m_TexturesId.find(i_Id);
	if (itr != m_TexturesId.end())
	{
		return (*itr).second;
	}

	return nullptr;
}

FORCEINLINE Material * ResourceManager::GetMaterialById(UINT64 i_Id) const
{
	auto itr = m_MaterialsId.find(i_Id);
	if (itr != m_MaterialsId.end())
	{
		return (*itr).second;
	}

	return nullptr;
}

size_t ResourceManager::GetResourceCount(EResourceType i_ResourceType) const
{
	switch (i_ResourceType)
	{
	case ResourceManager::eAll:			return m_AllResources.size();
	case ResourceManager::eMesh:		return m_MeshesId.size();
	case ResourceManager::eTexture:		return m_TexturesId.size();
	case ResourceManager::eMaterial:	return m_MaterialsId.size();
	default:							return 0;
	}
}

Resource * ResourceManager::GetResourceById(UINT64 i_Id) const
{
	// this will search by resource
	auto itr = m_AllResources.find(i_Id);
	if (itr != m_AllResources.end())
	{
		return (*itr).second;
	}

	return nullptr;
}

Mesh * ResourceManager::GetMeshByIndex(size_t i_Index) const
{
	if (i_Index > m_Meshes.size())	return nullptr;

	auto itr = m_MeshesId.begin();
	while (itr != m_MeshesId.end())
	{
		if (i_Index == 0)		return (*itr).second;
		++itr;
		--i_Index;
	}

	return nullptr;
}

Material * ResourceManager::GetMaterialByIndex(size_t i_Index) const
{
	if (i_Index > m_MaterialsId.size())	return nullptr;

	auto itr = m_MaterialsId.begin();
	while (itr != m_MaterialsId.end())
	{
		if (i_Index == 0)		return (*itr).second;
		++itr;
		--i_Index;
	}

	return nullptr;
}

Texture * ResourceManager::GetTextureByIndex(size_t i_Index) const
{
	if (i_Index > m_TexturesId.size())	return nullptr;

	auto itr = m_TexturesId.begin();
	while (itr != m_TexturesId.end())
	{
		if (i_Index == 0)		return (*itr).second;
		++itr;
		--i_Index;
	}

	return nullptr;
}

bool ResourceManager::ReleaseResource(const UINT64 i_Id)
{
	TO_DO;
	return false;
}

void ResourceManager::CleanUnusedResources()
{
	TO_DO;
}

void ResourceManager::CleanResources()
{
	auto itr = m_AllResources.begin();
	while (itr != m_AllResources.end())
	{
		// unload each resources
		(*itr).second->Unload();
		delete (*itr).second;
		++itr;
	}
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}
