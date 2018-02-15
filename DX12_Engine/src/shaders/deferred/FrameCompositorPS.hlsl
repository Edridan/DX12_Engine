// frame compositior
// this take GBuffer and compute the final frame

// GBuffer frame management


struct VS_OUTPUT
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD;
};

float4 main(const VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color
	return float4(input.uv.xy, 1.f, 1.f);
}