#include "stdafx.h"

#include "MotionController.h"
#include "..\InterceptStrategy\VelocityController.h"

#define _USE_MATH_DEFINES 
#include <math.h>

#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)
#define MAX_WHEEL_SPEED 125

MotionController::MotionController(double positionControlScale, double angleControlScale)
	: positionProportionalTerm(positionControlScale), angleProportionalTerm(angleControlScale)
{
}

double MotionController::DistanceTo(Vector3D targetPosition, Robot* bot)
{
	auto yDiff = targetPosition.y - bot->pos.y;
	auto xDiff = targetPosition.x - bot->pos.x;
	return sqrt(xDiff*xDiff+yDiff*yDiff);
}

void MotionController::Control(Vector3D targetPosition, Vector3D currentVelocity, Robot* bot)
{
#ifdef LOG
	std::fstream logFile("c:\\strategy\\MotionControllerLog.csv", std::fstream::out | std::fstream::app);
	logFile << clock() << "," << targetPosition.x << "," << targetPosition.y << "," << bot->pos.x << "," << bot->pos.y << "," << bot->rotation << ",";
#endif

	auto yDiff = targetPosition.y - bot->pos.y;
	auto xDiff = targetPosition.x - bot->pos.x;

	double angle;

	if (xDiff == 0)
	{
		// directly above or below
		if (fabs(yDiff) == yDiff) // target is below us
			angle = -M_PI_2;
		else // target is above us
			angle = M_PI_2;
	}
	else
	{
		angle = atan(yDiff/xDiff);
		if (fabs(xDiff) != xDiff) // xDiff is negative, so target is to the left
			angle = angle + M_PI; // rotate around pi radians to allow for tan function periodicity
	}

	auto botAngle = DEGREES_TO_RADIANS(bot->rotation); // Convert the robot angle into radians, which all the C math functions use...

	// Move target angle into robot frame of reference
	angle = angle - botAngle;
	
	// Normalise between pi and -pi
	if (angle > M_PI)
		angle -= 2*M_PI;
	if (angle < -M_PI)
		angle += 2*M_PI;

	auto distance = sqrt(xDiff*xDiff+yDiff*yDiff);

	auto vel = distance * positionProportionalTerm;

	Vector3D desiredVelocity;
	desiredVelocity.x = vel * cos(angle);
	desiredVelocity.y = vel * sin(angle);

	VelocityController control;
	control.Control(desiredVelocity, currentVelocity, bot);
#ifdef LOG
	logFile << bot->velocityLeft << "," << bot->velocityRight << std::endl;
	logFile.close();
#endif
}

MotionController::~MotionController(void)
{
}
