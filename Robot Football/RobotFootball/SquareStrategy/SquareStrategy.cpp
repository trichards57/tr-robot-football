// SquareStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

#include "UserData.h"
#include "MotionController.h"
#include "PathController.h"

#define CM_TO_INCHES(X) (X / 2.54)
#define LEFT_X (FLEFTX + CM_TO_INCHES(55))
#define RIGHT_X (FRIGHTX - CM_TO_INCHES(55))
#define TOP_Y (FTOP - CM_TO_INCHES(30))
#define BOTTOM_Y (FBOT + CM_TO_INCHES(30))

#define STRAIGHT_LINE_POS(STARTPOS, ENDPOS, TIME, PERIOD) ((ENDPOS - STARTPOS) * (TIME) / PERIOD + STARTPOS)
#define PATH_LENGTH 4000

clock_t startTime = 0;
PathController controller;

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
		if (t < 5000)
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

extern "C" STRATEGY_API void Destroy(Environment* env)
{
	
}



Vector3D OldPath()
{
	if (startTime == 0)
		startTime = clock();

	auto time = clock() - startTime;

	Vector3D currentTarget;

	if (time < PATH_LENGTH)
	{
		currentTarget.x = STRAIGHT_LINE_POS(LEFT_X, RIGHT_X, time, PATH_LENGTH);
		currentTarget.y = TOP_Y;
	}
	else if (time < PATH_LENGTH * 2)
	{
		currentTarget.x = RIGHT_X;
		currentTarget.y = STRAIGHT_LINE_POS(TOP_Y, BOTTOM_Y, time - PATH_LENGTH, PATH_LENGTH);
	}
	else if (time < PATH_LENGTH * 3)
	{
		currentTarget.x = STRAIGHT_LINE_POS(RIGHT_X, LEFT_X, time - PATH_LENGTH * 2, PATH_LENGTH);
		currentTarget.y = BOTTOM_Y;
	}
	else if (time < PATH_LENGTH * 4)
	{
		currentTarget.x = LEFT_X;
		currentTarget.y = STRAIGHT_LINE_POS(BOTTOM_Y, TOP_Y, time - PATH_LENGTH * 3, PATH_LENGTH);
	}
	else
	{
		currentTarget.x = LEFT_X;
		currentTarget.y = TOP_Y;
	}
	return currentTarget;
}

extern "C" STRATEGY_API void Strategy(Environment* env)
{
	controller.StepPaths(env, new MotionController());
}