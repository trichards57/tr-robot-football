#pragma once

#include "stdafx.h"

namespace NativeRouteFinders
{

#pragma pack(push, 8)

	/// @brief An object with position and size
	///
	/// Represents an object with position and size that is used for a route finding algorithm.
	///
	/// Maps to RouteFinders::PositionedObject for native interop purposes.
	///
	/// Uses sequential layout, with packing alignment set to 8. 
	///
	/// @see RouteFinders::PositionedObject
	struct PositionedObject
	{
	public:
		/// @brief The size of the object
		SIZE Size;
		/// @brief The position of the object
		POINT Position;
	};

#pragma pack(pop)

}