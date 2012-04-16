#include "StdAfx.h"
#include "PotentialFieldGenerator.h"

#include "Utilities.h"
#include "..\SquareStrategy\MotionController.h"

namespace Strategies
{
#define HEIGHT (FTOP - FBOT)
#define WIDTH (FRIGHTX - FLEFTX)
#define GRID_RESOLUTION 0.1f
#define GRADIENT_RESOLUTION (GRID_RESOLUTION)

	struct StatusReport
	{
		Environment environment;
		Vector3D fieldVector;
		Vector3D ballVelocity;
		int FieldType;
	};

	enum SquarePosition
	{
		Up = 0,
		Down = 1,
		Left = 2,
		Right = 3
	};

	inline void PotentialFieldGenerator::CheckError(const int error, const TCHAR * name)
	{
		if (error != CL_SUCCESS)
		{
			WCHAR buf[5];
			_itow_s(error, buf, 5, 10);

			std::wstring errMsg(_T("ERROR: "));
			errMsg += name;
			errMsg += _T(" (");
			errMsg + buf;
			errMsg += _T(")");

			MessageBox(nullptr, errMsg.c_str(), _T("Open CL Error"), MB_OK | MB_ICONERROR);
			exit(EXIT_FAILURE);
		}
	}

	PotentialFieldGenerator::PotentialFieldGenerator(void)
	{
		cl_int err;
		cl::vector<cl::Platform> platformList;
		cl::Platform::get(&platformList);
		CheckError(platformList.size() != 0 ? CL_SUCCESS : -1, _T("cl::Platform::get"));

		cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
		context = cl::Context(CL_DEVICE_TYPE_CPU, cprops, nullptr, nullptr, &err);
		CheckError(err, _T("Context::Context()"));

		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		CheckError(devices.size() > 0 ? CL_SUCCESS : -1, _T("devices.size() > 0"));

		std::ifstream file("field_kernel.cl");
		CheckError(file.is_open() ? CL_SUCCESS : -1, _T("field_kernel.cl"));

		std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

		cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
		cl::Program program(context, source);
		err = program.build(devices, "");
		CheckError(err, _T("Program::build()"));

		kernel = cl::Kernel(program, "fieldAtPoints", &err);
		CheckError(err, _T("Kernel::Kernel()"));
		possessionKernel = cl::Kernel(program, "possessionFieldAtPoints", &err);
		CheckError(err, _T("Kernel::Kernel()"));
	}

	PotentialFieldGenerator::~PotentialFieldGenerator(void)
	{
	}

	inline void PopulateRepulsers(cl_float2* repulsers, const Environment* env)
	{
		for (int i = 0; i < 5; i++)
		{
			repulsers[i] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->opponent[i].pos, env->fieldBounds));
			repulsers[i+5] = Utilities::Vector3DToCLFloat2(Utilities::Subtract(env->home[i].pos, env->fieldBounds));
		}
	}

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

		PopulateRepulsers(repulsers, env);
		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		ballVel.s[0] = 0;
		ballVel.s[1] = 0;

		cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

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

		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		queue.finish();

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

		PopulateRepulsers(repulsers, env);

		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		cl_float2 vectorToGoal = Utilities::Vector3DToCLFloat2(Utilities::Subtract(Utilities::Subtract(env->currentBall.pos, goalTarg), env->fieldBounds));

		ballVel = vectorToGoal;

		//ballVel.s[0] =  -7;// (float)ballVelocity.x;
		//ballVel.s[1] = 0;//(float)ballVelocity.y;

		cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

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

		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		queue.finish();

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

		PopulateRepulsers(repulsers, env);
		PopulateInPoints(inPoints, bot.pos, env->fieldBounds);

		cl::Buffer inRepulsersBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));
		cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
		CheckError(err, _T("Buffer::Buffer()"));

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

		cl::CommandQueue queue(context, devices[0], 0, &err);
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueNDRangeKernel(possessionKernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1));
		CheckError(err, _T("CommandQueue::CommandQueue()"));

		err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
		CheckError(err, _T("CommandQueue::enqueueReadBuffer()"));

		queue.finish();

		force.x = (outPoints[Left] - outPoints[Right])/(2*GRADIENT_RESOLUTION);
		force.y = (outPoints[Down] - outPoints[Up])/(2*GRADIENT_RESOLUTION);

		return force;
	}

	Vector3D PotentialFieldGenerator::FieldVectorToBall(const int botIndex, const Environment* env, const Vector3D ballVelocity, int** fieldState)
	{
		if (*fieldState == nullptr)
			*fieldState = new int();

		const auto ballSpeed = Utilities::Length(ballVelocity);
		const auto bot = env->home[botIndex];
		const auto xHeight = (int)ceil((FTOP - FBOT) / GRID_RESOLUTION);
		const auto xWidth = (int)ceil((FRIGHTX - FLEFTX) / GRID_RESOLUTION);
		Vector3D goalTarg;

		goalTarg.x = GRIGHT;
		goalTarg.y = (GBOTY + GTOPY) / 2;

		auto vectorToGoal = Utilities::Subtract(Utilities::Subtract(env->currentBall.pos, goalTarg), env->fieldBounds);

		Vector3D attractTarget = Utilities::Subtract(env->currentBall.pos, Utilities::Multiply(Utilities::Divide(vectorToGoal, Utilities::Length(vectorToGoal)), -7));

		Vector3D force;
		StatusReport rep;

		const auto dist = Utilities::Length(attractTarget, bot.pos);
		const auto ballDist = Utilities::Length(env->currentBall.pos, bot.pos);

		if (ballDist > 7)
			fieldType = 0;
		if (dist < 6 && fieldType == 1)
			fieldType = 2;
		else if (dist < 1.0f)
			fieldType = 1;

		switch (fieldType)
		{
		case 0:
			force = CalculateMainField(env, bot, ballVelocity);
			break;
		case 1:
			force = CalculateFinalApproachField(env, bot);
			break;
		case 2:
			force = CalculatePossessionField(env, bot);
			break;
		}

		(**fieldState) = fieldType;

		rep.environment = *env;
		rep.fieldVector = force;
		rep.FieldType = fieldType;
		rep.ballVelocity = ballVelocity;

		auto success = CallNamedPipe(_T("\\\\.\\pipe\\fieldRendererPipe"), &rep, sizeof(StatusReport), nullptr, 0, nullptr, 0);
		if (success == 0)
		{
			LPVOID lpMsgBuf = nullptr;
			auto error = GetLastError();

			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)(&lpMsgBuf), 0, nullptr);

			OutputDebugString((LPCWSTR)lpMsgBuf);

			LocalFree(lpMsgBuf);
		}

		return force;
	}
}