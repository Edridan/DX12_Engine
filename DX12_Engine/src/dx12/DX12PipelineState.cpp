#include "DX12PipelineState.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12Shader.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12RootSignature.h"

bool DX12PipelineState::IsValid(EElementFlags i_Flag)
{
	// if the flag have nothing more than position only (need normal to be rendered)
	return (i_Flag != eNone);
}

UINT DX12PipelineState::GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout)
{
	// size of one element
	UINT elementSize = 0;

	// go into the structure and get the size of the buffer
	for (UINT i = 0; i < i_InputLayout.NumElements; ++i)
	{
		D3D12_INPUT_ELEMENT_DESC element = i_InputLayout.pInputElementDescs[i];

		if ((element.AlignedByteOffset != D3D12_APPEND_ALIGNED_ELEMENT) && (element.AlignedByteOffset != elementSize))
		{
			elementSize = element.AlignedByteOffset;
		}

		// update the size of the current buffer
		elementSize += SizeOfFormatElement(element.Format);
	}

	return elementSize;
}

UINT64 DX12PipelineState::CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout)
{
	UINT64 flags = EElementFlags::eNone;
	// go into the structure and get the size of the buffer
	for (UINT i = 0; i < i_InputLayout.NumElements; ++i)
	{
		D3D12_INPUT_ELEMENT_DESC element = i_InputLayout.pInputElementDescs[i];

		if (strcmp(element.SemanticName, "TEXCOORD") == 0)	flags |= EElementFlags::eHaveTexcoord;
		else if (strcmp(element.SemanticName, "NORMAL") == 0)	flags |= EElementFlags::eHaveNormal;
	}

	return flags;
}

void DX12PipelineState::CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags)
{
	// compute size of elements
	D3D12_INPUT_ELEMENT_DESC * elements;
	UINT size = 1;
	UINT index = 0;
	UINT offset = 0;

	if (i_Flags & EElementFlags::eHaveTexcoord)			++size;
	if (i_Flags & EElementFlags::eHaveNormal)			++size;

	elements = new D3D12_INPUT_ELEMENT_DESC[size];
	o_InputLayout.NumElements = size;
	o_InputLayout.pInputElementDescs = elements;

	// layout order definition depending flags : 
	// 1 - Position
	// 2 - Normal
	// 3 - Texcoord
	// 4 - Color

	// default position
	elements[index++] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	offset += 3 * sizeof(float);

	if (i_Flags & EElementFlags::eHaveNormal)
	{
		elements[index++] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 3 * sizeof(float);
	}
	if (i_Flags & EElementFlags::eHaveTexcoord)
	{
		elements[index++] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 2 * sizeof(float);
	}
}

DX12PipelineState::DX12PipelineState(const PipelineStateDesc & i_Desc)
	:m_RootSignature(i_Desc.RootSignature)
	,m_PixelShader(i_Desc.PixelShader)
	,m_VertexShader(i_Desc.VertexShader)
	,m_IsCreated(false)
	,m_PipelineState(nullptr)
	,m_RenderTargetCount(i_Desc.RenderTargetCount)
{
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	// get the input layout
	CopyInputLayout(m_InputLayout, i_Desc.InputLayout);

	// assert error
	ASSERT(m_PixelShader->GetType() == DX12Shader::ePixel);
	ASSERT(m_VertexShader->GetType() == DX12Shader::eVertex);
	ASSERT(i_Desc.RenderTargetCount < 8);

	// create pipeline state
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Create default pso
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

	pipelineDesc.InputLayout = m_InputLayout; // the structure describing our input layout
	pipelineDesc.pRootSignature = m_RootSignature->GetRootSignature(); // the root signature that describes the input data this pso needs
	pipelineDesc.VS = m_VertexShader->GetByteCode(); // structure describing where to find the vertex shader bytecode and how large it is	(thx ZELDARCK)
	pipelineDesc.PS = m_PixelShader->GetByteCode(); // same as VS but for pixel shader
	pipelineDesc.PrimitiveTopologyType = i_Desc.PrimitiveTopologyType;
	pipelineDesc.SampleDesc = sampleDesc;
	pipelineDesc.SampleMask = 0xffffffff;
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineDesc.BlendState = i_Desc.BlendState;
	pipelineDesc.NumRenderTargets = i_Desc.RenderTargetCount;
	pipelineDesc.DSVFormat = i_Desc.DepthStencilFormat;
	pipelineDesc.DepthStencilState = i_Desc.DepthStencilDesc; // a default depth stencil state
	
	for (UINT i = 0; i < i_Desc.RenderTargetCount; ++i)
	{
		pipelineDesc.RTVFormats[i] = i_Desc.RenderTargetFormat[i]; // format of the render target
	}

	DX12_ASSERT(device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_PipelineState)));
}

DX12PipelineState::~DX12PipelineState()
{
	// clean DX12 resources
	SAFE_RELEASE(m_PipelineState);

	// clear allocated resources
	delete [] m_InputLayout.pInputElementDescs;

}

const D3D12_INPUT_LAYOUT_DESC & DX12PipelineState::GetLayoutDesc() const
{
	return m_InputLayout;
}

UINT DX12PipelineState::GetRenderTargetCount() const
{
	return m_RenderTargetCount;
}

const ID3D12PipelineState * DX12PipelineState::GetPipelineState() const
{
	return m_PipelineState;
}

const DX12RootSignature * DX12PipelineState::GetRootSignature() const
{
	return m_RootSignature;
}

FORCEINLINE void DX12PipelineState::CopyInputLayout(D3D12_INPUT_LAYOUT_DESC & o_Buffer, const D3D12_INPUT_LAYOUT_DESC & i_InputLayout)
{
	D3D12_INPUT_ELEMENT_DESC *pElement = new D3D12_INPUT_ELEMENT_DESC[i_InputLayout.NumElements];
	for (UINT i = 0; i < i_InputLayout.NumElements; ++i)
	{
		pElement[i] = i_InputLayout.pInputElementDescs[i];
	}

	// setup the buffer input layout
	o_Buffer.pInputElementDescs = pElement;
	o_Buffer.NumElements = i_InputLayout.NumElements;
}
