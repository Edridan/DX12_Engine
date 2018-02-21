// frame compositior
// this take GBuffer and compute the final frame

// GBuffer frame management

// texture sampler for material
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
Texture2D tex_position		: register(t3);
//Texture2D tex_depth		: register(t4);
SamplerState tex_sample		: register(s0);

// target compositor buffer
cbuffer FrameBuffer : register(b0)
{
	float4 clear_color;

}

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	//float4 color = float4(input.uv.xy, 0.f, 1.f);

	// first quadran
	const float4 diffuse = tex_diffuse.Sample(tex_sample, input.uv);

	// color
	float4 color = lerp(clear_color, diffuse, diffuse.a);
	return color;
}