#pragma once
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class MotionController
{
private:
	double positionProportionalTerm;
	double angleProportionalTerm;
public:
	MotionController(double positionControlScale = 1.85, double angleControlScale = 5);
	~MotionController(void);
	void Control(Vector3D targetPosition, Robot* bot);
	double DistanceTo(Vector3D targetPosition, Robot* bot);
};

