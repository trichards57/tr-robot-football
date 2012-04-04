#pragma once

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
	int gridWidth;
	int gridHeight;
	int length;
	cl_float4 * points;
	cl::Kernel kernel;
	cl::Kernel possessionKernel;
	cl::Context context;
	cl::vector<cl::Device> devices;
	cl::Buffer outCl;
	cl::CommandQueue queue;
	task_group taskGroup;
	HANDLE pipe;
	Vector3D CalculateMainField(Environment* env, Robot bot);
	Vector3D CalculatePossessionField(Environment* env, Robot bot);
	bool latchClose;
public:
	PotentialFieldGenerator(void);
	~PotentialFieldGenerator(void);
	Vector3D FieldVectorToBall(int botIndex, Environment *env);
};

