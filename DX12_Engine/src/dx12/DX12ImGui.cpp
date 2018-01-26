#include "DX12Imgui.h"

#include <windows.h>
#include "DX12RenderEngine.h"
#include "DX12Shader.h"
#include "DX12Texture.h"
#include "engine/Engine.h"


HRESULT ImGuiD3D12::InitializeDX12ImGui()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	ID3D12Device * device = render.GetDevice();
	ID3D12GraphicsCommandList * commandList = render.GetCommandList();
	ImGuiIO & io = ImGui::GetIO();

	// Base creation (Shader resource view and Root signature for ImGui)

	// Create one shader resource descriptor for the font texture
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeap;
	srvDescHeap.NumDescriptors = 1;
	srvDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeap.NodeMask = 0;
	device->CreateDescriptorHeap(&srvDescHeap
		, __uuidof(ID3D12DescriptorHeap)
		, (void**)&SrvHeap
	);

	// A root parameter describes one slot of a root signature
	// Parameter types are: DESCRIPTOR_TABLE, 32BIT_CONSTANTS, CBV, SRV, UAV
	//
	// Root Descriptor Table: { uint NumDescriptorRanges, const DescriptorRange* pDescriptorRanges }
	// Root Descriptor:       { uint ShaderRegister, uint RegisterSpace }
	// Root Constants:        { uint ShaderRegister, uint RegisterSpace, uint Num32BitValues

	D3D12_DESCRIPTOR_RANGE descRange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,            // Range Type
		1,                                          // Number of descriptors
		0,                                          // Base shader register
		0,                                          // Register space
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },     // Offset in descriptors from the start of the root signature
	};

	// This root signature will have two parameters, one descriptor table for SRVs and one constant buffer descriptor
	D3D12_ROOT_PARAMETER rootParameter[] =
	{
		{ D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,{ 1,descRange }, D3D12_SHADER_VISIBILITY_ALL },
		{ D3D12_ROOT_PARAMETER_TYPE_CBV,{ 0,0 }, D3D12_SHADER_VISIBILITY_VERTEX },
	};

	rootParameter[1].Descriptor.RegisterSpace = 0;
	rootParameter[1].Descriptor.ShaderRegister = 0;

	// Include a statci sampler
	D3D12_STATIC_SAMPLER_DESC samplerDesc;
	memset(&samplerDesc, 0, sizeof(samplerDesc));
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = 0.f;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Root signature description
	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters			= 2;
	descRootSignature.pParameters			= rootParameter;
	descRootSignature.NumStaticSamplers		= 1;
	descRootSignature.pStaticSamplers		= &samplerDesc;
	descRootSignature.Flags					= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Serialise the root signature into memory
	ID3DBlob* outBlob;
	ID3DBlob* errorBlob;
	D3D12SerializeRootSignature(&descRootSignature
		, D3D_ROOT_SIGNATURE_VERSION_1
		, &outBlob
		, &errorBlob
	);

	// Create the root signature using the binary blob
	device->CreateRootSignature(0
		, outBlob->GetBufferPointer()
		, outBlob->GetBufferSize()
		, __uuidof(ID3D12RootSignature)
		, (void**)&RootSignature
	);

	outBlob->Release();

	// create the pipeline state object
	// and the font texture

	// retreive default shader set
	VertexShader = new DX12Shader(DX12Shader::eVertex, L"resources/shaders/ImGuiVertex.hlsl");
	PixelShader = new DX12Shader(DX12Shader::ePixel, L"resources/shaders/ImGuiPixel.hlsl");

	if ((!VertexShader->IsLoaded()) || (!PixelShader->IsLoaded()))
	{
		DEBUG_BREAK;
		return E_FAIL;
	}

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, (size_t)(&((ImDrawVert*)0)->uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, (size_t)(&((ImDrawVert*)0)->col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.ForcedSampleCount = 1;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// Create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = RootSignature;
	psoDesc.VS = VertexShader->GetByteCode();
	psoDesc.PS = PixelShader->GetByteCode();
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), (void**)&PipelineState);

	// create upload ring buffer
	UploadBuffer = render.CreateComittedResource(DX12RenderEngine::HeapProperty::Upload, IMGUI_GPU_BUFFER_SIZE * 8);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;

	// Create fonts texture and SRV descriptor for it
	BYTE* pixels = 0;
	int width, height;

	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Create fonts texture
	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ID3D12Resource * fontResource = render.CreateComittedResource(DX12RenderEngine::HeapProperty::Default, &desc, 0);

	// Upload the fonts texture
	uint32_t subres = 0;
	uint32_t numRows;
	uint64_t rowPitch;
	uint64_t totalBytes;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	device->GetCopyableFootprints(&desc
		, subres
		, 1
		, 0
		, &layout
		, &numRows
		, &rowPitch
		, &totalBytes
	);

	uint8_t* mappedBuffer;

	// Upload the font
	UploadBuffer->Map(0, NULL, (void**)&mappedBuffer);
	memcpy(mappedBuffer, pixels, (size_t)totalBytes);
	UploadBuffer->Unmap(0, NULL);

	D3D12_BOX box;
	box.left = 0;
	box.top = 0;
	box.right = (UINT)desc.Width;
	box.bottom = (UINT)desc.Height;
	box.front = 0;
	box.back = 1;

	D3D12_TEXTURE_COPY_LOCATION dst = { fontResource,   D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,{ subres } };
	D3D12_TEXTURE_COPY_LOCATION src = { UploadBuffer, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,  layout };
	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, &box);

	// create shader resource view
	SrvHandle = SrvHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateShaderResourceView(fontResource, &srvDesc, SrvHandle);

	return S_OK;
}

