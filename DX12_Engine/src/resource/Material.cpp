#include "Material.h"

#include "engine/Debug.h"
#include "engine/Engine.h"
#include "DX12Material.h"
#include "resource/DX12ResourceManager.h"

DX12Material * Material::GetDX12Material() const
{
	return nullptr;
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
	
}

void Material::LoadFromData(const void * i_Data)
{
	MaterialData * data = (MaterialData*)i_Data;
	DX12Material::DX12MaterialData * mData = new DX12Material::DX12MaterialData;

	// fill resource data
	mData->Name = data->Name;
	mData->Filepath = data->Filepath;

	// material data
	mData->Ka = data->Ka;
	mData->Kd = data->Kd;
	mData->Ke = data->Ke;
	mData->Ks = data->Ks;

	// To do : 
	mData->map_Ka = mData->map_Kd = mData->map_Ks = false;

	m_Material = Engine::GetInstance().GetRenderResourceManager()->PushMaterial(&mData);

	ASSERT(m_Material != nullptr);
}
