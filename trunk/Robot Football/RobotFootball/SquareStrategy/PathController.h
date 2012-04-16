#pragma once

#include <functional>
#include <map>
#include <vector>
#include <ctime>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include "MotionController.h"

namespace Strategies
{
	/// <summary>Defines an alias representing a control function.</summary>
	typedef std::function<Vector3D(clock_t)> ControlFunction;

	/// <summary>Defines an alias representing a section of ControlFunction.</summary>
	typedef std::vector<ControlFunction> Path;

	/// <summary>A controller that will make the robots follow a set of pre-registered paths.</summary>
	/// Holds a collection of Path for each robot, and issues the required commands to move
	/// the robots along that path on every StepPaths.
	class PathController
	{
	private:
		/// <summary>The current path for the robot.</summary>
		std::map<int, Path> currentPath;
		/// <summary>The current time offset used to run each path.</summary>
		std::map<int, clock_t> currentTimeOffset;
	public:
		/// <summary>Registers a path to follow.</summary>
		/// <param name="path">The path to register.</param>
		/// <param name="bot"> The id of the robot to assign the path to.</param>
		void RegisterPath(Path path, int bot);
		/// <summary>Step each path currently registered with the controller.</summary>
		/// <param name="env">		 The environment created by the simulator.</param>
		/// <param name="controller">The MotionController to use to move the robots.</param>
		/// <param name="velocities">The current velocity of each robot.</param>
		/// <returns>true if it succeeds, false if it fails.</returns>
		/// Steps through all the registered paths, controlling each robot to follow the provided Path.  If the
		/// control function returns a target point of (-1,-1), the controller will move to the next ControlFunction
		/// in the path.  If the end of the path is reached, it will instruct the robot's motors to stop (with no
		/// velocity control, just 0 on each motor.
		bool StepPaths(Environment* env, MotionController* controller, Vector3D* velocities);
	};
}