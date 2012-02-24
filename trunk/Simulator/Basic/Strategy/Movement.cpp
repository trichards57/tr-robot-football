/*************************************************************************
This file contains movement commands for the robots

**************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "Geometry.h"
#include "StrategyUtils.h"
#include "Movement.h"
#include <time.h>
/*
Added P Birkin May 2006
*/

void NavigateTo(Environment *env, RealEnvironment REnv, Vector2d Target)
{
	/*
	Call to send the robot to the required position. 
	This is in all the positioning functions as a hook
	*/
	PositionToTarget(env, REnv, Target);
}
void PositionToTarget(Environment *env, RealEnvironment REnv, Vector2d Target)
{
	//Position the robot for place kicks, driving in a straight line
	//All speeds are hard-coded in this call

	Vector2d CurrentPos;
	double TargetAngle = 0;
	double ReqRotation  = 0;
	bool status = false;

	CurrentPos.m_x = env->home[REnv.RobotID].pos.x;	//In inches
	CurrentPos.m_y = env->home[REnv.RobotID].pos.y;	//Origin is bottom left of pitch
	//Convert from Vector3D to Vector2d

	double CurrentDistance = CurrentPos.GetDistanceTo(Target);		//In inches
	if (CurrentDistance <= .25)
	{//Must be there
		status = RotateAtTarget(env, REnv, Target);
	}
	else
	{//Drive there

		double CurrentAngle = env->home[REnv.RobotID].rotation;	//Cartesian angle
		double TargetX = Target.m_x - CurrentPos.m_x;
		double TargetY = Target.m_y - CurrentPos.m_y;
		ReqRotation = OrientationToTarget(TargetX, TargetY, CurrentAngle, REnv.PlayTo);

		if (abs(ReqRotation)> 20)
			PointToTarget(env, REnv, ReqRotation);	//Too big to take out by motion
		else
		{
			double DriveSpeed = 25;	//REnv.Speed;
			if (CurrentDistance <= .25)
			{
				DriveSpeed = 0;
			}
			else
				if (CurrentDistance <= 2)
					DriveSpeed = 1;
				else
					if (CurrentDistance <= 4)
						DriveSpeed = 3;
			
			double LeftDiff = 2;
			double RightDiff = 2;
			double sign = -1;	//-ve rotation going forward left > right
			if (ReqRotation > 0)
				sign = 1;		//+ve rotation going forward left < right
			
			double ReqRotation1 = abs(ReqRotation);
			if (ReqRotation1 > 10 && CurrentDistance > 10)
			{
				LeftDiff = -sign * LeftDiff;
				RightDiff = sign * RightDiff;
			}
			
			env->home[REnv.RobotID].velocityLeft = DriveSpeed + LeftDiff;
			env->home[REnv.RobotID].velocityRight = DriveSpeed + RightDiff;
		}
	}
}

bool RotateAtTarget(Environment *env, RealEnvironment REnv, Vector2d Target)
{//Point the robot at the target by rotating on the spot
	bool status = false;
	double CurrentAngle = env->home[REnv.RobotID].rotation;	//Cartesian angle
	double DiffAngle = Geometry::DegAngleDiff(CurrentAngle, Target.m_degphi);
	double sign = -1;
	if (DiffAngle > 0)
		sign = 1;
	double SpinSpeed = 5 * sign;
	if (abs(DiffAngle) < 3)
	{
		SpinSpeed = 0;
		status = true;
	}	
	else
		if (abs(DiffAngle) < 20)
		{
			SpinSpeed = sign;
		}	

	//Applied the brakes to stop rotation when pointing at the target position
	//taking the shortest path

	env->home[REnv.RobotID].velocityLeft = SpinSpeed;
	env->home[REnv.RobotID].velocityRight = -SpinSpeed;

	return status;
}

int OrientationToTarget(double TargetX, double TargetY, double CurrentAngle, int PlayTo)
{//Returns the required rotation to point the front of the robot facing the target 
	double TargetAngle = Geometry::Angle(TargetY, TargetX);
	TargetAngle = Geometry::ToDegrees(TargetAngle);
	TargetAngle = Geometry::DegCorrectAngle(TargetAngle);
	if (PlayTo == LEFT_SIDE)
		if (TargetY > 0)
				TargetAngle = -(180 - TargetAngle);
			else
				TargetAngle = 180 + TargetAngle;
    
	return (int)(Geometry::DegAngleDiff(TargetAngle, CurrentAngle));
}

int TurnToTarget(int ReqRotation, bool * FacingTarget)
{//Returns the shortest turn to point the robot at the target
	int ReqTurn;
	*FacingTarget = false;		

	if (ReqRotation > 90)
		ReqTurn = ReqRotation - 180;
	else
		if (ReqRotation > -90)
		{
			ReqTurn = ReqRotation;
			*FacingTarget = true;		//Front is facing target
		}
		else
			ReqTurn = 180 + ReqRotation;
	return ReqTurn;
}

bool PointToTarget(Environment *env, RealEnvironment REnv, double ReqRotation)
{//Point the robot at the target by rotating on the spot
	bool status = false;
	double sign = -1;
	if (ReqRotation > 0)
		sign = 1;
	ReqRotation = abs(ReqRotation);
	double SpinSpeed = 10*sign;
	if (ReqRotation < 4)
	{
		SpinSpeed = 1;
		status = true;
	}
	else
		if (ReqRotation < 15.0)
			SpinSpeed = 2*sign;
		else
			if (ReqRotation < 40.0)
				SpinSpeed = 3*sign;
	//Applied the brakes to stop rotation when pointing at the target position
	//taking the shortest path

	env->home[REnv.RobotID].velocityLeft = -SpinSpeed;
	env->home[REnv.RobotID].velocityRight = SpinSpeed;
    
	return status;
}

