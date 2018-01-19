#pragma once

#include <string>
#include <DirectXMath.h>
#include "d3dx12.h"

// class predef
class DX12Shader;

class DX12Text
{
public:
	DX12Text(const wchar_t * i_Text);
	~DX12Text();

	void			SetText(const wchar_t * i_Text);
	const wchar_t *	GetText() const;

private:
	// Struct definition
	struct TextVertex
	{
		TextVertex(
			float r, float g, float b, float a,	// Color
			float u, float v,					// uv
			float tw, float th,					// text size
			float x, float y,					// 2D position
			float w, float h)					// text size
		:Color(r,g,b,a)
		,TexCoord(u, v, tw, th)
		,Pos(x, y, w, h)
		{};

		DirectX::XMFLOAT4	Pos;
		DirectX::XMFLOAT4	TexCoord;
		DirectX::XMFLOAT4	Color;
	};

	std::wstring m_Text;		// Text to display

	// static
	static const UINT		s_MaxTextCharacter;

	// dx12
	ID3D12Resource **		m_BufferUploadHeap;
	static UINT8 **			m_TextVBGPUAddress;


	static HRESULT			CreateTextPipelineStateObject();	// Create the default text pipeline state object

	// static
	static ID3D12PipelineState *		s_TextPipelineState;
	static ID3D12RootSignature *		s_RootSignature;

	static DX12Shader *		s_PixelShader;
	static DX12Shader *		s_VertexShader;

	static const D3D12_INPUT_ELEMENT_DESC	s_TextInputElement[];
	static const D3D12_INPUT_LAYOUT_DESC	s_TextInputLayout;
};