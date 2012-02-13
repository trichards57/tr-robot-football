#include "stdafx.h"

#include "GridSquare.h"
#include "PositionedObject.h"
#include "PointUtilities.h"

void InitGrid(POINT startPoint, POINT endPoint, SIZE fieldSize, SIZE resolution, SIZE gridSize, 
			  GridSquare** grid, int objectClearance, SIZE movingObjectSize, 
			  PositionedObject* opponents, int opponentsCount);

unique_ptr<vector<POINT>> ReconstructPath(map<GridSquare*, GridSquare*>* cameFrom, GridSquare* currentSquare);
float CalculateHeuristic(GridSquare* square, POINT endPoint, bool useSqrt);
float CalculateLength(POINT startPoint, POINT endPoint, bool useSqrt);

NATIVEROUTEFINDERS_API HRESULT AStarFindRoute(POINT startPoint, POINT endPoint, 
											  SIZE fieldSize, SIZE resolution, 
											  SIZE movingObjectSize, int objectClearance,
											  PositionedObject* opponents, int opponentsCount,
											  POINT* routeResult, unsigned int routeResultLength, bool useSqrt)
{
	// Prevent any divide by zero errors
	if (resolution.cx < 1 || resolution.cy < 1)
		return E_INVALIDARG;

	// Calculate the number of squares in the grid
	SIZE gridSize;
	gridSize.cx = fieldSize.cx / resolution.cx;
	gridSize.cy = fieldSize.cy / resolution.cy;

	// Create the grid as an array (for speed of access)
	auto grid = new GridSquare*[gridSize.cx * gridSize.cy];

	InitGrid(startPoint, endPoint, fieldSize, resolution, gridSize, grid, objectClearance, movingObjectSize, opponents, opponentsCount);

	vector<GridSquare*> openSet;
	vector<GridSquare*> closedSet;
	map<GridSquare*, GridSquare*> cameFrom;

	// Would rather do this in parallel, but concurrent_vector doesn't allow you to remove things,
	// which makes later things more awkward.
	for_each(grid, grid + gridSize.cx * gridSize.cy, [=,&openSet](GridSquare* square)
	{
		if (square->Type != Origin)
			return;
		square->KnownScore = 0;
		square->HeuristicScore = CalculateHeuristic(square, endPoint, useSqrt);
		openSet.push_back(square);
	});

	while (!openSet.empty())
	{
		sort(openSet.begin(), openSet.end(), [](GridSquare* square1, GridSquare* square2) { return square1->GetTotalScore() < square2->GetTotalScore(); });

		auto square = openSet[0];
		if (square->Type == Destination)
		{
			unique_ptr<vector<POINT>> path = ReconstructPath(&cameFrom, cameFrom[square]);
			path->push_back(square->Location);
			if (path->size() > routeResultLength)
				return ERROR_INSUFFICIENT_BUFFER;
			for (unsigned int i = 0; i < path->size(); i++)
			{
				routeResult[i].x = (*path)[i].x * resolution.cx;
				routeResult[i].y = (*path)[i].y * resolution.cy;
			}
			return ERROR_SUCCESS;
		}

		openSet.erase(openSet.begin(), openSet.begin()+1);
		closedSet.push_back(square);

		auto index = IndexFromPoint(square->Location, gridSize.cx);

		int neighbourIndexes[] = { index + gridSize.cx + 1, 
								  index - gridSize.cx + 1,
								  index + gridSize.cx - 1, 
								  index - gridSize.cx - 1, index + 1, 
							      index - 1, 
								  index + gridSize.cx, 
								  index - gridSize.cx,
								  
		}; // BUG: This doesn't detect wrapping on to the next line...

		for (auto i = 0; i < 8; i++)
		{
			if (neighbourIndexes[i] < 0 || neighbourIndexes[i] >= gridSize.cx * gridSize.cy)
				continue; // Skip any indexes out of the range of the grid

			auto neighbour = grid[neighbourIndexes[i]];
			
			if (find(closedSet.begin(), closedSet.end(), neighbour) != closedSet.end())
				continue;

			auto tentativeScore = square->KnownScore + CalculateLength(square->Location, neighbour->Location, useSqrt);
			auto tentativeIsBetter = false;

			if (find(openSet.begin(), openSet.end(), neighbour) == openSet.end())
			{
				openSet.push_back(neighbour);
				neighbour->HeuristicScore = CalculateHeuristic(neighbour, endPoint, useSqrt);
				tentativeIsBetter = true;
			}
			else if (tentativeScore < neighbour->KnownScore)
				tentativeIsBetter = true;
			else
				tentativeIsBetter = false;

			if (tentativeIsBetter)
			{
				cameFrom[neighbour] = square;
				neighbour->KnownScore = tentativeScore;
			}
		}
	}

	return E_FAIL;
}

