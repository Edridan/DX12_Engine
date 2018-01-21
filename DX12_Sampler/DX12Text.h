#pragma once

#include <string>
#include <DirectXMath.h>
#include "DX12Utils.h"
#include "d3dx12.h"

// class predef
class DX12Shader;
class DX12Font;

class DX12Text
{
public:
	DX12Text(const wchar_t * i_Text, DX12Font * i_Font);
	DX12Text(DX12Font * i_Font);
	~DX12Text();

	void			SetText(const std::wstring & i_Text);
	const wchar_t *	GetText() const;
	void			SetColor(const Color & i_Color);
	void			SetFont(DX12Font * i_Font);

	// DX12
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);

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

		TextVertex() {};

		DirectX::XMFLOAT4	Pos;
		DirectX::XMFLOAT4	TexCoord;
		DirectX::XMFLOAT4	Color;
	};

	// Update frame buffer
	void		UpdateGPUBuffer(int i_FrameIndex);

	std::wstring	m_Text;		// Text to display
	DX12Font *		m_Font;
	Color			m_Color;

	// computed data
	TextVertex *		m_Vertices;

	// static
	static const UINT		s_MaxTextCharacter;

	// dx12
	UINT					m_FrameCount;
	ID3D12Resource **		m_BufferUploadHeap;
	UINT8 **				m_TextVBGPUAddress;
	bool *					m_BufferUpdated;

	static HRESULT			CreateTextPipelineStateObject();	// Create the default text pipeline state object

	// static
	static ID3D12PipelineState *		s_TextPipelineState;
	static ID3D12RootSignature *		s_RootSignature;

	static DX12Shader *		s_PixelShader;
	static DX12Shader *		s_VertexShader;

	static const D3D12_INPUT_ELEMENT_DESC	s_TextInputElement[];
	static const D3D12_INPUT_LAYOUT_DESC	s_TextInputLayout;
};