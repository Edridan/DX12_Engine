// include render light lib
#include "Material.hlsli"

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
	float3 color;

	if (map_a)
	{
		color = float3(0.f, 1.f, 0.f);
	}
	else
	{
		color = float3(0.f, 0.f, 1.f);
	}
	// compute color of a pixel
	//tex.Sample(tex_sample, input.uv).xyz;
	
	return float4(color, 1.f);
}