#pragma once

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// @brief A controller that controls the robot's position.
	class MotionController
	{
	private:
		/// @brief The position control proportional term.
		double positionProportionalTerm;
		/// @brief The angle control proportional term.
		double angleProportionalTerm;
	public:
		/// @brief Initializes a new instance of the MotionController class.
		/// @param[in] positionControlScale The position control scaling term.
		/// @param[in] angleControlScale The angle control scaling term.
		MotionController(double positionControlScale = 1.5, double angleControlScale = 3);
		/// @brief Controls the given robot.
		/// @param[in] targetPosition The target position.
		/// @param[in] currentVelocity The robot's current velocity.
		/// @param[in,out] bot The robot to control.
		void Control(Vector3D targetPosition, Vector3D currentVelocity, Robot* bot);
	};
}