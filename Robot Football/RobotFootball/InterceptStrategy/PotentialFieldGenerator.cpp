#include "StdAfx.h"
#include "PotentialFieldGenerator.h"

#include "..\SquareStrategy\MotionController.h"

#define HEIGHT (FTOP - FBOT)
#define WIDTH (FRIGHTX - FLEFTX)
#define BALL_WEIGHT 150
#define OBSTACLE_WEIGHT 1000000
#define OBSTACLE_EFFECT_RADIUS 10
#define OBSTACLE_SIGMA 2
#define GRID_RESOLUTION 0.1
#define WORK_GROUP_SIZE 2

using namespace Concurrency;

#ifdef OPENCL
inline void checkErr(cl_int err, const WCHAR * name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		std::wstring errMsg(L"ERROR: ");
		errMsg += name;
		errMsg += L" (";
		WCHAR buf[5];
		errMsg += _itow(err, buf, 10);
		errMsg += L")";

		MessageBox(nullptr, errMsg.c_str(), L"Open CL Error", MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
}
#endif


PotentialFieldGenerator::PotentialFieldGenerator(void)
{
#ifdef OPENCL
	cl_int err;
	cl::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, L"cl::Platform::get");
	std::cerr << "Platform number is: " << platformList.size() << std::endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cerr << "Platform is by: " << platformVendor << "\n";
	cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};

	context = cl::Context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
	checkErr(err, L"Context::Context()");

	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, L"devices.size() > 0");

	std::ifstream file("field_kernel.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, L"field_kernel.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	err = program.build(devices, "");
	checkErr(err, L"Program::build()");

	kernel = cl::Kernel(program, "fieldAtPoints", &err);
	checkErr(err, L"Kernel::Kernel()");

	
#endif
}


PotentialFieldGenerator::~PotentialFieldGenerator(void)
{
#ifdef OPENCL
	delete points;
#endif
}

double FieldAtPoint(Vector3D point, Environment* env)
{
	// Attract to the ball
	auto xDiff = env->currentBall.pos.x - point.x;
	auto yDiff = env->currentBall.pos.y - point.y;

	auto dist = sqrt(xDiff*xDiff + yDiff*yDiff);
	auto attractField = 0.5 * BALL_WEIGHT * dist;

	//combinable<double> repelField;
	double repField = 0;
	// Repel from friendlies
	for (int i = 0; i < 5; i++)
	{
		auto xDiff = env->home[i].pos.x - point.x;
		auto yDiff = env->home[i].pos.y - point.y;
		
		if (fabs(xDiff) < 2*GRID_RESOLUTION && fabs(yDiff) < 2*GRID_RESOLUTION)
		{
			continue; // This is probably the robot being analysed as it is so close. Skip it
		}
		
		auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
		repField += force;
	}
	//auto repField = repelField.combine(std::plus<double>());

	//repelField.clear();

	// Repel from opponents
	for (int i = 0; i < 5; i++)
	{
		auto xDiff = env->opponent[i].pos.x - point.x;
		auto yDiff = env->opponent[i].pos.y - point.y;
		
		auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
		repField += force;
	}

	//repField += repelField.combine(std::plus<double>());

	return  repField +attractField;
}

Vector3D PotentialFieldGenerator::FieldVectorToBall(Robot bot, Environment* env)
{
	auto xWidth = (int)ceil((FRIGHTX - FLEFTX) / GRID_RESOLUTION);
	auto xHeight = (int)ceil((FTOP - FBOT) / GRID_RESOLUTION);

	auto t1 = clock();

#ifdef DELEGATEOPENCL
	auto success = CallNamedPipe(L"\\\\.\\pipe\\fieldRendererPipe", env, sizeof(Environment), nullptr, 0, nullptr, 0);
	if (success == 0)
	{
		auto error = GetLastError();
		LPVOID lpMsgBuf = nullptr;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)(&lpMsgBuf), 0, NULL);

		LocalFree(lpMsgBuf);
	}
