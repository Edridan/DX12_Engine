// this is used for rendering object as mesh
// this is attached to an actor

// To do : create specific components for rendering mesh, post process and other stuff

#pragma once

#include "ActorComponent.h"
#include "dx12/d3dx12.h"
#include "dx12/DX12Utils.h"
#include <vector>
#include <string>

class DX12MeshBuffer;
class DX12Texture;
class DX12Mesh;
class Actor;

class RenderComponent : public ActorComponent
{
public:
	// define the render pass of the component
	enum RenderPass
	{
		eOpaqueGeometry,	// default
		eSemiTransparent,
	};

	// render flags for specific rendering
	enum RenderFlags
	{
		eDepthEnabled		= 1 << 0,
	};

	// struct definition
	struct RenderComponentDesc
	{
		std::wstring	Mesh = L"";				// mesh filepath
		UINT			SubMeshId = ((UINT)-1);	// submeshes id
	};

	RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor);
	~RenderComponent();

	// manage command list
	virtual void PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);
	
	// information
	RenderPass		GetRenderPass() const;
	UINT64			GetRenderFlags() const;
	ADDRESS_ID		GetConstBufferAddress() const;

private:
	// dx12 rendering
	const DX12MeshBuffer *		m_Mesh;
	ID3D12PipelineState *		m_PipelineState;
	ID3D12RootSignature *		m_RootSignature;
	ADDRESS_ID					m_ConstBuffer;	// const buffer
	std::vector<DX12Texture *>	m_Textures;

	// informations
	RenderPass		m_RenderPass;
	RenderFlags		m_RenderFlags;
};