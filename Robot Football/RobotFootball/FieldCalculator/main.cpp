#include <utility>
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

#define HEIGHT (77.2392 - 6.3730)
#define WIDTH (93.4259-6.8118)
#define RESOLUTION 0.1f
#define WORK_GROUP_SIZE 16

/// <summary>Checks an OpenCL error code and outputs to cerr if there is a problem.</summary>
/// <param name="err"> The error to check.</param>
/// <param name="name">The name of the function that caused the error.</param>
/// <remarks>If there is an issue, the program will close, returning <c>EXIT_FAILURE</c></remarks>
inline void checkErr(cl_int err, const char * name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/// <summary>Main entry-point for this application.</summary>
/// <returns>Exit-code for the process - 0 for success, else an error code.</returns>
int main(void)
{
	cl_int err;

	// Initialize the OpenCL platform
	cl::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cerr << "Platform number is: " << platformList.size() << std::endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cerr << "Platform is by: " << platformVendor << "\n";
	cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
	// Select only which platform type to use (CPU or GPU)
	cl::Context context(CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
	checkErr(err, "Context::Context()");

	// Make the grid dimensions and overall size a multiple of WORK_GROUP_SIZE
	int gridWidth = (int)ceil(WIDTH/RESOLUTION);
	int remainder = gridWidth % WORK_GROUP_SIZE;
	gridWidth += WORK_GROUP_SIZE - remainder;

	int gridHeight = (int)ceil(HEIGHT/RESOLUTION);
	remainder = gridHeight % WORK_GROUP_SIZE;
	gridHeight += WORK_GROUP_SIZE - remainder;

	int length = gridHeight * gridWidth;

	// Place the ball at a given point.
	cl_float2 ball;
	ball.s[0] = 35.4331f - 6.8118f; // Ball X
	ball.s[1] = 43.30705f - 6.3730f; // Ball Y

	cl_float fieldResolution;
	fieldResolution = RESOLUTION;

	const int repulseCount = 10;

	cl_float2 basicRepulsers[repulseCount];

	// Create an OpenCL buffer for the input repulser array
	cl::Buffer inRepulsers(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2)*repulseCount, &basicRepulsers, &err);
	checkErr(err, "Buffer::Buffer()");

	// Create an OpenCL buffer to hold the output
	float* outH = new float[length];
	cl::Buffer outCl(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, length * sizeof(float), outH, &err);
	checkErr(err, "Buffer::Buffer()");

	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	// Load the kernel file
	std::ifstream file("D:\\Users\\Tony\\Documents\\tr-robot-football\\Simulator\\field_kernel.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, "lesson1_kernel.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	// Build the kernel
	err = program.build(devices, "");

	std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
	std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
	std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
	checkErr(err, "Program::build()");

	// Collect the "main" kernel
	cl::Kernel kernel(program, "main", &err);
	checkErr(err, "Kernel::Kernel()");

	// Set up the kernel's arguments
	err = kernel.setArg(0, sizeof(cl_float2), &ball);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(1, fieldResolution);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(2, inRepulsers);
	checkErr(err, "Kernel::setArg()");
	err = kernel.setArg(3, outCl);
	checkErr(err, "Kernel::setArg()");

	// Collect the "fieldAtPoints" kernel
	cl::Kernel pointsKernel(program, "fieldAtPoints", &err);
	checkErr(err, "Kernel::Kernel()");

	cl_float2 inputPoints[4];
	float oPoints[4];
	// Set up a buffer for the output points
	cl::Buffer outPoints(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 4 * sizeof(float), oPoints, &err);
	checkErr(err, "Buffer::Buffer()");
	// Set up a buffer for the input points
	cl::Buffer inPoints(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2)*4, &inputPoints, &err);
	checkErr(err, "Buffer::Buffer()");

	// Set up the pointsKernel arguments
	err = pointsKernel.setArg(0, sizeof(cl_float2), &ball);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(1, inPoints);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(2, inRepulsers);
	checkErr(err, "Kernel::setArg()");
	err = pointsKernel.setArg(3, outPoints);
	checkErr(err, "Kernel::setArg()");

	// Create a command queue
	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");

	// Run the main kernel on the platform
	/*err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(gridWidth, gridHeight), cl::NDRange(WORK_GROUP_SIZE, WORK_GROUP_SIZE));
	checkErr(err, "CommandQueue::CommandQueue()");

	// Read out the results
	err = queue.enqueueReadBuffer(outCl, CL_TRUE, 0, length * sizeof(float), outH);

	checkErr(err, "CommandQueue::enqueueReadBuffer()");*/

	// Run the points kernel on the platform
	err = queue.enqueueNDRangeKernel(pointsKernel, cl::NullRange, cl::NDRange(4), cl::NDRange(4));
	checkErr(err, "CommandQueue::CommandQueue()");

	// Read out the results
	err = queue.enqueueReadBuffer(outPoints, CL_TRUE, 0, 4 * sizeof(float), oPoints);
	checkErr(err, "CommandQueue::enqueueReadBuffer()");

	// Wait for everything to finish
	queue.finish();
	checkErr(err, "CommandQueue::Finish()");

	return EXIT_SUCCESS;
}