void InitGrid(POINT startPoint, POINT endPoint, SIZE fieldSize, SIZE resolution, SIZE gridSize, 
			  GridSquare** grid, int objectClearance, SIZE movingObjectSize, 
			  PositionedObject* opponents, int opponentsCount)
{
	// The number of elements in the grid array
	const int gridLength = gridSize.cx * gridSize.cy;

	SIZE playerSize;
	playerSize.cx = (LONG)ceil((float)(movingObjectSize.cx + objectClearance) / (resolution.cx * objectClearance));
	playerSize.cy = (LONG)ceil((float)(movingObjectSize.cy + objectClearance) / (resolution.cy * objectClearance));

	auto clearance = max(playerSize.cx,playerSize.cy);

	parallel_for(0, gridLength, [=](int i) { grid[i] = new GridSquare(); grid[i]->Location = PointFromIndex(i, gridSize.cx); });

	parallel_for(0, opponentsCount, [=](int i) { 
		auto opponent = opponents[i];

		POINT centerGridPoint;
		centerGridPoint.x = (LONG)(floorf((float)opponent.Position.x / resolution.cx));
		centerGridPoint.y = (LONG)(floorf((float)opponent.Position.y / resolution.cy));

		auto minX = max(0, centerGridPoint.x - playerSize.cx - clearance);
		auto maxX = min(centerGridPoint.x + playerSize.cx + clearance, gridSize.cx);
		auto minY = max(0, centerGridPoint.y - playerSize.cy - clearance);
		auto maxY = min(centerGridPoint.y + playerSize.cy + clearance, gridSize.cy);

		for (auto i = minX; i < maxX; i++)
		{
			for (auto j = minY; j < maxY; j++)
			{
				grid[IndexFromPoint(i, j, gridSize.cx)]->Type = Obstacle;
			}
		}
	});

	
	auto gridEndX = (LONG)floorf((float)endPoint.x / resolution.cx);
	auto gridEndY = (LONG)floorf((float)endPoint.y / resolution.cy);

	grid[IndexFromPoint(gridEndX, gridEndY, gridSize.cx)]->Type = Destination;

	auto gridStartX = (LONG)floorf((float)startPoint.x / resolution.cx);
	auto gridStartY = (LONG)floorf((float)startPoint.y / resolution.cy);

	grid[IndexFromPoint(gridStartX, gridStartY, gridSize.cx)]->Type = Origin;
}

//float CalculateLength(POINT startPoint, POINT endPoint)
//{
//	auto xLength = startPoint.x - endPoint.x;
//	auto yLength = startPoint.y - endPoint.y;
//
//	return (float)sqrt((double)xLength*xLength + yLength*yLength);
//}

float CalculateLength(POINT startPoint, POINT endPoint, bool useSqrt)
{
	auto xLength = startPoint.x - endPoint.x;
	auto yLength = startPoint.y - endPoint.y;

	float val = xLength*xLength + yLength*yLength;

	if (useSqrt)
		val = sqrt(val);
	return val;
}

float CalculateHeuristic(GridSquare* square, POINT endPoint, bool useSqrt)
{
	if (square->Type == Obstacle)
		return numeric_limits<float>::infinity();
	return CalculateLength(square->Location, endPoint, useSqrt);
}

unique_ptr<vector<POINT>> ReconstructPath(map<GridSquare*, GridSquare*>* cameFrom, GridSquare* currentSquare)
{
	if (cameFrom->find(currentSquare) != cameFrom->end())
	{
		auto path = ReconstructPath(cameFrom, cameFrom->at(currentSquare));
		path->push_back(currentSquare->Location);
		return path;
	}
	unique_ptr<vector<POINT>> path(new vector<POINT>());
	path->push_back(currentSquare->Location);
	return path;
}