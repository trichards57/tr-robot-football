#pragma once

#include <functional>
#include <map>
#include <vector>
#include <ctime>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "MotionController.h"

/// @brief A function that produces a Vector3D based on the input time
typedef std::function<Vector3D(clock_t)> ControlFunction;

/// @brief A list of ControlFunction that make up a path.
typedef std::vector<ControlFunction> Path;

/// @brief A controller that will make the robots follow a set of pre-registered paths
///
/// Holds a collection of Path for each robot, and issues the required commands to move
/// the robots along that path on every StepPaths.
class PathController
{
private:
	/// @brief The current path for each robot
	std::map<int, Path> currentPath;
	/// @brief The current time offset used to run each path
	std::map<int, clock_t> currentTimeOffset;
public:
	/// @brief Registers a path for the given robot
	void RegisterPath(Path path, int bot);
	/// @brief Steps each registered path, using the provided MotionController
	bool StepPaths(Environment* env, MotionController* controller, Vector3D* velocities);
};

