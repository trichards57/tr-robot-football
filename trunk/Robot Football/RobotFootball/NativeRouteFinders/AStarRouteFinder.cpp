#include "stdafx.h"

#include "Field.h"
#include "GridSquare.h"
#include "PositionedObject.h"
#include "PointUtilities.h"

NATIVEROUTEFINDERS_API HRESULT AStarFindRoute(POINT startPoint, POINT endPoint, 
	                                          Field field, SIZE resolution, 
											  PositionedObject movingObject, int objectClearance,
											  PositionedObject* opponents, int opponentsCount)
{
	// Prevent any divide by zero errors
	if (resolution.cx < 1 || resolution.cy < 1)
		return E_INVALIDARG;

	// Calculate the number of squares in the grid
	SIZE gridSize;
	gridSize.cx = field.FieldSize.cx / resolution.cx;
	gridSize.cy = field.FieldSize.cy / resolution.cy;

	// Create the grid as an array (for speed of access)
	auto grid = new GridSquare*[gridSize.cx * gridSize.cy];

	return 0;
}

void InitGrid(POINT startPoint, POINT endPoint, Field field, SIZE resolution, SIZE gridSize, 
	          GridSquare** grid, int objectClearance, PositionedObject movingObject, 
			  PositionedObject* opponents, int opponentsCount)
{
	// The number of elements in the grid array
	const int gridLength = gridSize.cx * gridSize.cy;

	SIZE playerSize;
	playerSize.cx = (LONG)ceil((float)(movingObject.Size.cx + objectClearance) / (resolution.cx * objectClearance));
	playerSize.cy = (LONG)ceil((float)(movingObject.Size.cy + objectClearance) / (resolution.cy * objectClearance));

	auto clearance = max(playerSize.cx,playerSize.cy);

	parallel_for(0, gridLength, [=](int i) { grid[i] = new GridSquare(); grid[i]->Location = PointFromIndex(i, gridSize.cx); });

	parallel_for(0, opponentsCount, [=](int i) { 
		auto opponent = opponents[i];
		
		POINT centerGridPoint;
		centerGridPoint.x = (int)(floorf((float)opponent.Position.x / resolution.cx));
		centerGridPoint.y = (int)(floorf((float)opponent.Position.y / resolution.cy));

		auto minX = max(0, centerGridPoint.x - playerSize.cx - clearance);
		auto maxX = min(centerGridPoint.x + playerSize.cx + clearance, gridSize.cx);
		auto minY = max(0, centerGridPoint.y - playerSize.cy - clearance);
		auto maxY = min(centerGridPoint.y + playerSize.cy + clearance, gridSize.cy);

		for (auto i = minX; i < maxX; i++)
		{
			for (auto j = minY; j < maxY; j++)
			{
				POINT gridPoint;
				gridPoint.x = i;
				gridPoint.y = j;
				grid[IndexFromPoint(gridPoint, gridSize.cx)]->Type = SquareType::Obstacle;
			}
		}


	});
}