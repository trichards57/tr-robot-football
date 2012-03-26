#include <utility>
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

const std::string hw("Hello World\n");

#define HEIGHT (77.2392 - 6.3730)
#define WIDTH (93.4259-6.8118)
#define RESOLUTION 0.1
#define WORK_GROUP_SIZE 16
#define BALL_WEIGHT 150

inline void checkErr(cl_int err, const char * name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

double FieldAtPoint(float pointX, float pointY, float ballX, float ballY)
{
	// Attract to the ball
	auto xDiff = ballX - pointX;
	auto yDiff = ballY - pointY;

	auto dist = sqrt(xDiff*xDiff + yDiff*yDiff);
	auto attractField = 0.5 * BALL_WEIGHT * dist;

	////combinable<double> repelField;
	double repField = 0;
	//// Repel from friendlies
	//for (int i = 0; i < 5; i++)
	//{
	//	auto xDiff = env->home[i].pos.x - point.x;
	//	auto yDiff = env->home[i].pos.y - point.y;

	//	if (fabs(xDiff) < 2*GRID_RESOLUTION && fabs(yDiff) < 2*GRID_RESOLUTION)
	//	{
	//		continue; // This is probably the robot being analysed as it is so close. Skip it
	//	}

	//	auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
	//	repField += force;
	//}
	////auto repField = repelField.combine(std::plus<double>());

	////repelField.clear();

	//// Repel from opponents
	//for (int i = 0; i < 5; i++)
	//{
	//	auto xDiff = env->opponent[i].pos.x - point.x;
	//	auto yDiff = env->opponent[i].pos.y - point.y;

	//	auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
	//	repField += force;
	//}

	//repField += repelField.combine(std::plus<double>());

	return  repField +attractField;
}

int main(void)
{
	cl_int err;
	cl::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cerr << "Platform number is: " << platformList.size() << std::endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cerr << "Platform is by: " << platformVendor << "\n";
	cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};

	cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	checkErr(err, "Context::Context()");

	int gridWidth = (int)ceil(WIDTH/RESOLUTION);
	int remainder = gridWidth % WORK_GROUP_SIZE;
	gridWidth += WORK_GROUP_SIZE - remainder;

	int gridHeight = (int)ceil(HEIGHT/RESOLUTION);
	remainder = gridHeight % WORK_GROUP_SIZE;
	gridHeight += WORK_GROUP_SIZE - remainder;

	int length = gridHeight * gridWidth;

	cl_float4 ball;
	ball.s[0] = 35.4331 - 6.8118; // Ball X
	ball.s[1] = 43.30705 - 6.3730; // Ball Y
	ball.s[2] = 0;
	ball.s[3] = BALL_WEIGHT;

	cl_float4 field;
	field.s[0] = WIDTH;
	field.s[1] = HEIGHT;
	field.s[2] = RESOLUTION;

	cl::Buffer inBall(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float4), &ball, &err);
	checkErr(err, "Buffer::Buffer()");
	cl::Buffer inField(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float4), &field, &err);
	checkErr(err, "Buffer::Buffer()");

	cl_float4* outH = new cl_float4[length];
	cl::Buffer outCl(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, length * sizeof(cl_float4), outH, &err);
	checkErr(err, "Buffer::Buffer()");

	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	std::ifstream file("lesson1_kernels.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, "lesson1_kernel.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	err = program.build(devices, "");

	if (err == CL_BUILD_PROGRAM_FAILURE)
	{
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
	}

	checkErr(err, "Program::build()");

	cl::Kernel kernel(program, "hello", &err);
	checkErr(err, "Kernel::Kernel()");
 
	err = kernel.setArg(0, inBall);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(1, inField);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(2, outCl);
	checkErr(err, "Kernel::setArg()");

	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");

	cl::Event event;
	
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(gridWidth, gridHeight), cl::NDRange(WORK_GROUP_SIZE, WORK_GROUP_SIZE), NULL, &event);
	checkErr(err, "CommandQueue::CommandQueue()");

	//event.wait();
	err = queue.enqueueReadBuffer(outCl, CL_TRUE, 0, length * sizeof(cl_float4), outH);
	
	checkErr(err, "CommandQueue::enqueueReadBuffer()");

	return EXIT_SUCCESS;
}
