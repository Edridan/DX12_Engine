#include "DX12PipelineState.h"

#include "dx12/DX12Utils.h"

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
		else if (strcmp(element.SemanticName, "COLOR") == 0)	flags |= EElementFlags::eHaveColor;
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

	if (i_Flags & EElementFlags::eHaveColor)			++size;
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
	if (i_Flags & EElementFlags::eHaveColor)
	{
		elements[index++] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		offset += 3 * sizeof(float);
	}
}

DX12PipelineState::DX12PipelineState()
	:m_IsCreated(false)
{
}

DX12PipelineState::~DX12PipelineState()
{
}

void DX12PipelineState::SetRootSignature(const DX12RootSignature * i_RootSignature)
{

}

bool DX12PipelineState::IsCreated() const
{
	return m_IsCreated;
}
