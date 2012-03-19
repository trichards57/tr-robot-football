#pragma once

#include <functional>
#include <ctime>
#include <fstream>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class MotionController
{
private:
	double positionProportionalTerm;
	double angleProportionalTerm;
public:
	MotionController(double positionControlScale = 1.5, double angleControlScale = 3);
	~MotionController(void);
	void Control(Vector3D targetPosition, Vector3D currentVelocity,  Robot* bot);
	double DistanceTo(Vector3D targetPosition, Robot* bot);
};
