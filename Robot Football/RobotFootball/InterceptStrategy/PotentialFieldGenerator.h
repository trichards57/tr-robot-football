#pragma once

#include <cstdlib>

#define __NO_STD_VECTOR
#include <CL/cl.hpp>

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// @brief The generator of the artificial potential field.
	/// Provides force vectors which control the robot to score a goal.
	class PotentialFieldGenerator
	{
	private:
		/// @brief Type of the field currently in use.
		int fieldType;
		/// @brief The main kernel to calculate the field.
		cl::Kernel kernel;
		/// @brief The kernel to calculate the field when the robot has posession of the ball.
		cl::Kernel possessionKernel;
		/// @brief The OpenCL context to execute the code in.
		cl::Context context;
		/// @brief The OpenCL devices available for use.
		cl::vector<cl::Device> devices;
		/// @brief Handle of the named pipe used to communicate with the FieldRenderer.
		HANDLE pipe;
		/// @brief Calculates the main field.
		/// @param[in] env The environment created by the simulator.
		/// @param[in] bot The robot to control.
		/// @param[in] ballVelocity The current ball velocity.
		/// @return The calculated force vector.
		Vector3D CalculateMainField(const Environment* env, const Robot bot, const Vector3D ballVelocity);
		/// @brief Calculates the field when the robot has the ball.
		/// @param[in] env The environment created by the simulator.
		/// @param[in] bot The robot to control.
		/// @return The calculated force vector.
		Vector3D CalculatePossessionField(const Environment* env, const Robot bot);
		/// @brief Calculates the field when the robot is on final approach to the ball.
		/// @param[in] env The environment created by the simulator.
		/// @param[in] bot The robot to control.
		/// @return The calculated force vector.
		Vector3D CalculateFinalApproachField(const Environment* env, const Robot bot);
		/// @brief Checks for an OpenCL error and displays it.
		/// @param[in] error The error code.
		/// @param[in] name The name of the function that returned the error.
		void CheckError(const int error, const TCHAR* name);
	public:
		/// @brief Initializes a new instance of the PotentialFieldGenerator class.
		PotentialFieldGenerator(void);
		/// @brief Calculates the force vector for the provided robot.
		/// @param[in] botIndex Zero-based index of the robot to control.
		/// @param[in] env The environment created by the simulator.
		/// @param[in] ballVelocity The current ball velocity.
		/// @param[out] fieldState The current field state.
		/// @return The calculated force vector.
		///
		/// The field used to calculate the force depends on the current state of play.
		///
		/// + If the robot is far from the ball, it uses CalculateMainField
		/// + If the robot is near the ball's initial approach point, it uses CalculateFinalApproachField
		/// + If the robot is in possession of the ball, it uses CalculatePossessionField
		Vector3D FieldVectorToBall(const int botIndex, const Environment *env, const Vector3D ballVelocity, int** fieldState);
	};
}