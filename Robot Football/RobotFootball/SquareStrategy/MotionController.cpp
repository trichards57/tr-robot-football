#include "stdafx.h"

#include "MotionController.h"
#include "..\InterceptStrategy\VelocityController.h"
#include "..\InterceptStrategy\Utilities.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)
#define MAX_WHEEL_SPEED 125

namespace Strategies
{
	MotionController::MotionController(double positionControlScale, double angleControlScale)
		: positionProportionalTerm(positionControlScale), angleProportionalTerm(angleControlScale)
	{
	}

	void MotionController::Control(Vector3D targetPosition, Vector3D currentVelocity, Robot* bot)
	{
#ifdef LOG
		std::fstream logFile("c:\\strategy\\MotionControllerLog.csv", std::fstream::out | std::fstream::app);
		logFile << clock() << "," << targetPosition.x << "," << targetPosition.y << "," << bot->pos.x << "," << bot->pos.y << "," << bot->rotation << ",";
#endif

		auto diff = Utilities::Subtract(targetPosition, bot->pos);
		auto angle = atan2(diff.y, diff.x);
		auto botAngle = DEGREES_TO_RADIANS(bot->rotation); // Convert the robot angle into radians, which all the C math functions use...
		auto distance = Utilities::Length(diff);
		auto vel = distance * positionProportionalTerm;

		// Move target angle into robot frame of reference
		angle = angle - botAngle;

		// Normalise between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;

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
}