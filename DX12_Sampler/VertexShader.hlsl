struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color: COLOR;
};

struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
};

// b0 buffer
cbuffer ConstantBuffer : register(b0)
{
	float4x4 transform;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 pos = input.pos;

	// Transform the vertex position into projected space.
	output.pos = mul(input.pos, transform);
	output.color = input.color;
	
	return output;
}