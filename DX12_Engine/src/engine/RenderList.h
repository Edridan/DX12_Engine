// a render list containing all rendering components for the frame
// this include the logic to render components (that are only data)
// this will bind needed resources, start multi threaded rendering and stuff
// this manage render pass and resource binding

#pragma once

#include "dx12/d3dx12.h"
#include "dx12/DX12Utils.h"
#include "engine/Light.h"
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

// class predef
class RenderComponent;	// this is the basis component to render objects
class LightComponent;
class Actor;
class DX12Material;
class DX12Mesh;

class RenderList
{
public:
	struct RenderListSetup
	{
		// matrix for rendering
		XMMATRIX		ViewMatrix;
		XMMATRIX		ProjectionMatrix;
		// dx12
		ID3D12GraphicsCommandList *		DeferredCommandList = nullptr;	// command list to render
		ID3D12GraphicsCommandList *		ImmediateCommandList = nullptr;	// command list to render
	};

	// Constant buffer definition
	__declspec(align(16)) struct TransformConstantBuffer
	{
		// 3D space computing
		DirectX::XMFLOAT4X4		m_Model;
		DirectX::XMFLOAT4X4		m_View;
		DirectX::XMFLOAT4X4		m_Projection;
		// other
		FLOAT					m_Time;
		DirectX::XMFLOAT3		m_CameraForward;
	};

	// render list
	RenderList();
	~RenderList();

	// management
	void	SetupRenderList(const RenderListSetup & i_Setup);
	size_t	RenderComponentCount() const;
	void	RenderGBuffer() const;	// render meshes, opaque geometry
	void	RenderLight() const;	// render lights and immediate pass
	void	Reset();	// reset render list var

	// add rendering objects
	void	PushRenderComponent(const RenderComponent * i_Component);
	void	PushLightComponent(const LightComponent * i_Component);

private:
	// components to render
	std::vector<const RenderComponent *>		m_RenderComponents;
	std::vector<const LightComponent *>			m_LightComponents;

	// light management
	__declspec(align(16)) struct LightData
	{
		DirectX::XMFLOAT4		Color;
		DirectX::XMFLOAT3		Position;
		float					Range;
		DirectX::XMFLOAT3		Attenuate;
		float					Pad;
	};
	
	__declspec(align(16)) struct LightDesc
	{
		int			LightCount;
		LightData	Data[MAX_LIGHT];
	};

	const size_t					m_MaxLight;
	LightDesc *						m_LightsData;

	DX12Mesh *			m_RectMesh;
	ADDRESS_ID			m_LightConstantAddress;
	ADDRESS_ID			m_LightCameraConstAddress;

	// to do : render objects per materials and not loop between components
	// materials management
	struct RenderMeshData
	{
		Actor *				m_Actor = nullptr;
		DX12Mesh		 *	m_Mesh = nullptr;
		ADDRESS_ID			m_ConstBuffer = UnavailableAdressId;
	};

	std::vector<std::pair< DX12Material *, std::vector<RenderMeshData>>>	m_RenderMeshData;

	// rendering purpose
	XMMATRIX	m_View;
	XMMATRIX	m_Projection;
	// dx12
	ID3D12GraphicsCommandList *	m_ImmediateCommandList;
	ID3D12GraphicsCommandList *	m_DeferredCommandList;

};