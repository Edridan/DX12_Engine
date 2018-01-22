// default constant buffer in include file

// b0 buffer
cbuffer ConstantBuffer : register(b0)
{
	// basics matrix for compute space position
	matrix	model;
	matrix	view;
	matrix	projection;
	// other useful matrix for effects
	float	time;	// move this variable to the global buffer
};

// to do : create another buffer especially for the pixel shaders
// some buffers need to be a single place and are updated once
