#pragma once

#include <fstream>
#include <ctime>
#define _USE_MATH_DEFINES
#include <cmath>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// <summary>A controller to control a robot's velocity.</summary>
	class VelocityController
	{
	private:
		/// <summary>The velocity control proportional term.</summary>
		double velocityProportionalTerm;
		/// <summary>The angle control proportional term.</summary>
		double angleProportionalTerm;
	public:
		/// <summary>Initializes a new instance of the VelocityController class.</summary>
		/// <param name="velocityControlScale">The velocity control scaling term.</param>
		/// <param name="angleControlScale">   The angle control scaling term.</param>
		VelocityController(double velocityControlScale = 2, double angleControlScale = 5);
		/// <summary>Finalizes an instance of the VelocityController class.</summary>
		~VelocityController(void);
		/// <summary>Controls the given robot.</summary>
		/// <param name="targetVelocity">  The target velocity.</param>
		/// <param name="currentVelocity"> The robot's current velocity.</param>
		/// <param name="bot">			   The robot to control.</param>
		/// <param name="absoluteVelocity">If true, the velocity is relative to the field, otherwise it is relative to the robot's current orientation.</param>
		/// <param name="nearBall">		   If true, the acts as if the robot is in contact with the ball.</param>
		void Control(Vector3D targetVelocity, Vector3D currentVelocity, Robot* bot, bool absoluteVelocity = false, bool nearBall = false);
	};
}