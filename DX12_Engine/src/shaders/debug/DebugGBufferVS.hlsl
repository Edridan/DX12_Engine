// frame compositior
// this take GBuffer and compute the final frame

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

// buffer for rect
cbuffer Transform : register(b0)
{
	float2 pos;
}

VS_OUTPUT main(const VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = float4(input.pos.xyz + float3(pos, 0.f), 1.f);
	output.uv = input.uv;

	return output;
}