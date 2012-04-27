#include "stdafx.h"

#include "MotionController.h"
#include "..\InterceptStrategy\VelocityController.h"
#include "..\InterceptStrategy\Utilities.h"

namespace Strategies
{
	MotionController::MotionController(double positionControlScale, double angleControlScale)
		: positionProportionalTerm(positionControlScale), angleProportionalTerm(angleControlScale)
	{
	}

	void MotionController::Control(Vector3D targetPosition, Vector3D currentVelocity, Robot* bot)
	{
		auto diff = Utilities::Subtract(targetPosition, bot->pos); // Get the vector from the robot to the target position
		auto angle = atan2(diff.y, diff.x);                        // Work out the angle to the target position
		auto botAngle = DEGREES_TO_RADIANS(bot->rotation);         // Convert the robot angle into radians, which all the C math functions use...
		auto distance = Utilities::Length(diff);                   // Get the actual distance to the target position
		auto vel = distance * positionProportionalTerm;            // Calculate the desired speed

		angle = angle - botAngle;                                  // Move target angle into robot frame of reference

		// Normalise the new angle between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;

		Vector3D desiredVelocity;                                  // Calculate the desired speed
		desiredVelocity.x = vel * cos(angle);                 
		desiredVelocity.y = vel * sin(angle);

		VelocityController control;                                
		control.Control(desiredVelocity, currentVelocity, bot);    // Pass the information on to the velocity controller
	}
}