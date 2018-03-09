// point light buffer pixel shader
// lights predefine
// this is include in each shaders that compute lights

// define amount of lights
#define			MAX_LIGHTS		64


// texture sampler for lights calculation
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
Texture2D tex_position		: register(t3);
//Texture2D tex_depth		: register(t4);	// To do
SamplerState tex_sample		: register(s0);

// point light struct definition
struct PointLight
{
	// ---
	float4		color;
	// ---
	float3		position;
	float		range;
	// ---
	float3		attenuate;
	float		pad;
};

// Pixel specs (for on particular pixel)
struct PixelData
{
	float4		diffuse_color;
	float4		specular_color;
	float4		normal;
	float4		position;
};

// result data for a pixel
struct PixelResult
{
	float4	diffuse;
	float4	specular;
	float4	ambient;
};


/////////////////////////////////////////
// constant buffer definition
cbuffer TransformBuffer : register(b0)
{
	// basics matrix for compute space position
	matrix	view;
	matrix	projection;
};


// contains data to render point lights
cbuffer LightData : register(b1)
{
	int				light_count;		// light to compute this frame
	PointLight		lights[MAX_LIGHTS];	// lights data
}

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

/////////////////////////////////////////
// helpers
#include "../Lib/Math.hlsli"

/////////////////////////////////////////
// Light computation function
void		ComputePointLight(in PointLight light, in PixelData pixel, out PixelResult result)
{
	// transform light position into clip space
	float4 light_pos = float4(light.position, 0.f);

	light_pos = mul(light_pos, view);
	light_pos = mul(light_pos, projection);

	float distance = length(pixel.position - light_pos);
	float factor = min(max(0.f, 1.f - InvertLerp(0.f, 20.f, distance)), 1.f);
	
	result.ambient = light.color * factor;
	result.specular = factor;
	result.diffuse = float4(0.f, 0.f, 0.f, 0.f);
}

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// setup the variable
	float4	eyePos = {0.f, 0.f, 0.f, 1.f};	// everything is in view space

	// fill the material
	PixelData pixel;
	pixel.diffuse_color		= tex_diffuse.Sample(tex_sample, input.uv);
	pixel.specular_color	= tex_specular.Sample(tex_sample, input.uv);
	pixel.normal			= tex_normal.Sample(tex_sample, input.uv);
	pixel.position			= tex_position.Sample(tex_sample, input.uv);

	PixelResult result;

	result.diffuse = tex_diffuse.Sample(tex_sample, input.uv);
	result.specular = float4(0.f, 0.f, 0.f, 0.f);
	result.ambient = float4(0.01f, 0.01f, 0.01f, 0.01f);

	if (light_count == 0)
	{
		result.diffuse = tex_diffuse.Sample(tex_sample, input.uv) * 0.1f;
	}
	else
	{
		// compute each lights
		[unroll(MAX_LIGHTS)]
		for (int i = 0; i < light_count; ++i)
		{
			ComputePointLight(lights[i], pixel, result);
		}
	}

	// return interpolated color
	// color
	float4 color = result.ambient + result.diffuse + result.specular;
	return color;
}