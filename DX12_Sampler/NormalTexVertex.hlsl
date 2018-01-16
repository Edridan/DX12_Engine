struct VS_INPUT
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

// b0 buffer
cbuffer ConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 pos = float4(input.pos, 1.f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	// Return 
	output.pos = pos;
	output.uv = input.uv;

	return output;
}