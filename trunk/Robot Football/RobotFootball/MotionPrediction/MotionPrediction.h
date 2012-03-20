#pragma once
#include "stdafx.h"

#ifdef MOTIONPREDICTION_EXPORTS
#define MOTIONPREDICTION_API __declspec(dllexport)
#else
#define MOTIONPREDICTION_API __declspec(dllimport)
#endif

/// @brief Represents a quadratic equation of the form ax^2+bx+c
struct QuadraticEquation {
public:
	/// @brief a coefficient of the equation
	double a;
	/// @brief b coefficient of the equation
	double b;
	/// @brief c coefficient of the equation
	double c;
};

// This class is exported from the MotionPrediction.dll

/// @brief Class to handle prediction of motion
class MOTIONPREDICTION_API MotionPrediction {
private:
	/// @brief The pre-calculated timestep matrix
	float* timestepMatrix;
public:
	/// @brief Invert a 4x4 matrix stored as an array in src.
	void InvertMatrix(float* src);
public:
	/// @brief Initializes the timestep matrix
	void InitTimestep(float timestep);
	/// @brief Fits the readings to a quadratic equation
	QuadraticEquation CalculateQuadraticMotion(float readings[3]);
	/// @brief Initializes a new instance of the MotionPrediction class.
	MotionPrediction() : timestepMatrix(nullptr) {}
};