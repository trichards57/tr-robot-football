#include "stdafx.h"

#include "MotionController.h"

#define _USE_MATH_DEFINES 
#include <math.h>

#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)
#define MAX_WHEEL_SPEED 125

MotionController::MotionController(double positionControlScale, double angleControlScale)
	: positionProportionalTerm(positionControlScale), angleProportionalTerm(angleControlScale)
{
}

void MotionController::Control(std::function<Vector3D(clock_t)> controlFunction, clock_t timeOffset, Robot* bot)
{
	Control(controlFunction(clock() - timeOffset), bot);
}

double MotionController::DistanceTo(Vector3D targetPosition, Robot* bot)
{
	auto yDiff = targetPosition.y - bot->pos.y;
	auto xDiff = targetPosition.x - bot->pos.x;
	return sqrt(xDiff*xDiff+yDiff*yDiff);
}

void MotionController::Control(Vector3D targetPosition, Robot* bot)
{
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
		angle = atan(yDiff/xDiff);
	
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

	if (fabs(angle) > M_PI/2)
		// Target is behind is, will be quicker to go backwards
	{
		distance = -distance;
		// Rotate target angle around 180 degrees
		angle += M_PI;
		// Normalise between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;
	}

	
	// Set up the turn as required
	auto turnSpeed = angle * angleProportionalTerm;
	bot->velocityLeft = -turnSpeed;
	bot->velocityRight = turnSpeed;
	
	if (fabs(angle) < DEGREES_TO_RADIANS(20))
	{
		// Work out the forwards speed
		auto vel = distance * positionProportionalTerm;
		// Ensure we won't muck up the turn by trying to move too fast forwards
		if (vel + abs(turnSpeed) > MAX_WHEEL_SPEED)
			vel = MAX_WHEEL_SPEED - abs(bot->velocityLeft);
	
		// Apply the speed.
		bot->velocityLeft += vel;
		bot->velocityRight += vel;
	}
	else
	{
		/*bot->velocityLeft *= 10;
		bot->velocityRight *= 10;*/
	}
}

MotionController::~MotionController(void)
{
}
