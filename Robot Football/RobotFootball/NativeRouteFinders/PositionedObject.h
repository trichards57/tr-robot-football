#pragma once

#include "stdafx.h"

#pragma pack(push, 8)

struct PositionedObject
{
public:
	SIZE Size;
	POINT Position;
};

#pragma pack(pop)