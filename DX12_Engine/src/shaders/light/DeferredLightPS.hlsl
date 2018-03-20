// point light buffer pixel shader
// lights predefine
// this is include in each shaders that compute lights

// define amount of lights
#define			MAX_LIGHTS		64

#define POINT_LIGHT				0
#define SPOT_LIGHT				1
#define	DIRECTIONNAL_LIGHT		2

// texture sampler for lights calculation
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
Texture2D tex_position		: register(t3);
//Texture2D tex_depth		: register(t4);	// To do
SamplerState tex_sample		: register(s0); 

// size of this structure mush be 128 bytes
struct LightData
{
	int			type;			// light type
	// pack next one data : (do not use array because of the padding)
	float3		padding_1;	// first float4 completion
	float4		padding_2;
	float4		padding_3;
	float4		padding_4;
	float4		padding_5;
	float4		padding_6;
	float4		padding_7;
	float4		padding_8;
};

// point light struct definition
struct PointLight
{
	int			type;	// first val always type
    float3		position;
	// ---
	float4		color;
	// ---
	float		constant;
	float		lin;		// linear
	float		quad;		// quadratic
	float		range;		// range
};

// spot light struct definition
struct SpotLight
{
	int			type;	// first val always type
    float3		position;
	// ---
	float4		color;
	// ---
	float		constant;
	float		lin;		// linear
	float		quad;		// quadratic
	float		range;		// range
	// ---
	float3		direction;
	float		theta;
	// -- First 16 bytes
	float		outer_cutoff;
	float		inner_cutoff;
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
cbuffer SceneData : register(b0)
{
	// basics matrix for compute space position
	float3	camera_pos;
	int		light_count;		// light to compute this frame
};


// contains data to render point lights
cbuffer LightData : register(b1)
{
	LightData		lights[MAX_LIGHTS];	// lights data
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
float3		ComputePointLight(in PointLight light, in PixelData pixel)
{
	float4 light_pos = float4(light.position, 1.f);

	// retreive the light direction and range between the 
	const float3 light_diff = light_pos.xyz - pixel.position.xyz;
	const float distance = length(light_diff);

	float3 ret_value = float3(0.f, 0.f, 0.f);

	if (distance < light.range)
	{
		// diffuse light calculation
		const float3 light_dir = normalize(light_diff);
		const float diff = max(dot(pixel.normal.xyz, light_dir), 0.f);
		const float3 light_diffuse = pixel.diffuse_color.rgb * diff * light.color.rgb;

		// specular calculation
		const float3 view_dir = normalize(camera_pos.xyz - pixel.position.xyz);
		const float3 reflect_dir = reflect(-light_dir, pixel.normal.xyz); 
		const float spec = pow(max(dot(view_dir, reflect_dir), 0.f), 32.f);
		const float4 specular = light.color * spec * pixel.specular_color;

		// attenuation
		float attenuation = 1.f / (light.constant + light.lin * distance + light.quad * (distance * distance));
		ret_value = (specular.rgb * attenuation) + (light_diffuse.rgb * attenuation);
	}

	return ret_value;
}

float3		ComputeSpotLight(in SpotLight light, in PixelData pixel)
{
	float4 light_pos = float4(light.position, 1.f);

	// retreive the light direction and range between the 
	const float3 light_diff = light_pos.xyz - pixel.position.xyz;
	const float distance = length(light_diff);
	const float3 light_dir = normalize(light_diff);

	// check if lighting is inside the spotlight cone
	const float theta = dot(light_dir, normalize(-light.direction));

	float3 ret_value = float3(0.f, 0.f, 0.f);

	// radian angles
	if (theta > light.theta)
	{
		// diffuse
		const float diff = max(dot(pixel.normal.xyz, light_dir), 0.0);
		const float3 light_diffuse = pixel.diffuse_color.rgb * diff * light.color.rgb;

		// specular calculation
		const float3 view_dir = normalize(camera_pos.xyz - pixel.position.xyz);
		const float3 reflect_dir = reflect(-light_dir, pixel.normal.xyz);
		const float spec = pow(max(dot(view_dir, reflect_dir), 0.f), 32.f);
		const float4 specular = light.color * spec * pixel.specular_color;

		// soft edges
		const float theta = dot(light_dir, normalize(-light.direction));
		const float epsilon = (light.theta - light.outer_cutoff);
		const float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.f, 1.f);

		// attenuation
		float attenuation = 1.f / (light.constant + light.lin * distance + light.quad * (distance * distance));
		ret_value = (specular.rgb * attenuation * intensity) + (light_diffuse.rgb * attenuation * intensity);
	}

	return ret_value;
}

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// fill the material
	PixelData pixel;
	pixel.diffuse_color		= tex_diffuse.Sample(tex_sample, input.uv);
	pixel.specular_color	= tex_specular.Sample(tex_sample, input.uv);
	pixel.normal			= tex_normal.Sample(tex_sample, input.uv);
	pixel.position			= tex_position.Sample(tex_sample, input.uv);

	// compute pixel if necessary (diffuse exist)
	if (pixel.diffuse_color.a != 0.f)
	{
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
				switch (lights[i].type)
				{
				case POINT_LIGHT:
					lighting += ComputePointLight((PointLight)lights[i], pixel);
					break;
				case SPOT_LIGHT:
					lighting += ComputeSpotLight((SpotLight)lights[i], pixel);
					break;
				case DIRECTIONNAL_LIGHT:
					break;
				}
				
			}
		}

		// return interpolated color
		return float4(lighting, 1.f);
	}

	return pixel.diffuse_color;
}