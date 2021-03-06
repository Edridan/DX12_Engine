// DX12 debugger informations
// this allow debug DX12 as GBuffer, light debugging

#pragma once

#include "dx12/DX12Utils.h"

#ifdef DX12_DEBUG


#include "d3dx12.h"
#include "engine/Utils.h"
#include "dx12/DX12RenderEngine.h"

#include <DirectXMath.h>
#include <vector>

// class predef
class DX12Mesh;
class DX12RenderTarget;
class DX12PipelineState;
class DX12RootSignature;
class DX12DepthBuffer;
// engine
class Transform;

class DX12Debug
{
public:
	// instance
	struct DX12DebugDesc
	{
		// default debug
		bool					EnabledByDefault = true;

		// render target setups
		DX12RenderTarget *		BackBuffer = nullptr;
		DX12RenderTarget *		RenderTarget[DX12RenderEngine::eRenderTargetCount];	// Render targets
		// depth buffer descriptor
		DX12DepthBuffer *		DepthBuffer = nullptr;

		// 3D render debug
		UINT32		LineCount;	// line count for 3D debug
	};

	// Singleton
	static DX12Debug		&	GetInstance();
	static void					Create(const DX12DebugDesc & i_Setup);
	static void					Delete();

	// draw debug 3D (To do : implement)
	void			DrawDebugBox(const DirectX::XMFLOAT3 & i_Position, const Transform & i_Transform, Color & i_Color);
	void			DrawDebugLine(const DirectX::XMFLOAT3 & i_Start, const DirectX::XMFLOAT3 & i_End, Color & i_Color);

	// debug management
	void			SetEnabled(bool i_Enabled);
	bool			IsEnabled() const;

	// friend class
	friend class DX12RenderEngine;

private:
	// singleton management
	static DX12Debug *		s_Instance;

	// internal debug management (managed by RenderEngine)
	void			DrawDebugGBuffer(ID3D12GraphicsCommandList * i_CommandList) const;	// draw GBuffer on the immediate context
	void			GenerateViewportGrid(std::vector<Rect> & o_Vec, UINT i_XCount, UINT i_YCount);

	DX12Debug(const DX12DebugDesc & i_Setup);
	~DX12Debug();

	// debug management
	bool		m_Enabled;

	// GBuffer render targets
	DX12RenderTarget *			m_GBufferRT[DX12RenderEngine::eRenderTargetCount];
	// Depth
	DX12DepthBuffer *			m_DepthDesc;	// depth render (special render target)

	std::vector<Rect>			m_Rect;

	// DX12 GBuffer debug
	DX12PipelineState *			m_GBufferDebugPSO;
	DX12RootSignature *			m_GBufferDebugRS;
};

#endif /* DX12_DEBUG */