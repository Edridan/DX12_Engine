// debug shader used for drawing normals of objects rendered on the scene
// 
struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
};

float4 main() : SV_TARGET
{
	return max(float4(normal, 1.f), float4(-normal / 4.f, 1.f));
}