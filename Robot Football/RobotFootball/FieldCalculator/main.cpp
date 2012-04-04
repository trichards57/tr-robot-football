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
#define RESOLUTION 0.1f
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

	cl_float2 ball;
	ball.s[0] = 35.4331f - 6.8118f; // Ball X
	ball.s[1] = 43.30705f - 6.3730f; // Ball Y

	cl_float field;
	field = RESOLUTION;

	const int repulseCount = 1;

	cl_float2 basicRepulsers[10];

	cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2)*repulseCount, &basicRepulsers, &err);
	checkErr(err, "Buffer::Buffer()");

	float* outH = new float[length];
	cl::Buffer outCl(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, length * sizeof(float), outH, &err);
	checkErr(err, "Buffer::Buffer()");

	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	std::ifstream file("D:\\Users\\Tony\\Documents\\tr-robot-football\\Simulator\\field_kernel.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, "lesson1_kernel.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	err = program.build(devices, "");

	//if (err == CL_BUILD_PROGRAM_FAILURE)
	{
		std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
		std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
		std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
	}

	checkErr(err, "Program::build()");

	cl::Kernel kernel(program, "main", &err);
	checkErr(err, "Kernel::Kernel()");
 
	err = kernel.setArg(0, sizeof(cl_float2), &ball);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(1, field);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(2, inRepulsers);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(3, outCl);
	checkErr(err, "Kernel::setArg()");

	cl::Kernel pointsKernel(program, "fieldAtPoints", &err);
	checkErr(err, "Kernel::Kernel()");

	cl_float2 inputPoints[4];
	float oPoints[4];

	cl::Buffer outPoints(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 4 * sizeof(float), oPoints, &err);
	checkErr(err, "Buffer::Buffer()");

	cl::Buffer inPoints(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2)*4, &inputPoints, &err);
	checkErr(err, "Buffer::Buffer()");

	err = pointsKernel.setArg(0, sizeof(cl_float2), &ball);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(1, inPoints);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(2, inRepulsers);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(3, outPoints);
	checkErr(err, "Kernel::setArg()");

	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");

	cl::vector<cl::Event> firstEvents;
	cl::vector<cl::Event> secondEvents;
	cl::vector<cl::Event> allEvents;

	cl::Event kernelEvent;
	cl::Event readEvent1;
	cl::Event readEvent2;
	cl::Event pointKernelEvent;

	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(gridWidth, gridHeight), cl::NDRange(WORK_GROUP_SIZE, WORK_GROUP_SIZE), NULL, &kernelEvent);
	checkErr(err, "CommandQueue::CommandQueue()");

	firstEvents.push_back(kernelEvent);

	//event.wait();
	err = queue.enqueueReadBuffer(outCl, CL_FALSE, 0, length * sizeof(float), outH, &firstEvents, &readEvent1);

	checkErr(err, "CommandQueue::enqueueReadBuffer()");

	err = queue.enqueueNDRangeKernel(pointsKernel, cl::NullRange, cl::NDRange(gridWidth, gridHeight), cl::NDRange(WORK_GROUP_SIZE, WORK_GROUP_SIZE), NULL, &pointKernelEvent);
	checkErr(err, "CommandQueue::CommandQueue()");

	secondEvents.push_back(pointKernelEvent);

	//event.wait();
	err = queue.enqueueReadBuffer(outPoints, CL_FALSE, 0, 4 * sizeof(float), oPoints, &secondEvents, &readEvent2);
	checkErr(err, "CommandQueue::CommandQueue()");

	allEvents.push_back(kernelEvent);
	allEvents.push_back(readEvent1);
	allEvents.push_back(readEvent2);
	allEvents.push_back(pointKernelEvent);

	queue.enqueueWaitForEvents(allEvents);
	checkErr(err, "CommandQueue::WaitForEvents()");

	return EXIT_SUCCESS;
}
