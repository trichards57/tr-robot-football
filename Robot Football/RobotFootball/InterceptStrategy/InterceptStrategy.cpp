// InterceptStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"	

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "..\SquareStrategy\PathController.h"
#include "..\SquareStrategy\MotionController.h"
#include "VelocityController.h"
#include "PotentialFieldGenerator.h"

#ifdef YELLOW
	bool yellowTeam = true;
#else
	bool yellowTeam = false;
#endif

clock_t startTime = 0;
Vector3D lastPositions[5];
Vector3D ballVelocity;
clock_t lastTime = 0;

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

	double minDistance = 1000000000;
	int minDistanceBot;
	
	ballVelocity.x = (env->currentBall.pos.x - env->lastBall.pos.x)/(currentTime - lastTime);
	ballVelocity.y = (env->currentBall.pos.y - env->lastBall.pos.y)/(currentTime - lastTime);

	for (int i = 0; i < 5; i++)
	{
		velocities[i].x = (env->home[i].pos.x - lastPositions[i].x) / (currentTime - lastTime);
		velocities[i].y = (env->home[i].pos.y - lastPositions[i].y) / (currentTime - lastTime);
		velocities[i].z = 0;
	}

	for (int i = 0; i < 5; i++)
	{
		double xDiff = env->currentBall.pos.x - env->home[i].pos.x;
		double yDiff = env->currentBall.pos.y - env->home[i].pos.y;

		double distance = sqrt(xDiff*xDiff+yDiff*yDiff);

		if (distance < minDistance)
		{
			minDistance = distance;
			minDistanceBot = i;
		}
	}

	Vector3D interceptVector;

	

	
	interceptVector = generator.FieldVectorToBall(env->home[1], env);
	

	VelocityController velControl;
	velControl.Control(interceptVector, velocities[1], &(env->home[1]), true);

	Vector3D stopVector;
	stopVector.x = 0;
	stopVector.y = 0;

	//for (int i = 0; i < 5; i++)
	//{
	//	if (i != minDistanceBot)
	//	{
	//		switch (i)
	//		{
	//		case 0: // Goal Keeper
	//			velControl.Control(stopVector, velocities[i], &(env->home[i]));
	//			break;
	//		case 1: // Defence Top
	//			velControl.Control(stopVector, velocities[i], &(env->home[i]));
	//			break;
	//		case 2: // Defence Bottom
	//			velControl.Control(stopVector, velocities[i], &(env->home[i]));
	//			break;
	//		case 3: // Attack Top
	//			velControl.Control(stopVector, velocities[i], &(env->home[i]));
	//			break;
	//		case 5: // Attack Bottom
	//			velControl.Control(stopVector, velocities[i], &(env->home[i]));
	//			break;
	//		}
	//		
	//	}
	//}

	lastTime = currentTime;
	for (int i = 0; i < 5; i++)
	{
		lastPositions[i].x = env->home[i].pos.x;
		lastPositions[i].y = env->home[i].pos.y;
	}
}