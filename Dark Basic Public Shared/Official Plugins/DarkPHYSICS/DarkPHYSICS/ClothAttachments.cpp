
/////////////////////////////////////////////////////////////////////////////////////
// CLOTH ATTACHMENTS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	contains all functions relating to cloth attachments
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INCLUDES /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "ClothAttachments.h"
#include "globals.h"
#include "clothobject.h"
#include "clothstructure.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE DATA ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY ATTACH CLOTH TO SHAPE%LLL%?dbPhyAttachClothToShape@@YAXHHH@Z%
	PHY ATTACH CLOTH TO ALL SHAPES%LL%?dbPhyAttachClothToAllShapes@@YAXHH@Z%
	PHY DETACH CLOTH FROM SHAPE%LL%?dbPhyDetachClothFromShape@@YAXHH@Z%
	PHY ATTACH CLOTH VERTEX%LLFFF%?dbPhyAttachClothVertex@@YAXHHMMM@Z%
	PHY ATTACH CLOTH VERTEX TO SHAPE%LLLFFFL%?dbPhyAttachClothVertexToShape@@YAXHHHMMMH@Z%
	PHY FREE CLOTH VERTEX%LL%?dbPhyFreeClothVertex@@YAXHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// FUNCTION LISTINGS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyAttachClothToShape ( int iID, int iObject, int iMode )
{
	// attach cloth to an object

	// get the cloth and object
	PhysX::sCloth*		 pCloth  = dbPhyGetCloth  ( iID, true );
	PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject );

	// check object is valid
	if ( !pObject )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid rigid body specified in phy attach cloth to shape", "", 0 );
		return;
	}

	// check actor list
	if ( pObject->pActorList.size ( ) == 0 )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid rigid body actor specified in phy attach cloth to shape", "", 0 );
		return;
	}

	// check it has been built
	if ( !pCloth->pCloth )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid cloth object in phy attach cloth to shape", "", 0 );
		return;
	}

	// check the actual cloth
	if ( !pCloth->pCloth->getNxCloth ( ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid cloth data in phy attach cloth to shape", "", 0 );
		return;
	}

	// now add into the list
	pCloth->pCloth->getNxCloth ( )->attachToShape ( *pObject->pActorList [ 0 ]->getShapes ( ), iMode );

	// send this shape into the list
	pCloth->pShapeList.push_back ( *pObject->pActorList [ 0 ]->getShapes ( ) );
}

void dbPhyAttachClothToAllShapes ( int iID, int iMode )
{
	// attach the cloth to all colliding shapes

	if ( NxCloth* pCloth = dbPhyGetClothObject ( iID, true ) )
		pCloth->attachToCollidingShapes ( iMode );
}

void dbPhyDetachClothFromShape ( int iID, int iShape )
{
	// detach cloth from a shape

	// get the cloth and it's data, display errors if something is wrong
	PhysX::sCloth*	pCloth		= dbPhyGetCloth		  ( iID, true );
	NxCloth*		pClothData	= dbPhyGetClothObject ( iID, true );

	// return on invalid pointers
	if ( !pCloth || !pClothData )
		return;

	// remove specific shape or all shapes
	if ( iShape == -1 )
	{
		// all shapes
		for ( DWORD dwShape = 0; dwShape < pCloth->pShapeList.size ( ); dwShape++ )
			pClothData->detachFromShape ( pCloth->pShapeList [ dwShape ] );
	}
	else
	{
		// specific shape
		if ( iShape < ( int ) pCloth->pShapeList.size ( ) )
			pClothData->detachFromShape ( pCloth->pShapeList [ iShape ] );
	}
}

void dbPhyAttachClothVertex ( int iID, int iVertex, float fX, float fY, float fZ )
{
	// attach vertex of a cloth to a point in space

	if ( NxCloth* pCloth = dbPhyGetClothObject ( iID, true ) )
		pCloth->attachVertexToGlobalPosition ( iVertex, NxVec3 ( fX, fY, fZ ) );
}

void dbPhyAttachClothVertexToShape ( int iID, int iVertex, int iObject, float fX, float fY, float fZ, int iMode )
{
	// attach vertex of a cloth to a shape

	// get data
	PhysX::sPhysXObject*	pObject = dbPhyGetObject	  ( iObject );
	NxCloth*				pCloth  = dbPhyGetClothObject ( iID, true );

	// return on failure
	if ( !pObject || !pCloth )
		return;

	// check actor list
	if ( pObject->pActorList.size ( ) == 0 )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid rigid body actor specified in phy attach cloth vertex to shape", "", 0 );
		return;
	}

	// atach to shape
	pCloth->attachVertexToShape ( iVertex, *pObject->pActorList [ 0 ]->getShapes ( ), NxVec3 ( fX, fY, fZ ), iMode );
}

void dbPhyFreeClothVertex ( int iID, int iVertex )
{
	// free a cloth vertex from whatever it was attached to

	if ( NxCloth* pCloth = dbPhyGetClothObject ( iID, true ) )
		pCloth->freeVertex ( iVertex );
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////