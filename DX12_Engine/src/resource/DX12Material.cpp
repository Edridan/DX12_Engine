#include "DX12Material.h"

#include "dx12/DX12RootSignature.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12DepthBuffer.h"
#include "dx12/DX12ConstantBuffer.h"

DX12Material::DX12Material()
	:DX12Resource()
	,m_ConstantBuffer(nullptr)
	,m_PipelineState(nullptr)
	,m_RootSignature(nullptr)
	,m_BufferAddress(UnavailableAdressId)
{
}

DX12Material::~DX12Material()
{
	Release();
}

void DX12Material::PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const
{
	// add pso and root signature to the commandlist
	i_CommandList->SetGraphicsRootSignature(m_RootSignature->GetRootSignature());
	// Setup the pipeline state
	i_CommandList->SetPipelineState(m_PipelineState->GetPipelineState());
}

void DX12Material::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList, UINT i_RootParameter /* = 2 */) const
{
	// bind a buffer on the root signature to the index needed
	// warning : this can be changed if the mesh is multiple materials rendering
	i_CommandList->SetGraphicsRootConstantBufferView(i_RootParameter, m_ConstantBuffer->GetUploadVirtualAddress(m_BufferAddress));
}

FORCEINLINE void DX12Material::UpdateConstantBuffer() const
{
	if (m_BufferAddress == UnavailableAdressId || m_ConstantBuffer == nullptr)
	{
		PRINT_DEBUG("Error unable to update the constant buffer");
		DEBUG_BREAK;
		return;
	}

	// update constant buffer on each frame (To do : do not duplicate the constant buffer for materials)
	m_ConstantBuffer->UpdateConstantBufferForEachFrame(m_BufferAddress, &m_Data, sizeof(MaterialData));
}

void DX12Material::LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device)
{
	const DX12MaterialData * data = (const DX12MaterialData*)i_Data;

	// colors
	m_Data.Ka = ColorToVec4(data->Ka);
	m_Data.Kd = ColorToVec4(data->Kd);
	m_Data.Ke = ColorToVec4(data->Ke);
	m_Data.Ks = ColorToVec4(data->Ks);

	m_Data.Ns = data->Ns;

	// To do : manage texture
	m_Data.Map_A = m_Data.Map_D = m_Data.Map_S = false;

	// upload data to the GPU
	m_ConstantBuffer = DX12RenderEngine::GetInstance().GetConstantBuffer(DX12RenderEngine::eMaterial);
	m_BufferAddress = m_ConstantBuffer->ReserveVirtualAddress();
	UpdateConstantBuffer();

	// generate pipeline state
	GenerateRootSignature(i_Device);
	GeneratePipelineState(i_Device);

	// delete the data
	delete data;
}

void DX12Material::PreloadData(const void * i_Data)
{
	const DX12MaterialData * data = (const DX12MaterialData*)i_Data;

	// informations
	m_Name = data->Name;
	m_Filepath = data->Filepath;
}

void DX12Material::Release()
{
	// release address from constant buffer
	if (m_ConstantBuffer != nullptr)
	{
		m_ConstantBuffer->ReleaseVirtualAddress(m_BufferAddress);
	}

	// release dx12 resources
	if (m_RootSignature)	delete m_RootSignature;
	if (m_PipelineState)	delete m_PipelineState;

	DX12Resource::Release();
}

FORCEINLINE void DX12Material::GenerateRootSignature(ID3D12Device * i_Device)
{
	ASSERT(m_RootSignature == nullptr);

	// create root signature
	m_RootSignature = new DX12RootSignature();
	// generate default root signature

	// constant buffer
	m_RootSignature->AddConstantBuffer(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	// b0 : transform constant
	m_RootSignature->AddConstantBuffer(1, 0, D3D12_SHADER_VISIBILITY_ALL);		// b1 : global constant
	m_RootSignature->AddConstantBuffer(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);	// b2 : material constant

	// To do : manage textures
	//D3D12_DESCRIPTOR_RANGE descriptorTableRanges[eCount];

	//for (UINT i = 0; i < eCount; ++i)
	//{
	//	descriptorTableRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
	//	descriptorTableRanges[i].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
	//	descriptorTableRanges[i].BaseShaderRegister = i; // start index of the shader registers in the range
	//	descriptorTableRanges[i].RegisterSpace = 0; // space 0. can usually be zero
	//	descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
	//}

	//m_RootSignature->AddDescriptorRange(&descriptorTableRanges[0], 1, D3D12_SHADER_VISIBILITY_PIXEL);
	//m_RootSignature->AddDescriptorRange(&descriptorTableRanges[1], 1, D3D12_SHADER_VISIBILITY_PIXEL);
	//m_RootSignature->AddDescriptorRange(&descriptorTableRanges[2], 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//// add static sampler for textures
	//D3D12_STATIC_SAMPLER_DESC sampler = {};

	//sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	//sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.MipLODBias = 0;
	//sampler.MaxAnisotropy = 0;
	//sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	//sampler.MinLOD = 0.0f;
	//sampler.MaxLOD = D3D12_FLOAT32_MAX;
	//sampler.ShaderRegister = 0;
	//sampler.RegisterSpace = 0;
	//sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//m_RootSignature->AddStaticSampler(sampler);

	m_RootSignature->Create(i_Device,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
}

FORCEINLINE void DX12Material::GeneratePipelineState(ID3D12Device * i_Device)
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	DX12Shader * PShader = new DX12Shader(DX12Shader::ePixel, L"src/shaders/deferred/GBufferPixel.hlsl");
	DX12Shader * VShader = new DX12Shader(DX12Shader::eVertex, L"src/shaders/deferred/GBufferVertex.hlsl");

	// create pipeline state object
	D3D12_INPUT_LAYOUT_DESC inputLayout;
	DX12PipelineState::CreateInputLayoutFromFlags(inputLayout, DX12PipelineState::EElementFlags::eHaveNormal | DX12PipelineState::EElementFlags::eHaveTexcoord);

	DX12PipelineState::PipelineStateDesc desc;

	desc.InputLayout = inputLayout;
	desc.RootSignature = m_RootSignature;
	desc.VertexShader = VShader;
	desc.PixelShader = PShader;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// setup render target
	desc.RenderTargetCount = DX12RenderEngine::ERenderTargetId::eRenderTargetCount;
	desc.RenderTargetFormat[0] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eNormal)->GetFormat();
	desc.RenderTargetFormat[1] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eDiffuse)->GetFormat();
	desc.RenderTargetFormat[2] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eSpecular)->GetFormat();
	desc.RenderTargetFormat[3] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::ePosition)->GetFormat();

	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blend state.
	desc.DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	desc.DepthStencilFormat = render.GetDepthBuffer()->GetFormat();

	m_PipelineState = new DX12PipelineState(desc);
}

