#include "StdAfx.h"
#include "VelocityController.h"

#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)
#define MAX_WHEEL_SPEED 125

VelocityController::VelocityController(double velocityControlScale, double angleControlScale)
	: velocityProportionalTerm(velocityControlScale), angleProportionalTerm(angleControlScale)
{
}


VelocityController::~VelocityController(void)
{
}

void VelocityController::Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity)
{
#ifdef LOG
	std::fstream logFile("c:\\strategy\\VelocityControllerLog.csv", std::fstream::out | std::fstream::app);
	logFile << clock() << "," << targetVelocity.x << "," << targetVelocity.y << "," << currentVelocity.x << "," << currentVelocity.y << "," << bot->rotation << ",";
#endif

	auto xDiff = targetVelocity.x - currentVelocity.x;
	auto yDiff = targetVelocity.y - currentVelocity.y;

	double angle;

	if (xDiff == 0 && yDiff == 0)
	{
		angle = 0;
		absoluteVelocity = true;
	}
	else if (xDiff == 0)
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

	auto botAngle = DEGREES_TO_RADIANS(bot->rotation);

	if (absoluteVelocity)
		angle = angle - botAngle;

	// Normalise between pi and -pi
	if (angle > M_PI)
		angle -= 2*M_PI;
	if (angle < -M_PI)
		angle += 2*M_PI;

	auto error = sqrt(xDiff*xDiff+yDiff*yDiff);

	if (fabs(angle) > M_PI/2)
		// Target is behind is, will be quicker to go backwards
	{
		error = -error;
		// Rotate target angle around 180 degrees
		angle += M_PI;
		// Normalise between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;
	}
	
	auto turnSpeed = angle * angleProportionalTerm;
	bot->velocityLeft = -turnSpeed;
	bot->velocityRight = turnSpeed;

	

	if (fabs(angle) < DEGREES_TO_RADIANS(20))
	{
		// Work out the forwards speed
		auto vel = error * velocityProportionalTerm;
		// Ensure we won't muck up the turn by trying to move too fast forwards
		if (vel + abs(turnSpeed) > MAX_WHEEL_SPEED)
			vel = MAX_WHEEL_SPEED - abs(bot->velocityLeft);
	
		// Apply the speed.
		bot->velocityLeft += vel;
		bot->velocityRight += vel;
	}
#ifdef LOG
	logFile << bot->velocityLeft << "," << bot->velocityRight << std::endl;
	logFile.close();
#endif
}