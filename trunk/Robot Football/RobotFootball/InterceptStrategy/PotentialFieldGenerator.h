#pragma once

#include "..\..\..\Simulator\Strategy Source\Strategy.h"

class PotentialFieldGenerator
{
public:
	PotentialFieldGenerator(void);
	~PotentialFieldGenerator(void);
	Vector3D FieldVectorToBall(Robot bot, Environment *env);
};

