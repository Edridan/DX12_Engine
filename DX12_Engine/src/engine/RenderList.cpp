#include "RenderList.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12ConstantBuffer.h"
#include "components/RenderComponent.h"
#include "engine/Actor.h"

RenderList::RenderList()
{
	m_Components.reserve(0x100);

	// create default variable
	Reset();
}

RenderList::~RenderList()
{
	// errors
	if (m_Components.size() != 0)
	{
		PRINT_DEBUG("[RenderList] Warning, there is still components ready to be rendered in a render list");
		DEBUG_BREAK;
	}
}

void RenderList::SetupRenderList(const RenderListSetup & i_Setup)
{
	// setup the render list for this frame
	m_ImmediateCommandList	= i_Setup.ImmediateCommandList;
	m_DeferredCommandList	= i_Setup.DeferredCommandList;
	m_Projection			= i_Setup.ProjectionMatrix;
	m_View					= i_Setup.ViewMatrix;
}

size_t RenderList::RenderComponentCount() const
{
	return m_Components.size();
}

void RenderList::RenderLight() const
{
	if (m_ImmediateCommandList == nullptr)
	{
		PRINT_DEBUG("[RenderList] call RenderLight before a setup call");
		DEBUG_BREAK;
		return;
	}
}

void RenderList::RenderGBuffer() const
{
	if (m_DeferredCommandList == nullptr)
	{
		PRINT_DEBUG("[RenderList] call RenderGBuffer list before a setup call");
		DEBUG_BREAK;
		return;
	}

	// push all data on command list, also update if necessary the buffers
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	
	// -- Opaque geomtry -- //

	// precompute needed matrices and store them into a constant buffer
	TransformConstantBuffer constantBuffer;	// constant buffer copied into the GPU memory
	XMStoreFloat4x4(&constantBuffer.m_View, XMMatrixTranspose(m_View));
	XMStoreFloat4x4(&constantBuffer.m_Projection, XMMatrixTranspose(m_Projection));

	// rendering opaque geometry
	for (size_t i = 0; i < m_Components.size(); ++i)
	{
		// retreive component data
		const RenderComponent * component = m_Components[i];

		// may some components are not renderable
		if (!component->IsRenderable())	
		{
			// if editor : some objects are currently under edition
#ifndef WITH_EDITOR
			PRINT_DEBUG("The component is not renderable");
			DEBUG_BREAK;
#endif
			continue;
		}

		// get component data to prepare for rendering
		Actor * actor = component->GetActor();
		ADDRESS_ID cbvAddress = component->GetConstBufferAddress();

		if (cbvAddress == UnavailableAdressId)
		{
			PRINT_DEBUG("Error : the actor %S have no CBV registered", actor->GetName().c_str());
			continue;
		}

		// retreive the model matrix
		XMStoreFloat4x4(&constantBuffer.m_Model, XMMatrixTranspose(actor->GetWorldTransform()));

		// update the constant buffer on GPU
		render.GetConstantBuffer(DX12RenderEngine::eTransform)->UpdateConstantBuffer(cbvAddress, &constantBuffer, sizeof(TransformConstantBuffer));
		component->PushOnCommandList(m_DeferredCommandList);
	}
}

void RenderList::PushRenderComponent(const RenderComponent * i_RenderComponent)
{
	// the component is not valid
	if (!i_RenderComponent->IsValid())
	{
		PRINT_DEBUG("Error : component is not valid");
		DEBUG_BREAK;
		return;
	}

	m_Components.push_back(i_RenderComponent);
	
	auto itr = m_RenderMeshData.begin();

	while (itr != m_RenderMeshData.end())
	{
		//if (((UINT64)(*itr)->first) == i_RenderComponent->GetMaterial()->GetId());
	}

}

void RenderList::PushLightComponent(const LightComponent * i_Component)
{
}

void RenderList::Reset()
{
	// reset variable, and allow to resetup and call the render list
	m_DeferredCommandList	= nullptr;
	m_ImmediateCommandList	= nullptr;

	// clear list of components
	m_Components.clear();
}