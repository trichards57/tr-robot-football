#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define BALL_WEIGHT 10
#define OBSTACLE_WEIGHT 100
#define OBSTACLE_SIGMA 5

/// @brief Calculates the effect of a basic repulsive field
/// @param realPos The real position of the point being examined
/// @param repulser The real position of the repulsive point
inline float basicRepel(float2 realPos, float2 repulser)
{
	float2 diff = repulser - realPos;
	// native_exp: proabably less accurate, but uses far fewer GPRs, so runs much faster...
	return OBSTACLE_WEIGHT * native_exp(-((diff.x*diff.x) + (diff.y*diff.y)) / (2*OBSTACLE_SIGMA));
}

/// @brief Calculates the effect of a stretched repulsive field
/// @param realPos The real position of the point being examined
/// @param repulser The real position of the repulsive point
inline float stretchedBasicRepel(float2 realPos, float2 repulser)
{
	float2 diff = repulser - realPos;
	// native_exp: proabably less accurate, but uses far fewer GPRs, so runs much faster...
	return OBSTACLE_WEIGHT * native_exp(-((diff.x*diff.x) / (2* OBSTACLE_SIGMA * 6) + (diff.y*diff.y)/ (2*OBSTACLE_SIGMA)) );
}

/// @brief Calculates the initial approach field at a given point
/// @param realPos The real position of the point being examined
/// @param ball The real position of the ball
/// @param ballVelocity The current velocity of the ball
/// @param basicRepulsers The real positions of each of the basic repulsive objects
float fieldAtPoint(float2 realPos, float2 ball, float2 ballVelocity, __constant float2 *basicRepulsers)
{
	float2 posShift = normalize(ballVelocity)*-7;
	float2 attractBall = ball - posShift;
	float2 repelBall = ball + posShift;
	float dist = distance(attractBall, realPos);
	float attractField = BALL_WEIGHT * dist;

	float repField;

	if (length(posShift) > 1.0f)
		repField += basicRepel(realPos, repelBall);

	for (int i = 0; i < 10; i++)
	{
		repField += basicRepel(realPos, basicRepulsers[i]);
	}

	return attractField + repField;
}

/// @brief Calculates the possession field at a given point
/// @param realPos The real position of the point being examined
/// @param ball The real position of the ball
/// @param basicRepulsers The real positions of each of the basic repulsive objects
/// @param goalTarget The real position of the target goal point
float possessionFieldAtPoint(float2 realPos, float2 ball, __constant float2 *basicRepulsers, float2 goalTarget)
{
	float2 vectorToGoal = normalize(goalTarget - realPos)*5;
	float2 attractBall = ball + vectorToGoal;

	float dist = distance(attractBall, realPos);
	float attractField = BALL_WEIGHT * dist;

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

__kernel void main(float2 ball, float fieldResolution, __constant float2 *basicRepulsers, __global float * out, float2 ballVelocity)
{
	int2 gridPos = (int2)(get_global_id(0),get_global_id(1));

	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float2 realPos = convert_float2(gridPos) * fieldResolution;

	float res = fieldAtPoint(realPos, ball, ballVelocity, basicRepulsers);

	out[index] = res;
}

__kernel void fieldAtPoints(float2 ball, __constant float2 *fieldPoints, __constant float2 *basicRepulsers, __global float * out, float2 ballVelocity)
{
	size_t pointId = get_global_id(0);

	out[pointId] = fieldAtPoint(fieldPoints[pointId], ball, ballVelocity, basicRepulsers);
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