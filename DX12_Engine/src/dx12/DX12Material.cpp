#include "dx12/DX12Material.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12ConstantBuffer.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12RootSignature.h"
#include "dx12/DX12RenderTarget.h"
#include "dx12/DX12Texture.h"
#include "engine/Utils.h"



DX12Material::DX12Material(const DX12MaterialDesc & i_Desc)
	:m_ColorAmbient(i_Desc.Ka)
	, m_ColorDiffuse(i_Desc.Kd)
	, m_ColorSpecular(i_Desc.Ks)
	, m_ColorEmissive(i_Desc.Ke)
	, m_SpecularExponent(i_Desc.Ns)
	, m_HaveChanged(true)
	, m_ConstantBuffer(UnavailableAdressId)
	, m_Name(i_Desc.Name)
	, m_Id((UINT64)this)
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12Device * device = render.GetDevice();

	// manage if the material have texture or not
	SetTexture(i_Desc.map_Kd, eDiffuse);
	SetTexture(i_Desc.map_Ks, eSpecular);
	SetTexture(i_Desc.map_Ka, eAmbient);

	// reserve address for constant buffer
	m_ConstantBuffer = render.GetConstantBuffer(DX12RenderEngine::eMaterial)->ReserveVirtualAddress();
	UpdateConstantBufferView();	// update to constant buffer

	// create root signature
	m_RootSignature = new DX12RootSignature();
	// generate default root signature

	// constant buffer
	m_RootSignature->AddConstantBuffer(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	// b0 : transform constant
	m_RootSignature->AddConstantBuffer(1, 0, D3D12_SHADER_VISIBILITY_ALL);		// b1 : global constant
	m_RootSignature->AddConstantBuffer(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);	// b2 : material constant

	// add textures
	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[eCount];

	for (UINT i = 0; i < eCount; ++i)
	{
		descriptorTableRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
		descriptorTableRanges[i].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
		descriptorTableRanges[i].BaseShaderRegister = i; // start index of the shader registers in the range
		descriptorTableRanges[i].RegisterSpace = 0; // space 0. can usually be zero
		descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
	}

	m_RootSignature->AddDescriptorRange(&descriptorTableRanges[0], 1, D3D12_SHADER_VISIBILITY_PIXEL);
	m_RootSignature->AddDescriptorRange(&descriptorTableRanges[1], 1, D3D12_SHADER_VISIBILITY_PIXEL);
	m_RootSignature->AddDescriptorRange(&descriptorTableRanges[2], 1, D3D12_SHADER_VISIBILITY_PIXEL);

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

	m_RootSignature->AddStaticSampler(sampler);

	m_RootSignature->Create(device, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);


	// create shaders
	//DX12Shader * PShader = new DX12Shader(DX12Shader::ePixel, L"src/shaders/forward/NormalTexPixel.hlsl");
	//DX12Shader * VShader = new DX12Shader(DX12Shader::eVertex, L"src/shaders/forward/NormalTexVertex.hlsl");

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
	
	//desc.RenderTargetCount = 1;
	//desc.RenderTargetFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	desc.RenderTargetCount = DX12RenderEngine::ERenderTargetId::eRenderTargetCount;
	desc.RenderTargetFormat[0] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eNormal)->GetFormat();
	desc.RenderTargetFormat[1] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eDiffuse)->GetFormat();
	desc.RenderTargetFormat[2] = render.GetRenderTarget(DX12RenderEngine::ERenderTargetId::eSpecular)->GetFormat();

	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	desc.DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	desc.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

	m_PipelineState = new DX12PipelineState(desc);
}

DX12Material::~DX12Material()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	if (m_ConstantBuffer != UnavailableAdressId)
	{
		render.GetConstantBuffer(DX12RenderEngine::eMaterial)->ReleaseVirtualAddress(m_ConstantBuffer);
	}
}

inline void DX12Material::SetTexture(DX12Texture * i_Texture, ETextureType i_Type)
{
	if (i_Type < eCount)
	{
		m_Textures[i_Type] = i_Texture;

		if (i_Texture)
		{
			m_Descriptors[i_Type] = i_Texture->GetDescriptorHeap();
		}
		else
		{
			m_Descriptors[i_Type] = nullptr;
		}
	}
}

void DX12Material::SetAmbientColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorAmbient = i_Color;
}

void DX12Material::SetDiffuseColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorDiffuse = i_Color;
}

