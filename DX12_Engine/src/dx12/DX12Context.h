// DX12 context management (used for deferred context)
// this contains a render target a command list and fence management

#pragma once

#include <string>
#include "dx12/d3dx12.h"

// class predef
class DX12RenderTarget;	// render target of the current context


class DX12Context
{
public:
	struct ContextDesc
	{
		// default
		std::string		Name = "UnnamedContext";
		// dx12 specs
		UINT			FrameCount = 0;	// if 0 : take the frame count index from dx12 render engine

	};

	DX12Context(const ContextDesc & i_Desc);
	~DX12Context();

	// informations
	ID3D12GraphicsCommandList *		GetCommandList() const;
	ID3D12Fence *					GetFence(UINT i_Id) const;
	UINT64							GetFenceValue(UINT i_Id) const;


	friend class DX12RenderEngine;
private:
	// dx12
	ID3D12GraphicsCommandList *		m_CommandList;	// current command list for the context
	ID3D12Fence **					m_Fences;		// fences management
	UINT64 *						m_FencesValues;	// values

	DX12RenderTarget **				m_RenderTarget;	// render targets for the current context

	// internal management
	UINT							m_FrameCount;
};