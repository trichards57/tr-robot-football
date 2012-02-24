#include "Strategy.h"			//FIRA supplied for compatibility with simulator
#include "Vector2d.h"

#define	REPULSIVE	0
#define	ATTRACTIVE	1
#define	P1  0
#define P2  1
#define P3  2
#define P4  3
#define P5  4

//A Free ball
#define BOTTOM_LEFT		1
#define BOTTOM_RIGHT	2
#define TOP_LEFT		3
#define TOP_RIGHT		4

//Extra Game States
#define PLAY_GAME		0	//Default game state
#define DEVELOPMENT     6	//for Development

typedef struct
{
	bool Detected;
	int InQuadrant;
	//Uses free ball definitions as viewed from behind our goal 
	int InPenArea, InGoalArea, InBackGoal;
	//0 = out of areas, 1 = in our areas, 2 = in other areas
} PenaltyState;

typedef struct
{
	double BallAttackAngle;
	double minStrikePosX;
	double minStrikePosY;
	double maxStrikePosX;
	double maxStrikePosY;
	double minStrikeAngle;
	double maxStrikeAngle;
} StrikePos;

typedef struct
{
	int OnTopEdge;
	int OnBottomEdge;
	int OnLeftSide;
	int OnRightSide;
} EdgeState;

typedef struct
{
	double GoalKeeperPosX;
	double GoalKeeperPosY;
} DefendPos;


typedef struct
{	
	PenaltyState home[PLAYERS_PER_SIDE + 1];		//Ball is the last element
	EdgeState OnEdgeState[PLAYERS_PER_SIDE + 1];	//Object on edges, ball is the last element
	StrikePos BallStrikePos;						//Hitting the ball to the other goal
	DefendPos GoalKeeperPos;						//In line with the ball
	int NumPenaltyArea;								//Number of players in our pen area, excludes the goal area
	int NumGoalArea;								//Number of players in our goal area
	int NumOtherGoalArea;							//Number of players in their goal area
	long Position;									//Free Ball positions
	bool IsBlueTeam;
	long PlayTo;
	long RobotID;
	long Speed;
	Robot RobotHistory[5][3];
	Vector2d BallHistory[3];
} RealEnvironment;

static	Robot RobotHistory[5][3];
static	Vector2d BallHistory[3];


double Gaussian(double x , double u , double o);
void PutObjectHistory(RealEnvironment *REnv);
void UpdateObjectHistory(Environment *env);

const double FWIDTH     = FTOP + FBOT;
const double FLENGTH    = FLEFTX + FRIGHTX;

//Declare all the functions to be run in the strategy here
void Player1(Environment *env, RealEnvironment REnv);
void Player2(Environment *env, RealEnvironment REnv);
void Player3(Environment *env, RealEnvironment REnv);
void Player4(Environment *env, RealEnvironment REnv);
void Player5(Environment *env, RealEnvironment REnv);
void PlayerAction(Environment *env, RealEnvironment REnv);
