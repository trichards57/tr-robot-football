#include "StdAfx.h"
#include "VelocityController.h"
#include "Utilities.h"

#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)
#define MAX_WHEEL_SPEED 125

namespace Strategies
{
	VelocityController::VelocityController(double velocityControlScale, double angleControlScale)
		: velocityProportionalTerm(velocityControlScale), angleProportionalTerm(angleControlScale)
	{
	}

	VelocityController::~VelocityController(void)
	{
	}

	void VelocityController::Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity, bool nearBall)
	{
#ifdef LOG
		std::fstream logFile("c:\\strategy\\VelocityControllerLog.csv", std::fstream::out | std::fstream::app);
		logFile << clock() << "," << targetVelocity.x << "," << targetVelocity.y << "," << currentVelocity.x << "," << currentVelocity.y << "," << bot->rotation << ",";
#endif
		auto diff = Utilities::Subtract(targetVelocity, currentVelocity);
		auto angle = atan2(diff.y, diff.x);
		auto botAngle = DEGREES_TO_RADIANS(bot->rotation);
		auto error = Utilities::Length(diff);

		if (absoluteVelocity)
			angle = angle - botAngle;

		// Normalise between pi and -pi
		if (angle > M_PI)
			angle -= 2*M_PI;
		if (angle < -M_PI)
			angle += 2*M_PI;

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

		if (fabs(angle) < DEGREES_TO_RADIANS(20) || nearBall)
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
}