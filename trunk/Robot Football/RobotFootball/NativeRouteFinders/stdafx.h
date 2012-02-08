// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef NATIVEROUTEFINDERS_EXPORTS
#define NATIVEROUTEFINDERS_API extern "C" __declspec(dllexport)
#else
#define NATIVEROUTEFINDERS_API extern "C" __declspec(dllimport)
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <math.h>
#include <ppl.h>

using namespace Concurrency;