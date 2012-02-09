#include "StdAfx.h"
#include "PointUtilities.h"

POINT PointFromIndex(int index, int width)
{
	POINT p;
	p.x = index % width;
	p.y = index / width;

	return p;
}

int IndexFromPoint(POINT point, int width)
{
	return point.x + point.y * width;
}