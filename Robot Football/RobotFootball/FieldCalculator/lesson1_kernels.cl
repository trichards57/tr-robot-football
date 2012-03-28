#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel void main(__constant float2 * ball, __constant float4 *field, __global float * out)
{
	const float ballWeight = 150;

	float2 gridPos = (float2)(get_global_id(0),get_global_id(1));

	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float2 realPos = gridPos * field[0].z;

	float2 diff = ball[0] - realPos;

	float dist = sqrt(diff.x*diff.x+diff.y*diff.y);

	float attractField = 0.5f * ballWeight * dist;

	float res = attractField;

	out[index] = res;
}

__kernel void colorize(__constant  float* max, __constant float *min, __global float *in, __global char* out)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));
	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float normalised = (in[index] - min[0]) / (max[0] - min[0]);

	// From http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV

	const float value = 1.0f;
	const float saturation = 1.0f;
	const float chroma = value * saturation;

	float hue = normalised * 300.0f;
	float hueDash = hue / 60.0f;
	float x = chroma * (1 - fabs(fmod(hueDash,2) - 1));

	float red;
	float green;
	float blue;

	if (hueDash < 1)
	{
		red = chroma;
		green = x;
		blue = 0;
	}
	else if (hueDash < 2)
	{
		red = x;
		green = chroma;
		blue = 0;
	}
	else if (hueDash < 3)
	{
		red = 0;
		green = chroma;
		blue = x;
	}
	else if (hueDash < 4)
	{
		red = 0;
		green = x;
		blue = chroma;
	}
	else if (hueDash < 5)
	{
		red = x;
		green = 0;
		blue = chroma;
	}
	else
	{
		red = chroma;
		green = 0;
		blue = x;
	}
	
	out[3*index] = (char)floor(blue * 255);
	out[3*index+1] = (char)floor(green * 255);
	out[3*index+2] = (char)floor(red * 255);
}