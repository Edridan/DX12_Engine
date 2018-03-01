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

	mesh->PreloadData(i_Data);

	// setup data
	newResource.Resource = mesh;
	newResource.Data = i_Data;

	ASSERT(newResource.Data && newResource.Resource);

	// the resource is ready to be pushed on the GPU
	m_ResourceQueue.push_back(newResource);

	return mesh;
}

DX12Material * DX12ResourceManager::PushMaterial(void * i_Data)
{
	// create the resource
	ResourceData newResource;
	DX12Material * material = new DX12Material;

	material->PreloadData(i_Data);

	// setup data
	newResource.Resource	= material;
	newResource.Data		= i_Data;

	ASSERT(newResource.Data && newResource.Resource);

	// the resource is ready to be pushed on the GPU
	m_ResourceQueue.push_back(newResource);

	return material;
}

DX12Texture * DX12ResourceManager::PushTexture(void * i_Data)
{
	// create the resource
	ResourceData newResource;
	DX12Texture * texture = new DX12Texture;

	texture->PreloadData(i_Data);

	// setup data
	newResource.Resource	= texture;
	newResource.Data		= i_Data;

	ASSERT(newResource.Data && newResource.Resource);

	// the resource is ready to be pushed on the GPU
	m_ResourceQueue.push_back(newResource);

	return texture;
}

DX12ResourceManager::DX12ResourceManager()
{
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	DX12_ASSERT(device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_CommandQueue))); // create the command queue

	DX12Context::ContextDesc desc;

	desc.CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.FencesFlags = D3D12_FENCE_FLAG_NONE;
	desc.Name = L"Upload Resouces";

	m_CopyContext = new DX12Context(desc);
	m_CopyContext->GetCommandList()->Close();

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	ASSERT(m_FenceEvent != nullptr);
}

DX12ResourceManager::~DX12ResourceManager()
{
	SAFE_RELEASE(m_CommandQueue);

	// clear resource queue
	m_ResourceQueue.clear();
}

void DX12ResourceManager::PushResourceOnGPUWithWait()
{
	// if we don't have any resource to load
	if (m_ResourceQueue.size() == 0)		return;

	// initialize context
	m_CopyContext->ResetContext();
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	for (size_t i = 0; i < m_ResourceQueue.size(); ++i)
	{
		ResourceData & data = m_ResourceQueue[i];
		data.Resource->LoadFromData(data.Data, m_CopyContext->GetCommandList(), device);

		// resource veryfication
		ASSERT(data.Resource->GetFilepath() != "");
		ASSERT(data.Resource->GetName() != "");
	}

	m_CopyContext->GetCommandList()->Close();

	// push the command list on the GPU
	// create an array of command lists (only one command list here)
	ID3D12CommandList* deferredCommandList[] = { m_CopyContext->GetCommandList() };
	UINT64 fenceValue		= m_CopyContext->GetFenceValue();
	ID3D12Fence * fence		= m_CopyContext->GetFence();

	// execute the array of command lists
	m_CommandQueue->ExecuteCommandLists(_countof(deferredCommandList), deferredCommandList);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the m_Fences value will be set to "m_FenceValue" from the GPU since the command
	// queue is being executed on the GPU
	DX12_ASSERT(m_CommandQueue->Signal(fence, fenceValue));

	// we have the m_Fences create an event which is signaled once the m_Fences's current value is "m_FenceValue"
	DX12_ASSERT(fence->SetEventOnCompletion(fenceValue, m_FenceEvent));

	// we wait for the deferred context to be executed by the GPU
	WaitForSingleObject(m_FenceEvent, INFINITE);

	// callbacks for resources to load
	for (size_t i = 0; i < m_ResourceQueue.size(); ++i)
	{
		// callback to finish loadings
		m_ResourceQueue[i].Resource->FinishLoading();
	}

	m_ResourceQueue.clear();
}

void DX12ResourceManager::PushResourceOnGPU()
{
}
