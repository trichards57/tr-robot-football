#pragma once

#include <functional>
#include <map>
#include <vector>
#include <ctime>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "MotionController.h"

typedef std::function<Vector3D(clock_t)> ControlFunction;

typedef std::vector<ControlFunction> Path;

class PathController
{
private:
	std::map<int, Path> currentPath;
	std::map<int, clock_t> currentTimeOffset;
public:
	PathController(void);
	~PathController(void);
	void RegisterPath(Path path, int bot);
	bool StepPaths(Environment* env, MotionController* controller);
};

