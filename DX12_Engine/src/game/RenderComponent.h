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
		const DX12MeshBuffer *					Mesh = nullptr;			// mesh pointer
		const DX12Material::DX12MaterialDesc *	Material = nullptr;		// if null, we take the default mesh material
	};

	RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor);
	RenderComponent(Actor * i_Actor);	// empty component
	~RenderComponent();

	// manage command list
	virtual void PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const;
	
	// information
	RenderPass		GetRenderPass() const;
	UINT64			GetRenderFlags() const;
	ADDRESS_ID		GetConstBufferAddress() const;

	// manage render stuff
	void					SetMaterial(const DX12Material::DX12MaterialDesc & i_Desc);
	DX12Material *			GetMaterial();
	void					SetMeshBuffer(const DX12MeshBuffer * i_Mesh);
	const DX12MeshBuffer *	GetMeshBuffer();

	// render management
	bool			IsRenderable() const;

private:
	// rendering
	const DX12MeshBuffer *		m_Mesh;
	DX12Material *				m_Material;	// material instance that manage the rendering pass

	// dx12
	ADDRESS_ID					m_ConstBuffer;	// const buffer for 3D matrices

	// informations
	RenderPass			m_RenderPass;
	RenderFlags			m_RenderFlags;

#ifdef WITH_EDITOR
	friend class UIActorBuilder;

private:
	// With editor only : draw component throught ui
	virtual void	DrawUIComponentInternal() override;
	void			DrawUIMaterial();
	void			DrawUIMesh();

#endif
};