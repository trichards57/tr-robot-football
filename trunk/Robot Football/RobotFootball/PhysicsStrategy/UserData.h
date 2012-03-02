#pragma once

#include "stdafx.h"

enum ExperimentState
{
	RobotMotion,
	RobotMotionSweep
};

class UserData
{
public:
	ExperimentState State;
	std::vector<double> x;
	std::vector<double> y;
	std::fstream outStream;
	int velocityStartTime;
	int velocity;

	UserData()
		: State (RobotMotion), outStream("D:\\Users\\Tony\\Documents\\DataDump.csv", std::fstream::out | std::fstream::app)
	{ }
	~UserData()
	{
		outStream.flush();
		outStream.close();
	}
};