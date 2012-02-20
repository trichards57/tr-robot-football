// NativeTestBed.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MotionPrediction.h"

int _tmain(int argc, _TCHAR* argv[])
{
	float matrix[] = {1,2,3,4, 1,3,2,4, 1,2,4,3, 2,1,4,3};

	MotionPrediction predictor;
	predictor.InvertMatrix(matrix);

	return 0;
}

