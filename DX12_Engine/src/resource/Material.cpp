#include "Material.h"

#include "engine/Debug.h"
#include "engine/Engine.h"
#include "engine/ResourceManager.h"
#include "resource/DX12Material.h"
#include "resource/DX12ResourceManager.h"

// lib
#include "../lib/tinyobjloader/tiny_obj_loader.h"

DX12Material * Material::GetDX12Material(const std::string & i_Name) const
{
	for (size_t i = 0; i < m_Materials.size(); ++i)
	{
		if (m_Materials[i]->GetName() == i_Name)
			return m_Materials[i];
	}

	// no material found
	return nullptr;
}

DX12Material * Material::GetDX12Material(size_t i_Index /* = 0 */) const
{
	if (i_Index < m_Materials.size())	
		return m_Materials[i_Index];	

	// outside the buffer
	return nullptr;
}

size_t Material::GetMaterialCount() const
{
	return m_Materials.size();
}


Material::Material()
	:Resource()
{
}

Material::~Material()
{
}

void Material::LoadFromFile(const std::string & i_Filepath)
{
	// resource manage to load some other mData if needed (textures)
	ResourceManager * resourceManager = Engine::GetInstance().GetResourceManager();

	// generate the file reader to load the material with tinyobj loader
	tinyobj::MaterialFileReader fileReader(i_Filepath);
	std::vector<tinyobj::material_t> materials;
	std::string error;

	// fill global information
	m_Filepath	= i_Filepath;
	m_Name		= i_Filepath;

	fileReader("", &materials, nullptr, &error);

	if (materials.size() == 0)
	{
		ASSERT_ERROR(error.c_str());
		return;
	}

	for (size_t i = 0; i < materials.size(); ++i)
	{
		// create mData (this will be deleted by DX12Resources)
		DX12Material::DX12MaterialData * mData = new DX12Material::DX12MaterialData;
		tinyobj::material_t mat = materials[i];

		// fill global mData resource
		mData->Filepath	= m_Filepath;
		mData->Name		= mat.name;

		// retreive other mData
		mData->Ka = mat.ambient;
		mData->Kd = mat.diffuse;
		mData->Ke = mat.emission;
		mData->Ks = mat.specular;

		DX12Material * material = Engine::GetInstance().GetRenderResourceManager()->PushMaterial(mData);

		if (material == nullptr)
		{
			ASSERT_ERROR("Unable to load DX12Material correctly");
			DEBUG_BREAK;
		}
		else
		{
			m_Materials.push_back(material);
		}
	}

	NotifyFinishLoad();
}

void Material::LoadFromData(const void * i_Data)
{
	MaterialData * data = (MaterialData*)i_Data;

	for (size_t i = 0; i < data->MaterialCount; ++i)
	{
		// create material data for each material to be created (will be deleted by the DX12Material when finish loading)
		DX12Material::DX12MaterialData * mData = new DX12Material::DX12MaterialData;
		const MaterialSpec & mDesc = data->Materials[i];

		// fill resource mData
		mData->Name		= mDesc.Name;
		mData->Filepath = data->Filepath;

		// material mData
		mData->Ka = mDesc.Ka;
		mData->Kd = mDesc.Kd;
		mData->Ke = mDesc.Ke;
		mData->Ks = mDesc.Ks;

		// To do : 
		mData->map_Ka = mData->map_Kd = mData->map_Ks = false;

		DX12Material * material = Engine::GetInstance().GetRenderResourceManager()->PushMaterial(mData);

		if (material == nullptr)
		{
			ASSERT_ERROR("Unable to load DX12Material correctly");
			DEBUG_BREAK;
			return;
		}

		m_Materials.push_back(material);
	}

	// notify the load is finished for this texture
	NotifyFinishLoad();
}
