// InterceptStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "..\SquareStrategy\PathController.h"
#include "..\SquareStrategy\MotionController.h"
#include "VelocityController.h"
#include "PotentialFieldGenerator.h"
#include "Utilities.h"

/// @file InterceptStrategy.cpp 
/// @brief Contains all the standard strategy functions for InterceptStrategy

using namespace Strategies;

/// @brief The last recorded positions of the robots
Vector3D lastPositions[5];
/// @brief The potential field generator used to calculate the controlling forces
PotentialFieldGenerator generator;

/// @brief Creates this object.
/// @param[in] env The environment created by the simulator.
extern "C" STRATEGY_API void Create(Environment* env)
{
}

/// @brief Destroys this object.
/// @param[in] env The environment created by the simulator.
/// @remark Does not appear to be called
extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

/// @brief Called on each time-step to control the robots.
/// @param[in] env The environment created by the simulator.
/// @todo Fix the velocity calculation and retune the controllers
extern "C" STRATEGY_API void Strategy(Environment* env)
{
	auto currentTime = clock();
	Vector3D velocities[5];

	// Calculate the current ball velocity
	Vector3D ballVelocity = Utilities::Divide(Utilities::Subtract(env->currentBall.pos, env->lastBall.pos), 20);

	// Calculate the current robot velocities
	for (int i = 0; i < 5; i++)
	{
		velocities[i] = Utilities::Divide(Utilities::Subtract(env->home[i].pos, lastPositions[i]), 20);
	}

	auto fieldState = new int*();
	*fieldState = new int();

	// Calculate the force vector to move the robot
	Vector3D interceptVector= generator.FieldVectorToBall(1, env, ballVelocity, fieldState);

	// Normalise the length of the force vector
	auto len = Utilities::Length(interceptVector);
	auto normalisedVector = Utilities::Divide(interceptVector, len);
	interceptVector = Utilities::Multiply(normalisedVector, 10); // Fix the intercept vector length

	// Feed the velocity to a VelocityController
	VelocityController velControl;
	velControl.Control(interceptVector, velocities[1], &(env->home[1]), true, **fieldState == 2);

	// Store the current positions for velocity calculations
	for (int i = 0; i < 5; i++)
	{
		lastPositions[i] = env->home[i].pos;
	}
}