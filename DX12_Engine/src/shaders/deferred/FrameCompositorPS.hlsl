// frame compositior
// this take GBuffer and compute the final frame

// GBuffer frame management

// texture sampler for material
Texture2D tex_ambient		: register(t0);
Texture2D tex_specular		: register(t1);
Texture2D tex_diffuse		: register(t2);
SamplerState tex_sample		: register(s0);


struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	return float4(input.uv.xy, 1.f, 1.f);
}