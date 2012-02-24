#pragma once

#include "stdafx.h"

enum ExperimentState
{
	RobotMotion
};

class UserData
{
public:
	ExperimentState State;
	std::vector<double> x;
	std::vector<double> y;
	std::fstream outStream;
	UserData()
		: State (RobotMotion), outStream("H:\\DataDump.csv", std::fstream::out | std::fstream::app)
	{ }
	~UserData()
	{
		outStream.flush();
		outStream.close();
	}
};