#include "StdAfx.h"
#include "PotentialFieldGenerator.h"

#include <iostream>
#include <conio.h>

#include "Utilities.h"
#include "..\SquareStrategy\MotionController.h"


namespace Strategies
{
#define HEIGHT (FTOP - FBOT)
#define WIDTH (FRIGHTX - FLEFTX)
#define GRID_RESOLUTION 0.1f
#define GRADIENT_RESOLUTION (GRID_RESOLUTION)

	/// @copydoc FieldRenderer::Classes::StatusReport
	struct StatusReport
	{
		/// @copydoc FieldRenderer::Classes::StatusReport::Environment
		Environment environment;
		/// @copydoc FieldRenderer::Classes::StatusReport::FieldVector
		Vector3D fieldVector;
		/// @copydoc FieldRenderer::Classes::StatusReport::BallVelocity
		Vector3D ballVelocity;
		/// @copydoc FieldRenderer::Classes::StatusReport::FieldType
		int FieldType;
	};

	/// @brief Represents the position of a square relative to the square being examined
	enum SquarePosition
	{
		/// @brief Square is above the examined square
		Up = 0,
		/// @brief Square is below the examined square
		Down = 1,
		/// @brief Square is left of the examined square
		Left = 2,
		/// @brief Square is right of the examined square
		Right = 3
	};

	inline void PotentialFieldGenerator::CheckError(const int error, const TCHAR * name)
	{
		if (error != CL_SUCCESS)
		{
			// If the OpenCL function has failed, build up an error message
			WCHAR buf[5];
			_itow_s(error, buf, 5, 10);

			std::wstring errMsg(_T("ERROR: "));
			errMsg += name;
			errMsg += _T(" (");
			errMsg + buf;
			errMsg += _T(")");

			// Show it in a message box
			MessageBox(nullptr, errMsg.c_str(), _T("Open CL Error"), MB_OK | MB_ICONERROR);
			// Close the simulator
			exit(EXIT_FAILURE);
		}
	}

	PotentialFieldGenerator::PotentialFieldGenerator(void)
	{
		// Initialize OpenCL
		cl_int err;
		cl::vector<cl::Platform> platformList;
		cl::Platform::get(&platformList);
		CheckError(platformList.size() != 0 ? CL_SUCCESS : -1, _T("cl::Platform::get"));

		cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
		context = cl::Context(CL_DEVICE_TYPE_CPU, cprops, nullptr, nullptr, &err);
		CheckError(err, _T("Context::Context()"));

		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		CheckError(devices.size() > 0 ? CL_SUCCESS : -1, _T("devices.size() > 0"));

		// Load the kernel file
		std::ifstream file("field_kernel.cl");
		CheckError(file.is_open() ? CL_SUCCESS : -1, _T("field_kernel.cl"));

		std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

		// Build the kernels
		cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
		cl::Program program(context, source);
		err = program.build(devices, "");
		CheckError(err, _T("Program::build()"));

		// Extract the kernels wanted
		kernel = cl::Kernel(program, "fieldAtPoints", &err);
		CheckError(err, _T("Kernel::Kernel()"));
		possessionKernel = cl::Kernel(program, "possessionFieldAtPoints", &err);
		CheckError(err, _T("Kernel::Kernel()"));
	}

