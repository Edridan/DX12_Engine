#include "RenderList.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12RootSignature.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12ConstantBuffer.h"
#include "dx12/DX12RenderTarget.h"
#include "components/RenderComponent.h"
#include "resource/DX12Mesh.h"
#include "engine/Actor.h"

RenderList::RenderList()
	:m_MaxLight(MAX_LIGHT)
{
	// compilation assert
	static_assert(sizeof(RenderList::LightData) == sizeof(PointLightData), "The point light data structures need to be the same size (until some errors during lights computation will comes)");
	static_assert(sizeof(RenderList::LightData) == sizeof(SpotLightData), "The spot light data structures need to be the same size (until some errors during lights computation will comes)");
	static_assert(sizeof(RenderList::LightData) == sizeof(DirectionnalLightData), "The spot light data structures need to be the same size (until some errors during lights computation will comes)");

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	m_RenderComponents.reserve(0x100);
	m_LightComponents.reserve(m_MaxLight);
	m_RectMesh = render.GetRectMesh();	// retreive the mesh for draw full frame

	m_LightConstantAddress		= render.GetConstantBuffer(DX12RenderEngine::eLight)->ReserveVirtualAddress();
	m_LightCameraConstAddress	= render.GetConstantBuffer(DX12RenderEngine::eGlobal)->ReserveVirtualAddress();

	// create light data storage
	m_LightsData = new LightDesc;

	// create default variable
	Reset();
}

RenderList::~RenderList()
{
	// errors
	if (m_RenderComponents.size() != 0)
	{
		PRINT_DEBUG("[RenderList] Warning, there is still components ready to be rendered in a render list");
		DEBUG_BREAK;
	}
	
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	render.GetConstantBuffer(DX12RenderEngine::eLight)->ReleaseVirtualAddress(m_LightConstantAddress);
	render.GetConstantBuffer(DX12RenderEngine::eGlobal)->ReleaseVirtualAddress(m_LightCameraConstAddress);

	// clean resources
	delete m_LightsData;	// delete the array
}

void RenderList::SetupRenderList(const RenderListSetup & i_Setup)
{
	// setup the render list for this frame
	m_ImmediateCommandList	= i_Setup.ImmediateCommandList;
	m_DeferredCommandList	= i_Setup.DeferredCommandList;
	m_Projection			= i_Setup.ProjectionMatrix;
	m_CameraPosition		= i_Setup.CameraPosition;
	m_View					= i_Setup.ViewMatrix;
}

size_t RenderList::RenderComponentCount() const
{
	return m_RenderComponents.size();
}

