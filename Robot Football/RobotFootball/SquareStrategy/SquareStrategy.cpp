// SquareStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

#include "UserData.h"
#include "MotionController.h"
#include "PathController.h"

/// @file SquareStrategy.cpp 
/// @brief Contains all the standard strategy functions for SquareStrategy

/// @brief Converts centimetres to inches
#define CM_TO_INCHES(X) (X / 2.54)
/// @brief The left x coordinate to aim for
#define LEFT_X (FLEFTX + CM_TO_INCHES(55))
/// @brief The right x coordinate to aim for
#define RIGHT_X (FRIGHTX - CM_TO_INCHES(55))
/// @brief The top y coordinate to aim for
#define TOP_Y (FTOP - CM_TO_INCHES(30))
/// @brief The bottom y coordinate to aim for
#define BOTTOM_Y (FBOT + CM_TO_INCHES(30))

/// @brief Calculates the position along a straight line path at the given time
/// @param STARTPOS The starting position
/// @param ENDPOS The end position
/// @param TIME The current time
/// @param PERIOD The total length of the path in time
#define STRAIGHT_LINE_POS(STARTPOS, ENDPOS, TIME, PERIOD) ((ENDPOS - STARTPOS) * (TIME) / PERIOD + STARTPOS)
/// @brief The target length of each path in time
#define PATH_LENGTH 3000

using namespace Strategies;

/// @brief The main PathController
PathController controller;
/// @brief The last recorded positions of each robot
Vector3D lastPositions[5];

/// @brief Calculates the position along a cubic blend path at a given time
/// @param startPoint The starting position
/// @param endPoint The end position
/// @param time The current time
/// @param timePeriod The total length of the path in time
/// @todo Cite this
double CubicPath(double startPoint, double endPoint, clock_t time, clock_t timePeriod)
{
	time /= CLOCKS_PER_SEC;
	timePeriod /= CLOCKS_PER_SEC;
	// Taken from the Robotics Trajectory Planning Notes
	auto a0 = startPoint;
	auto a2 = 3*(endPoint - startPoint)/(timePeriod*timePeriod);
	auto a3 = -2 * (endPoint - startPoint)/(timePeriod*timePeriod*timePeriod);

	return a0 + a2*time*time + a3*time*time*time;
}

/// @brief Creates this object.
/// @param[in] env The environment created by the simulator.
///
/// Sets up and registers the path the robot should follow.
extern "C" STRATEGY_API void Create(Environment* env)
{
	auto data = new UserData();
	data->targetPosition.x = (FLEFTX + FRIGHTX)/2;
	data->targetPosition.y = (FTOP + FBOT)/2;
	env->userData = data;

	ControlFunction sec1 = [](clock_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = CubicPath(LEFT_X, RIGHT_X, t, PATH_LENGTH);
			result.y = TOP_Y;
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec2 = [](clock_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = RIGHT_X;
			result.y = CubicPath(TOP_Y, BOTTOM_Y, t, PATH_LENGTH);
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec3 = [](clock_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = CubicPath(RIGHT_X, LEFT_X, t, PATH_LENGTH);
			result.y = BOTTOM_Y;
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec4 = [](clock_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = LEFT_X;
			result.y = CubicPath(BOTTOM_Y, TOP_Y, t, PATH_LENGTH);
		}
		else
		{
			result.x = LEFT_X;
			result.y = TOP_Y;
		}
		return result;
	};

	Path p;
	p.push_back(sec1);
	p.push_back(sec2);
	p.push_back(sec3);
	p.push_back(sec4);

	controller.RegisterPath(p, 1);
}

/// @brief Destroys this object.
/// @param[in] env The environment created by the simulator.
/// @remark Does not appear to be called
extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

/// @brief Called on each time-step to control the robots.
/// @param[in] env The environment created by the simulator.
///
/// Triggers the PathController to step through the path at each cycle
/// @todo Use the simulator fixed time, not the real wall-time
extern "C" STRATEGY_API void Strategy(Environment* env)
{
	auto currentTime = clock();
	Vector3D velocities[5];

	for (int i = 0; i < 5; i++)
	{
		velocities[i].x = (env->home[i].pos.x - lastPositions[i].x) / 20;
		velocities[i].y = (env->home[i].pos.y - lastPositions[i].y) / 20;
		velocities[i].z = 0;
	}

	controller.StepPaths(env, new MotionController(), velocities);

	for (int i = 0; i < 5; i++)
	{
		lastPositions[i].x = env->home[i].pos.x;
		lastPositions[i].y = env->home[i].pos.y;
	}
}