void DriveToTarget(Environment *env, RealEnvironment REnv, Vector2d Target)
{//Template for moving the robot to the target
	Vector2d CurrentPos;
	CurrentPos.m_x = env->home[REnv.RobotID].pos.x;	//In inches
	CurrentPos.m_y = env->home[REnv.RobotID].pos.y;	//Origin is bottom left of pitch

	double TargetX = Target.m_x - CurrentPos.m_x;
	double TargetY = Target.m_y - CurrentPos.m_y;
	double CurrentAngle = env->home[REnv.RobotID].rotation;	//Cartesian angle
	int ReqRotation = OrientationToTarget(TargetX, TargetY, CurrentAngle, REnv.PlayTo);

	bool FacingTarget = true;	//Always true after OrientationToTarget
	int ReqTurn = TurnToTarget(ReqRotation, &FacingTarget);

	MoveToTarget(env, REnv, ReqTurn, FacingTarget);
}

void MoveToTarget(Environment *env, RealEnvironment REnv, int ReqTurn, bool FacingTarget)
{//Template for moving the robot in the correct direction 
	int left = 0;
	int right = 0;
	double DifSpeedFactor = 1;

	if ((ReqTurn < -65) ||(ReqTurn > 65))
		DifSpeedFactor = 3.0;
	else
		if ((ReqTurn >= -65 && ReqTurn < -25) || (ReqTurn > 25 && ReqTurn <= 65))
			DifSpeedFactor = 1.5;
		else
			if ((ReqTurn >= -25 && ReqTurn < -5) || (ReqTurn > 5 && ReqTurn <= 25))
				DifSpeedFactor = 1.2;

	int DifSpeed = (int)(REnv.Speed / DifSpeedFactor);

	if (FacingTarget)
		if (ReqTurn < 0)
		{//-ve turn
			left = REnv.Speed;
			right = DifSpeed;
		}
		else
		{//+ve turn
			left = DifSpeed;
			right = REnv.Speed;
		}
	else		//Back to target
		if (ReqTurn < 0)
		{//-ve turn
			left = -DifSpeed;
			right = -REnv.Speed;
		}
		else
		{//+ve turn
			left = -REnv.Speed;
			right = -DifSpeed;
		}

	env->home[REnv.RobotID].velocityLeft = left;
	env->home[REnv.RobotID].velocityRight = right;
}

// Added by cdw07u March 09

int Randomise(){
	int seed;
	int noise;
	int neg;
	seed = time(NULL);
	srand(seed);
	noise = rand() % 2;
	neg = rand()% 1;
	if (neg == 1){
		noise = noise * -1;
	}
	return noise;
}

// Modified PositionToTarget edited by cdw07u

void GoToTarget(Environment *env, RealEnvironment REnv, Vector2d Target)
{
	//Position the robot for place kicks, driving in a straight line
	//All speeds are hard-coded in this call

	Vector2d CurrentPos;
	double TargetAngle = 0;
	double ReqRotation  = 0;
	bool status = false;

	CurrentPos.m_x = env->home[REnv.RobotID].pos.x;	//In inches
	CurrentPos.m_y = env->home[REnv.RobotID].pos.y;	//Origin is bottom left of pitch
	//Convert from Vector3D to Vector2d

	double CurrentDistance = CurrentPos.GetDistanceTo(Target);		//In inches
	if (CurrentDistance <= .25)
	{//Must be there
		status = RotateAtTarget(env, REnv, Target);
	}
	else
	{//Drive there

		double CurrentAngle = env->home[REnv.RobotID].rotation;	//Cartesian angle
		double TargetX = Target.m_x - CurrentPos.m_x;
		double TargetY = Target.m_y - CurrentPos.m_y;
		ReqRotation = OrientationToTarget(TargetX, TargetY, CurrentAngle, REnv.PlayTo);

		if (abs(ReqRotation)> 20)
			PointToTarget(env, REnv, ReqRotation);	//Too big to take out by motion
		else
		{
			double DriveSpeed = 120;	//REnv.Speed;
			if (CurrentDistance <= .25)
			{
				DriveSpeed = 0;
			}
			else
				if (CurrentDistance <= 2)
					DriveSpeed = 1;
				else
					if (CurrentDistance <= 4)
						DriveSpeed = 3;
			
			double LeftDiff = 2;
			double RightDiff = 2;
			double sign = -1;	//-ve rotation going forward left > right
			if (ReqRotation > 0)
				sign = 1;		//+ve rotation going forward left < right
			
			double ReqRotation1 = abs(ReqRotation);
			if (ReqRotation1 > 10 && CurrentDistance > 10)
			{
				LeftDiff = -sign * LeftDiff;
				RightDiff = sign * RightDiff;
			}
			
			env->home[REnv.RobotID].velocityLeft = DriveSpeed + LeftDiff;
			env->home[REnv.RobotID].velocityRight = DriveSpeed + RightDiff;
		}
	}
}

//Added by yxh08u

void spin(Robot *robot, int speed, int direction)   
{   
/*  direction   
    0 - anticlockwise  
    1 - clockwise  
  
     
    speed = velocity of spin  
*/   
    double Vl, Vr;   
    speed = 105;   
    if(direction == 0)   
    {   
        Vl = 0-speed;   
        Vr = speed;   
    }   
    else   
    {   
        if(direction == 1)   
        {   
            Vl = speed;   
            Vr = 0-speed;   
        }   
    }   
   
    robot->velocityLeft = Vl;   
    robot->velocityRight = Vr;   
   
       
    return;    
}   
