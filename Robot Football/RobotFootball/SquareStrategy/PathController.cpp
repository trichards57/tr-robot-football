#include "stdafx.h"

#include "PathController.h"

/// Registers a path for the given robot.  Will replace any path already registered to that robot.
///
/// @param path The path to register
/// @param bot The robot to assign the path to
void PathController::RegisterPath(Path path, int bot)
{
	currentPath[bot] = path;
	currentTimeOffset[bot] = 0;
}

/// Steps through all the registered paths, controlling each robot to follow the provided Path.  If the
/// control function returns a target point of (-1,-1), the controller will move to the next ControlFunction
/// in the path.  If the end of the path is reached, it will instruct the robot's motors to stop (with no
/// velocity control, just 0 on each motor.
///
/// @param env The environment containing the robots to move
/// @param controller The MotionController to use to move the robot
/// @param velocities The velocity feedback required by the MotionController
bool PathController::StepPaths(Environment* env, MotionController* controller, Vector3D* velocities)
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
			controller->Control(nextPoint, velocities[i->first], &(env->home[i->first]));
		}
		else
		{
			env->home[i->first].velocityLeft = 0;
			env->home[i->first].velocityRight = 0;
		}
	}
	return true;
}
