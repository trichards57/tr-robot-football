#pragma once
#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

/// @brief Represents data stored by the simulator between time-steps
class UserData
{
public:
	/// @brief The target position currently being aimed for.
	Vector3D targetPosition;
};