#pragma once

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "MotionController.h"

typedef std::function<Vector3D(time_t)> ControlFunction;

typedef std::vector<ControlFunction> Path;

class PathController
{
private:
	std::map<int, Path> currentPath;
	std::map<int, time_t> currentTimeOffset;
public:
	PathController(void);
	~PathController(void);
	void RegisterPath(Path path, int bot);
	bool StepPaths(Environment* env, MotionController* controller);
};

