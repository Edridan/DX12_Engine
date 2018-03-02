#include "DX12Light.h"

#include "DX12RootSignature.h"
#include "DX12PipelineState.h"
#include "resource/DX12Mesh.h"

DX12Light::PSO		DX12Light::s_PipelineState[ELightType::eCount];
DX12Mesh *			DX12Light::s_RectMesh = nullptr;

DX12Light::DX12Light()
{
}

DX12Light::~DX12Light()
{
}

DX12Light::ELightType DX12Light::GetType() const
{
	return ELightType();
}

void DX12Light::SetType(ELightType i_Type)
{
	m_Type = i_Type;
}

void DX12Light::PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const
{
	// setup the current step pass
	i_CommandList->SetGraphicsRootSignature(s_PipelineState[m_Type].RootSignature->GetRootSignature());
	i_CommandList->SetPipelineState(s_PipelineState[m_Type].PipelineState->GetPipelineState());
}

void DX12Light::PushLightDataToConstantBuffer() const
{
	if (m_Type == ePointLight)
	{

	}
}

void DX12Light::Render(ID3D12GraphicsCommandList * i_CommandList) const
{
	

	s_RectMesh->PushOnCommandList(i_CommandList);
}

/*static*/void DX12Light::SetupPipelineStateObjects(PSO * i_PipelineStateObjects)
{

}
