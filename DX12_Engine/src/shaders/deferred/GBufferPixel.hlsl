// GBuffer rendering pixel shader
// this takes data from a mesh and materials and push results into GBuffer
// GBuffers are : 
// - Normals	(float4)
// - Colors		(float4)
// - Specular	(float4)
// - Depth		(int64)

// include render light lib
#include "../lib/Material.hlsli"
#include "../lib/GlobalBuffer.hlsli"

struct VS_OUTPUT
{
	float4 position :		SV_POSITION;
	float3 normal :			NORMAL;
	float depth :			DEPTH_VIEW_SPACE;
};

struct PS_OUTPUT
{
	float4 normal :			SV_Target0;
	float4 diffuse :		SV_Target1;
	float4 specular :		SV_Target2;
	//float depth :			SV_Target4;	// To do : use a render target to fill depth buffer
};

PS_OUTPUT main(const VS_OUTPUT input)
{
	PS_OUTPUT output;

	/////////////////////////////////////////////
	// update the normal buffer
	output.normal = float4(input.normal.xyz, 1.f);
	
	/////////////////////////////////////////////
	// update the diffuse buffer
	output.diffuse = float4(0.f, 1.f, 0.f, 1.f);

	/////////////////////////////////////////////
	// update the specular buffer
	output.specular = float4(0.f, 0.f, 1.f, 1.f);

	/////////////////////////////////////////////
	// update the depth buffer
	// To do ...

	return output;
}