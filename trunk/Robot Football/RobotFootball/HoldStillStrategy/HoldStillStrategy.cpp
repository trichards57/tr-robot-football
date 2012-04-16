// HoldStillStrategy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\..\..\Simulator\Strategy Source\Strategy.h"

/// <summary>Creates this object.</summary>
/// <param name="env">The environment created by the simulator.</param>
extern "C" STRATEGY_API void Create(Environment* env)
{
}

/// <summary>Destroys this object.</summary>
/// <param name="env">The environment created by the simulator.</param>
/// <remarks>Does not appear to be called</remarks>
extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

/// <summary>Called on each time-step to control the robots</summary>
/// <param name="env">The environment created by the simulator.</param>
extern "C" STRATEGY_API void Strategy(Environment* env)
{
}