// imgui binding for d3d12
// define the imgui pipeline state objects and input management
// retreived from : https://github.com/onatto/imgui/tree/d3d12
#pragma once

#include <Windows.h>

#include "../lib/imgui-d3d12/imgui.h"

// struct predef
#include "dx12/d3dx12.h"

// class predef
class DX12Shader;

// dxgi, device, cmd queue, cmd allocator, command list generation with vsh, psh
#define IMGUI_GPU_BUFFER_SIZE 1024*1024

namespace ImGuiD3D12
{
	// internal call (UILayer)
	HRESULT InitializeDX12ImGui();
	bool	ImguiIsInitialized();
	void	CleanResources();

	// rendering
	void	SetRenderDataImGui(ID3D12GraphicsCommandList * i_CommandList, D3D12_CPU_DESCRIPTOR_HANDLE i_RenderTarget);
	void	RenderDrawListImGui(ImDrawData * i_DrawData);

	// dx12
	static ID3D12RootSignature	*	RootSignature	= nullptr;
	static ID3D12PipelineState *	PipelineState	= nullptr;
	static ID3D12Resource *			UploadBuffer	= nullptr;
	static ID3D12DescriptorHeap *	SrvHeap			= nullptr;
	static D3D12_CPU_DESCRIPTOR_HANDLE SrvHandle;

	// rendering
	static ID3D12GraphicsCommandList *	CommandList = nullptr;
	static D3D12_CPU_DESCRIPTOR_HANDLE	RenderTarget;

	// shaders
	static DX12Shader *		PixelShader = nullptr, * VertexShader = nullptr;
}