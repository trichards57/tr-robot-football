#include "..\..\..\Simulator\Strategy Source\Strategy.h"
#include <math.h>

#define __NO_STD_VECTOR
#include <CL/cl.hpp>

namespace Strategies
{
	/// <summary>A set of utilities to make working with Vector3D and cl_float2 easier.</summary>
	class Utilities
	{
	public:
		/// <summary>Calculates the lengths of a.</summary>
		/// <param name="a">The Vector3D to get the length of.</param>
		/// <returns>The length of a.</returns>
		///
		/// The length is defined as @f$ l = \sqrt{\left(x^2+y^2\right)} @f$
		static float Length(const Vector3D a);
		/// <summary>Calculates the length of the vector between a and b.</summary>
		/// <param name="a">The const Vector3D to process.</param>
		/// <param name="b">The const Vector3D to process.</param>
		/// <returns>The length of (a - b).</returns>
		///
		///	Uses Length(a-b)
		static float Length(const Vector3D a, const Vector3D b);
		/// <summary>Subtracts a from b, componentwise.</summary>
		/// <param name="a">The first Vector3D.</param>
		/// <param name="b">The second Vector3D.</param>
		/// <returns>A Vector3D containing a-b.</returns>
		static Vector3D Subtract(const Vector3D a, const Vector3D b);
		/// <summary>Subtracts a set of Bounds from a Vector3D.</summary>
		/// <param name="a">The Vector3D to process.</param>
		/// <param name="b">The Bounds to process.</param>
		/// <returns>The Vector3D, offset so that is relative to the origin of the Bounds.</returns>
		static Vector3D Subtract(const Vector3D a, const Bounds b);
		/// <summary>Converts a Vector3D (from the Simulator) to a cl_float2 (for OpenCL).</summary>
		/// <param name="in">The input Vector3D.</param>
		/// <returns>The x and y properties of the Vector3D as a cl_float2.</returns>
		static cl_float2 Vector3DToCLFloat2(const Vector3D in);
		/// <summary>Moves a Vector3D by the amounts given in right and up.</summary>
		/// <param name="a">	The Vector3D to process.</param>
		/// <param name="right">The amount to move the Vector3D right by.</param>
		/// <param name="up">   The amount to move the Vector3D up by.</param>
		/// <returns>The Vector3D, translated by right and up.</returns>
		static Vector3D Move(const Vector3D a, const float right, const float up);
		/// <summary>Divides a Vector3D by the given value, componentwise.</summary>
		/// <param name="a">  The Vector3D to process.</param>
		/// <param name="val">The value to divide the components by.</param>
		/// <returns>The Vector3D with the x and y elements divided by val.</returns>
		static Vector3D Divide(const Vector3D a, const double val);

		static Vector3D Multiply(const Vector3D a, const double val);
	};
}