#include "DX12Context.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12RenderEngine.h"
#include "engine/Debug.h"


DX12Context::DX12Context(const ContextDesc & i_Desc)
{
	// retreive framecount
	if (i_Desc.FrameCount == 0)
		m_FrameCount = DX12RenderEngine::GetInstance().GetFrameBufferCount();
	else
		m_FrameCount = i_Desc.FrameCount;

	// create resources
	m_FencesValues = new UINT64[m_FrameCount];


}

DX12Context::~DX12Context()
{
	SAFE_RELEASE(m_CommandList);

	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		SAFE_RELEASE(m_Fences[i]);
		delete m_RenderTarget[i];
	}
}

ID3D12GraphicsCommandList * DX12Context::GetCommandList() const
{
	return m_CommandList;
}

ID3D12Fence * DX12Context::GetFence(UINT i_Id) const
{
	ASSERT(i_Id < m_FrameCount);
	return m_Fences[i_Id];
}

UINT64 DX12Context::GetFenceValue(UINT i_Id) const
{
	ASSERT(i_Id < m_FrameCount);
	return m_FencesValues[i_Id];
}
