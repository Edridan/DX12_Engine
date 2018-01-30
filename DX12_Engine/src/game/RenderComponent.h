// this is used for rendering object as mesh
// this is attached to an actor

// To do : create specific components for rendering mesh, post process and other stuff

#pragma once

#include "ActorComponent.h"
#include "dx12/d3dx12.h"
#include "dx12/DX12Utils.h"
#include "dx12/DX12Material.h"
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
		// mesh
		const DX12MeshBuffer *	Mesh = nullptr;			// mesh pointer
		const DX12Material::DX12MaterialDesc * Material = nullptr;	// if null, we take the default mesh material desc each render component have is own material instance
		// To do : delete this (this is contained in the material now)
		std::vector<DX12Texture *>	Textures;
	};

	RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor);
	~RenderComponent();

	// manage command list
	virtual void PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const;
	
	// information
	RenderPass		GetRenderPass() const;
	UINT64			GetRenderFlags() const;
	ADDRESS_ID		GetConstBufferAddress() const;

	// manage render stuff
	void			SetTexture(DX12Texture * i_Texture);
	//void			SetTexture(const DX12Texture * i_Texture, TextureDef i_Def);	// To do : implement

private:
	// rendering
	const DX12MeshBuffer *		m_Mesh;
	const DX12Material *		m_Material;

	// dx12
	ID3D12PipelineState *		m_PipelineState;
	ID3D12RootSignature *		m_RootSignature;
	ADDRESS_ID					m_ConstBuffer;	// const buffer for 3D matrices
	std::vector<DX12Texture *>	m_Textures;

	// informations
	RenderPass		m_RenderPass;
	RenderFlags		m_RenderFlags;
};