#include "dx12/DX12Text.h"

#include "dx12/DX12Font.h"
#include "dx12/DX12Mesh.h"
#include "dx12/DX12Shader.h"
#include "dx12/DX12RenderEngine.h"

// static text
const UINT DX12Text::s_MaxTextCharacter = 256;

// static dx12
ID3D12PipelineState * DX12Text::s_TextPipelineState = nullptr;
ID3D12RootSignature * DX12Text::s_RootSignature		= nullptr;

DX12Shader * DX12Text::s_VertexShader	= nullptr;
DX12Shader * DX12Text::s_PixelShader	= nullptr;

const D3D12_INPUT_ELEMENT_DESC	DX12Text::s_TextInputElement[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
};

const D3D12_INPUT_LAYOUT_DESC	DX12Text::s_TextInputLayout =
{
	s_TextInputElement,
	sizeof(s_TextInputElement) / sizeof(D3D12_INPUT_ELEMENT_DESC),
};

DX12Text::DX12Text(const wchar_t * i_Text, DX12Font * i_Font)
	:m_Text()
	,m_Vertices(nullptr)
	,m_Font(i_Font)
	,m_Color(color::White)
{
	m_Text.reserve(s_MaxTextCharacter);

	// we need to create root signature and pipeline state
	if (s_RootSignature == nullptr || s_TextPipelineState == nullptr)
	{
		CreateTextPipelineStateObject();
	}

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	ID3D12Device * device = render.GetDevice();
	ID3D12GraphicsCommandList * commandList = render.GetCommandList();
	m_FrameCount = render.GetFrameBufferCount();

	m_BufferUploadHeap = new ID3D12Resource*[m_FrameCount];
	m_TextVBGPUAddress = new UINT8 *[m_FrameCount];

	m_BufferUpdated = new bool[m_FrameCount];

	// create the upload heap for text
	HRESULT hr;

	// create a buffer for each frame
	for (size_t i = 0; i < m_FrameCount; ++i)
	{
		hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(s_MaxTextCharacter * sizeof(TextVertex)), // resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
			nullptr,
			IID_PPV_ARGS(&m_BufferUploadHeap[i]));

		m_BufferUploadHeap[i]->SetName(L"Text buffer upload heap");


		CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

		// map the resource heap to get a gpu virtual address to the beginning of the heap
		hr = m_BufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_TextVBGPUAddress[0]));

		m_BufferUpdated[i] = false;
	}

	// create vertices data
	m_Vertices = new TextVertex[s_MaxTextCharacter];

	// setup the text
	SetText(i_Text);
}

DX12Text::DX12Text(DX12Font * i_Font)
	:DX12Text(L"", i_Font)
{
}

DX12Text::~DX12Text()
{
	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		SAFE_RELEASE(m_BufferUploadHeap[i]);
	}

	delete[] m_BufferUploadHeap;
	delete[] m_TextVBGPUAddress;
}

void DX12Text::SetText(const std::wstring & i_Text)
{
	// if the font is null we can't display the text
	if (m_Font == nullptr)
	{
		return;
	}

	// retreive the text from input
	if (i_Text.size() > s_MaxTextCharacter)
	{
		m_Text.clear();
		m_Text.append(i_Text, s_MaxTextCharacter - 1);
		m_Text.append(L"\n");
	}
	else
	{
		m_Text = i_Text;
	}

	wchar_t lastChar		= -1; // no last character to start with
	UINT numChar			= 0;
	const Padding padding	= m_Font->GetPadding();
	const float	hPadding	= (padding.left + padding.right);
	const float vPadding	= (padding.top + padding.bottom);

	float x = 0, y = 0;

	// recompute text vertices
	for (size_t i = 0; i < m_Text.size(); ++i)
	{
		wchar_t c						= m_Text[i];
		
		// end of the text or buffer overflow
		if (c == L'\n' || i >= s_MaxTextCharacter)
			break;
		
		DX12Font::FontChar * fontChar	= m_Font->GetFontChar(c);

		// the character is not in the char font
		if (fontChar == nullptr)
			continue;

		// To do : manage new line
		
		// create vertices
		float kerning = 0.f;
		
		if (i > 0)
		{
			kerning = m_Font->GetKerning(lastChar, c);
		}

		m_Vertices[numChar] = TextVertex(
			m_Color.r, m_Color.g, m_Color.b, 1.f,
			fontChar->U, fontChar->V,
			fontChar->tWidth, fontChar->tHeight,
			x + (fontChar->xOffset + kerning),
			y - fontChar->yOffset,
			fontChar->Width, fontChar->Height
		);

		// remove horizontal padding
		x += fontChar->xAdvance - hPadding;

		// increment values
		++numChar;
		lastChar = c;
	}

	// reset buffer updated
	for (UINT i = 0; i < m_FrameCount; ++i)
	{
		m_BufferUpdated[i] = false;
	}
}

const wchar_t * DX12Text::GetText() const
{
	return m_Text.c_str();
}

void DX12Text::SetColor(const Color & i_Color)
{
	m_Color = i_Color;
}

