#include "StdAfx.h"
#include "VelocityController.h"
#include "Utilities.h"

#define MAX_WHEEL_SPEED 125

namespace Strategies
{
	VelocityController::VelocityController(double velocityControlScale, double angleControlScale)
		: velocityProportionalTerm(velocityControlScale), angleProportionalTerm(angleControlScale)
	{
	}

	void VelocityController::Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity, bool nearBall)
	{
		auto diff = Utilities::Subtract(targetVelocity, currentVelocity); // Get the difference between the current and desired velocity
		auto angle = atan2(diff.y, diff.x);  // Work out the angle difference
		auto botAngle = DEGREES_TO_RADIANS(bot->rotation); // Get the robot's angle in radians
		auto error = Utilities::Length(diff); // Get the magnitude of the velocity error

		if (absoluteVelocity) // If the velocity is supposed to be an absolute velocity, translate it into a relative one
			angle = angle - botAngle;

		// Normalise between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;

		if (fabs(angle) > M_PI/2) // Target is behind is, will be quicker to go backwards
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

		if (fabs(angle) < DEGREES_TO_RADIANS(20) || nearBall) // If we're nearly at the right angle, or near the ball, try to turn and move
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
	}
}