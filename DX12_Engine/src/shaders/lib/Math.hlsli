// basic math library


/////////////////////////////////////////
// InvertLerp
float		InvertLerp(float min, float max, float value)
{
	return (value - min) / (max - min);
}