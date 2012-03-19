#include "StdAfx.h"
#include "PotentialFieldGenerator.h"

#include "..\SquareStrategy\MotionController.h"

#define BALL_WEIGHT 150
#define OPPONENT_WEIGHT 1
#define OPPONENT_EFFECT_RADIUS 100

PotentialFieldGenerator::PotentialFieldGenerator(void)
{
}


PotentialFieldGenerator::~PotentialFieldGenerator(void)
{
}

Vector3D PotentialFieldGenerator::FieldVectorToBall(Robot bot, Environment* env)
{
	auto xDiff = env->currentBall.pos.x - bot.pos.x;
	auto yDiff = env->currentBall.pos.y - bot.pos.y;

	auto xDiffDist = 1; // TODO : Check this...
	auto yDiffDist = 1;

	auto xForce = BALL_WEIGHT * xDiff * xDiffDist;
	auto yForce = BALL_WEIGHT * yDiff * yDiffDist;

	Vector3D vector;
	vector.x = xForce;
	vector.y = yForce;

	return vector;
}