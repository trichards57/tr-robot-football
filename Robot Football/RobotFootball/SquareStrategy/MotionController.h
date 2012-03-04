#pragma once
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class MotionController
{
private:
	double positionProportionalTerm;
	double angleProportionalTerm;
public:
	MotionController(double positionControlScale = 2, double angleControlScale = 5);
	~MotionController(void);
	void Control(Vector3D targetPosition, Robot* bot);
	void Control(std::function<Vector3D(time_t)> controlFunction, time_t timeOffset, Robot* bot);
	double DistanceTo(Vector3D targetPosition, Robot* bot);
};
