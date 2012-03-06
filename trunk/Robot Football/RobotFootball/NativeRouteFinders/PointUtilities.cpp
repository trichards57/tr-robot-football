#include "StdAfx.h"
#include "PointUtilities.h"

namespace NativeRouteFinders
{
	/// @param index The index to convert
	/// @param width The width of the grid
	///
	/// Converts an array index into a set of coordinates, using the width of the field. Works using
    /// equations:
	///
	/// \f$ x = index \mod width \f$
	///
	/// \f$ y = index \div width \f$
	///
	/// @returns A POINT structure containing x and y as LONGs
	POINT PointFromIndex(int index, int width)
	{
		POINT p;
		p.x = index % width;
		p.y = index / width;

		return p;
	}

	/// @param x The x coordinate
	/// @param y The y coordinate
	/// @param width The width of the grid
	/// 
	/// Converts a coordinate into an array index, using the width of the field. Works using the equation:
	///
	/// \f$ index = x + y \times width \f$
	///
	/// @returns An int containing the index
	int IndexFromPoint(LONG x, LONG y, LONG width)
	{
		return x + y * width;
	}

	/// @param point The coordinate
	/// @param width The width of the grid
	///
	/// Converts a coordinate into an array index, using the width of the field. Works using the equation:
	///
	/// \f$ index = point.x + point.y \times width \f$
	///
	/// @returns An int containing the index
	int IndexFromPoint(POINT point, LONG width)
	{
		return point.x + point.y * width;
	}
}