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
	};

	// input element layout helper
	static bool		IsValid(EElementFlags i_Flag);

	// input layout description
	static UINT		GetElementSize(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static UINT64	CreateFlagsFromInputLayout(D3D12_INPUT_LAYOUT_DESC i_InputLayout);
	static void		CreateInputLayoutFromFlags(D3D12_INPUT_LAYOUT_DESC & o_InputLayout, UINT64 i_Flags);

	// pipeline state descriptor
	struct PipelineStateDesc
	{
		// engine
		DX12RootSignature *		RootSignature;
		const DX12Shader *		VertexShader, * PixelShader;	// to do : support other shader
		// dx12
		D3D12_PRIMITIVE_TOPOLOGY_TYPE	PrimitiveTopologyType;
		D3D12_INPUT_LAYOUT_DESC			InputLayout;
		DXGI_FORMAT						RenderTargetFormat[8];	// 8 render target maximum
		UINT							RenderTargetCount = 1;	// render target count
		CD3DX12_DEPTH_STENCIL_DESC		DepthStencilDesc;
		// depth
		bool							DepthEnabled;
		DXGI_FORMAT						DepthStencilFormat;
		D3D12_BLEND_DESC				BlendState;
	};

	// pipeline state object implementation
	DX12PipelineState(const PipelineStateDesc & i_Desc);
	~DX12PipelineState();

	// information
	const D3D12_INPUT_LAYOUT_DESC	&	GetLayoutDesc() const;
	UINT								GetRenderTargetCount() const;
	ID3D12PipelineState *				GetPipelineState() const;
	const DX12RootSignature *			GetDX12RootSignature() const;

	// helpers
	static void			CopyInputLayout(D3D12_INPUT_LAYOUT_DESC & o_Buffer, const D3D12_INPUT_LAYOUT_DESC & i_InputLayout);

private:


	// dx12
	D3D12_INPUT_LAYOUT_DESC		m_InputLayout;	// saved input layout
	ID3D12PipelineState *		m_PipelineState;

	// internal
	const UINT					m_RenderTargetCount;
	bool						m_IsCreated;
	const DX12Shader *			m_PixelShader;
	const DX12Shader *			m_VertexShader;
	const DX12RootSignature *	m_RootSignature;


};