// PhysicsStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "UserData.h"

/// @file PhysicsStrategy.cpp 
/// @brief Contains all the standard strategy functions for PhysicsStrategy

/// @brief The constant velocity input
#define VELOCITY_INPUT 5.0

/// @brief Creates this object.
/// @param[in] env The environment created by the simulator.
///
/// Initiailizes the UserData and writes the simulation start time to the output file.
extern "C" STRATEGY_API void Create(Environment* env)
{
	auto userData = new UserData();
	env->userData = userData;

	auto t = time(nullptr);
	auto currentTime = localtime(&t);
	userData->outStream << asctime(currentTime) << "," << VELOCITY_INPUT << std::endl;
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
/// Records the current high performance timer value it's frequency and the robot's position to the output file.
extern "C" STRATEGY_API void Strategy(Environment* env)
{
	auto userData = (UserData*) env->userData;
	auto bot = &env->home[1];

	bot->velocityLeft = VELOCITY_INPUT;
	bot->velocityRight = VELOCITY_INPUT;

	LARGE_INTEGER hiResTimer;
	LARGE_INTEGER hiResTimerFreq;
	QueryPerformanceCounter(&hiResTimer);
	QueryPerformanceFrequency(&hiResTimerFreq);

	userData->outStream << hiResTimerFreq.HighPart << "," << hiResTimerFreq.LowPart << "," << hiResTimer.HighPart << "," << hiResTimer.LowPart << "," << bot->pos.x << "," << bot->pos.y << std::endl;
}