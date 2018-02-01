// material buffer definition
// define all constant for the buffer that are pushed to shaders

/*
DirectX::XMFLOAT3		Ka, Kd, Ks, Ke;
BOOL					Map_A, Map_D, Map_S;
float					Ns;
*/

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