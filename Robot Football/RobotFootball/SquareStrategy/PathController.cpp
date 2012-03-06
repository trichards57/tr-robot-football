#include "stdafx.h"

#include "PathController.h"

void PathController::RegisterPath(Path path, int bot)
{
	currentPath[bot] = path;
	currentTimeOffset[bot] = 0;
}

bool PathController::StepPaths(Environment* env, MotionController* controller)
{
	auto currentTime = clock();
	for (auto i = currentPath.begin(); i != currentPath.end(); i++)
	{
		Vector3D nextPoint;
		if (i->second.size() > 0)
		{
			if (currentTimeOffset[i->first] == 0)
				currentTimeOffset[i->first] = clock();
			auto point = i->second.front()(currentTime - currentTimeOffset[i->first]);
			if (point.x < 0 || point.y < 0)
			{
				if (i->second.size() > 1)
				{
					i->second.erase(i->second.begin());
					currentTimeOffset[i->first] = clock();
					point = i->second.front()(currentTime - currentTimeOffset[i->first]);
				}
			}
			nextPoint = point;
		}
		if (nextPoint.x > -1 && nextPoint.y > -1)
		{
			controller->Control(nextPoint, &(env->home[i->first]));
		}
		else
		{
			env->home[i->first].velocityLeft = 0;
			env->home[i->first].velocityRight = 0;
		}
	}
	return true;
}

PathController::PathController(void)
{
}


PathController::~PathController(void)
{
}
