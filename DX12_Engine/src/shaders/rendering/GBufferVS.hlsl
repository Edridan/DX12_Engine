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
	float2 uv		: TEXCOORD;
};

struct VS_OUTPUT
{
	// data for pipeline
	float4 position :		SV_POSITION;
	// GBuffer needed data
	float4 view_position :	POSITION;
	float3 normal :			NORMAL;
	float depth :			DEPTH_VIEW_SPACE;
};

VS_OUTPUT main( const VS_INPUT input )
{
	VS_OUTPUT output;

	float4 pos = float4(input.pos, 1.f);
	// compute normal using matrix 3x3 (removing the position)
	float3x3 mod;
	mod[0] = model[0].xyz;
	mod[1] = model[1].xyz;
	mod[2] = model[2].xyz;
	float3 norm = normalize(mul(input.normal, mod));

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	// Return 
	output.position = pos;
	// data for GBuffer
	output.normal = float3(norm.xyz);
	output.view_position = pos;
	output.depth = 0.1f;		// dummy

	return output;
}