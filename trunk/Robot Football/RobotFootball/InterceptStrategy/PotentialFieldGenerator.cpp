#include "StdAfx.h"
#include "PotentialFieldGenerator.h"

#include "..\SquareStrategy\MotionController.h"

#define BALL_WEIGHT 150
#define OBSTACLE_WEIGHT 1000000
#define OBSTACLE_EFFECT_RADIUS 10
#define OBSTACLE_SIGMA 2
#define GRID_RESOLUTION 0.01

using namespace Concurrency;

PotentialFieldGenerator::PotentialFieldGenerator(void)
{
}


PotentialFieldGenerator::~PotentialFieldGenerator(void)
{
}

double FieldAtPoint(Vector3D point, Environment* env)
{
	// Attract to the ball
	auto xDiff = env->currentBall.pos.x - point.x;
	auto yDiff = env->currentBall.pos.y - point.y;

	auto dist = sqrt(xDiff*xDiff + yDiff*yDiff);
	auto attractField = 0.5 * BALL_WEIGHT * dist;

	combinable<double> repelField;
	// Repel from friendlies
	parallel_for(0,5, [env, point, &repelField](int i) 
	{
		auto xDiff = env->home[i].pos.x - point.x;
		auto yDiff = env->home[i].pos.y - point.y;
		
		if (fabs(xDiff) < 2*GRID_RESOLUTION && fabs(yDiff) < 2*GRID_RESOLUTION)
		{
			repelField.local() = 0;
			return; // This is probably the robot being analysed as it is so close. Skip it
		}
		
		auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
		repelField.local() = force;
	});
	auto repField = repelField.combine(std::plus<double>());

	repelField.clear();

	// Repel from opponents
	parallel_for(0,5, [env, point, &repelField](int i) 
	{
		auto xDiff = env->opponent[i].pos.x - point.x;
		auto yDiff = env->opponent[i].pos.y - point.y;
		
		auto force = 0.5 * OBSTACLE_WEIGHT * exp(-((xDiff*xDiff)/(2*OBSTACLE_SIGMA) + (yDiff*yDiff)/(2*OBSTACLE_SIGMA)));
		repelField.local() = force;
	});

	repField += repelField.combine(std::plus<double>());

	return  repField +attractField;
}

Vector3D PotentialFieldGenerator::FieldVectorToBall(Robot bot, Environment* env)
{
	Vector3D upPoint, downPoint, leftPoint, rightPoint;

	upPoint.x = bot.pos.y + GRID_RESOLUTION;
	upPoint.y = bot.pos.x;
	downPoint.x = bot.pos.y - GRID_RESOLUTION;
	downPoint.y = bot.pos.x;
	leftPoint.x = bot.pos.x - GRID_RESOLUTION;
	leftPoint.y = bot.pos.y;
	rightPoint.x = bot.pos.x + GRID_RESOLUTION;
	rightPoint.y = bot.pos.y;

	auto upField = FieldAtPoint(upPoint, env);
	auto downField = FieldAtPoint(downPoint, env);
	auto leftField = FieldAtPoint(leftPoint, env);
	auto rightField = FieldAtPoint(rightPoint, env);

	Vector3D force;
	force.x = (leftField - rightField)/(2*GRID_RESOLUTION);
	force.y = (downField - upField)/(2*GRID_RESOLUTION);

	return force;
}