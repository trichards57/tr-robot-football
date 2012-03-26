#pragma once

#define OPENCL 1

#include <cstdlib>

#include <ppl.h>

using namespace Concurrency;

#ifdef OPENCL
#define __NO_STD_VECTOR
#include <CL/cl.hpp>
#endif

#include <iostream>
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
	cl::CommandQueue* queue;
	task_group taskGroup;
#endif
public:
	PotentialFieldGenerator(void);
	~PotentialFieldGenerator(void);
	Vector3D FieldVectorToBall(Robot bot, Environment *env);
};

