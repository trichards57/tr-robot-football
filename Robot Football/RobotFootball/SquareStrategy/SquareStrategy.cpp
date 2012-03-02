// SquareStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

#include "UserData.h"
#include "MotionController.h"

#define CM_TO_INCHES(X) (X / 2.54)

clock_t startTime = 0;

extern "C" STRATEGY_API void Create(Environment* env)
{
	auto data = new UserData();
	data->targetPosition.x = (FLEFTX + FRIGHTX)/2;
	data->targetPosition.y = (FTOP + FBOT)/2;
	data->startTime = clock();
	env->userData = data;
}

extern "C" STRATEGY_API void Destroy(Environment* env)
{
		
}

#define LEFT_X (FLEFTX + CM_TO_INCHES(55))
#define RIGHT_X (FRIGHTX - CM_TO_INCHES(55))
#define TOP_Y (FTOP - CM_TO_INCHES(30))
#define BOTTOM_Y (FBOT + CM_TO_INCHES(30))

#define STRAIGHT_LINE_POS(STARTPOS, ENDPOS, TIME, PERIOD) ((ENDPOS - STARTPOS) * (TIME) / PERIOD + STARTPOS)
#define PATH_LENGTH 1500

Vector3D Path()
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
	auto bot = &(env->home[1]); // Just going to work with the first bot.
	auto data = (UserData*)env->userData;

	MotionController controller;

	controller.Control(Path(), bot);
}