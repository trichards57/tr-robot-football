// PhysicsStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "UserData.h"

extern "C" STRATEGY_API void Create(Environment* env)
{
	auto userData = new UserData();
	env->userData = userData;
	
	auto t = time(nullptr);
	auto currentTime = localtime(&t);
	userData->outStream << asctime(currentTime) << std::endl;
}

extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

extern "C" STRATEGY_API void Strategy(Environment* env)
{
	auto userData = (UserData*) env->userData;
	switch (userData->State)
	{
	case RobotMotion:
		// We're currently testing the motion of the robot to determine weight
		auto bot = &env->home[1];
		bot->velocityLeft = 25.0;
		bot->velocityRight = 25.0;
		LARGE_INTEGER hiResTimer;
		LARGE_INTEGER hiResTimerFreq;
		QueryPerformanceCounter(&hiResTimer);
		QueryPerformanceFrequency(&hiResTimerFreq);
		userData->outStream << hiResTimerFreq.HighPart << "," << hiResTimerFreq.LowPart << "," << hiResTimer.HighPart << "," << hiResTimer.LowPart << "," << bot->pos.x << "," << bot->pos.y << std::endl;
		break;
	}
}
