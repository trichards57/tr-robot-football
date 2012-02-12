#pragma once

#include "stdafx.h"

enum SquareType
{
	Empty = 0,
	Origin,
	Obstacle,
	Destination
};

class GridSquare
{
public:
	float KnownScore;
	float HeuristicScore;
	float GetTotalScore() { return KnownScore + HeuristicScore; }
	SquareType Type;
	POINT Location;
	GridSquare() { KnownScore = 0; HeuristicScore = 0; Type = Empty; Location.x = 0; Location.y = 0; }
};