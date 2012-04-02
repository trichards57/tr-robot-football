#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define BALL_WEIGHT 20
#define OBSTACLE_WEIGHT 200
#define OBSTACLE_SIGMA 5
#define M_PI 3.141593

float fieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers);

__kernel void main(float2 ball, float fieldResolution, __constant float2 *basicRepulsers, __global float * out)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));

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
	
	for (int i = 0; i < 10; i++)
	{
		diff = basicRepulsers[i] - realPos;
		// native_exp: proabably less accurate, but uses far fewer GPRs, so runs much faster...
		repField += 0.5f * OBSTACLE_WEIGHT * native_exp(-((diff.x*diff.x) + (diff.y*diff.y)) / (2*OBSTACLE_SIGMA));
	}

	diff = ball - realPos;

	float ballShapedField = 0.5f * 5 * OBSTACLE_WEIGHT * native_exp(-(diff.x*diff.x+diff.y*diff.y-200.0)*(diff.x*diff.x+diff.y*diff.y-200.0)/40000) * fabs(atan2(diff.y, diff.x)/M_PI);

	repField += ballShapedField;

	return attractField + repField;
}

__kernel void colorize(float max, float min, __global float *in, __global char* levelOut)
{
	size_t gridWidth = get_global_size(0);
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));
	size_t index = gridPos.x + gridPos.y * gridWidth;

	float normalised = (in[index] - min) / (max - min);

	levelOut[3*index] = (char)floor(normalised * 255);
	levelOut[3*index+1] = (char)floor(normalised * 255);
	levelOut[3*index+2] = (char)floor(normalised * 255);
}

__kernel void calculateGradient(__global float *in, __global float* out)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));
	size_t gridHeight = get_global_size(1) + 2;
	size_t gridWidth = get_global_size(0) + 2;

	size_t index = gridPos.x + gridPos.y * gridWidth;
	size_t rightIndex = index + 1;
	size_t leftIndex = index - 1;
	size_t upIndex = index + gridWidth;
	size_t downIndex = index - gridWidth;

	float2 gradient = (float2)(in[leftIndex] - in[rightIndex], in[downIndex] - in[upIndex]);
	out[index] = length(gradient);
	
}