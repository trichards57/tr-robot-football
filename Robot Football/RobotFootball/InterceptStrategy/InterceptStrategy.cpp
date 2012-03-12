// InterceptStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "..\SquareStrategy\PathController.h"
#include "..\SquareStrategy\MotionController.h"

extern "C" STRATEGY_API void Create(Environment* env)
{

}

extern "C" STRATEGY_API void Destroy(Environment* env)
{

}

extern "C" STRATEGY_API void Strategy(Environment* env)
{
	MotionController controller;
	controller.Control(env->currentBall.pos, &env->home[1]);
}