#include "DX12ResourceManager.h"

// include
#include "DX12Resource.h"
#include "resource/DX12Mesh.h"
#include "resource/DX12Texture.h"
#include "resource/DX12Material.h"

#include "dx12/DX12Context.h"
#include "dx12/DX12RenderEngine.h"

DX12Mesh * DX12ResourceManager::PushMesh(void * i_Data)
{
	// create the resource
	ResourceData newResource;
	DX12Mesh * mesh = new DX12Mesh;

	// setup data
	newResource.IsFile = false;
	newResource.Resource = mesh;
	newResource.Data = i_Data;

	ASSERT(newResource.Data && newResource.Resource);

	// the resource is ready to be pushed on the GPU
	m_ResourceQueue.push_back(newResource);

	return mesh;
}

DX12Material * DX12ResourceManager::PushMaterial(void * i_Data)
{
	return nullptr;
}

DX12Texture * DX12ResourceManager::PushTexture(void * i_Data)
{
	return nullptr;
}

DX12Material * DX12ResourceManager::PushMaterial(std::string & i_File)
{
	return nullptr;
}

DX12Texture * DX12ResourceManager::PushTexture(std::string & i_File)
{
	return nullptr;
}

DX12ResourceManager::DX12ResourceManager()
{

}

DX12ResourceManager::~DX12ResourceManager()
{
}

void DX12ResourceManager::PushResourceOnGPUWithWait()
{
	// if we don't have any resource to load
	if (m_ResourceQueue.size() == 0)		return;

	m_CopyContext->ResetContext();
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	for (size_t i = 0; i < m_ResourceQueue.size(); ++i)
	{
		ResourceData & data = m_ResourceQueue[i];

		if (data.IsFile)
			data.Resource->LoadFromFile((char*)data.Data, m_CopyContext->GetCommandList(), device);
		else
			data.Resource->LoadFromData(data.Data, m_CopyContext->GetCommandList(), device);

		// resource veryfication
		ASSERT(data.Resource->GetFilepath() != "");
		ASSERT(data.Resource->GetName() != "");
	}

	// push the command list on the GPU

}

void DX12ResourceManager::PushResourceOnGPU()
{
}
