
#ifndef _CPLANE_H_
#define _CPLANE_H_

#include "CVector.h"

#ifndef NULL
#define NULL 0
#endif

typedef enum _CLASSIFY
{
    CP_FRONT    = 1,
    CP_BACK     = 2,
    CP_ONPLANE  = 3,
    CP_SPANNING = 4
} CLASSIFY;

class cPlane3
{
	public:
		cPlane3 ( );
		cPlane3 ( cVector3& vecNormal, float fDistance );
		cPlane3 ( cVector3& vecNormal, cVector3& vecPointOnPlane );

		CLASSIFY ClassifyPoly    ( cVector3 pVertices [ ], int iVertexCount, int iVertexStride );
		CLASSIFY ClassifyPoint   ( cVector3& vecPoint, float* fDist = NULL );
		CLASSIFY ClassifyLine    ( cVector3& vecPoint1, cVector3& vecPoint2 );
		cVector3 GetPointOnPlane ( void );
		bool     GetRayIntersect ( cVector3& vecRayStart, cVector3& vecRayEnd, cVector3& vecIntersection, float* pPercentage = NULL );
		bool     SameFacing      ( cVector3& vecNormal );

		cVector3        m_vecNormal;
		float           m_fDistance;
};

#endif