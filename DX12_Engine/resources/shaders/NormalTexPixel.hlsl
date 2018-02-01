// include render light lib
#include "Material.hlsli"


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