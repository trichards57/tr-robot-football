#include "stdafx.h"

#include "PathController.h"
#include "MotionController.h"

namespace Strategies
{
	void PathController::RegisterPath(Path path, int bot)
	{
		currentPath[bot] = path;
		currentTimeOffset[bot] = 0;
	}

	bool PathController::StepPaths(Environment* env, MotionController* controller, Vector3D* velocities)
	{
		auto currentTime = clock();
		// Work through each registered path in turn
		for (auto i = currentPath.begin(); i != currentPath.end(); i++)
		{
			Vector3D nextPoint;
			if (i->second.size() > 0)
			{
				if (currentTimeOffset[i->first] == 0)
					currentTimeOffset[i->first] = clock(); // Path hasn't run yet, so set it up
				auto point = i->second.front()(currentTime - currentTimeOffset[i->first]); // Attempt to get the next point
				if (point.x < 0 || point.y < 0)
				{
					// The path has reached the end.  Try to get the next one
					if (i->second.size() > 1) 
					{
						// There is another point from the next path, so fetch it
						i->second.erase(i->second.begin());
						currentTimeOffset[i->first] = clock();
						point = i->second.front()(currentTime - currentTimeOffset[i->first]);
					}
				}
				nextPoint = point;
			}
			if (nextPoint.x > -1 && nextPoint.y > -1)
			{
				// There is a point to process
				controller->Control(nextPoint, velocities[i->first], &(env->home[i->first]));
			}
			else
			{
				// There's no point, so just set the motors to off
				env->home[i->first].velocityLeft = 0;
				env->home[i->first].velocityRight = 0;
			}
		}
		return true;
	}
}