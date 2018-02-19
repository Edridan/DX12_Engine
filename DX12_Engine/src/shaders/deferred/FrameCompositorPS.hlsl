// frame compositior
// this take GBuffer and compute the final frame

// GBuffer frame management

// texture sampler for material
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
SamplerState tex_sample		: register(s0);


struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	//float4 color = float4(input.uv.xy, 0.f, 1.f);
	// quadrands

	// first quadran

	float4 color = tex_normal.Sample(tex_sample, input.uv);



	return color;
}