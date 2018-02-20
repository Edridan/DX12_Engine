#include "DX12Debug.h"

#ifdef DX12_DEBUG

#include "engine/Debug.h"

// singleton management
DX12Debug * DX12Debug::s_Instance = nullptr;

DX12Debug & DX12Debug::GetInstance()
{
	ASSERT(s_Instance != nullptr);
	return *s_Instance;
}

void DX12Debug::Create(const DX12DebugDesc & i_Setup)
{
	ASSERT(s_Instance == nullptr);
	s_Instance = new DX12Debug(i_Setup);
}

void DX12Debug::Delete()
{
	ASSERT(s_Instance != nullptr);
	delete s_Instance;
}

void DX12Debug::DrawDebugBox(const DirectX::XMFLOAT3 & i_Position, const Transform & i_Transform, Color & i_Color)
{
	// To do : implement
}

void DX12Debug::DrawDebugLine(const DirectX::XMFLOAT3 & i_Start, const DirectX::XMFLOAT3 & i_End, Color & i_Color)
{
	// To do : Implement
}

void DX12Debug::SetEnabled(bool i_Enabled)
{
	m_Enabled = i_Enabled;
}

bool DX12Debug::IsEnabled() const
{
	return m_Enabled;
}

DX12Debug::DX12Debug(const DX12DebugDesc & i_Setup)
	// debug options
	:m_Enabled(i_Setup.EnabledByDefault)
	// render targets
	,m_BackBuffer(i_Setup.BackBuffer)
	,m_SpecularRT(i_Setup.SpecularRT)
	,m_PositionRT(i_Setup.PositionRT)
	,m_DiffuseRT(i_Setup.DiffuseRT)
	,m_NormalRT(i_Setup.NormalRT)

{
	// generate all debug pipeline
	//ASSERT(m_BackBuffer != nullptr && m_NormalRT != nullptr && m_SpecularRT != nullptr /* && m_PositionRT != nullptr && m_DepthRT != nullptr */);


}

DX12Debug::~DX12Debug()
{
}

void DX12Debug::DrawDebugGBuffer(ID3D12GraphicsCommandList * i_CommandList) const
{
	if (!m_Enabled)		return;
}

#endif /* DX12_DEBUG */