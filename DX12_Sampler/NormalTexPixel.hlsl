Texture2D tex			: register(t0);
SamplerState tex_sample	: register(s0);

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return tex.Sample(tex_sample, input.texCoord);
}