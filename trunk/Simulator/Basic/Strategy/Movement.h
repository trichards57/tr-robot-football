#include "Vector2d.h"

#define STOP_DISTANCE 20
#define STOP_ANGLE 10
#define STOP_FACTOR 2
#define STOP_ANGLE_LIMIT 1
#define STOP_DISTANCE_LIMIT 1
//The pitch is done this way to avoid releasing the whole of the RSE to get dimensions
#define FIRA_LENGTH 2200
#define FIRA_WIDTH 1800
#define PITCH_LENGTH 2150		//For Notts pitch
#define PITCH_WIDTH 1750		//For Notts pitch

static bool RotateAtTargetDone;
static int TickCount = -1;

/*
void MoveTo(int ReqRot , int ReqSpeed, int RobotID, Environment *env, bool forcefront);
void MoveToOld(int ReqRot , int ReqSpeed, int RobotID, Environment *env, bool forcefront);
void RotateTo(int des_rot , int speed, int id,  Environment *env ,bool forcefront);
*/
void NavigateTo(Environment *env, RealEnvironment REnv, Vector2d Target);
void PositionToTarget(Environment *env, RealEnvironment REnv, Vector2d Target);
int OrientationToTarget(double TargetX, double TargetY, double CurrentAngle, int PlayTo);
int TurnToTarget(int ReqRotation, bool * FacingTarget);
void MoveToTarget(Environment *env, RealEnvironment REnv, int ReqTurn, bool FacingTarget);
void DriveToTarget(Environment *env, RealEnvironment REnv, Vector2d Target);
bool PointToTarget(Environment *env, RealEnvironment REnv, double ReqRotation);
bool RotateAtTarget(Environment *env, RealEnvironment REnv, Vector2d Target);
int Randomise();
void GoToTarget(Environment *env, RealEnvironment REnv, Vector2d Target);
void spin(Robot *robot, int speed, int direction);
enum {LEFT_SIDE = 1, RIGHT_SIDE = 2, TOP_SIDE = 3, BOTTOM_SIDE = 4};

const double Kp	=	0.1 , Kd  =  0;

