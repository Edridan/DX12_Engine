// include render light lib
#include "Material.hlsli"
#include "GlobalBuffer.hlsli"

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float4 location	: LOCATION;
	float2 uv		: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// compute color of a pixel
	float4 sunpos = float4(100.f, 0.f, 0.f, 1.f);

	return ComputeColor(input.location, input.normal, cam_pos.xyz, input.uv, sunpos);

	//return float4(color, 1.f);
}