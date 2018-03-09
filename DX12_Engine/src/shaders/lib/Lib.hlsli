////////////////////////////////////////////////
// this header is included into the generated shader

///////////////////////////////////////////////
// Typedef
typedef matrix<float, 3, 3>		matrix3f;
typedef matrix<float, 2, 2>		matrix2f;


////////////////////////////////////////////////
// Clip/World transformation 

// convert clip space coordinate to view space
float4 ClipToView(float4 clip, float4x4 inverse_projection)
{
	// view space position
	float4 view = mul(inverse_projection, clip);
	// perspective projection
	view = view / view.w;

	return view;
}

float4 ScreenToView(float4 screen, float2 screen_dimension, float4x4 inverse_projection)
{
	// convert to normalized texture coordinate
	float2 tex_coord = screen.xy / screen_dimension;

	// convert to clip space
	float4 clip = float4(float2(tex_coord.x, 1.0f - tex_coord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip, inverse_projection);
}