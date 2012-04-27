#include "..\..\..\Simulator\Strategy Source\Strategy.h"

/// @file HoldStillStrategy.cpp
/// @brief Contains all the standard strategy functions for HoldStillStrategy

/// @brief Creates this object.
/// @param[in] env The environment created by the simulator.
extern "C" STRATEGY_API void Create(Environment* env)
{
}

/// @brief Destroys this object.
/// @param[in] env The environment created by the simulator.
/// @remark Does not appear to be called
extern "C" STRATEGY_API void Destroy(Environment* env)
{
}

/// @brief Called on each time-step to control the robots.
/// @param[in] env The environment created by the simulator.
extern "C" STRATEGY_API void Strategy(Environment* env)
{
}