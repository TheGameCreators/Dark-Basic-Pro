#include "CBounds.h"
#include "CPlane.h"

cBounds3::cBounds3 ( ) 
{
	Reset ( );
}

cBounds3::cBounds3 ( cVector3& vecMin, cVector3& vecMax ) 
{
	Min = vecMin;
	Max = vecMax;
}

cBounds3::cBounds3 ( float xMin, float yMin, float zMin, float xMax, float yMax, float zMax )
{
    Min = cVector3 ( xMin, yMin, zMin );
    Max = cVector3 ( xMax, yMax, zMax );
}

void cBounds3::Reset ( )
{
    Min = cVector3 (  9999999,  9999999,  9999999 );
	Max = cVector3 ( -9999999, -9999999, -9999999 );
}

cVector3 cBounds3::GetDimensions ( )
{
    return Max - Min;
}

cVector3 cBounds3::GetCentre ( )
{
    return ( Max + Min ) / 2;
}

cPlane3 cBounds3::GetPlane ( int iSide )
{
    cPlane3 BoundsPlane;
    
    switch ( iSide )
	{
        case BOUNDS_PLANE_TOP:
            BoundsPlane.m_vecNormal.y = 1;
            BoundsPlane.m_fDistance   = Max.Dot ( BoundsPlane.m_vecNormal );
		break;

        case BOUNDS_PLANE_RIGHT:
            BoundsPlane.m_vecNormal.x = 1;
            BoundsPlane.m_fDistance   = Max.Dot ( BoundsPlane.m_vecNormal );
        break;

        case BOUNDS_PLANE_BACK:
            BoundsPlane.m_vecNormal.z = 1;
            BoundsPlane.m_fDistance   = Max.Dot ( BoundsPlane.m_vecNormal );
        break;

        case BOUNDS_PLANE_BOTTOM:
            BoundsPlane.m_vecNormal.y = -1;
            BoundsPlane.m_fDistance   = Min.Dot ( BoundsPlane.m_vecNormal );
        break;

        case BOUNDS_PLANE_LEFT:
            BoundsPlane.m_vecNormal.x = -1;
            BoundsPlane.m_fDistance      = Min.Dot ( BoundsPlane.m_vecNormal );
        break;

        case BOUNDS_PLANE_FRONT:
            BoundsPlane.m_vecNormal.z = -1;
            BoundsPlane.m_fDistance   = Min.Dot ( BoundsPlane.m_vecNormal );
        break;
    }

    return BoundsPlane;
}

cBounds3& cBounds3::CalculateFromPolygon ( cVector3 pVertices [ ], int iVertexCount, int iVertexStride, bool bReset )
{
    cVector3*		vtx;
    unsigned char*  pVerts = ( unsigned char* ) pVertices;

    if ( !pVertices )
		return *this;

    if ( bReset )
		Reset ( );

    for ( int iVertex = 0; iVertex < iVertexCount; iVertex++, pVerts += iVertexStride ) 
    {
        vtx = ( cVector3* ) pVerts;

        if ( vtx->x < Min.x ) Min.x = vtx->x;
        if ( vtx->y < Min.y ) Min.y = vtx->y;
        if ( vtx->z < Min.z ) Min.z = vtx->z;
        if ( vtx->x > Max.x ) Max.x = vtx->x;
        if ( vtx->y > Max.y ) Max.y = vtx->y;
        if ( vtx->z > Max.z ) Max.z = vtx->z;
    }

    return *this;
}

void cBounds3::Validate ( void )
{
    float fTemp;

    if ( Max.x < Min.x ) { fTemp = Max.x; Max.x = Min.x; Min.x = fTemp; }
    if ( Max.y < Min.y ) { fTemp = Max.y; Max.y = Min.y; Min.y = fTemp; }
    if ( Max.z < Min.z ) { fTemp = Max.z; Max.z = Min.z; Min.z = fTemp; }
}

#ifndef __GNUC__
bool cBounds3::IntersectedByRay ( cVector3& RayStart, cVector3& RayDir, cVector3* pIntersection )
{
    return IntersectedByRay ( RayStart, RayDir, cVector3 ( 0, 0, 0 ), pIntersection );
}

