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
		std::wstring	Name = L"UnnamedContext";
		// command list management
		D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		// fences
		D3D12_FENCE_FLAGS		FencesFlags = D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE;
		
	};

	DX12Context(const ContextDesc & i_Desc);
	~DX12Context();

	// informations
	ID3D12GraphicsCommandList *		GetCommandList() const;
	ID3D12Device *					GetDevice() const;
	ID3D12Fence *					GetFence(UINT i_Id = (UINT)-1) const;
	UINT64							GetFenceValue(UINT i_Id = (UINT)-1) const;

	// management
	void							IncrementFenceValue(UINT i_Id = (UINT)-1);
	void							SetFenceValue(UINT64 i_Val, UINT i_Id = (UINT)-1);

	// context management
	void				ResetContext();

	friend class DX12RenderEngine;
private:


	// internal
	UINT	GetIndex(UINT i_Id) const;

	// dx12
	ID3D12GraphicsCommandList *		m_CommandList;	// command list
	ID3D12CommandAllocator **		m_CommandAllocator;
	ID3D12Fence **					m_Fences;		// fences management
	ID3D12Device *					m_Device;		// device of the context
	UINT64 *						m_FencesValues;	// values

	// internal management
	UINT							m_FrameCount;
	UINT							m_FrameIndex;
};