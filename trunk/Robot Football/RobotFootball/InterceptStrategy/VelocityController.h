#pragma once

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// @brief A controller to control a robot's velocity.
	class VelocityController
	{
	private:
		/// @brief The velocity control proportional term.
		double velocityProportionalTerm;
		/// @brief The angle control proportional term.
		double angleProportionalTerm;
	public:
		/// @brief Initializes a new instance of the VelocityController class.
		/// @param[in] velocityControlScale The velocity control scaling term.
		/// @param[in] angleControlScale The angle control scaling term.
		VelocityController(double velocityControlScale = 2, double angleControlScale = 5);
		/// @brief Controls the given robot.
		/// @param[in] targetVelocity The target velocity.
		/// @param[in] currentVelocity The robot's current velocity.
		/// @param[in] bot The robot to control.
		/// @param[in] absoluteVelocity If true, the velocity is relative to the field, otherwise it is relative to the robot's current orientation.
		/// @param[in] nearBall If true, the acts as if the robot is in contact with the ball.
		void Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity = false, bool nearBall = false);
	};
}