void DX12Text::SetFont(DX12Font * i_Font)
{
	m_Font = i_Font;

	SetText(m_Text.c_str());
}

void DX12Text::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList)
{
	int frameIndex = DX12RenderEngine::GetInstance().GetFrameIndex();
	UpdateGPUBuffer(frameIndex);

	i_CommandList->DrawInstanced(4u, (UINT)m_Text.size(), 0, 0);
}

void DX12Text::UpdateGPUBuffer(int i_FrameIndex)
{
	if (!m_BufferUpdated[i_FrameIndex])
	{
		// copy the vertice buffer to the needed frame index GPU buffer
		size_t size = (m_Text.size() > s_MaxTextCharacter ? s_MaxTextCharacter : m_Text.size());
		memcpy(m_TextVBGPUAddress[i_FrameIndex], m_Vertices, size * sizeof(DX12Text::TextVertex));

		m_BufferUpdated[i_FrameIndex] = true;
	}
}

inline HRESULT DX12Text::CreateTextPipelineStateObject()
{
	if (s_RootSignature != nullptr || s_TextPipelineState != nullptr)
	{
		// the root signature and the pipeline state are already created
		return S_FALSE;
	}

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	ID3D12Device * device						= render.GetDevice();
	ID3D12GraphicsCommandList * commandList		= render.GetCommandList();

	// create source signature

	// create a root descriptor, which explains where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// create a descriptor range (descriptor table) and fill it out
	// this is a range of descriptors inside a descriptor heap
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; // only one range right now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
	descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
	descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
	descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

																									   // create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

	// create a root parameter for the root descriptor and fill it out
	D3D12_ROOT_PARAMETER  rootParameters[2]; // two root parameters
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our pixel shader will be the only shader accessing this parameter for now

	// fill out the parameter for our descriptor table. Remember it's a good idea to sort parameters by frequency of change. Our constant
	// buffer will be changed multiple times per frame, while our descriptor table will not be changed at all (in this tutorial)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	rootParameters[1].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // our pixel shader will be the only shader accessing this parameter for now

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter				= D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias			= 0;
	sampler.MaxAnisotropy		= 0;
	sampler.ComparisonFunc		= D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor			= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD				= 0.0f;
	sampler.MaxLOD				= D3D12_FLOAT32_MAX;
	sampler.ShaderRegister		= 0;
	sampler.RegisterSpace		= 0;
	sampler.ShaderVisibility	= D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 2 root parameters
		rootParameters, // a pointer to the beginning of our root parameters array
		1, // we have one static sampler
		&sampler, // a pointer to our static sampler (array)
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* errorBuff;	// a buffer holding the error data if any
	ID3DBlob* signature;	// signature root serialized

	// create root signature
	DX12_ASSERT(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff));
	DX12_ASSERT(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&s_RootSignature)));

	s_PixelShader = new DX12Shader(DX12Shader::ePixel, L"TextPixel.hlsl");
	s_VertexShader = new DX12Shader(DX12Shader::eVertex, L"TextVertex.hlsl");
	
	D3D12_BLEND_DESC textBlendStateDesc = {};
	textBlendStateDesc.AlphaToCoverageEnable					= FALSE;
	textBlendStateDesc.IndependentBlendEnable					= FALSE;
	textBlendStateDesc.RenderTarget[0].BlendEnable				= TRUE;
	textBlendStateDesc.RenderTarget[0].SrcBlend					= D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlend				= D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOp					= D3D12_BLEND_OP_ADD;
	textBlendStateDesc.RenderTarget[0].SrcBlendAlpha			= D3D12_BLEND_SRC_ALPHA;
	textBlendStateDesc.RenderTarget[0].DestBlendAlpha			= D3D12_BLEND_ONE;
	textBlendStateDesc.RenderTarget[0].BlendOpAlpha				= D3D12_BLEND_OP_ADD;
	textBlendStateDesc.RenderTarget[0].RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	textDepthStencilDesc.DepthEnable = false;

	// pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC textPipelineDesc = {};
	textPipelineDesc.InputLayout			= s_TextInputLayout;
	textPipelineDesc.pRootSignature			= s_RootSignature;
	textPipelineDesc.VS						= s_VertexShader->GetByteCode();
	textPipelineDesc.PS						= s_PixelShader->GetByteCode();
	textPipelineDesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	textPipelineDesc.RTVFormats[0]			= DXGI_FORMAT_UNKNOWN;
	textPipelineDesc.SampleDesc				= render.GetSampleDesc();
	textPipelineDesc.SampleMask				= 0xffffffff;
	textPipelineDesc.RasterizerState		= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// add specific blendstate and depth stencil state for the text (no depth for text)
	textPipelineDesc.BlendState				= textBlendStateDesc;
	textPipelineDesc.DepthStencilState		= textDepthStencilDesc;

	DX12_ASSERT(device->CreateGraphicsPipelineState(&textPipelineDesc, IID_PPV_ARGS(&s_TextPipelineState)));

	// pipeline state object successful creation
	return S_OK;
}
