// Geometry.h: interface for the Geometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRY_H__545C19D8_27F1_11D3_BF77_EFE1563E9558__INCLUDED_)
#define AFX_GEOMETRY_H__545C19D8_27F1_11D3_BF77_EFE1563E9558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef GEOMETRY_EXPORTS
#define GEOMETRY_API __declspec(dllexport)
#else
#define GEOMETRY_API __declspec(dllimport)
#endif

#define PI			3.14159265359 //gb4 

class GEOMETRY_API Geometry  
{
public:
	Geometry();
	virtual ~Geometry();

	// Converts an angle supplied in degrees to radians
	static double ToRadians(double angle);
	
	// Converts an angle in radians to degrees
	static double ToDegrees(double angle);
	
	// returns the angle that x,y makes with the +ve x axis
	static double Angle(double x, double y);
	
	static double DegAngleDiff(double deg_angle1, double deg_angle2); //GB

	static double DegCorrectAngle(double deg_angle);
};

#endif // !defined(AFX_GEOMETRY_H__545C19D8_27F1_11D3_BF77_EFE1563E9558__INCLUDED_)
