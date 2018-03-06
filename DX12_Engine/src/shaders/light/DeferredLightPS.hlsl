// point light buffer pixel shader
// this is applied using the GBuffer

// texture sampler for lights calculation
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
Texture2D tex_position		: register(t3);
//Texture2D tex_depth		: register(t4);	// To do :
SamplerState tex_sample		: register(s0);

// struct definition
// point light struct definition
struct PointLight
{
	float4		color;
	float3		position;
	float		range;
};


// basics matrix for compute space position
cbuffer TransformBuffer : register(b0)
{
	matrix	model;
	matrix	view;
	matrix	projection;
};

// struct for helper
struct Pixel
{

};


cbuffer LightData : register(b1)
{
	
}

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	// first quadran
	const float4 diffuse = tex_diffuse.Sample(tex_sample, input.uv);

	// color
	float4 color = lerp(float4(1.f, 1.f, 0.f, 1.f), diffuse, diffuse.r);
	return color;
}