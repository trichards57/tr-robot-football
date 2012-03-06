// StrategyTestBench.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

#include "..\SquareStrategy\PathController.h"

#define CM_TO_INCHES(X) (X / 2.54)
#define LEFT_X (FLEFTX + CM_TO_INCHES(55))
#define RIGHT_X (FRIGHTX - CM_TO_INCHES(55))
#define TOP_Y (FTOP - CM_TO_INCHES(30))
#define BOTTOM_Y (FBOT + CM_TO_INCHES(30))

#define STRAIGHT_LINE_POS(STARTPOS, ENDPOS, TIME, PERIOD) ((ENDPOS - STARTPOS) * (TIME) / PERIOD + STARTPOS)
#define PATH_LENGTH 1500

int _tmain(int argc, _TCHAR* argv[])
{
	ControlFunction sec1 = [](time_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = STRAIGHT_LINE_POS(LEFT_X, RIGHT_X, t, PATH_LENGTH);
			result.y = TOP_Y;
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec2 = [](time_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = RIGHT_X;
			result.y = STRAIGHT_LINE_POS(TOP_Y, BOTTOM_Y, t, PATH_LENGTH);
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec3 = [](time_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = STRAIGHT_LINE_POS(RIGHT_X, LEFT_X, t, PATH_LENGTH);
			result.y = BOTTOM_Y;
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};
	ControlFunction sec4 = [](time_t t)->Vector3D{
		Vector3D result;
		if (t < PATH_LENGTH)
		{
			result.x = LEFT_X;
			result.y = STRAIGHT_LINE_POS(BOTTOM_Y, TOP_Y, t, PATH_LENGTH);
		}
		else
		{
			result.x = result.y = -1;
		}
		return result;
	};

	Path p;
	p.push_back(sec1);
	p.push_back(sec2);
	p.push_back(sec3);
	p.push_back(sec4);

	PathController controller;
	controller.RegisterPath(p, 0);

	Environment* env = new Environment();
	env->home[0].pos.x = 0;
	env->home[0].pos.y = 0;

	while (1)
	{
		controller.StepPaths(env, new MotionController());
		std::cout << "Controller output - Left : " << env->home[0].velocityLeft << ", Right - " << env->home[0].velocityRight << std::endl;
		Sleep(100);
	}
}

