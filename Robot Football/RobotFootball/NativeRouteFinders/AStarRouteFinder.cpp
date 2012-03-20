#include "stdafx.h"

#include "GridSquare.h"
#include "PositionedObject.h"
#include "PointUtilities.h"

namespace NativeRouteFinders
{
	/// @brief Initializes a given grid using the information provided.
	void InitGrid(POINT startPoint, POINT endPoint, SIZE fieldSize, SIZE resolution, SIZE gridSize, 
				  GridSquare** grid, int objectClearance, SIZE movingObjectSize, 
				  PositionedObject* opponents, int opponentsCount);

	/// @brief Reconstructs a path from the given cameFrom information.
	unique_ptr<vector<POINT>> ReconstructPath(map<GridSquare*, GridSquare*>* cameFrom, GridSquare* currentSquare);
	/// @brief Calculates the heuristic score for the given square
	float CalculateHeuristic(GridSquare* square, POINT endPoint, bool useSqrt);
	/// @brief Calculates the length from one point to another
	float CalculateLength(POINT startPoint, POINT endPoint, bool useSqrt);

	/// @brief Initializes the Native A* Route Finder
	///
	/// This function is used by managed software to initialize the DLL for time-testing processes.
	/// It is used to ensure that the late-bound DLL has been loaded, so that the loading time does
	/// not register in the time tests.
	///
	/// @returns ERROR_SUCCESS under all conditions.
	NATIVEROUTEFINDERS_API HRESULT Init()
	{
		return ERROR_SUCCESS;
	}

	/// @brief Finds a route using the A* algorithm
	/// 
	/// @param startPoint The point to start the algorithm from.
	/// @param endPoint The point to find a route to
	/// @param fieldSize The size of the field that will be searched.
	/// @param resolution The size of each square in the field, once it has been discretized
	/// @param movingObjectSize The size of the object that will follow the route
	/// @param objectClearance The distance that obstacles must be cleared by
	/// @param opponents The opponents that act as obstacles
	/// @param opponentsCount The length of the opponents array
	/// @param routeResult The route that has been produced by the algorithm (an output parameter)
	/// @param routeResultLength The length of the routeResult array
	/// @param useSqrt If false, all distance algorithms skip the square root step of the calculation
	///
	/// @return ERROR_SUCCESS if a route has been found and placed in routeResult
	/// @return E_INVALIDARG if the specified resolution is 0
	/// @return E_FAIL if no route has been found
	/// @return ERROR_INSUFFICIENT_BUFFER if the find route is too long for the given buffer
	///
	/// @bug Does not currently detect when the 'neighboring squares have actually wrapped onto the next line
	///
	/// Determines a route from startPoint to endPoint using the A* algorithm.
	///
	/// Algorithm taken from @cite aiModernApproach.
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

		// Loop through until there are no more squares to examine.
		while (!openSet.empty())
		{
			// Search the best squares first.
			auto smallestSquare = min_element(openSet.begin(), openSet.end(), [](GridSquare* square1, GridSquare* square2) { return square1->GetTotalScore() < square2->GetTotalScore(); });

			auto square = (*smallestSquare);
			if (square->Type == Destination)
			{
				// We've arrived!
				unique_ptr<vector<POINT>> path = ReconstructPath(&cameFrom, cameFrom[square]);
				path->push_back(square->Location);
				if (path->size() > routeResultLength)
					return ERROR_INSUFFICIENT_BUFFER;
				// Copy the result into the given buffer
				for (unsigned int i = 0; i < path->size(); i++)
				{
					routeResult[i].x = (*path)[i].x * resolution.cx;
					routeResult[i].y = (*path)[i].y * resolution.cy;
				}
				return ERROR_SUCCESS;
			}

			// Remove the square that has just been examined from the open set and put it into the closed set.
			openSet.erase(smallestSquare, smallestSquare+1);
			closedSet.push_back(square);

			auto index = IndexFromPoint(square->Location, gridSize.cx);

			int neighbourIndexes[] = { index + gridSize.cx + 1, 
									  index - gridSize.cx + 1,
									  index + gridSize.cx - 1, 
									  index - gridSize.cx - 1, index + 1, 
									  index - 1, 
									  index + gridSize.cx, 
									  index - gridSize.cx,
								  
			};


			// Check all of the current squares neighbours
			for (auto i = 0; i < 8; i++)
			{
				if (neighbourIndexes[i] < 0 || neighbourIndexes[i] >= gridSize.cx * gridSize.cy)
					continue; // Skip any indexes out of the range of the grid

				auto neighbour = grid[neighbourIndexes[i]];
			
				// Is the neighbour in the closed set?
				if (find(closedSet.begin(), closedSet.end(), neighbour) != closedSet.end())
					// Yes, so skip it as it's already been processed.
					continue;

				auto tentativeScore = square->KnownScore + CalculateLength(square->Location, neighbour->Location, useSqrt);
				auto tentativeIsBetter = false;

				// Is the neighbout in the open set?
				if (find(openSet.begin(), openSet.end(), neighbour) == openSet.end())
				{
					// No, so add it and make a guess at it's distance to the destination
					openSet.push_back(neighbour);
					neighbour->HeuristicScore = CalculateHeuristic(neighbour, endPoint, useSqrt);
					tentativeIsBetter = true;
				}
				else if (tentativeScore < neighbour->KnownScore)
					// Yes, and it's closer to the start than previously thought
					tentativeIsBetter = true;
				else
					// No
					tentativeIsBetter = false;

				// Is this square closer to the start than previously examined neigbouts?
				if (tentativeIsBetter)
				{
					// Mark this square as coming from the original square
					cameFrom[neighbour] = square;
					neighbour->KnownScore = tentativeScore;
				}
			}
		}

