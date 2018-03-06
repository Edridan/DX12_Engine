#include "DX12Light.h"

#include "DX12RootSignature.h"
#include "DX12PipelineState.h"
#include "DX12RenderEngine.h"
#include "DX12RenderTarget.h"
#include "DX12Shader.h"
#include "resource/DX12Mesh.h"

DX12PipelineState *		DX12Light::s_PipelineState;
DX12RootSignature *		DX12Light::s_RootSignature;

DX12Light::DX12Light()
	:m_ConstantAddress(UnavailableAdressId)
{
	m_Data = {};
	// set point light by default
	SetType(ePointLight);
}

DX12Light::~DX12Light()
{
}

DX12Light::ELightType DX12Light::GetType() const
{
	return m_Type;
}

void DX12Light::SetType(ELightType i_Type)
{
	if (m_Type == i_Type)	return;

	// update some data
	m_Type = i_Type;

	if (m_Type == ePointLight)
	{
		// initialize some data
		if (m_Data.Range == 0.f)		SetRange(10.f);
	}
}

void DX12Light::SetRange(float i_Range)
{
	m_Data.Range = i_Range;
	m_NeedBufferUpdate = true;
}

void DX12Light::SetColor(const DirectX::XMFLOAT4 & i_Color)
{
	m_Data.Color = i_Color;
	m_NeedBufferUpdate = true;
}

void DX12Light::SetIntensity(float i_Intensity)
{
	m_Data.Intensity = i_Intensity;
	m_NeedBufferUpdate = true;
}

void DX12Light::SetSpotAngle(float i_SpotAngle)
{
	m_Data.SpotAngle = i_SpotAngle;
	m_NeedBufferUpdate = true;
}

void DX12Light::SetSpotCutoff(float i_Cutoff)
{
	m_Data.SpotCutoff = i_Cutoff;
	m_NeedBufferUpdate = true;
}

float DX12Light::GetRange() const
{
	return m_Data.Range;
}

DirectX::XMFLOAT4 DX12Light::GetColor() const
{
	return m_Data.Color;
}

float DX12Light::GetIntensity() const
{
	return m_Data.Intensity;
}

float DX12Light::GetSpotAngle() const
{
	return m_Data.SpotAngle;
}

float DX12Light::GetSpotCutoff() const
{
	return m_Data.SpotCutoff;
}

void DX12Light::PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const
{
	// setup the current step pass
	i_CommandList->SetGraphicsRootSignature(s_RootSignature->GetRootSignature());
	i_CommandList->SetPipelineState(s_PipelineState->GetPipelineState());
}

void DX12Light::PushLightDataToConstantBuffer() const
{
	ASSERT(m_ConstantAddress != UnavailableAdressId);
	if (m_NeedBufferUpdate)
	{

	}
}

void DX12Light::SetupPipelineStateObjects(ID3D12Device * i_Device)
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// create signatures
	s_RootSignature = new DX12RootSignature;

	// add static sampler for textures
	D3D12_STATIC_SAMPLER_DESC sampler = {};

	sampler.Filter			= D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU		= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV		= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW		= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias		= 0;
	sampler.MaxAnisotropy	= 0;
	sampler.ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor		= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD			= 0.0f;
	sampler.MaxLOD			= D3D12_FLOAT32_MAX;
	sampler.ShaderRegister	= 0;
	sampler.RegisterSpace	= 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	s_RootSignature->AddStaticSampler(sampler);	// add static sampler

	// add textures
	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[DX12RenderEngine::eRenderTargetCount];

	for (UINT i = 0; i < DX12RenderEngine::eRenderTargetCount; ++i)
	{
		descriptorTableRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
		descriptorTableRanges[i].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
		descriptorTableRanges[i].BaseShaderRegister = i; // start index of the shader registers in the range
		descriptorTableRanges[i].RegisterSpace = 0; // space 0. can usually be zero
		descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the s_RootSignature signature descriptor tables
	}

	// add render targets 
	s_RootSignature->AddDescriptorRange(&descriptorTableRanges[0], 1, D3D12_SHADER_VISIBILITY_PIXEL);	// normal texture
	s_RootSignature->AddDescriptorRange(&descriptorTableRanges[1], 1, D3D12_SHADER_VISIBILITY_PIXEL);	// diffuse
	s_RootSignature->AddDescriptorRange(&descriptorTableRanges[2], 1, D3D12_SHADER_VISIBILITY_PIXEL);	// specular
	s_RootSignature->AddDescriptorRange(&descriptorTableRanges[3], 1, D3D12_SHADER_VISIBILITY_PIXEL);	// position

	s_RootSignature->AddConstantBuffer(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

	s_RootSignature->Create(i_Device);

	// generate root signatures for each lights specs
	DX12PipelineState::PipelineStateDesc desc;

	// shader loading
	//DX12Shader * VShader		= new DX12Shader(DX12Shader::eVertex, L"src/shaders/deferred/FrameCompositorVS.hlsl");
	//DX12Shader * pointLightPS	= new DX12Shader(DX12Shader::ePixel, L"src/shaders/lights/pointlightPS.hlsl");

	//desc.InputLayout			= render.GetRectMesh()->GetInputLayoutDesc();
	//desc.RootSignature			= s_RootSignature;
	//desc.VertexShader			= VShader;
	//desc.PixelShader			= pointLightPS;
	//desc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//desc.RenderTargetCount		= 1;
	//desc.RenderTargetFormat[0]	= render.GetBackBuffer()->GetFormat();
	//desc.BlendState				= CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	//desc.DepthEnabled			= false;

	//s_PipelineState = new DX12PipelineState(desc);
}

