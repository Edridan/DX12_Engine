// lighting data definition

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

// define
#define			MAX_LIGHT		64

// default light
class Light
{
public:
	// defines/enum
	enum ELightType
	{
		ePointLight,
		eSpotLight,
		eDirectionalLight,
		eLightTypeCount,
	};

	// destructor
	Light();	// called by children
	~Light();

	// information
	const XMFLOAT4 &	GetColor() const;
	ELightType			GetType() const;
	float				GetIntensity() const;

	// pointlight/spotlight data
	float				GetRange() const;
	float				GetQuadratic() const;
	float				GetLinear() const;
	float				GetConstant() const;

	// light management
	void				SetType(const ELightType & i_Type);
	void				SetColor(const XMFLOAT4 & i_Color);
	void				SetRange(float i_Range);
	void				SetIntensity(float i_Intensity);
	// pointlight/spotlight 
	void				SetConstant(float i_Constant);	// compute attenuation based on these params
	void				SetLinear(float i_Linear);
	void				SetQuadratic(float i_Quadratic);

	// To do: compute data to already created data (and just copy them into the list)

private:
	// global data for lights (each lights have these values)
	DirectX::XMFLOAT4		m_DiffuseColor;
	ELightType				m_LightType;
	float					m_Intensity;
	// point/spotlight data
	float					m_Range;
	// attenuation data
	float					m_Constant;
	float					m_Quadratic;
	float					m_Linear;

	// internal helpers

	void		ComputeLightData();	// this compute light data and save it to a pointer (fast retreive for render list)
};