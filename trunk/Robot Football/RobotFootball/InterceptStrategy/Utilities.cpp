#include "stdafx.h"

#include "Utilities.h"

namespace Strategies
{
	float Utilities::Length(const Vector3D a)
	{
		return (float)sqrt(a.x*a.x + a.y*a.y);
	}

	Vector3D Utilities::Subtract(const Vector3D a, const Vector3D b)
	{
		Vector3D ret;

		ret.x = a.x - b.x;
		ret.y = a.y - b.y;

		return ret;
	}

	float Utilities::Length(const Vector3D a, const Vector3D b)
	{
		return Length(Subtract(a,b));
	}

	Vector3D Utilities::Subtract(const Vector3D a, const Bounds b)
	{
		Vector3D res;

		res.x = a.x - b.left;
		res.y = a.y - b.bottom;
		return res;
	}

	cl_float2 Utilities::Vector3DToCLFloat2(const Vector3D in)
	{
		cl_float2 out;

		out.s[0] = (float)in.x;
		out.s[1] = (float)in.y;
		return out;
	}

	Vector3D Utilities::Move(const Vector3D a, const float right, const float up)
	{
		Vector3D out;

		out.x = a.x + right;
		out.y = a.y + up;
		return out;
	}

	Vector3D Utilities::Divide(const Vector3D a, const double val)
	{
		Vector3D out;

		out.x = a.x / val;
		out.y = a.y / val;

		return out;
	}

	Vector3D Utilities::Multiply(const Vector3D a, const double val)
	{
		Vector3D out;

		out.x = a.x * val;
		out.y = a.y * val;

		return out;
	}
}