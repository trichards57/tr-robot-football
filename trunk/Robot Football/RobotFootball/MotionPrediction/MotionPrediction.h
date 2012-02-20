#pragma once
#include "stdafx.h"

#ifdef MOTIONPREDICTION_EXPORTS
#define MOTIONPREDICTION_API __declspec(dllexport)
#else
#define MOTIONPREDICTION_API __declspec(dllimport)
#endif

/// <summary>
/// Represents a quadratic equation of the form ax^2+bx+c
/// </summary>
struct QuadraticEquation {
public:
	double a,b,c;
};

// This class is exported from the MotionPrediction.dll
class MOTIONPREDICTION_API MotionPrediction {
private:
	float* timestepMatrix;
public:
	/// <summary>
	/// Invert a 4x4 matrix stored as an array in <paramref name="src" />.
	/// </summary>
	/// Uses Cramer's rule and the processor's Streaming SIMD instructions to efficiently invert a 4x4 matrix.
	///
	/// Returns the inverted matrix back in to <paramref name="src" />.
	///
	/// Taken from the @cite IntelMatrixInvertDoc
	void InvertMatrix(float* src);
public:
	void InitTimestep(float timestep);
	QuadraticEquation CalculateQuadraticMotion(float readings[3]);
	MotionPrediction() : timestepMatrix(nullptr) {}
};