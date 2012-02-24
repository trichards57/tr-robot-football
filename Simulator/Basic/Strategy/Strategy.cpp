// Strategy.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//Add RSE includes here
#include <stdlib.h>

#include "StrategyUtils.h"
#include "Movement.h"
#include "Goalie.h"
#include "Defender1.h"
#include "Defender2.h"
#include "Attacker1.h"
#include "Attacker2.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// Wizard - This is an example of an exported variable
STRATEGY_API int nStrategy=0;

// Wizard - This is an example of an exported function.
STRATEGY_API int fnStrategy(void)
{
	return 42;
}

// Wizard - This is the constructor of a class that has been exported.
// see Strategy.h for the class definition
CStrategy::CStrategy()
{ 
	return; 
}


//Must be defined - do NOT delete
extern "C" STRATEGY_API void Create ( Environment *env )
{	

}
//Must be defined - do NOT delete
extern "C" STRATEGY_API void Destroy ( Environment *env )
{

}

//Must be defined - do NOT delete
extern "C" STRATEGY_API void Strategy ( Environment *env )
{//Standard football interface
	//Uses mm for positions
	// Soccer Environment Specific
	RealEnvironment REnv;
	REnv = *(RealEnvironment*)env->userData;
	REnv.Speed = 10;		//Standard Speed

	// Fira: Strategy Coordinate System
	// All coords are in inches
	// 0 deg is facing forward (black triangle determines the front), and 
	// 0,0 is bottom right corner, when viewed from behind the yellow goal keeper.
	// + angles are anticlockwise 0,+180
	// - angles are clockwise 0,-180

	Goalie(env, REnv);
	Defender1(env, REnv);
	Defender2(env, REnv);
	Attacker1(env, REnv);
	Attacker2(env, REnv);
	//Process each player in turn, splitting them out for the group project
	
	UpdateObjectHistory(env);
}







