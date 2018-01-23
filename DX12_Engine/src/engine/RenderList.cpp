#include "RenderList.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"
#include "game/RenderComponent.h"
#include "game/Actor.h"

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
	// setup is call before reset
	if (m_CommandList != nullptr)
	{
		PRINT_DEBUG("[RenderList] call a setup before a reset");
		DEBUG_BREAK;
		return;
	}

	// setup the render list for this frame
	m_CommandList	= i_Setup.CommandList;
	m_Projection	= i_Setup.ProjectionMatrix;
	m_View			= i_Setup.ViewMatrix;

	ASSERT(m_CommandList != nullptr);
}

size_t RenderList::RenderComponentCount() const
{
	return m_Components.size();
}

void RenderList::PushRenderComponent(const RenderComponent * i_RenderComponent)
{
	m_Components.push_back(i_RenderComponent);
}

void RenderList::PushOnCommandList() const
{
	if (m_CommandList == nullptr)
	{
		PRINT_DEBUG("[RenderList] call push on command list before a setup call");
		DEBUG_BREAK;
		return;
	}

	// push all data on command list, also update if necessary the buffers
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	
	// -- Opaque geomtry -- //

	// precompute needed matrices and store them into a constant buffer
	DX12RenderEngine::DefaultConstantBuffer constantBuffer;	// constant buffer copied into the GPU memory
	XMStoreFloat4x4(&constantBuffer.m_View, XMMatrixTranspose(m_View));
	XMStoreFloat4x4(&constantBuffer.m_Projection, XMMatrixTranspose(m_Projection));

	// rendering opaque geometry
	for (size_t i = 0; i < m_Components.size(); ++i)
	{
		// retreive component data
		const RenderComponent * component = m_Components[i];

		// the component is not valid
		if (!component->IsValid())
		{
			PRINT_DEBUG("Error : component is not valid");
			DEBUG_BREAK;
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
		render.UpdateConstantBuffer(cbvAddress, constantBuffer);
		component->PushOnCommandList(m_CommandList);
	}
}

void RenderList::Reset()
{
	// reset variable, and allow to resetup and call the render list
	m_CommandList	= nullptr;
	m_Projection	= XMMatrixIdentity();
	m_View			= XMMatrixIdentity();

	// clear list of components
	m_Components.clear();
}