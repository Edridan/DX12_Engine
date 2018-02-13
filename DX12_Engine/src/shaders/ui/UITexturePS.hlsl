// render texture on a screen as UI
// this render a texture on a rect rendered on the buffer
Texture2D tex				: register(t0);
SamplerState tex_sample		: register(s0);

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// return the texture
	return tex.Sample(tex_sample, input.uv).xyzw;
}