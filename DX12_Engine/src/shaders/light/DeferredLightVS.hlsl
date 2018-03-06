// this is a vertex shader used to render lights in deferred rendering
// this draw a rect and push data to render the light


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

// light computation


VS_OUTPUT main(const VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = float4(input.pos.xyz, 1.f);
	output.uv = input.uv;

	return output;
}