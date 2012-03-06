#pragma once

#include "stdafx.h"

/// @brief All the native route finders
///
/// Namespace intended to contain all the native route finders.
namespace NativeRouteFinders
{
	/// @brief The types of GridSquare available.
	///
	/// Represents the different types of GridSquare that can exist in an environment.
	enum SquareType
	{
		/// @brief A square containing nothing
		Empty = 0,
		/// @brief A square to start a route from
		Origin,
		/// @brief A square that cannot be passed
		Obstacle,
		/// @brief A square to end a route on
		Destination
	};

	/// @brief A square in a discretised environment
	///
	/// Represents a discrete square in an environment, as required by some algorithms such as A*.
	class GridSquare
	{
	public:
		/// @brief The known score for the square.
		///
		/// Shows how far the square is from the start of the route.
		float KnownScore;
		/// @brief The heuristic score for the square.
		///
		/// The score for the square determined by the current heuristic method.
		float HeuristicScore;
		/// @brief The total score for the square.
		/// @return A single-precision floating point number
		///
		/// The sum of the KnownScore and HeuristicScore.
		float GetTotalScore() { return KnownScore + HeuristicScore; }
		/// @brief The type of square.
		///
		/// Used by the route finding algorithms to determine the KnownScore.
		SquareType Type;
		/// @brief The location of the square.
		///
		/// The (x,y) coordinates of the square in the field.  Used to save recalculating the position of
		/// the square every time it is needed.
		POINT Location;
		/// @brief Initializes a new instance of the GridSquare class.
		///
		/// Sets all member values to their defaults of:
		/// 
		/// * KnownScore = 0
		///
		/// * HeuristicScore = 0
		///
		/// * Type = SquareType::Empty
		///
		/// * Location = (0,0)
		GridSquare() { KnownScore = 0; HeuristicScore = 0; Type = Empty; Location.x = 0; Location.y = 0; }
	};
}