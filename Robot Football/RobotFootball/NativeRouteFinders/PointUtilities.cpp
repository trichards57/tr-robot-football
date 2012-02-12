#include "StdAfx.h"
#include "PointUtilities.h"

POINT PointFromIndex(int index, int width)
{
	POINT p;
	p.x = index % width;
	p.y = index / width;

	return p;
}

int IndexFromPoint(LONG x, LONG y, LONG width)
{
	return x + y * width;
}

int IndexFromPoint(POINT point, LONG width)
{
	return point.x + point.y * width;
}