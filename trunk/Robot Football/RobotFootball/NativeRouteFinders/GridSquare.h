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
};