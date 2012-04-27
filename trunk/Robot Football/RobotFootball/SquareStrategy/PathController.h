#pragma once

#include <functional>
#include <map>
#include <vector>
#include <ctime>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

/// @brief Namespace to contain all the strategy code
namespace Strategies
{
	/// @brief Defines an alias representing a control function.
	typedef std::function<Vector3D(clock_t)> ControlFunction;

	/// @brief Defines an alias representing a section of ControlFunction.
	typedef std::vector<ControlFunction> Path;

	/// @brief A controller that will make the robots follow a set of pre-registered paths.
	///
	/// Holds a collection of \ref Path for each robot, and issues the required commands to move
	/// the robots along that path on every call to StepPaths.
	class PathController
	{
	private:
		/// @brief The current \ref Path for each robot.
		std::map<int, Path> currentPath;
		/// @brief The current time offset used to run each \ref Path.
		std::map<int, clock_t> currentTimeOffset;
	public:
		/// @brief Registers a \ref Path to follow.
		/// @param[in] path The \ref Path to register.
		/// @param[in] bot The id of the robot to assign the \ref Path to.
		void RegisterPath(Path path, int bot);
		/// @brief Step each \ref Path currently registered with the controller.
		/// @param[in] env The environment created by the simulator.
		/// @param[in] controller The MotionController to use to move the robots.
		/// @param[in] velocities The current velocity of each robot.
		/// @retval true The function has succeeded.
		/// @retval false The function has failed.</returns>
		///
		/// Steps through all the registered paths, controlling each robot to follow the provided \ref Path.  If the
		/// control function returns a target point of (-1,-1), the controller will move to the next \ref ControlFunction
		/// in the path.  If the end of the path is reached, it will instruct the robot's motors to stop (with no
		/// velocity control, just 0 on each motor).
		///
		/// @todo Find out why the robot's twitch when the paths end.
		bool StepPaths(Environment* env, MotionController* controller, Vector3D* velocities);
	};
}