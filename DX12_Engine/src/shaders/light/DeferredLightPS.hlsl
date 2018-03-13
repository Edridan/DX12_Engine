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

// global unchanged var
static const float4 eye_pos = float4(0.f, 0.f, 0.f, 1.f);

// point light struct definition
struct PointLight
{
	// ---
	float4		color;
	// ---
	float3		position;
	float		constant;
	// ---
	float		lin;		// linear
	float		quad;		// quadratic
	float		range;		// range
	// ---		offset : 11 remaining : 5 (16)
	float		padding[5];	// padding
};

// Pixel specs (for on particular pixel)
struct PixelData
{
	float4		diffuse_color;
	float4		specular_color;
	float4		normal;
	float4		position;
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
float3		ComputePointLight(in PointLight light, in PixelData pixel, float3 view_dir)
{
	// transform light position into clip space
	float4 light_pos = float4(light.position, 1.f);
	light_pos = mul(light_pos, view);
	light_pos = mul(light_pos, projection);

	// retreive the light direction and range between the 
	const float3 light_diff = light_pos.xyz - pixel.position.xyz;
	const float distance = length(light_diff);

	if (distance < light.range)
	{
		// diffuse light calculation
		const float3 light_dir = normalize(light_diff);
		float3 light_diffuse = max(dot(pixel.normal.xyz, light_dir), 0.f) * pixel.diffuse_color.rgb * light.color.rgb;

		// specular calculation
		const float3 half_way_dir = normalize(light_dir + view_dir);
		const float spec = pow(max(dot(pixel.normal.xyz, half_way_dir), 0.f), 16.f);
		float4 specular = light.color * spec * pixel.specular_color;

		// attenuation
		float attenuation = 1.f / (light.constant + light.lin * distance + light.quad * (distance * distance));
		return (specular.rgb * attenuation) + (light_diffuse.rgb * attenuation);
	}
	return float3(0.f, 0.f, 0.f);
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

	// compute pixel if necessary (diffuse exist)
	if (pixel.diffuse_color.a != 0.f)
	{

		float3 view_dir = normalize(eye_pos.xyz - pixel.position.xyz);
		float3 lighting = float3(0.f, 0.f, 0.f);

		if (light_count == 0)
		{
			lighting = tex_diffuse.Sample(tex_sample, input.uv).rgb;
		}
		else
		{
			// compute each lights
			[unroll(MAX_LIGHTS)]
			for (int i = 0; i < light_count; ++i)
			{
				lighting += ComputePointLight(lights[i], pixel, view_dir);
			}
		}

		// return interpolated color
		return float4(lighting, 1.f);
	}

	return pixel.diffuse_color;
}