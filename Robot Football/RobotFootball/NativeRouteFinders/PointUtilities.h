#pragma once

#include "stdafx.h"

POINT PointFromIndex(int index, int width);
int IndexFromPoint(POINT point, LONG width);
int IndexFromPoint(LONG x, LONG y, LONG width);