bool cBounds3::IntersectedByRay ( cVector3& RayStart, cVector3& RayDir, cVector3& Tolerance, cVector3* pIntersection )
{
    bool bInside = true;
    cVector3 MaxT = cVector3 ( -1, -1, -1 );
    cVector3 Intersection;

	if ( RayStart.x < Min.x )
	{
        Intersection.x  = Min.x;
        bInside         = false;
        
		if ( ( int& ) RayDir.x )
			MaxT.x = ( Min.x - RayStart.x ) / RayDir.x;

    }
	else if ( RayStart.x > Max.x )
	{
		Intersection.x  = Max.x;
		bInside		    = false;
		
		if ( ( int& ) RayDir.x )
			MaxT.x = ( Max.x - RayStart.x ) / RayDir.x;
    }

    if ( RayStart.y < Min.y )
	{
        Intersection.y  = Min.y;
        bInside         = false;
        
        if ( ( int& ) RayDir.y )
			MaxT.y = ( Min.y - RayStart.y ) / RayDir.y;
    }
	else if ( RayStart.y > Max.y )
	{
		Intersection.y  = Max.y;
		bInside		    = false;
		
        if ( ( int& ) RayDir.y )
			MaxT.y = ( Max.y - RayStart.y ) / RayDir.y;
    }

    if ( RayStart.z < Min.z )
	{
        Intersection.z = Min.z;
        bInside        = false;
        
		if ( ( int& ) RayDir.z )
			MaxT.z = ( Min.z - RayStart.z ) / RayDir.z;
    }
	else if ( RayStart.z > Max.z )
	{
		Intersection.z = Max.z;
		bInside		   = false;
		
        if ( ( int& ) RayDir.z )
			MaxT.z = ( Max.z - RayStart.z ) / RayDir.z;
    }
	
	if ( bInside )
	{
		if ( pIntersection )
			*pIntersection = RayStart;
		
		return true;
	}

	float* fPlane = &MaxT.x;

	if ( MaxT.y > *fPlane )
		fPlane = &MaxT.y;

	if ( MaxT.z > *fPlane )
		fPlane = &MaxT.z;

	if ( ( ( int& ) fPlane ) & 0x80000000 )
		return false;

    if ( &MaxT.x != fPlane )
	{
        Intersection.x = RayStart.x + *fPlane * RayDir.x;

        if ( Intersection.x < Min.x - Tolerance.x || Intersection.x > Max.x + Tolerance.x )
			return false;
    }

    if ( &MaxT.y != fPlane )
	{
        Intersection.y = RayStart.y + *fPlane * RayDir.y;
        
		if ( Intersection.y < Min.y - Tolerance.y || Intersection.y > Max.y + Tolerance.y )
			return false;
    }

	if ( &MaxT.z != fPlane )
	{
        Intersection.z = RayStart.z + *fPlane * RayDir.z;

        if ( Intersection.z < Min.z - Tolerance.z || Intersection.z > Max.z + Tolerance.z )
			return false;
    }

    if ( pIntersection )
		*pIntersection = Intersection;

	return true;
}
#endif

bool cBounds3::IntersectedByBounds ( cBounds3& Bounds )
{
    return ( Min.x <= Bounds.Max.x ) && ( Min.y <= Bounds.Max.y ) &&
           ( Min.z <= Bounds.Max.z ) && ( Max.x >= Bounds.Min.x ) &&
           ( Max.y >= Bounds.Min.y ) && ( Max.z >= Bounds.Min.z );
}

bool cBounds3::IntersectedByBounds ( cBounds3& Bounds, cVector3& Tolerance )
{
	return ( ( Min.x - Tolerance.x ) <= ( Bounds.Max.x + Tolerance.x ) ) &&
           ( ( Min.y - Tolerance.y ) <= ( Bounds.Max.y + Tolerance.y ) ) &&
           ( ( Min.z - Tolerance.z ) <= ( Bounds.Max.z + Tolerance.z ) ) &&
           ( ( Max.x + Tolerance.x ) >= ( Bounds.Min.x - Tolerance.x ) ) &&
           ( ( Max.y + Tolerance.y ) >= ( Bounds.Min.y - Tolerance.y ) ) &&
           ( ( Max.z + Tolerance.z ) >= ( Bounds.Min.z - Tolerance.z ) );
}

bool cBounds3::PointInBounds ( cVector3& Point )
{
    if ( Point.x < Min.x || Point.x > Max.x ) return false;
    if ( Point.y < Min.y || Point.y > Max.y ) return false;
    if ( Point.z < Min.z || Point.z > Max.z ) return false;

    return true;
}

bool cBounds3::PointInBounds ( cVector3& Point, cVector3& Tolerance )
{
    if ( Point.x < Min.x - Tolerance.x || Point.x > Max.x + Tolerance.x ) return false;
    if ( Point.y < Min.y - Tolerance.y || Point.y > Max.y + Tolerance.y ) return false;
    if ( Point.z < Min.z - Tolerance.z || Point.z > Max.z + Tolerance.z ) return false;

    return true;
}

cBounds3& cBounds3::operator += ( cVector3& vecShift )
{
    Min += vecShift;
    Max += vecShift;

    return *this;
}
