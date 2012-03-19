#pragma once

#include <fstream>
#include <ctime>
#define _USE_MATH_DEFINES
#include <cmath>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class VelocityController
{
private:
	double velocityProportionalTerm;
	double angleProportionalTerm;
public:
	VelocityController(double velocityControlScale = 2, double angleControlScale = 5);
	~VelocityController(void);
	void Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity = false);
};

