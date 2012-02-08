#include "stdafx.h"

#include "Field.h"
#include "GridSquare.h"
#include "PositionedObject.h"
#include "PointUtilities.h"

NATIVEROUTEFINDERS_API HRESULT AStarFindRoute(POINT startPoint, POINT endPoint, 
	                                          Field field, SIZE resolution, 
											  PositionedObject movingObject, int objectClearance)
{
	if (resolution.cx < 1 || resolution.cy < 1)
		return E_INVALIDARG;

	SIZE gridSize;
	gridSize.cx = field.FieldSize.cx / resolution.cx;
	gridSize.cy = field.FieldSize.cy / resolution.cy;

	auto grid = new GridSquare*[gridSize.cx * gridSize.cy];

	return 0;
}

void InitGrid(POINT startPoint, POINT endPoint, Field field, SIZE resolution, SIZE gridSize, 
	          GridSquare** grid, int objectClearance, PositionedObject movingObject)
{
	const int gridLength = gridSize.cx * gridSize.cy;

	SIZE playerSize;
	playerSize.cx = (LONG)ceil((float)(movingObject.Size.cx + objectClearance) / (resolution.cx * objectClearance));
	playerSize.cy = (LONG)ceil((float)(movingObject.Size.cy + objectClearance) / (resolution.cy * objectClearance));

	auto clearance = max(playerSize.cx,playerSize.cy);

	parallel_for(0, gridLength, [=](int i) { grid[i] = new GridSquare(); grid[i]->Location = PointFromIndex(i, gridSize.cx); });

}