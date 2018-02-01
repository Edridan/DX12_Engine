// material buffer definition
// define all constant for the buffer that are pushed to shaders

/*
DirectX::XMFLOAT3		Ka, Kd, Ks, Ke;
BOOL					Map_A, Map_D, Map_S;
float					Ns;
*/

// texture sampler for material
Texture2D tex_ambient		: register(t0);
Texture2D tex_specular		: register(t1);
Texture2D tex_diffuse		: register(t2);
SamplerState tex_sample		: register(s0);

// sun light position static for now
// To do : make the sun move and add light casters
static const float3		SunLightPos = float3(0.f, 100.f, 0.f);
static const float4		SunLightCol = float4(1.f, 1.f, 1.f, 1.f);

cbuffer MaterialBuffer : register(b1)	// the material buffer is instanced on the buffer 1
{
	// default material implementation
	float4	ka;
	float4	kd;
	float4	ks;
	float4	ke;
	bool	map_a;
	bool	map_d;
	bool	map_s;
	float	ns;
};

//////////////////////////////////////////////////////////////////
// get different color attibute of the material
// this take texture or color depending the material parameters
float4 GetDiffuse(float2 uv)
{
	// if a texture exist
	if (map_d)	return tex_diffuse.Sample(tex_sample, uv).xyzw;
	else		return kd;
}

float4 GetAmbient(float2 uv)
{
	// if a texture exist
	if (map_d)	return tex_ambient.Sample(tex_sample, uv).xyzw;
	else		return ka;
}

float4 GetSpecular(float uv)
{
	if (map_s)	return tex_specular.Sample(tex_sample, uv).xyzw;
	else		return ka;
}
//////////////////////////////////////////////////////////////////

// compute color depending the materials in the constant buffer
// compute with uv and [optional] textures in the buffer
// (float4) : position of the pixel to compute
// (float3)	: normal of the pixel (Warning : we assumed that the normal is already normalized)
// (float2) : uv of the object
float4 ComputeColor(float4 pos, float3 normal, float3 cam_pos, float2 uv)
{
	// ambient color
	float4 ambient = GetAmbient(uv);

	// diffuse
	float3 lightDir = normalize(SunLightPos - pos.xyz);
	float diff = max(dot(normal, lightDir), 0.f);
	float4 diffuse = SunLightCol * (diff * GetDiffuse(uv));

	// specular


	float4 color = ambient + diffuse;

	return float4(1.f, 0.f, 0.f, 1.f);
}


// compute with vertex color so no textures will be used
// (float4) : position of the pixel to compute
// (float3)	: normal of the pixel (Warning : we assumed that the normal is already normalized)
// (float4) : color of the pixel
float4 ComputeColor(float4 pos, float3 normal, float3 cam_pos, float4 color)
{
	return color;
}