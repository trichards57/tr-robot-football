#pragma once

#include <functional>
#include <ctime>
#include <fstream>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// <summary>A controller that controls the robot's position.</summary>
	class MotionController
	{
	private:
		/// <summary>The position control proportional term.</summary>
		double positionProportionalTerm;
		/// <summary>The angle control proportional term.</summary>
		double angleProportionalTerm;
	public:
		/// <summary>Initializes a new instance of the MotionController class.</summary>
		/// <param name="positionControlScale">The position control scaling term.</param>
		/// <param name="angleControlScale">   The angle control scaling term.</param>
		MotionController(double positionControlScale = 1.5, double angleControlScale = 3);
		/// <summary>Finalizes an instance of the MotionController class.</summary>
		~MotionController(void);
		/// <summary>Controls the given robot.</summary>
		/// <param name="targetPosition"> The target position.</param>
		/// <param name="currentVelocity">The robot's current velocity.</param>
		/// <param name="bot">			  The robot to control.</param>
		void Control(Vector3D targetPosition, Vector3D currentVelocity, Robot* bot);
	};
}