	/// @brief Populates the repulsers array with data from the environment
	/// @param[in,out] repulsers An array of points to contain the repulsers
	/// @param[in] env The environment created by the simulator.
	inline void PopulateRepulsers(cl_float2* repulsers, const Environment* env)
	{
		for (int i = 0; i < 5; i++)
		{
			repulsers[i] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->opponent[i].pos, env->fieldBounds));
			repulsers[i+5] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->home[i].pos, env->fieldBounds));
		}
	}

	/// @brief Populate the input points from the given position and field bounds
	/// @param[in,out] inPoints An array of points to contain the input points
	/// @param[in] position The current position to place the point around
	/// @param[in] fieldBounds The bounds of the playing field
	inline void PopulateInPoints(cl_float2* inPoints, const Vector3D position, const Bounds fieldBounds)
	{
		inPoints[Up] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Move(position, 0, GRADIENT_RESOLUTION), fieldBounds));
		inPoints[Down] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Move(position, 0, -GRADIENT_RESOLUTION), fieldBounds));
		inPoints[Left] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Move(position, -GRADIENT_RESOLUTION, 0), fieldBounds));
		inPoints[Right] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Move(position, GRADIENT_RESOLUTION, 0), fieldBounds));
	}

	Vector3D PotentialFieldGenerator::CalculateFinalApproachField(const Environment* env, const Robot bot)
	{
		const float field = GRID_RESOLUTION;

		cl_int err;
		cl_float2 ball = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->currentBall.pos, env->fieldBounds));
		cl_float2 ballVel;
		cl_float2 inPoints[4];
		cl_float2 repulsers[10];
		float outPoints[4];
		Vector3D force;

		// Set up the input data
		PopulateRepulsers(repulsers, env);
		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		// Disable the paired field sources
		ballVel.s[0] = 0;
		ballVel.s[1] = 0;

		// Set up the buffers for the input and output data
		cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

		// Set the kernel arguments
		err = kernel.setArg(0, sizeof(cl_float2), &ball);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(1, inPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(2, inRepulsers);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(3, outPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(4, sizeof(cl_float2), &ballVel);
		CheckError(err, _T("Kernel::setArg()"));

		// Create the command queue
		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the kernel
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the memory read
		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		// Wait for everything to finish
		queue.finish();

		// Calculate the final force
		force.x = (outPoints[Left] - outPoints[Right])/(2*GRADIENT_RESOLUTION);
		force.y = (outPoints[Down] - outPoints[Up])/(2*GRADIENT_RESOLUTION);

		return force;
	}

	Vector3D PotentialFieldGenerator::CalculateMainField(const Environment* env, const Robot bot, const Vector3D ballVelocity)
	{
		const float field = GRID_RESOLUTION;

		cl_int err;
		cl_float2 ball = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->currentBall.pos, env->fieldBounds));
		cl_float2 ballVel;
		cl_float2 goalTarget;
		cl_float2 inPoints[4];
		cl_float2 repulsers[10];
		float outPoints[4];
		Vector3D force;
		Vector3D goalTarg;

		goalTarg.x = GRIGHT;
		goalTarg.y = (GBOTY + GTOPY) / 2;

		// Set up the input data
		PopulateRepulsers(repulsers, env);
		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		cl_float2 vectorToGoal = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Subtract(env->currentBall.pos, goalTarg), env->fieldBounds));

		ballVel = vectorToGoal;

		// Set up the input and output buffers
		cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

		// Set up the kernel arguments
		err = kernel.setArg(0, sizeof(cl_float2), &ball);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(1, inPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(2, inRepulsers);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(3, outPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = kernel.setArg(4, sizeof(cl_float2), &ballVel);
		CheckError(err, _T("Kernel::setArg()"));

		// Create the command queue
		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the kernel
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the memory read
		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		// Wait for the code to finish
		queue.finish();

		// Calculate the output force
		force.x = (outPoints[Left] - outPoints[Right])/(2*GRADIENT_RESOLUTION);
		force.y = (outPoints[Down] - outPoints[Up])/(2*GRADIENT_RESOLUTION);

		return force;
	}

	Vector3D PotentialFieldGenerator::CalculatePossessionField(const Environment* env, const Robot bot)
	{
		const float field = GRID_RESOLUTION;

		cl_int err;
		cl_float2 ball = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->currentBall.pos, env->fieldBounds));
		cl_float2 goalTarget;
		cl_float2 inPoints[4];
		cl_float2 repulsers[10];
		float outPoints[4];
		Vector3D force;

		goalTarget.s[0] = (float)GRIGHT - env->fieldBounds.left;
		goalTarget.s[1] = (float)(GBOTY + GTOPY) / 2 - env->fieldBounds.bottom;

		// Set up the input data
		PopulateRepulsers(repulsers, env);
		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		// Set up the input and output buffers
		cl::Buffer inRepulsersBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

		// Set up the kernel arguments
		err = possessionKernel.setArg(0, sizeof(cl_float2), &ball);
		CheckError(err, _T("Kernel::setArg()"));
		err = possessionKernel.setArg(1, sizeof(cl_float2), &goalTarget);
		CheckError(err, _T("Kernel::setArg()"));
		err = possessionKernel.setArg(2, inPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = possessionKernel.setArg(3, inRepulsersBuffer);
		CheckError(err, _T("Kernel::setArg()"));
		err = possessionKernel.setArg(4, outPointsBuffer);
		CheckError(err, _T("Kernel::setArg()"));

		// Create the command queue
		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the kernel
		err = queue.enqueueNDRangeKernel(possessionKernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		// Enqueue the memory read
		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		// Wait for the code to finish
		queue.finish();

		// Calculate the output force
		force.x = (outPoints[Left] - outPoints[Right])/(2*GRADIENT_RESOLUTION);
		force.y = (outPoints[Down] - outPoints[Up])/(2*GRADIENT_RESOLUTION);

		return force;
	}

	Vector3D PotentialFieldGenerator::FieldVectorToBall(const int botIndex, const Environment* env, const Vector3D ballVelocity, int** fieldState)
	{
		if (*fieldState == nullptr)
			*fieldState = new int();

		Vector3D force;
		StatusReport rep;
		const auto ballSpeed = Utilities::Length(ballVelocity);
		const auto bot = env->home[botIndex];
		const auto xHeight = (int)ceil((FTOP - FBOT) / GRID_RESOLUTION);
		const auto xWidth = (int)ceil((FRIGHTX - FLEFTX) / GRID_RESOLUTION);
		Vector3D goalTarg;

		goalTarg.x = GRIGHT;
		goalTarg.y = (GBOTY + GTOPY) / 2;

		// Work out the vector from the ball to the goal
		auto vectorToGoal = Utilities::Subtract(Utilities::Subtract(env->currentBall.pos, goalTarg), env->fieldBounds);
		// Work out where the attraction point should be for the paired source field
		Vector3D attractTarget = Utilities::Subtract(env->currentBall.pos, Utilities::Multiply(Utilities::Divide(vectorToGoal, Utilities::Length(vectorToGoal)), -7));

		// Work out how far the robot is from the attraction point
		const auto dist = Utilities::Length(attractTarget, bot.pos);
		// Work out how far the robot is from the ball
		const auto ballDist = Utilities::Length(env->currentBall.pos, bot.pos);

		if (ballDist > 7)
			// If far from the ball, go to state 0
			fieldType = 0;
		if (dist < 6 && fieldType == 1)
			// If near the ball and on final approach, go to state 2
			fieldType = 2;
		else if (dist < 1.0f)
			// If near the paired source attraction point, go to state 1
			fieldType = 1;

		switch (fieldType)
		{
		case 0:
			// Far away from the ball, so use the main intercept field
			force = CalculateMainField(env, bot, ballVelocity);
			break;
		case 1:
			// Near the ball, so use the final approach field
			force = CalculateFinalApproachField(env, bot);
			break;
		case 2:
			// In contact with the ball, so use the goal approach code
			force = CalculatePossessionField(env, bot);
			break;
		}

		// Store the current field state for return to the caller
		(**fieldState) = fieldType;

		// Set up the status report
		rep.environment = *env;
		rep.fieldVector = force;
		rep.FieldType = fieldType;
		rep.ballVelocity = ballVelocity;

		// Pass the status report to the field renderer
		auto success = CallNamedPipe(_T("\\\\.\\pipe\\fieldRendererPipe"), &rep, sizeof(StatusReport), nullptr, 0, nullptr, 0);
		if (success == 0)
		{
			// The named pipe didn't work
			LPVOID lpMsgBuf = nullptr;
			// Found out why
			auto error = GetLastError();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)(&lpMsgBuf), 0, nullptr);

			// And report the error to the debugger
			OutputDebugString((LPCWSTR)lpMsgBuf);
			LocalFree(lpMsgBuf);
		}

		return force;
	}
}