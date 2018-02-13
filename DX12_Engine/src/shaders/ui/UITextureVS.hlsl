// this render a 2D with a texture on UI side
// used for debugging

struct VS_INPUT
{
	float3 pos		: POSITION;
	float2 uv		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 pos = float4(input.pos, 1.f);

	// give informations to pixel shader
	output.pos = pos;
	output.uv = input.uv;

	return output;
}