		// Run out of squares to examine without finding the destination, therefore no route possible.
		return E_FAIL;
	}
						
	/// @brief Finds a route using the A* algorithm
	/// 
	/// @param startPoint The point to start the algorithm from.
	/// @param endPoint The point to find a route to
	/// @param fieldSize The size of the field that will be searched.
	/// @param resolution The size of each square in the field, once it has been discretised
	/// @param movingObjectSize The size of the object that will follow the route
	/// @param objectClearance The distance that obstacles must be cleared by
	/// @param opponents The opponents that act as obstacles
	/// @param opponentsCount The length of the opponents array
	/// @param routeResult The route that has been produced by the algorithm (an output parameter)
	/// @param routeResultLength The length of the routeResult array
	/// @param useSqrt If false, all distance algorithms skip the square root step of the calculation
	///
	/// @return ERROR_SUCCESS if a route has been found and placed in routeResult
	/// @return E_INVALIDARG if the specified resultion is 0
	/// @return E_FAIL if no route has been found
	/// @return ERROR_INSUFFICIENT_BUFFER if the find route is too long for the given buffer
	///
	/// @bug Does not currently detect when the 'neighbouring' squares have actually wrapped onto the next line
	///
	/// Determines a route from startPoint to endPoint using the A* algorithm.  Uses an unordered_set to 
	/// store the open and closed sets to speed up searches.  Otherwise, the algorithm is identical to
	/// AStarFindRoute
	///
	/// Algorithm taken from @cite aiModernApproach.
	///
	/// @see AStarFindRoute
	NATIVEROUTEFINDERS_API HRESULT HashMapAStarFindRoute(POINT startPoint, POINT endPoint, 
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

		unordered_set<GridSquare*> openSet;
		unordered_set<GridSquare*> closedSet;
		map<GridSquare*, GridSquare*> cameFrom;

		// Would rather do this in parallel, but concurrent_vector doesn't allow you to remove things,
		// which makes later things more awkward.
		for_each(grid, grid + gridSize.cx * gridSize.cy, [=,&openSet](GridSquare* square)
		{
			if (square->Type != Origin)
				return;
			square->KnownScore = 0;
			square->HeuristicScore = CalculateHeuristic(square, endPoint, useSqrt);
			openSet.insert(square);
		});

		while (!openSet.empty())
		{
			auto smallestSquare = min_element(openSet.begin(), openSet.end(), [](GridSquare* square1, GridSquare* square2) { return square1->GetTotalScore() < square2->GetTotalScore(); });

			auto square = (*smallestSquare);
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

			openSet.erase(smallestSquare);
			closedSet.insert(square);

			auto index = IndexFromPoint(square->Location, gridSize.cx);

			int neighbourIndexes[] = { index + gridSize.cx + 1, 
									  index - gridSize.cx + 1,
									  index + gridSize.cx - 1, 
									  index - gridSize.cx - 1, index + 1, 
									  index - 1, 
									  index + gridSize.cx, 
									  index - gridSize.cx,
								  
			};

			for (auto i = 0; i < 8; i++)
			{
				if (neighbourIndexes[i] < 0 || neighbourIndexes[i] >= gridSize.cx * gridSize.cy)
					continue; // Skip any indexes out of the range of the grid

				auto neighbour = grid[neighbourIndexes[i]];
			
				if (closedSet.find(neighbour) != closedSet.end())
					continue;

				auto tentativeScore = square->KnownScore + CalculateLength(square->Location, neighbour->Location, useSqrt);
				auto tentativeIsBetter = false;

				if (openSet.find(neighbour) == openSet.end())
				{
					openSet.insert(neighbour);
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

    /// Takes the already allocated GridSquare array grid and sets it up for use by AStarFindRoute and 
	/// HashMapAStarFindRoute.
	///
	/// Uses objectClearance and movingObjectSize to determine the apparent size of the opponents,
	/// and marks the squares that contain them as SquareType::Obstacle.
	///
	/// Marks the square that contains endPoint as SquareType::Destination.
	///
	/// Marks the square that contains startPoint as SquareType::Origin.
	///
	/// Works in parallel as far as possible, using the Microsoft Parallel Patterns Library 
	/// (http://msdn.microsoft.com/en-us/library/dd492418.aspx)
	void InitGrid(POINT startPoint, POINT endPoint, SIZE fieldSize, SIZE resolution, SIZE gridSize, 
				  GridSquare** grid, int objectClearance, SIZE movingObjectSize, 
				  PositionedObject* opponents, int opponentsCount)
	{
		// The number of elements in the grid array
		const int gridLength = gridSize.cx * gridSize.cy;

		// Work out the size of the player in squares
		SIZE playerSize;
		playerSize.cx = (LONG)ceil((float)(movingObjectSize.cx + objectClearance) / (resolution.cx * objectClearance));
		playerSize.cy = (LONG)ceil((float)(movingObjectSize.cy + objectClearance) / (resolution.cy * objectClearance));

		// Make the clearance around the players square to simplify things
		auto clearance = max(playerSize.cx,playerSize.cy);

		// Initialise each array element and set it's GridSquare::Location based on it's array location.
		parallel_for(0, gridLength, [=](int i) { grid[i] = new GridSquare(); grid[i]->Location = PointFromIndex(i, gridSize.cx); });

		// Set up the area covered by each opponent as a SquareType::Obstacle
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

		// Mark the destination and origin of the route	
		auto gridEndX = (LONG)floorf((float)endPoint.x / resolution.cx);
		auto gridEndY = (LONG)floorf((float)endPoint.y / resolution.cy);

		grid[IndexFromPoint(gridEndX, gridEndY, gridSize.cx)]->Type = Destination;

		auto gridStartX = (LONG)floorf((float)startPoint.x / resolution.cx);
		auto gridStartY = (LONG)floorf((float)startPoint.y / resolution.cy);

		grid[IndexFromPoint(gridStartX, gridStartY, gridSize.cx)]->Type = Origin;
	}

	/// @param startPoint The point to measure from
	/// @param endPoint The point to measure to
	/// @param useSqrt If set to false, skip the square-root step
	/// 
	/// Calculates the distance from startPoint to endPoint using the Euclidian distance.
	///
	/// @note Setting useSqrt to false does not appear to produce any time savings.  In fact, 
	/// some experimentation suggests it takes longer
	/// @return A floating point number representing the distance from startPoint to endPoint
	float CalculateLength(POINT startPoint, POINT endPoint, bool useSqrt)
	{
		auto xLength = startPoint.x - endPoint.x;
		auto yLength = startPoint.y - endPoint.y;

		double val = xLength*xLength + yLength*yLength;

		if (useSqrt)
			val = sqrt(val);
		return (float)val;
	}

	/// @param square The square to calculate the heuristic for
	/// @param endPoint The current point to aim for
	/// @param useSqrt If false, all distance algorithms skip the square root step of the calculation
	///
	/// Calculates the heuristic score for the given square.
	///
	/// @return \f$+\infty\f$ if the square is an obstacle
	/// @return CalculateLength(square->Location, endPoint, useSqrt) otherwise
	float CalculateHeuristic(GridSquare* square, POINT endPoint, bool useSqrt)
	{
		if (square->Type == Obstacle)
			return numeric_limits<float>::infinity();
		return CalculateLength(square->Location, endPoint, useSqrt);
	}

	/// @param cameFrom The list of 'came from' details produced by the A* algorithm
	/// @param currentSquare The current square to examine
	///
	/// A recursive algorithm that reconstructs the path that has been produced by the A* algorithm
	///
	/// Algorithm taken from @cite aiModernApproach
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
}