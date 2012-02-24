// Vector2d.h: interface for the Vector2d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR2D_H__545C19D9_27F1_11D3_BF77_EFE1563E9558__INCLUDED_)
#define AFX_VECTOR2D_H__545C19D9_27F1_11D3_BF77_EFE1563E9558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Matrix2d;

#ifdef GEOMETRY_EXPORTS
#define GEOMETRY_API __declspec(dllexport)
#else
#define GEOMETRY_API __declspec(dllimport)
#endif

class GEOMETRY_API Vector2d  
{
public:
	Vector2d();
	Vector2d(double x, double y);
	virtual ~Vector2d();
	void Transform(Matrix2d * pMatrix);
	double GetY();
	double GetX();
	void SetY(double y);
	void SetX(double x);
	bool IsNull();
	double GetLength();
	double GetAngle();
	double GetDiffAngle(Vector2d * pVector);
	void AddVector(Vector2d * pVector);
	void SubtractVector(Vector2d *pVector);
	void SetVector(double x, double y,double angle=0.0);
	void Rotate(double Theta);
	// work out the difference angle to the target position 
	// translate the target into this objects coordinates
	double GetAngleTo(Vector2d targetPosition);
	double GetDistanceTo(Vector2d targetPosition);
	void GetExtendedPosition(double currentAngle, long distance);
	void VScale(Vector2d *v, double lambda);
	void VScale2unit(Vector2d *v);
	double VScalarProduct(Vector2d v1, Vector2d v2);
	double VectorAngle(Vector2d v1, Vector2d v2);

	double m_y;
	double m_x;
	double m_degphi; //gb11 heading in deg in Mark's ref frame
//	double m_radphi; //gb041202 heading in radians in Guido's simulation
};

#endif // !defined(AFX_VECTOR2D_H__545C19D9_27F1_11D3_BF77_EFE1563E9558__INCLUDED_)
