
#include "ClothRayCast.h"
#include "globals.h"
#include "clothobject.h"
#include "clothstructure.h"
#include "error.h"

/*
	PHY CLOTH RAY CAST[%LLFFFFFF%?dbPhyClothRayCast@@YAHHMMMMMM@Z%
	PHY CLOTH RAY CAST HIT X[%FL%?dbPhyClothRayCastHitX@@YAMH@Z%
	PHY CLOTH RAY CAST HIT Y[%FL%?dbPhyClothRayCastHitY@@YAMH@Z%
	PHY CLOTH RAY CAST HIT Z[%FL%?dbPhyClothRayCastHitZ@@YAMH@Z%
	PHY CLOTH RAY CAST HIT VERTEX[%LL%?dbPhyClothRayCastHitVertex@@YAHH@Z%
*/

int dbPhyClothRayCast ( int iID, float fX, float fY, float fZ, float fDirX, float fDirY, float fDirZ )
{
	PhysX::sCloth* pCloth       = dbPhyGetCloth       ( iID );
	NxCloth*       pClothObject = dbPhyGetClothObject ( iID );

	if ( !pCloth )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cloth object does not exist in call to phy cloth ray cast", "", 0 );
		return -1;
	}

	if ( !pClothObject )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to access cloth object in call to phy cloth ray cast", "", 0 );
		return -1;
	}

	NxRay  ray ( NxVec3 ( fX, fY, fZ ), NxVec3 ( fDirX, fDirY, fDirZ ) );
	NxVec3 hit;
	NxU32  vertex;
	
	if ( pClothObject->raycast ( ray, hit, vertex ) )
	{
		pCloth->fRayCastHitX      = hit.x;
		pCloth->fRayCastHitY      = hit.y;
		pCloth->fRayCastHitZ      = hit.z;
		pCloth->iRayCastHitVertex = ( int ) vertex;

		return 1;
	}

	return 0;
}

float dbPhyClothRayCastHitX ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		return pCloth->fRayCastHitX;
	}

	return 0.0f;
}

float dbPhyClothRayCastHitY	( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		return pCloth->fRayCastHitY;
	}

	return 0.0f;
}

float dbPhyClothRayCastHitZ	( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		return pCloth->fRayCastHitZ;
	}

	return 0.0f;
}

int dbPhyClothRayCastHitVertex ( int iID )
{
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		return pCloth->iRayCastHitVertex;
	}

	return 0;
}