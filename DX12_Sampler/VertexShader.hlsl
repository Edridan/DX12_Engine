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

cbuffer ConstantBuffer : register(b0)
{
	float4x4 wvpMat;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 pos = input.pos;
	
	// model view projection calculation
	/*pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);*/

	output.pos = pos;
	output.color = input.color;
	
	return output;
}