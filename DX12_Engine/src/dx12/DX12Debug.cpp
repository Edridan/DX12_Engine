#include "DX12Debug.h"

#ifdef DX12_DEBUG

#include "engine/Debug.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12RootSignature.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12DepthBuffer.h"
#include "dx12/DX12Utils.h"

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
	TO_DO;
}

void DX12Debug::DrawDebugLine(const DirectX::XMFLOAT3 & i_Start, const DirectX::XMFLOAT3 & i_End, Color & i_Color)
{
	TO_DO;
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
{
	// retreive render targets
	for (UINT i = 0; i < DX12RenderEngine::eRenderTargetCount; ++i)
	{
		ASSERT(i_Setup.RenderTarget[i] != nullptr);
		m_GBufferRT[i] = i_Setup.RenderTarget[i];
	}

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

	DX12Shader * PShader = nullptr;
	LOAD_SHADER(PShader, DX12Shader::ePixel, L"src/shaders/debug/DebugGBufferPS.hlsl", L"resources/build/shaders/DebugGBufferPS.cso");

	DX12Shader * VShader = nullptr;
	LOAD_SHADER(VShader, DX12Shader::eVertex, L"src/shaders/debug/DebugGBufferVS.hlsl", L"resources/build/shaders/DebugGBufferVS.cso");

	DX12PipelineState::PipelineStateDesc desc;

	desc.InputLayout = DX12RenderEngine::GetInstance().GetPrimitiveInputLayout();
	desc.RootSignature = m_GBufferDebugRS;
	desc.VertexShader = VShader;
	desc.PixelShader = PShader;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.RenderTargetCount = 1;
	desc.RenderTargetFormat[0] = i_Setup.BackBuffer->GetFormat();
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	desc.DepthEnabled = false;

	m_GBufferDebugPSO = new DX12PipelineState(desc);

	GenerateViewportGrid(m_Rect, 2, 2);
}

DX12Debug::~DX12Debug()
{
}


void DX12Debug::DrawDebugGBuffer(ID3D12GraphicsCommandList * i_CommandList) const
{
	if (!m_Enabled)		return;

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	const UINT frameIndex = render.GetFrameIndex();

	static const DX12RenderTarget * rt[4]
	{
		m_GBufferRT[DX12RenderEngine::eDiffuse],
		m_GBufferRT[DX12RenderEngine::eNormal],
		m_GBufferRT[DX12RenderEngine::eSpecular],
		m_GBufferRT[DX12RenderEngine::ePosition]
	};
	
	ID3D12DescriptorHeap * descriptors = nullptr;

	i_CommandList->SetGraphicsRootSignature(m_GBufferDebugRS->GetRootSignature());
	i_CommandList->SetPipelineState(m_GBufferDebugPSO->GetPipelineState());

	for (UINT i = 0; i < 4; ++i)
	{
		D3D12_VIEWPORT viewport = render.GetViewportOnRect(m_Rect[i]);

		i_CommandList->RSSetViewports(1, &viewport);

		// bind render targets as textures
		descriptors = rt[i]->GetShaderResourceDescriptorHeap()->GetDescriptorHeap();
		// update the descriptor for the resources
		i_CommandList->SetDescriptorHeaps(1, &descriptors);
		i_CommandList->SetGraphicsRootDescriptorTable(0, rt[i]->GetShaderResourceDescriptorHeap()->GetGPUDescriptorHandle(frameIndex));

		// draw 2D rect
		render.PushRectPrimitive2D(i_CommandList);
	}

	// reset the viewport for the next draws
	i_CommandList->RSSetViewports(1, &render.GetViewport());
}

void DX12Debug::GenerateViewportGrid(std::vector<Rect>& o_Vec, UINT i_XCount, UINT i_YCount)
{
	float XStep = 1.f / (float)i_XCount;
	float YStep = 1.f / (float)i_YCount;

	for (UINT x = 0; x < i_XCount; ++x)
	{
		for (UINT y = 0; y < i_YCount; ++y)
		{
			const float left = x * XStep;
			const float top = y * YStep;
			m_Rect.push_back(Rect(left, left + XStep, top, top + YStep));
		}
	}
}

#endif /* DX12_DEBUG */