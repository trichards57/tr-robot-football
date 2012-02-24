// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the STRATEGY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// STRATEGY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef Strategy_H
#define Strategy_H

#ifdef STRATEGY_EXPORTS
#define STRATEGY_API __declspec(dllexport)
#else
#define STRATEGY_API __declspec(dllimport)
#endif

// Wizard - This class is exported from the Strategy.dll
class STRATEGY_API CStrategy {
public:
	CStrategy(void);
	// TODO: add your methods here.
	//Declare all the module variables here	

};
//Wizard
extern STRATEGY_API int nStrategy;
//Wizard
STRATEGY_API int fnStrategy(void);

#include <string.h>		//might not be required, from simulator
#include <stdio.h>		//might not be required, from simulator

const long PLAYERS_PER_SIDE = 5;
/* The following are required for the simulator.
// At some point we are going to separate out the simulator from the real time RSE.
// If a simulator dll is required it will be defined by a variable on the compiler. 
*/

// Simulator -- gameState
const long FREE_BALL = 1;
const long PLACE_KICK = 2;
const long PENALTY_KICK = 3;
const long FREE_KICK = 4;
const long GOAL_KICK = 5;

// Simulator -- whosBall
const long ANYONES_BALL = 0;
const long BLUE_BALL = 1;
const long YELLOW_BALL = 2;

// Simulator -- global variables -- Useful field positions ... maybe?? in inches
const double FTOP = 77.2392;
//Ftop is the y coordinate for the top of the field that is the wall closest to the top of your screen. 
const double FBOT = 6.3730;
//Fbot is the y coordinate for the bottom of the field that is the wall closest to the bottom of your screen. 
const double GTOPY = 49.6801;
//GtopY is the y coordinate for the opening of the goal closest to the top of the screen for either goal 
const double GBOTY = 33.9320;
//Gbotx is the x coordinate for the opening of the goal closest to the bottom of the screen for either goal. 
const double GRIGHT = 97.3632;
//gRight is the x coordinate for the middle of the back wall of the right goal, that is the back wall of the goal closest to the right side of your screen 
const double GLEFT = 2.8748;
//gLeft is the x coordinate for the middle of the back wall of the left goal, that is the back wall of the goal closest to the Left side of your screen 
const double FRIGHTX = 93.4259;
//FrightX is the x coordinate for the right wall of the field that is the wall closest to the right side of your screen 
const double FLEFTX = 6.8118;
//FLeftX is the x coordinate for the Left wall of the field that is the wall closest to the Left side of your screen 

typedef struct
{
	double x, y, z;
} Vector3D;

typedef struct
{
	Vector3D pos;
	double rotation;
	double velocityLeft, velocityRight;
} Robot;

typedef struct
{
	Vector3D pos;
	double rotation;
} OpponentRobot;

typedef struct
{
	Vector3D pos;
} Ball;

typedef struct
{
	long left, right, top, bottom;
} Bounds;

typedef struct
{
	Robot home[PLAYERS_PER_SIDE];
	OpponentRobot opponent[PLAYERS_PER_SIDE];
	Ball currentBall, lastBall, predictedBall;
	Bounds fieldBounds, goalBounds;
	long gameState;
	long whosBall;
	void *userData;
} Environment;

typedef void (*MyStrategyProc)(Environment*);

#endif // Strategy_H
