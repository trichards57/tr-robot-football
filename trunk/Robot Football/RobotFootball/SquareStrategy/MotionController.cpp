#include "stdafx.h"

#include "MotionController.h"

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
	{
		angle = atan(yDiff/xDiff);
		if (fabs(xDiff) != xDiff) // target is to the left
			angle = angle + M_PI; // rotate around pi radians to allow for tan function periodicity
		if (angle > M_PI)
			angle = angle - 2*M_PI;
		if (angle < -M_PI)
			angle = angle + 2*M_PI;

		assert(abs(angle) <= M_PI);
	}

	auto botAngle = DEGREES_TO_RADIANS(bot->rotation); // Convert the robot angle into radians, which all the C math functions use...

	auto angleDiff = angle - botAngle;
	
	// Set up the turn as required
	auto turnSpeed = angleDiff * angleProportionalTerm;
	bot->velocityLeft = -turnSpeed;
	bot->velocityRight = turnSpeed;
	
	// Work out the forwards speed
	auto distance = sqrt(xDiff*xDiff+yDiff*yDiff);
	auto vel = distance * positionProportionalTerm;
	// Ensure we won't muck up the turn by trying to move too fast forwards
	if (vel + abs(turnSpeed) > MAX_WHEEL_SPEED)
		vel = MAX_WHEEL_SPEED - abs(bot->velocityLeft);
	
	// Apply the speed.
	bot->velocityLeft += vel;
	bot->velocityRight += vel;
}

MotionController::~MotionController(void)
{
}
