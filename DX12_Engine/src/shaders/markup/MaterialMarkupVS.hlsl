// GBuffer rendering vertex shader
// this takes data from a mesh and materials and push results into GBuffer
// GBuffers are : 
// - Normals	(float4)
// - Colors		(float4)
// - Specular	(float4)
// - Depth		(int64)

#include "../Lib/TransformBuffer.hlsli"

struct VS_INPUT
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float3 color	: COLOR;
};

struct VS_OUTPUT
{
	float4 position :		SV_POSITION;
	float3 normal :			NORMAL;
	float depth :			DEPTH_VIEW_SPACE;
};

VS_OUTPUT main(const VS_INPUT input)
{
	VS_OUTPUT output;

	float4 pos = float4(input.pos, 1.f);
	float4 norm = mul(model, float4(input.normal, 1.f));

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	// Return 
	output.position = pos;
	output.normal = float3(norm.xyz);

	return output;
}