void ImGuiD3D12::SetRenderDataImGui(ID3D12GraphicsCommandList * i_CommandList, D3D12_CPU_DESCRIPTOR_HANDLE i_RenderTarget)
{
	CommandList = i_CommandList;
	RenderTarget = i_RenderTarget;
}

void ImGuiD3D12::RenderDrawListImGui(ImDrawData * i_DrawData)
{
	// Range CPU will read from mapping the upload buffer
	// End < Begin specifies CPU will not read the mapped buffer
	D3D12_RANGE readRange;
	readRange.End = 0;
	readRange.Begin = 1;

	char* mappedBuffer = 0;
	UploadBuffer->Map(0, &readRange, (void**)&mappedBuffer);
	if (mappedBuffer == NULL)
		return;

	char* writeCursor = mappedBuffer;

	// Copy the projection matrix at the beginning of the buffer
	{
		float translate = -0.5f * 2.f;
		const float L = 0.f;
		const float R = ImGui::GetIO().DisplaySize.x;
		const float B = ImGui::GetIO().DisplaySize.y;
		const float T = 0.f;
		const float mvp[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f, },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		};

		memcpy(writeCursor, &mvp[0], sizeof(mvp));
		writeCursor += sizeof(mvp);
	}

	// Copy the vertices and indices for each command list
	for (int n = 0; n < i_DrawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = i_DrawData->CmdLists[n];
		size_t verticesCount = cmd_list->VtxBuffer.size();
		size_t indicesCount = cmd_list->IdxBuffer.size();
		size_t verticesSize = verticesCount * sizeof(ImDrawVert);
		size_t indicesSize = indicesCount * sizeof(ImDrawIdx);

		// Copy the vertex data
		memcpy(writeCursor, &cmd_list->VtxBuffer[0], verticesSize);
		writeCursor += verticesSize;

		// Copy the index data
		memcpy(writeCursor, &cmd_list->IdxBuffer[0], indicesSize);
		writeCursor += indicesSize;
	}

	ID3D12GraphicsCommandList* commandList = CommandList;

	D3D12_VIEWPORT viewport;
	viewport.Width = ImGui::GetIO().DisplaySize.x;
	viewport.Height = ImGui::GetIO().DisplaySize.y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;

	// initialize commandlist for rendering UI
	commandList->RSSetViewports(1, &viewport);
	//commandList->OMSetRenderTargets(1, &RenderTarget, FALSE, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootSignature(RootSignature);
	commandList->SetPipelineState(PipelineState);
	// shader resource view
	commandList->SetDescriptorHeaps(1, &SrvHeap);
	// set graphics root descriptor table
	D3D12_GPU_DESCRIPTOR_HANDLE srvDescHandle = SrvHeap->GetGPUDescriptorHandleForHeapStart();
	commandList->SetGraphicsRootDescriptorTable(0, srvDescHandle);

	D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = UploadBuffer->GetGPUVirtualAddress();
	commandList->SetGraphicsRootConstantBufferView(1, bufferAddress);

	uint64_t readCursor = 64; // Our constant buffer takes 64 bytes - one mat4x4

	for (int n = 0; n < i_DrawData->CmdListsCount; n++)
	{
		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;

		const ImDrawList* cmd_list = i_DrawData->CmdLists[n];
		size_t verticesCount = cmd_list->VtxBuffer.size();
		size_t indicesCount = cmd_list->IdxBuffer.size();
		size_t verticesSize = verticesCount * sizeof(ImDrawVert);
		size_t indicesSize = indicesCount * sizeof(ImDrawIdx);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		vertexBufferView.BufferLocation = bufferAddress + readCursor;
		vertexBufferView.StrideInBytes = sizeof(ImDrawVert);
		vertexBufferView.SizeInBytes = (UINT)verticesSize;
		readCursor += verticesSize;

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = bufferAddress + readCursor;
		indexBufferView.SizeInBytes = (UINT)indicesSize;
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		readCursor += indicesSize;

		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const D3D12_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				commandList->RSSetScissorRects(1, &r);
				commandList->DrawIndexedInstanced(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += (int)verticesCount;
	}
}