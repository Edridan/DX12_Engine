// include render light lib
#include "EffectLib.hlsli"
#include "Buffer.hlsli"

Texture2D tex			: register(t0);
SamplerState tex_sample	: register(s0);

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	matrix MyMat = model;

	// compute color of a pixel
	float3 color = tex.Sample(tex_sample, input.uv).xyz;
	float3 lightDir = LightDir.xyz;

	return ComputeDirectionalLight(lightDir, color, input.normal);
}