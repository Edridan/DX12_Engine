#include "DX12Context.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12RenderEngine.h"
#include "engine/Debug.h"

#include <string.h>


DX12Context::DX12Context(const ContextDesc & i_Desc)
{
	DX12RenderEngine & render	= DX12RenderEngine::GetInstance();
	m_Device					= render.GetDevice();

	m_FrameCount = render.GetFrameBufferCount();

	// initialize context
	m_FrameIndex = 0u;
	// create resources
	m_FencesValues = new UINT64[m_FrameCount];
	m_Fences = new ID3D12Fence*[m_FrameCount];

	// command allocator
	m_CommandAllocator = new ID3D12CommandAllocator*[m_FrameCount];


	// create the fences
	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		// create command allocator
		DX12_ASSERT(m_Device->CreateCommandAllocator(i_Desc.CommandListType, IID_PPV_ARGS(&m_CommandAllocator[i])));

		// Add name to the command buffer
		std::wstring commandName = i_Desc.Name + L" Command Allocator ";
		wchar_t buffer[8u];
		_itow_s(i, buffer, 10);
		commandName.append(buffer);

		m_CommandAllocator[i]->SetName(commandName.c_str());

		// initialize fences
		DX12_ASSERT(m_Device->CreateFence(0, i_Desc.FencesFlags, IID_PPV_ARGS(&m_Fences[i])));
		m_FencesValues[i] = 0u;
	}

	// create command list
	DX12_ASSERT(m_Device->CreateCommandList(0, i_Desc.CommandListType, m_CommandAllocator[m_FrameIndex], NULL, IID_PPV_ARGS(&m_CommandList)));

	std::wstring listName = i_Desc.Name;
	m_CommandList->SetName((i_Desc.Name + L" Command List").c_str());
}

DX12Context::~DX12Context()
{
	SAFE_RELEASE(m_CommandList);

	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		SAFE_RELEASE(m_CommandAllocator[i]);
		SAFE_RELEASE(m_Fences[i]);
	}
}

ID3D12GraphicsCommandList * DX12Context::GetCommandList() const
{
	return m_CommandList;
}

ID3D12Device * DX12Context::GetDevice() const
{
	return m_Device;
}

ID3D12Fence * DX12Context::GetFence(UINT i_Id) const
{
	ASSERT(GetIndex(i_Id) < m_FrameCount);
	return m_Fences[GetIndex(i_Id)];
}

UINT64 DX12Context::GetFenceValue(UINT i_Id) const
{
	ASSERT(GetIndex(i_Id) < m_FrameCount);
	return m_FencesValues[GetIndex(i_Id)];
}

void DX12Context::IncrementFenceValue(UINT i_Id)
{
	++m_FencesValues[GetIndex(i_Id)];
}

void DX12Context::SetFenceValue(UINT64 i_Val, UINT i_Id)
{
	m_FencesValues[GetIndex(i_Id)] = i_Val;
}

void DX12Context::ResetContext()
{
	// update frame index
	m_FrameIndex = DX12RenderEngine::GetInstance().GetFrameIndex();

	// prepare the context to render (setup the render target, update the commandlist...)
	DX12_ASSERT(m_CommandAllocator[m_FrameIndex]->Reset());
	DX12_ASSERT(m_CommandList->Reset(m_CommandAllocator[m_FrameIndex], nullptr));
}

inline UINT DX12Context::GetIndex(UINT i_Id) const
{
	return (i_Id == (UINT)-1) ?  m_FrameIndex : i_Id;
}
