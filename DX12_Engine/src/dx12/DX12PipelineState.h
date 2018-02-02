// create a pipeline state for rendering objects

#pragma once

#include "dx12/d3dx12.h"

// class predef
class DX12Shader;
class DX12RootSignature;

class DX12PipelineState
{
public:
	// input layout definition
	enum EElementFlags
	{
		eNone			= 0,	// default : error if only that flag
		// start
		eHaveNormal		= 1 << 0,	// required for rendering (if not present, crash)
		eHaveTexcoord	= 1 << 1,	// required for texture rendering or post process effects
		eHaveColor		= 1 << 2,	// [optional]
	};

	// input element layout helper
	static bool		IsValid(EElementFlags i_Flag);
	static UINT		GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static UINT64	CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static void		CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags);

	// pipeline state object implementation
	DX12PipelineState();
	~DX12PipelineState();

	// set root signature
	void		SetRootSignature(const DX12RootSignature * i_RootSignature);	// manage the parameters for the pipeline state


	// information
	bool	IsCreated() const;

private:
	

	// internal
	bool			m_IsCreated;

};