// include render light lib
#include "Material.hlsli"

// texture sampler for material
Texture2D tex_ambient		: register(t0);
//Texture2D tex_specular	: register(t1);
//Texture2D tex_diffuse		: register(t2);
SamplerState tex_sample		: register(s0);

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// compute color of a pixel
	float3 color = tex_ambient.Sample(tex_sample, input.uv).xyz;
	return float4(color, 1.f);
}