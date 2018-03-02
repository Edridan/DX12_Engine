#include "DX12Light.h"

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

void DX12Light::PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const
{
}

void DX12Light::PushDataOnConstantBuffer() const
{
}

/*static*/void DX12Light::SetupPipelineStateObjects(PSO * i_PipelineStateObjects)
{

}
