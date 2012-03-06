#pragma once

#include "stdafx.h"

namespace NativeRouteFinders
{
	/// @brief Converts an array index to a POINT.
	POINT PointFromIndex(int index, int width);
	/// @brief Converts a POINT to an array index.
	int IndexFromPoint(POINT point, LONG width);
	/// @brief Converts a point (x,y) to an array index.
	int IndexFromPoint(LONG x, LONG y, LONG width);
}