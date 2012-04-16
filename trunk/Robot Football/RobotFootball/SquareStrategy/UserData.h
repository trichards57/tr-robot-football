#pragma once
#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

/// <summary>Represents data stored by the simulator between time-steps</summary>
class UserData
{
public:
	/// <summary>The target position currently being aimed for.</summary>
	Vector3D targetPosition;
};