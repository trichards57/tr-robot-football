#pragma once

#define OPENCL 1
#define DELEGATEOPENCL 1

#include <cstdlib>

#include <ppl.h>

using namespace Concurrency;


#define __NO_STD_VECTOR
#include <CL/cl.hpp>

#include <iostream>
#include <conio.h>
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class PotentialFieldGenerator
{
private:
#ifdef OPENCL
	int gridWidth;
	int gridHeight;
	int length;
	cl_float4 * points;
	cl::Kernel kernel;
	cl::Context context;
	cl::vector<cl::Device> devices;
	cl::Buffer outCl;
	cl::CommandQueue queue;
	task_group taskGroup;
#endif
#ifdef DELEGATEOPENCL
	HANDLE pipe;
#endif
public:
	PotentialFieldGenerator(void);
	~PotentialFieldGenerator(void);
	Vector3D FieldVectorToBall(Robot bot, Environment *env);
};