#endif
#ifdef OPENCL
	cl_int err;
	
	cl_float2 ball;
	ball.s[0] = env->currentBall.pos.x - env->fieldBounds.left; // Ball X
	ball.s[1] = env->currentBall.pos.y - env->fieldBounds.bottom; // Ball Y

	cl_float field;
	field = GRID_RESOLUTION;

	cl_float2 repulsers[10];

	for (int i = 0; i < 5; i++)
	{
		repulsers[i].s[0] = env->opponent->pos.x - env->fieldBounds.left;
		repulsers[i].s[0] = env->opponent->pos.y - env->fieldBounds.bottom;
	}
	for (int i = 0; i < 5; i++)
	{
		repulsers[i+5].s[0] = env->home->pos.x - env->fieldBounds.left;
		repulsers[i+5].s[1] = env->home->pos.y - env->fieldBounds.bottom;
	}

	float outPoints[4];
	cl_float2 inPoints[4];
	
	inPoints[0].s[0] = bot.pos.x - env->fieldBounds.left;
	inPoints[0].s[1] = bot.pos.y + GRID_RESOLUTION - env->fieldBounds.bottom;
	inPoints[1].s[0] = bot.pos.x - env->fieldBounds.left;
	inPoints[1].s[1] = bot.pos.y - GRID_RESOLUTION - env->fieldBounds.bottom;
	inPoints[2].s[0] = bot.pos.x - GRID_RESOLUTION - env->fieldBounds.left;
	inPoints[2].s[1] = bot.pos.y - env->fieldBounds.bottom;
	inPoints[3].s[0] = bot.pos.x + GRID_RESOLUTION - env->fieldBounds.left;
	inPoints[3].s[1] = bot.pos.y - env->fieldBounds.bottom;

	cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2)*10, &repulsers, &err);
	checkErr(err, L"Buffer::Buffer()");
	cl::Buffer inPointsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float2) * 4, &inPoints, &err);
	checkErr(err, L"Buffer::Buffer()");
	cl::Buffer outPointsBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * 4, &outPoints, &err);
	checkErr(err, L"Buffer::Buffer()");

	err = kernel.setArg(0, sizeof(cl_float2), &ball);
	checkErr(err, L"Kernel::setArg()");
	err = kernel.setArg(1, inPointsBuffer);
	checkErr(err, L"Kernel::setArg()");
	err = kernel.setArg(2, inRepulsers);
	checkErr(err, L"Kernel::setArg()");
	err = kernel.setArg(3, outPointsBuffer);
	checkErr(err, L"Kernel::setArg()");

	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, L"CommandQueue::CommandQueue()");

	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4), cl::NDRange(1), NULL, &event);
	checkErr(err, L"CommandQueue::CommandQueue()");

	err = queue.enqueueReadBuffer(outPointsBuffer, CL_TRUE, 0, 4 * sizeof(float), outPoints);
	checkErr(err, L"CommandQueue::enqueueReadBuffer()");
#endif

	auto t2 = clock();

	/*std::cerr << ((t2 - t1)/CLOCKS_PER_SEC) << std::endl;

	Vector3D upPoint, downPoint, leftPoint, rightPoint;

	upPoint.x = bot.pos.y + GRID_RESOLUTION;
	upPoint.y = bot.pos.x;
	downPoint.x = bot.pos.y - GRID_RESOLUTION;
	downPoint.y = bot.pos.x;
	leftPoint.x = bot.pos.x - GRID_RESOLUTION;
	leftPoint.y = bot.pos.y;
	rightPoint.x = bot.pos.x + GRID_RESOLUTION;
	rightPoint.y = bot.pos.y;

	auto upField = FieldAtPoint(upPoint, env);
	auto downField = FieldAtPoint(downPoint, env);
	auto leftField = FieldAtPoint(leftPoint, env);
	auto rightField = FieldAtPoint(rightPoint, env);*/

	Vector3D force;
	force.x = (outPoints[2] - outPoints[3])/(2*GRID_RESOLUTION);
	force.y = (outPoints[1] - outPoints[0])/(2*GRID_RESOLUTION);

	return force;
}