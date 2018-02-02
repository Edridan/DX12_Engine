// debug shader used for drawing normals of objects rendered on the scene
// 
struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return max(float4(input.normal, 1.f), float4(-input.normal / 4.f, 1.f));
}