void RenderList::RenderLight() const
{
	if (m_ImmediateCommandList == nullptr)
	{
		PRINT_DEBUG("[RenderList] call RenderLight before a setup call");
		DEBUG_BREAK;
		return;
	}

	// -- Render Lights -- //
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	for (size_t i = 0; i < m_LightComponents.size(); ++i)
	{
		const LightComponent * lightComponent = m_LightComponents[i];
		Light * light				= lightComponent->GetLight();
		Actor * actor				= lightComponent->GetActor();

		switch (lightComponent->GetLightType())
		{
			// fill data for the point light
		case Light::ELightType::ePointLight:
		{
			PointLightData * desc = (PointLightData *)&m_LightsData->Data[i];

			// position transform
			XMMATRIX actorWorldTransform = actor->GetWorldTransform();
			XMFLOAT4X4 worldTransform;
			XMStoreFloat4x4(&worldTransform, actorWorldTransform);

			// fill light description
			desc->Position		= XMFLOAT3(&worldTransform._41);
			desc->Color			= light->GetColor();
			desc->Constant		= light->GetConstant();
			desc->Linear		= light->GetLinear();
			desc->Quadratic		= light->GetQuadratic();
			desc->Range			= light->GetRange();
			desc->Type			= light->GetType();
		}
		break;
			// fill data for the spot light
		case Light::ELightType::eSpotLight:
		{
			SpotLightData * desc = (SpotLightData *)&m_LightsData->Data[i];

			// position transform
			XMMATRIX actorWorldTransform = actor->GetWorldTransform();
			XMFLOAT4X4 worldTransform;
			XMStoreFloat4x4(&worldTransform, actorWorldTransform);

			// fill light description
			desc->Position		= XMFLOAT3(&worldTransform._41);
			desc->Color			= light->GetColor();
			desc->Constant		= light->GetConstant();
			desc->Linear		= light->GetLinear();
			desc->Quadratic		= light->GetQuadratic();
			desc->Range			= light->GetRange();

			desc->Direction		= XMFLOAT3(&worldTransform._31);
			desc->SpotAngle		= light->GetSpotAngle();
			desc->OuterCutoff	= light->GetEdgeCutoff();
			desc->Type			= light->GetType();
		}
		break;
			// fill data for directionnal lights
		case Light::ELightType::eDirectionalLight:
		{
			DirectionnalLightData * desc = (DirectionnalLightData *)&m_LightsData->Data[i];

			// position transform
			XMMATRIX actorWorldTransform = actor->GetWorldTransform();
			XMFLOAT4X4 worldTransform;
			XMStoreFloat4x4(&worldTransform, actorWorldTransform);

			desc->Position		= XMFLOAT3(&worldTransform._41);
			desc->Color			= light->GetColor();

			desc->Direction		= XMFLOAT3(&worldTransform._31);
			desc->Type			= light->GetType();
		}
		break;
			// error if fall in that case
		default:
			ASSERT_ERROR("Error on light type");
			break;
		}
	}

	// update constant buffer
	// transform buffer
	__declspec(align(16)) struct SceneDataBuffer
	{
		// data for light computation
		DirectX::XMFLOAT3		m_CameraPos;
		int						m_LightCount;
	};

	SceneDataBuffer buffer;
	//XMStoreFloat4x4(&buffer.m_View, XMMatrixTranspose(m_View));
	//XMStoreFloat4x4(&buffer.m_Projection, XMMatrixTranspose(m_Projection));
	buffer.m_CameraPos = m_CameraPosition;
	buffer.m_LightCount = (int)m_LightComponents.size();

	render.GetConstantBuffer(DX12RenderEngine::eGlobal)->UpdateConstantBuffer(m_LightCameraConstAddress, &buffer, sizeof(SceneDataBuffer));
	render.GetConstantBuffer(DX12RenderEngine::eLight)->UpdateConstantBuffer(m_LightConstantAddress, m_LightsData, ((int)m_LightComponents.size()) * sizeof(LightData));

	// setup pipeline state objects
	DX12RootSignature * rootSignature = render.GetLightRootSignature();
	DX12PipelineState * pipelineState = render.GetLightPipelineState();

	// setup rendering pipeline for lights
	m_ImmediateCommandList->SetGraphicsRootSignature(rootSignature->GetRootSignature());
	m_ImmediateCommandList->SetPipelineState(pipelineState->GetPipelineState());

	DX12RenderTarget * rt[DX12RenderEngine::eRenderTargetCount]
	{
		render.GetRenderTarget(DX12RenderEngine::eNormal),
		render.GetRenderTarget(DX12RenderEngine::eDiffuse),
		render.GetRenderTarget(DX12RenderEngine::eSpecular),
		render.GetRenderTarget(DX12RenderEngine::ePosition)
	};

	ID3D12DescriptorHeap * descriptors = nullptr;

	for (UINT i = 0; i < DX12RenderEngine::eRenderTargetCount; ++i)
	{
		// bind render targets as textures
		descriptors = rt[i]->GetShaderResourceDescriptorHeap()->GetDescriptorHeap();
		// update the descriptor for the resources
		m_ImmediateCommandList->SetDescriptorHeaps(1, &descriptors);
		m_ImmediateCommandList->SetGraphicsRootDescriptorTable(i, rt[i]->GetShaderResourceDescriptorHeap()->GetGPUDescriptorHandle(render.GetFrameIndex()));
	}

	// bind buffers
	m_ImmediateCommandList->SetGraphicsRootConstantBufferView(4, render.GetConstantBuffer(DX12RenderEngine::eGlobal)->GetUploadVirtualAddress(m_LightCameraConstAddress));
	m_ImmediateCommandList->SetGraphicsRootConstantBufferView(5, render.GetConstantBuffer(DX12RenderEngine::eLight)->GetUploadVirtualAddress(m_LightConstantAddress));

	// draw rect mesh
	m_RectMesh->PushOnCommandList(m_ImmediateCommandList);
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
	
	// -- Opaque geometry -- //

	// precompute needed matrices and store them into a constant buffer
	TransformConstantBuffer constantBuffer;	// constant buffer copied into the GPU memory
	XMStoreFloat4x4(&constantBuffer.m_View, XMMatrixTranspose(m_View));
	XMStoreFloat4x4(&constantBuffer.m_Projection, XMMatrixTranspose(m_Projection));

	// rendering opaque geometry
	for (size_t i = 0; i < m_RenderComponents.size(); ++i)
	{
		// retreive component data
		const RenderComponent * component = m_RenderComponents[i];

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
		Actor * actor			= component->GetActor();
		ADDRESS_ID cbvAddress	= component->GetConstBufferAddress();

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

	m_RenderComponents.push_back(i_RenderComponent);
}

void RenderList::PushLightComponent(const LightComponent * i_LightComponent)
{
	// max lights
	if (m_LightComponents.size() > m_MaxLight)		return;

	if (!i_LightComponent->IsValid())
	{
		PRINT_DEBUG("Error : light component unavailable");
		DEBUG_BREAK;
		return;
	}

	// push the component
	m_LightComponents.push_back(i_LightComponent);
}

void RenderList::Reset()
{
	// reset variable, and allow to resetup and call the render list
	m_DeferredCommandList	= nullptr;
	m_ImmediateCommandList	= nullptr;

	// clear list of components
	m_RenderComponents.clear();
	m_LightComponents.clear();
}