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

	// set lights data
	void			SetRange(float i_Range);
	void			SetColor(const DirectX::XMFLOAT4 & i_Color);
	void			SetIntensity(float i_Intensity);
	void			SetSpotAngle(float i_SpotAngle);
	void			SetSpotCutoff(float i_Cutoff);

	// light informations
	float				GetRange() const;
	DirectX::XMFLOAT4	GetColor() const;
	float				GetIntensity() const;
	float				GetSpotAngle() const;
	float				GetSpotCutoff() const;

	// setup the pipeline state
	void			PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const;
	void			PushLightDataToConstantBuffer() const;

	// initialize pipeline states into the render engine
	friend class DX12RenderEngine;
private:
	// specific light data
	struct LightData
	{
		// global data
		DirectX::XMFLOAT4	Color		= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		float				Intensity	= 1.f;	// intensity of the light
		float				Range		= 0.f;	// only for spot and point lights
		float				SpotAngle	= 0.f;	// only for spot lights
		float				SpotCutoff	= 0.f;	// only for spot lights
	};
	
	// light management
	ELightType			m_Type;	
	LightData			m_Data;	// data concerning light
	ADDRESS_ID			m_ConstantAddress;	// address for constant buffer
	bool				m_NeedBufferUpdate;

	// static data shared for each lights
	static DX12PipelineState *				s_PipelineState[ELightType::eCount];
	static DX12RootSignature *				s_RootSignature;	// root signature shared for each lights

	// internal static call
	static void		SetupPipelineStateObjects(ID3D12Device * i_Device);
};
