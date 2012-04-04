#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define BALL_WEIGHT 10
#define OBSTACLE_WEIGHT 100
#define OBSTACLE_SIGMA 5
#define M_PI 3.141593

inline float basicRepel(float2 realPos, float2 repulser)
{
	float2 diff = repulser - realPos;
	// native_exp: proabably less accurate, but uses far fewer GPRs, so runs much faster...
	return OBSTACLE_WEIGHT * native_exp(-((diff.x*diff.x) + (diff.y*diff.y)) / (2*OBSTACLE_SIGMA));
}

float fieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers)
{
	float2 attractBall = ball - (float2)(7,0);
	float2 repelBall = ball + (float2)(7,0);
	float dist = distance(attractBall, realPos);
	float attractField = BALL_WEIGHT * dist;

	float repField = basicRepel(realPos, repelBall);
	
	for (int i = 0; i < 10; i++)
	{
		repField += basicRepel(realPos, basicRepulsers[i]);
	}

	return attractField + repField;
}

float possessionFieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers, float2 goalTarget)
{
	float dist = distance(ball, realPos);
	float attractField = BALL_WEIGHT * dist;
	dist = distance(goalTarget, realPos);
	attractField = BALL_WEIGHT * dist;

	float repField = 0;
	
	for (int i = 0; i < 10; i++)
	{
		repField += basicRepel(realPos, basicRepulsers[i]);
	}

	return attractField + repField;
}

__kernel void possessionMain(float2 ball, float2 goalTarget, float fieldResolution, __constant float2 *basicRepulsers, __global float * out)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));

	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float2 realPos = convert_float2(gridPos) * fieldResolution;

	float res = possessionFieldAtPoint(realPos, ball, basicRepulsers, goalTarget);
	
	out[index] = res;
}

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

__kernel void possessionFieldAtPoints(float2 ball, float2 goalTarget, __constant float2 *fieldPoints, __constant float2 *basicRepulsers, __global float * out)
{
	size_t pointId = get_global_id(0);

	out[pointId] = possessionFieldAtPoint(fieldPoints[pointId], ball, basicRepulsers, goalTarget);
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