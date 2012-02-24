#include "stdafx.h"
#include "Geometry.h"
#include "Vector2d.h"
#include "StrategyUtils.h"
#include <math.h>

double Gaussian(double x , double u , double o)
{//length x , offset , distribution
	return ( exp( -(-x-u)*(-x-u) / (2*o*o) ) );
}
void PutObjectHistory(RealEnvironment *REnv)
{//Put the history into the REnv Structure
	for(int rn=0;rn<5;rn++)
	{
		REnv->RobotHistory[rn][0] = RobotHistory[rn][0];	
		REnv->RobotHistory[rn][1] = RobotHistory[rn][1];
		REnv->RobotHistory[rn][2] = RobotHistory[rn][2];
	}
    REnv->BallHistory[0] = BallHistory[0];	
	REnv->BallHistory[1] = BallHistory[1];
	REnv->BallHistory[2] = BallHistory[2];
}


void UpdateObjectHistory(Environment *env)
{//Holds three history environments 
	for(int rn=0;rn<5;rn++)
	{
		RobotHistory[rn][0] = RobotHistory[rn][1];	//	0 <- 1 <- 2 <- new
		RobotHistory[rn][1] = RobotHistory[rn][2];
		RobotHistory[rn][2] = env->home[rn];
	}
    BallHistory[0] = BallHistory[1];	//	0 <- 1 <- 2 <- new
	BallHistory[1] = BallHistory[2];
	BallHistory[2].m_x = env->currentBall.pos.x;
	BallHistory[2].m_y = env->currentBall.pos.y;
}

#ifdef _MERLIN
int aimfor(int id , Vector3D Destination, bool AvoidObstacles , Environment *env  );
void ChangeEnds( Environment *env );
int d(int alpha ,int beta);
double distance(Vector3D a , Vector3D b);
int l(int angle);
void potential_field(Vector3D P, Vector3D C, Vector3D *R, int Polarization);

/*************************************************************
The following call are for compatibility with the old system *
Please do not delete or modify                               *
*************************************************************/
int aimfor(int id , Vector3D Destination, bool AvoidObstacles , Environment *env  )
{//For compatibility with old system
	Vector3D Dest_Field, R;
    int des_vec;

	potential_field(env->home[id].pos , Destination, &Dest_Field ,ATTRACTIVE);

	/*
	if(AvoidObstacles)
	{
		Vector3D,Obstacle_Field[11];
		double Distance[11];
		// find the closest friendly robot
		int min_ptr=5;
		double importance;
		Distance[5]=999;
		for(int i=0;i<5;i++)
		{
			Distance[i] = distance(env->home[id].pos , env->home[i].pos);
			if(( Distance[min_ptr]> Distance[i])&&(i!=id)){min_ptr=i;}
		}
		i = min_ptr;

		// calculate the vectorfield of obstacles
		potential_field(env->home[id].pos , env->home[i].pos, &Obstacle_Field[i] ,REPULSIVE);

		// combine the vectorfield of the ball with the vectorfield of the obstacles
        importance = 6.0 * Gaussian(Distance[i], 0 ,4.5);
		Obstacle_Field[i].x = Obstacle_Field[i].x * importance;
		Obstacle_Field[i].y = Obstacle_Field[i].y * importance;
        if((importance > 0.65)&&(Distance[i]>2))
		{
			//dline(env->home[i].pos.x-1.5,env->home[i].pos.y,env->home[i].pos.x+1.5,env->home[i].pos.y);
			//dline(env->home[i].pos.x,env->home[i].pos.y-1.5,env->home[i].pos.x,env->home[i].pos.y+1.5);
			//dcircle(env->home[i].pos.x,env->home[i].pos.y, 3);
			NVectorAdd(Dest_Field , Obstacle_Field[i],&R);
		}
		else
		{
			R = Dest_Field;
		}
		// add obstacle avoidance to destination
	}
	else
	{
         	R = Dest_Field;
	}
	*/

	R = Dest_Field;
	des_vec = (int) (atan2(R.y,R.x) * 180.0/PI);
	return des_vec;
}

void ChangeEnds( Environment *env )
{
	for(unsigned char i = 0; i < PLAYERS_PER_SIDE; i++)
	{
		// Change the Home team positions
		env->home[i].pos.x = FLENGTH - env->home[i].pos.x; 
		env->home[i].pos.y = FWIDTH  - env->home[i].pos.y; 
		if((env->home[i].rotation -= 180) < 0)
			env->home[i].rotation += 360;

		// Change the Opponent team positions
		env->opponent[i].pos.x = FLENGTH - env->opponent[i].pos.x; 
		env->opponent[i].pos.y = FWIDTH  - env->opponent[i].pos.y;
		if((env->opponent[i].rotation -= 180) < 0)
			env->opponent[i].rotation += 360;
		
	}

	// Change the CurrentBall position
	env->currentBall.pos.x = FLENGTH - env->currentBall.pos.x;
	env->currentBall.pos.y = FWIDTH  - env->currentBall.pos.y;
}

int d(int alpha ,int beta)
{//For compatibility with old system
 return (int)Geometry::DegAngleDiff((double)alpha, (double)beta);
}

double distance(Vector3D a , Vector3D b)
{//For Compatibility with old system - Uses Strategy.h vectors
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) );
}

int l(int angle)
{//For compatibility with old system
	return (int)Geometry::DegCorrectAngle((double) angle);
}

void potential_field(Vector3D P, Vector3D C, Vector3D *R, int Polarization)
{//For compatibility with the old system  - current pos , Centre
	if(Polarization==ATTRACTIVE)
	{
		R->x = (C.x-P.x)/ sqrt( (C.x-P.x)*(C.x-P.x) + (C.y-P.y)*(C.y-P.y) ) ;
		R->y = (C.y-P.y)/ sqrt( (C.x-P.x)*(C.x-P.x) + (C.y-P.y)*(C.y-P.y) ) ;
	}
	else
	{
		R->x = (P.x-C.x)/ sqrt( (P.x-C.x)*(P.x-C.x) + (P.y-C.y)*(P.y-C.y) ) ;
		R->y = (P.y-C.y)/ sqrt( (P.x-C.x)*(P.x-C.x) + (P.y-C.y)*(P.y-C.y) ) ;
	}
}
#endif

