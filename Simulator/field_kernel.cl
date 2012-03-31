#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define BALL_WEIGHT 10
#define OBSTACLE_WEIGHT 100
#define OBSTACLE_SIGMA 5

float fieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers);

__kernel void main(float2 ball, float fieldResolution, __constant float2 *basicRepulsers, __global float * out)
{
	float2 gridPos = (int)(get_global_id(0),get_global_id(1));

	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float2 realPos = convert_float2(gridPos) * fieldResolution;

	float res = fieldAtPoint(realPos, ball, basicRepulsers);
	
	out[index] = res;
}

__kernel void fieldAtPoints(float2 ball, __constant float2 *fieldPoints, __constant float2 *basicRepulsers, __global float * out)
{
	size_t pointId = get_global_id(0);

	out[pointId] = fieldAtPoint(fieldPoints[pointId], ball, basicRepulsers);
}

float fieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers)
{
	float dist = distance(ball, realPos);

	float attractField = 0.5f * BALL_WEIGHT * dist;
	float repField = 0;

	float2 diff;
	
	for (int i = 0; i < 9; i++)
	{
		diff = basicRepulsers[i] - realPos;
		// native_exp: proabably less accurate, but uses far fewer GPRs, so runs much faster...
		repField += 0.5f * OBSTACLE_WEIGHT * native_exp(-((diff.x*diff.x)/(2*OBSTACLE_SIGMA) + (diff.y*diff.y)/(2*OBSTACLE_SIGMA)));
	}

	return attractField + repField;
}

__kernel void colorize(float max, float min, __global float *in, __global char* levelOut)
{
	/*float logMax = log(max[0]);
	float logMin = log(min[0]);*/

	size_t gridWidth = get_global_size(0);
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));
	size_t index = gridPos.x + gridPos.y * gridWidth;

	//float normalised = (log(in[index]) - logMin) / (logMax - logMin);
	float normalised = (in[index] - min) / (max - min);

	// From http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV

	const float value = 1.0f;
	const float saturation = 1.0f;
	const float chroma = value * saturation;

	float hue = normalised * 300.0f;
	float hueDash = hue / 60.0f;
	float xVal = chroma * (1 - fabs(fmod(hueDash,2) - 1));

	float red;
	float green;
	float blue;

	if (hueDash < 1)
	{
		red = chroma;
		green = xVal;
		blue = 0;
	}
	else if (hueDash < 2)
	{
		red = xVal;
		green = chroma;
		blue = 0;
	}
	else if (hueDash < 3)
	{
		red = 0;
		green = chroma;
		blue = xVal;
	}
	else if (hueDash < 4)
	{
		red = 0;
		green = xVal;
		blue = chroma;
	}
	else if (hueDash < 5)
	{
		red = xVal;
		green = 0;
		blue = chroma;
	}
	else
	{
		red = chroma;
		green = 0;
		blue = xVal;
	}
	
	/*levelOut[3*index] = (char)floor(blue * 255);
	levelOut[3*index+1] = (char)floor(green * 255);
	levelOut[3*index+2] = (char)floor(red * 255);*/

	levelOut[3*index] = (char)floor(normalised * 255);
	levelOut[3*index+1] = (char)floor(normalised * 255);
	levelOut[3*index+2] = (char)floor(normalised * 255);
}

__kernel void calculateGradient(__global float *in, __global float* out)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));
	size_t gridHeight = get_global_size(1);
	size_t gridWidth = get_global_size(0);

	size_t index = gridPos.x + gridPos.y * gridWidth;
	size_t rightIndex = gridPos.x + 1 + gridPos.y * gridWidth;
	size_t leftIndex = gridPos.x - 1 + gridPos.y * gridWidth;
	size_t upIndex = gridPos.x + (gridPos.y +1) * gridWidth;
	size_t downIndex = gridPos.x + (gridPos.y -1) * gridWidth;


	if (gridPos.x > 0 && gridPos.y > 0 && gridPos.x < gridWidth - 1 && gridPos.y < gridHeight - 1)
	{
		float2 gradient = (float2)(in[leftIndex] - in[rightIndex], in[downIndex] - in[upIndex]);
		out[index] = length(gradient);
	}
	else
	{
		out[index] = 0;
	}
}