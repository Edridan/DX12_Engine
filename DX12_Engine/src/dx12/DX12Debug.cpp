#include "DX12Debug.h"

#ifdef DX12_DEBUG

#include "engine/Debug.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12RootSignature.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12DepthBuffer.h"

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
	ASSERT(m_BackBuffer != nullptr && m_NormalRT != nullptr && m_SpecularRT != nullptr /* && m_PositionRT != nullptr && m_DepthRT != nullptr */);

	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	// generate pipeline state object for GBuffer draw debug
	m_GBufferDebugRS = new DX12RootSignature();

	// add static sampler for textures
	D3D12_STATIC_SAMPLER_DESC sampler = {};

	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	m_GBufferDebugRS->AddStaticSampler(sampler);	// add static sampl

													// add textures
	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];

	for (UINT i = 0; i < 1; ++i)
	{
		descriptorTableRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
		descriptorTableRanges[i].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
		descriptorTableRanges[i].BaseShaderRegister = i; // start index of the shader registers in the range
		descriptorTableRanges[i].RegisterSpace = 0; // space 0. can usually be zero
		descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
	}

	m_GBufferDebugRS->AddDescriptorRange(descriptorTableRanges, 1, D3D12_SHADER_VISIBILITY_PIXEL);	// texture to render

	m_GBufferDebugRS->Create(device);

	DX12Shader * PShader = new DX12Shader(DX12Shader::ePixel, L"src/shaders/debug/DebugGBufferPS.hlsl");
	DX12Shader * VShader = new DX12Shader(DX12Shader::eVertex, L"src/shaders/debug/DebugGBufferVS.hlsl");

	DX12PipelineState::PipelineStateDesc desc;

	desc.InputLayout = DX12RenderEngine::GetInstance().GetPrimitiveInputLayout();
	desc.RootSignature = m_GBufferDebugRS;
	desc.VertexShader = VShader;
	desc.PixelShader = PShader;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.RenderTargetCount = 1;
	desc.RenderTargetFormat[0] = m_BackBuffer->GetFormat();
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	desc.DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	desc.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

	m_GBufferDebugPSO = new DX12PipelineState(desc);
}

DX12Debug::~DX12Debug()
{
}


void DX12Debug::DrawDebugGBuffer(ID3D12GraphicsCommandList * i_CommandList) const
{
	if (!m_Enabled)		return;

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	const UINT frameIndex = render.GetFrameIndex();
	/*
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
	*/
	static const Rect rect[4] =
	{
		Rect(0.0f, 0.5f, 0.5f, 0.0f),
		Rect(0.0f, 0.5f, 1.0f, 0.5f),
		Rect(0.5f, 1.0f, 0.5f, 0.0f),
		Rect(0.5f, 1.0f, 1.0f, 0.5f)
	};

	static const DX12RenderTarget * rt[4]
	{
		m_NormalRT,
		m_NormalRT,	// m_PositionRT,
		m_SpecularRT,
		m_DiffuseRT
	};
	
	ID3D12DescriptorHeap * descriptors = nullptr;

	i_CommandList->SetGraphicsRootSignature(m_GBufferDebugRS->GetRootSignature());
	i_CommandList->SetPipelineState(m_GBufferDebugPSO->GetPipelineState());

	for (UINT i = 0; i < 4; ++i)
	{
		D3D12_VIEWPORT viewport = render.GetViewportOnRect(rect[i]);

		i_CommandList->RSSetViewports(1, &viewport);

		// bind render targets as textures
		descriptors = rt[i]->GetShaderResourceDescriptorHeap()->GetDescriptorHeap();
		// update the descriptor for the resources
		i_CommandList->SetDescriptorHeaps(1, &descriptors);
		i_CommandList->SetGraphicsRootDescriptorTable(1, rt[i]->GetShaderResourceDescriptorHeap()->GetGPUDescriptorHandle(frameIndex));

		// draw 2D rect
		render.PushRectPrimitive2D(i_CommandList);
	}

	// reset the viewport for the next draws
	i_CommandList->RSSetViewports(1, &render.GetViewport());
}

#endif /* DX12_DEBUG */