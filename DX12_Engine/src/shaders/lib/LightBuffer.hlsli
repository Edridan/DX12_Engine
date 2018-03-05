// light buffer specs
// this need to be included by all pixel shader for lights buffer

// texture sampler for material
Texture2D tex_normal		: register(t0);
Texture2D tex_diffuse		: register(t1);
Texture2D tex_specular		: register(t2);
Texture2D tex_position		: register(t3);
//Texture2D tex_depth		: register(t4);	// To do :
SamplerState tex_sample		: register(s0);

cbuffer LightData : register(b1)
{
	float4			light_color;
	float			intensity;
	float			range;
	float			spot_angle;
	float			spot_cutoff;
}