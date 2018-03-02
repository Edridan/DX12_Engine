// class for light rendering (Deferred shading)
#pragma once

#include "d3dx12.h"
#include "DX12ConstantBuffer.h"

// class predef
class DX12Shader;
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
	
	// setup the pipeline state
	void			PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const;
	void			PushDataOnConstantBuffer() const;

	// initialize pipeline states into the render engine
	friend class DX12RenderEngine;
private:
	DX12Shader			* m_Shader;	// this is a pixel shader to compute the light

	// these are pipeline state for lights
	struct PSO
	{
		DX12RootSignature *		RootSignature;
		DX12PipelineState *		PipelineState;
	};
	static PSO		s_PipelineState[ELightType::eCount];

	ADDRESS_ID		m_ConstantAddress;	// address for the light data

	// data for lights
	struct PointLightData
	{

	};

	union LightData
	{
		PointLightData PointLight;

	};

	LightData	m_LightData;

	// internal static call
	static void		SetupPipelineStateObjects(PSO * i_PipelineStateObjects);


};
