// GBuffer rendering pixel shader
// this takes data from a mesh and materials and push results into GBuffer
// GBuffers are : 
// - Normals	(float4)
// - Colors		(float4)
// - Specular	(float4)
// - Depth		(int64)


struct VS_OUTPUT
{
	float4 position :		SV_POSITION;
	float3 normal :			NORMAL;
	float depth :			DEPTH_VIEW_SPACE;
};

struct PS_OUTPUT
{
	float4	normal :		SV_Target0;
	float4 color :			SV_Target1;
	float4 specular :		SV_Target2;
	float depth :			SV_Target4;
};

float4 main(const VS_OUTPUT input)
{
	PS_OUTPUT output;

	return output;
}