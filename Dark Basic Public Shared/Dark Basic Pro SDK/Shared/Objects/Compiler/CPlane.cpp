#include "CPlane.h"
#include "Common.h"


cPlane3::cPlane3 ( )
{
    m_vecNormal   = cVector3 ( 0.0f, 0.0f, 0.0f );
    m_fDistance   = 0.0f;
}

cPlane3::cPlane3 ( cVector3& vecNormal, float fDistance )
{
	m_vecNormal    = vecNormal;
	m_fDistance    = fDistance;
	m_vecNormal.Normalize ( );
}

cPlane3::cPlane3 ( cVector3& vecNormal, cVector3& vecPointOnPlane )
{
	m_vecNormal = vecNormal;
	m_vecNormal.Normalize ( );
    m_fDistance = vecPointOnPlane.Dot ( m_vecNormal );
}

CLASSIFY cPlane3::ClassifyPoint ( cVector3& vecPoint, float* fDist )
{
	float fResult = vecPoint.Dot ( m_vecNormal ) - m_fDistance;

    if ( fDist )
		*fDist = fResult;

	if ( fResult < -EPSILON ) return CP_BACK;
	if ( fResult > EPSILON  ) return CP_FRONT;

	return CP_ONPLANE;
}

CLASSIFY cPlane3::ClassifyPoly ( cVector3 pVertices [ ], int iVertexCount, int iVertexStride )
{
    unsigned char*	pVerts  = ( unsigned char* ) pVertices;
	unsigned long   Infront = 0, Behind = 0, OnPlane=0;
    float	        result  = 0;

	for ( int i = 0; i < iVertexCount; i++, pVerts += iVertexStride )
    {
        // calculate distance
		result = ( * ( cVector3* ) pVerts ).Dot ( m_vecNormal ) - m_fDistance;
		
		// check the position
		if ( result > EPSILON )
		{
			Infront++;
		}
		else if ( result < -EPSILON )
		{
			Behind++;
		}
		else
		{
			OnPlane++;
			Infront++;
			Behind++;
        }
	}

    if ( OnPlane == ( ULONG ) iVertexCount ) return CP_ONPLANE;
	if ( Behind  == ( ULONG ) iVertexCount ) return CP_BACK;
	if ( Infront == ( ULONG ) iVertexCount ) return CP_FRONT;

	return CP_SPANNING;
}

CLASSIFY cPlane3::ClassifyLine ( cVector3& vecPoint1, cVector3& vecPoint2 )
{
	int		iInfront = 0, iBehind = 0, iOnPlane = 0;
	float	fResult  = 0;

    // calculate distance
	fResult = vecPoint1.Dot ( m_vecNormal ) - m_fDistance;

	// check the position
	if ( fResult > EPSILON )
	{
		iInfront++;
	}
	else if ( fResult < -EPSILON )
	{
		iBehind++;
	}
	else
	{
		iOnPlane++;
		iInfront++;
		iBehind++;
    }

    // calculate distance
	fResult = vecPoint2.Dot ( m_vecNormal ) - m_fDistance;

	// check the position
	if ( fResult > EPSILON )
	{
		iInfront++;
	}
	else if ( fResult < -EPSILON )
	{
		iBehind++;
	}
	else
	{
		iOnPlane++;
		iInfront++;
		iBehind++;
    }

    if ( iOnPlane == 2 ) return CP_ONPLANE;
	if ( iBehind  == 2 ) return CP_BACK;
	if ( iInfront == 2 ) return CP_FRONT;

	return CP_SPANNING;
}

cVector3 cPlane3::GetPointOnPlane ( void )
{
    return m_vecNormal * m_fDistance;
}

bool cPlane3::GetRayIntersect ( cVector3& vecRayStart, cVector3& vecRayEnd, cVector3& vecIntersection, float* pfPercentage )
{
	cVector3    vecDirection,
				vecL1,
				vecPoint;
	float       fLineLength,
				fDistanceFromPlane;
    float       fPercent;

	vecDirection.x = vecRayEnd.x - vecRayStart.x;
	vecDirection.y = vecRayEnd.y - vecRayStart.y;
	vecDirection.z = vecRayEnd.z - vecRayStart.z;

	fLineLength = vecDirection.Dot ( m_vecNormal );

	if ( fabsf ( fLineLength ) < EPSILON )
		return false; 

    vecPoint = GetPointOnPlane ( );
	vecL1.x = vecPoint.x - vecRayStart.x;
	vecL1.y = vecPoint.y - vecRayStart.y;
	vecL1.z = vecPoint.z - vecRayStart.z;

	fDistanceFromPlane = vecL1.Dot ( m_vecNormal );

	fPercent = fDistanceFromPlane / fLineLength; 

	if ( fPercent < 0.0f || fPercent > 1.0f )
		return false;

	vecIntersection.x = vecRayStart.x + vecDirection.x * fPercent;
	vecIntersection.y = vecRayStart.y + vecDirection.y * fPercent;
	vecIntersection.z = vecRayStart.z + vecDirection.z * fPercent;

    if ( pfPercentage )
		*pfPercentage = fPercent;

    return true;
}

bool cPlane3::SameFacing ( cVector3& vecNormal )
{
    if ( fabsf ( ( m_vecNormal.x - vecNormal.x ) + 
                 ( m_vecNormal.y - vecNormal.y ) + 
                 ( m_vecNormal.z - vecNormal.z ) ) < 0.1f )
				 return true;
    
    return false;
}