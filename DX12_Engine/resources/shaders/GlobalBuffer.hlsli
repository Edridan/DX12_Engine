// this is a global shared buffer used for each render objects
// this buffer is updated each frame and can bu used for algorithm purpose too
// include this files if you want to use global shader
cbuffer GlobalBuffer : register(b1)
{
	// other useful matrix for effects
	float	app_time;		// application time (from engine initialization)
	float	frame_time;		// frame time
	float4	cam_pos;		// position of the camera for lighting computation
};