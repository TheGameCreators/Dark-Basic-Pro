#ifndef _CBOUNDS_H_
#define _CBOUNDS_H_

#include "CVector.h"

#ifndef NULL
#define NULL 0
#endif

#define BOUNDS_PLANE_TOP        0
#define BOUNDS_PLANE_BOTTOM     1
#define BOUNDS_PLANE_LEFT       2
#define BOUNDS_PLANE_RIGHT      3
#define BOUNDS_PLANE_FRONT      4
#define BOUNDS_PLANE_BACK       5

class cBounds3
{
	public:
		cBounds3 ( );
		cBounds3 ( cVector3& vecMin, cVector3& vecMax );
		cBounds3 ( float xMin, float yMin, float zMin, float xMax, float yMax, float zMax );

		cVector3        Min;
		cVector3        Max;

		cVector3        GetDimensions        ( void );
		cVector3        GetCentre            ( void );
		cPlane3         GetPlane             ( int iSide );
		cBounds3&       CalculateFromPolygon ( cVector3 pVertices [ ], int iVertexCount, int iVertexStride, bool bReset = true );
		bool            IntersectedByRay     ( cVector3& RayStart, cVector3& RayDir, cVector3* pIntersection = NULL );
		bool            IntersectedByRay     ( cVector3& RayStart, cVector3& RayDir, cVector3& Tolerance, cVector3* pIntersection = NULL );
		bool            IntersectedByBounds  ( cBounds3& Bounds );
		bool            IntersectedByBounds  ( cBounds3& Bounds, cVector3& Tolerance );
		bool            PointInBounds        ( cVector3& Point );
		bool            PointInBounds        ( cVector3& Point, cVector3& Tolerance );
		void            Validate             ( void );
		void            Reset                ( void );

		cBounds3&        operator += ( cVector3& vecShift );
		cBounds3&        operator -= ( cVector3& vecShift );
};

#endif

