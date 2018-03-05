// point light buffer pixel shader
// this is applied using the GBuffer

#include "../Lib/LightBuffer.hlsli"

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	// first quadran
	const float4 diffuse = tex_diffuse.Sample(tex_sample, input.uv);

	// color
	float4 color = lerp(float4(1.f, 1.f, 0.f, 1.f), diffuse, diffuse.r);
	return color;
}