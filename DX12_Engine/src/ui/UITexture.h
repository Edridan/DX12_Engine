// this window is for debug purpose
// this display a texture on the render target (only for debug)

#pragma once

#include "ui/UIWindow.h"
#include "dx12/d3dx12.h"

// class predef
class DX12Texture;

class UITexture
{
public:
	UITexture(const RECT & i_Rect, ID3D12DescriptorHeap * i_Desc);
	~UITexture();

	// render the texture on the command list
	void		Render(ID3D12GraphicsCommandList * i_CommandList);

	// manage the window
	void SetTextureDescriptor(ID3D12DescriptorHeap * i_Desc);
	void SetRect(const RECT & i_Rect);	// from 0 to 1 screen size


private:
	void UpdateData(const RECT & i_Rect) const;

	struct UITexVertex
	{
		float X, Y, Z, W;
		float U, V;

		UITexVertex(float i_X, float i_Y, float i_U, float i_V)
			:X(i_X)
			,Y(i_Y)
			,Z(0.f)
			,W(1.f)
			,U(i_U)
			,V(i_V)
		{}

		UITexVertex()	{}
	};

	// dx12 internal
	ID3D12Resource*				m_VertexBuffer;
	ID3D12Resource*				m_IndexBuffer;
	// Buffer view
	D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView;
	// buffer upload
	ID3D12Resource*				m_BufferUploadHeap;
	// rendering texture
	ID3D12DescriptorHeap *		m_Descriptor;

	UITexVertex					m_VData[4];	// vertices

};