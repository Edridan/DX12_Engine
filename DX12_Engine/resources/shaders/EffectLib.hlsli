// shader library defined for shader computing
// main functions are coded in this file and 

// basic light calculation
// assuming that there is a directionnal light in the sky
static const float3 LightDir = float3(1.f, 1.f, -1.f);	// directionnal light vector

// return the color of the ambient depending the light dir and the normal surface
float4 ComputeDirectionalLight(float3 lightDir, float3 i_color, float3 i_normal /* need the transformed normal */, float i_ambient = 0.2f)
{
	// default
	float3 lightColor = float3(1.f, 1.f, 1.f);

	// ambient
	float3 ambient = i_ambient * lightColor;
	// diffuse
	float diff = max(dot(i_normal, LightDir), 0.f);
	float3 diffuse = diff * lightColor;

	float3 ret = (ambient + diffuse) * i_color;

	return float4(ret.xyz, 1.f);
}