void DX12Material::SetEmissiveColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorEmissive = i_Color;
}

void DX12Material::SetSpecularColor(const Color & i_Color)
{
	m_HaveChanged = true;

}

void DX12Material::Set(const DX12MaterialDesc & i_Desc)
{
	m_ColorAmbient = i_Desc.Ka;
	m_ColorDiffuse = i_Desc.Kd;
	m_ColorSpecular = i_Desc.Ks;
	m_ColorEmissive = i_Desc.Ke;

	m_HaveChanged = true;

	m_ConstantBuffer = UnavailableAdressId;

	m_Name = i_Desc.Name;

	// manage if the material have texture or not
	SetTexture(i_Desc.map_Kd, eDiffuse);
	SetTexture(i_Desc.map_Ks, eSpecular);
	SetTexture(i_Desc.map_Ka, eAmbient);

	m_HaveChanged = true;
}

UINT64 DX12Material::GetId() const
{
	return m_Id;
}

inline bool DX12Material::HaveTexture(ETextureType i_Type) const
{
	if (i_Type < eCount)
	{
		return m_Textures[i_Type] != nullptr;
	}
	return false;
}

bool DX12Material::IsCompatibleWithFlags(UINT64 i_ElementFlag) const
{
	const bool haveTex = HaveTexture(eAmbient) || HaveTexture(eDiffuse) || HaveTexture(eSpecular);

	if (haveTex && !(i_ElementFlag | DX12PipelineState::EElementFlags::eHaveTexcoord))
		return false;

	return true;
}

bool DX12Material::NeedUpdate() const
{
	return false;
}

inline void DX12Material::UpdateConstantBufferView()
{
	struct MaterialStruct
	{
		DirectX::XMFLOAT4		Ka, Kd, Ks, Ke;
		BOOL					Map_A, Map_D, Map_S;
		float					Ns;
	};

	// error management
	if (m_ConstantBuffer == UnavailableAdressId)
	{
		PRINT_DEBUG("Error, the constant buffer address is not set");
		DEBUG_BREAK;
		return;
	}

	if (m_HaveChanged)
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		// push constant buffer to the gpu
		MaterialStruct mat;

		mat.Ka = ColorToVec4(m_ColorAmbient);
		mat.Kd = ColorToVec4(m_ColorDiffuse);
		mat.Ks = ColorToVec4(m_ColorSpecular);
		mat.Ke = ColorToVec4(m_ColorEmissive);

		mat.Map_A = HaveTexture(eAmbient);
		mat.Map_D = HaveTexture(eDiffuse);
		mat.Map_S = HaveTexture(eSpecular);

		mat.Ns = m_SpecularExponent;

		// update the buffer
		render.GetConstantBuffer(DX12RenderEngine::eMaterial)->UpdateConstantBufferForEachFrame(m_ConstantBuffer, &mat, sizeof(MaterialStruct));

		m_HaveChanged = false;
	}
}

void DX12Material::SetupPipeline(ID3D12GraphicsCommandList * i_CommandList) const
{
	// add pso and root signature to the commandlist
	i_CommandList->SetGraphicsRootSignature(m_RootSignature->GetRootSignature());
	// Setup the pipeline state
	i_CommandList->SetPipelineState(m_PipelineState->GetPipelineState());
}

void DX12Material::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// error management
	if (m_ConstantBuffer == UnavailableAdressId)
	{
		PRINT_DEBUG("Error, the constant buffer address is not set");
		DEBUG_BREAK;
		return;
	}

	// set descriptors

	// parameter 0 is already used by the CBV for transform so we start to the 
	i_CommandList->SetGraphicsRootConstantBufferView(2, render.GetConstantBuffer(DX12RenderEngine::eMaterial)->GetUploadVirtualAddress(m_ConstantBuffer));

	// bind textures
	for (UINT i = 0; i < ETextureType::eCount; ++i)
	{
		ID3D12DescriptorHeap ** descriptors = (ID3D12DescriptorHeap **)(&m_Descriptors[i]);

		if (HaveTexture((ETextureType)i))
		{
			// update the descriptor for the resources
			i_CommandList->SetDescriptorHeaps(1, descriptors);
			i_CommandList->SetGraphicsRootDescriptorTable(3 + i, m_Textures[i]->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		}
	}

}

void DX12Material::CreateShaderCode(std::wstring & o_Code, const DX12MaterialDesc & i_Desc, DX12Shader::EShaderType i_Type)
{

}
