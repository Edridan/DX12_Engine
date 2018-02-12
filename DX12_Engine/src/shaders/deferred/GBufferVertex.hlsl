// GBuffer rendering vertex shader
// this takes data from a mesh and materials and push results into GBuffer
// GBuffers are : 
// - Normals	(float4)
// - Colors		(float4)
// - Specular	(float4)
// - Depth		(int64)

struct VS_INPUT
{

};

struct VS_OUTPUT
{
	float4 position :		SV_POSITION;
	float3 normal :			NORMAL;
	float depth :			DEPTH_VIEW_SPACE;
};

float4 main( const VS_INPUT input )
{

	return pos;
}