// include render light lib
#include "Material.hlsli"
#include "GlobalBuffer.hlsli"

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// compute color of a pixel
	float3 color1 = tex_diffuse.Sample(tex_sample, input.uv).xyz;
	float3 color2 = tex_specular.Sample(tex_sample, input.uv).xyz;
	float3 color = lerp(color1, color2, sin(app_time));

	return float4(color, 1.f);
}