// class for light rendering (Deferred shading)
#pragma once

#include "d3dx12.h"
#include "DX12ConstantBuffer.h"

// class predef
class DX12Shader;
class DX12Mesh;
class DX12PipelineState;
class DX12RootSignature;

class DX12Light
{
public:
	enum ELightType
	{
		ePointLight,
		eCount,
	};

	DX12Light();
	~DX12Light();

	// informations
	ELightType		GetType() const;
	void			SetType(ELightType i_Type);

	// setup the pipeline state
	void			PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const;
	void			PushLightDataToConstantBuffer() const;
	void			Render(ID3D12GraphicsCommandList * i_CommandList) const;

	// initialize pipeline states into the render engine
	friend class DX12RenderEngine;
private:
	// light data
	struct LightData
	{
		DirectX::XMFLOAT4			Color;
	};
	
	// light management
	ELightType			m_Type;



	// static data shared for each lights
	struct PSO
	{
		DX12RootSignature *		RootSignature;
		DX12PipelineState *		PipelineState;
	};
	static PSO			s_PipelineState[ELightType::eCount];
	static DX12Mesh *	s_RectMesh;

	// internal static call
	static void		SetupPipelineStateObjects(PSO * i_PipelineStateObjects);


};
