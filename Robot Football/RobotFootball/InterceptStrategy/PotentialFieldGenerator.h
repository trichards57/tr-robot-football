#pragma once

#include <cstdlib>

#define __NO_STD_VECTOR
#include <CL/cl.hpp>

#include <iostream>
#include <conio.h>
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

namespace Strategies
{
	/// <summary>The generator of the artificial potential field.</summary>
	/// Provides force vectors which control the robot to score a goal.
	class PotentialFieldGenerator
	{
	private:
		/// <summary>Type of the field currently in use.</summary>
		int fieldType;
		/// <summary>The main kernel to calculate the field.</summary>
		cl::Kernel kernel;
		/// <summary>The kernel to calculate the field when the robot has posession of the ball.</summary>
		cl::Kernel possessionKernel;
		/// <summary>The OpenCL context to execute the code in.</summary>
		cl::Context context;
		/// <summary>The OpenCL devices available for use.</summary>
		cl::vector<cl::Device> devices;
		/// <summary>Handle of the named pipe used to communicate with the FieldRenderer.</summary>
		HANDLE pipe;
		/// <summary>Calculates the main field.</summary>
		/// <param name="env">		   The environment created by the simulator.</param>
		/// <param name="bot">		   The robot to control.</param>
		/// <param name="ballVelocity">The current ball velocity.</param>
		/// <returns>The calculated force vector.</returns>
		Vector3D CalculateMainField(const Environment* env, const Robot bot, const Vector3D ballVelocity);
		/// <summary>Calculates the field when the robot has the ball.</summary>
		/// <param name="env">The environment created by the simulator.</param>
		/// <param name="bot">The robot to control.</param>
		/// <returns>The calculated force vector.</returns>
		Vector3D CalculatePossessionField(const Environment* env, const Robot bot);
		/// <summary>Calculates the field when the robot is on final approach to the ball.</summary>
		/// <param name="env">The environment created by the simulator.</param>
		/// <param name="bot">The robot to control.</param>
		/// <returns>The calculated force vector.</returns>
		Vector3D CalculateFinalApproachField(const Environment* env, const Robot bot);
		/// <summary>Checks for an OpenCL error and displays it.</summary>
		/// <param name="error">The error code.</param>
		/// <param name="name"> The name of the function that returned the error.</param>
		void CheckError(const int error, const TCHAR* name);
	public:
		/// <summary>Initializes a new instance of the PotentialFieldGenerator class.</summary>
		PotentialFieldGenerator(void);
		/// <summary>Finalizes an instance of the PotentialFieldGenerator class.</summary>
		~PotentialFieldGenerator(void);
		/// <summary>Calculates the force vector for the provided robot.</summary>
		/// <param name="botIndex">	   Zero-based index of the robot to control.</param>
		/// <param name="env">		   The environment created by the simulator.</param>
		/// <param name="ballVelocity">The current ball velocity.</param>
		/// <returns>The calculated force vector.</returns>
		/// The field used to calculate the force depends on the current state of play.
		///
		/// + If the robot is far from the ball, it uses CalculateMainField
		/// + If the robot is near the ball's initial approach point, it uses CalculateFinalApproachField
		/// + If the robot is in possession of the ball, it uses CalculatePossessionField
		Vector3D FieldVectorToBall(const int botIndex, const Environment *env, const Vector3D ballVelocity);
	};
}