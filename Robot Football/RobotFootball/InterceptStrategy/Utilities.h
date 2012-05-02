#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#define _USE_MATH_DEFINES
#include <math.h>

/// @file Utilities.h 
/// @brief Defines the Utilities class and contains useful precompiler instances

/// @brief Converts an angle in degrees into radians
#define DEGREES_TO_RADIANS(X) (X * M_PI / 180)

#define __NO_STD_VECTOR
#include <CL/cl.hpp>

namespace Strategies
{
	/// @brief A set of utilities to make working with Vector3D and cl_float2 easier.
	class Utilities
	{
	public:
		/// @brief Calculates the lengths of a.
		/// @param[in] a The Vector3D to get the length of.
		/// @return The length of a.
		///
		/// The length is defined as @f$ l = \sqrt{\left(x^2+y^2\right)} @f$
		static float Length(const Vector3D a);
		/// @brief Calculates the length of the vector between a and b.
		/// @param[in] a The start Vector3D.
		/// @param[in] b The end Vector3D.
		/// @return The length of (a - b).
		///
		///	Uses Length(a-b)
		static float Length(const Vector3D a, const Vector3D b);
		/// @brief Subtracts a from b, componentwise.
		/// @param[in] a The first Vector3D.
		/// @param[in] b The second Vector3D.
		/// @return A Vector3D containing a-b.
		static Vector3D Subtract(const Vector3D a, const Vector3D b);
		/// @brief Subtracts a set of Bounds from a Vector3D.
		/// @param[in] a The Vector3D to process.
		/// @param[in] b The Bounds to process.
		/// @return The Vector3D, offset so that is relative to the origin of the Bounds.
		static Vector3D Subtract(const Vector3D a, const Bounds b);
		/// @brief Converts a Vector3D (from the Simulator) to a cl_float2 (for OpenCL).
		/// @param[in] in The input Vector3D.
		/// @return The x and y properties of the Vector3D as a cl_float2.
		static cl_float2 Vector3DToCLFloat2(const Vector3D in);
		/// @brief Moves a Vector3D by the amounts given in right and up.
		/// @param[in] a The Vector3D to process.
		/// @param[in] right The amount to move the Vector3D right by.
		/// @param[in] up The amount to move the Vector3D up by.
		/// @return The Vector3D, translated by right and up.
		static Vector3D Move(const Vector3D a, const float right, const float up);
		/// @brief Divides a Vector3D by the given value, componentwise.
		/// @param[in] a The Vector3D to process.
		/// @param[in] val The value to divide the components by.
		/// @return The Vector3D with the x and y elements divided by val.
		static Vector3D Divide(const Vector3D a, const double val);
		/// @brief Multiplies a Vector3D by a given value, componentwise.
		/// @param[in] a The Vector3D to process.
		/// @param[in] val The value to multiply the components by.
		/// @return The Vector3D with teh x and y elements multiplied by val.
		static Vector3D Multiply(const Vector3D a, const double val);
	};
}