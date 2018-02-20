// texture to display on the rect
Texture2D tex			: register(t0);
SamplerState tex_sample	: register(s0);

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// draw the texture on the rectangle
	return tex.Sample(tex_sample, input.uv);
}