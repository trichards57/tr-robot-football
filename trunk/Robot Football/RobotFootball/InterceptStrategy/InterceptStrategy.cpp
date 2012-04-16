// InterceptStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "..\SquareStrategy\PathController.h"
#include "..\SquareStrategy\MotionController.h"
#include "VelocityController.h"
#include "PotentialFieldGenerator.h"
#include "Utilities.h"

using namespace Strategies;

Vector3D lastPositions[5];
PotentialFieldGenerator generator;

extern "C" STRATEGY_API void Create(Environment* env)
{
}

extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

extern "C" STRATEGY_API void Strategy(Environment* env)
{
	auto currentTime = clock();
	Vector3D velocities[5];

	Vector3D ballVelocity = Utilities::Divide(Utilities::Subtract(env->currentBall.pos, env->lastBall.pos), 20);

	for (int i = 0; i < 5; i++)
	{
		velocities[i] = Utilities::Divide(Utilities::Subtract(env->home[i].pos, lastPositions[i]), 20);
	}

	Vector3D interceptVector= generator.FieldVectorToBall(1, env, ballVelocity);

	auto distance = Utilities::Length(env->currentBall.pos, env->home[1].pos);

	VelocityController velControl;
	velControl.Control(interceptVector, velocities[1], &(env->home[1]), true, env->gameState == BLUE_BALL);

	for (int i = 0; i < 5; i++)
	{
		lastPositions[i] = env->home[i].pos;
	}
}