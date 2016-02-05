
/////////////////////////////////////////////////////////////////////////////////////
// CLOTH MAKE ///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	contains all functions relating to making cloth
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INCLUDES /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "ClothMake.h"
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
	PHY MAKE CLOTH%L%?dbPhyMakeCloth@@YAXH@Z%
	PHY MAKE CLOTH FROM OBJECT%LL%?dbPhyMakeClothFromObject@@YAXHH@Z%
	PHY MAKE CLOTH FROM LIMB%LLL%?dbPhyMakeClothFromLimb@@YAXHHH@Z%
	PHY DELETE CLOTH%L%?dbPhyDeleteCloth@@YAXH@Z%

	PHY BUILD CLOTH%L%?dbPhyBuildCloth@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// FUNCTION LISTINGS ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeCloth	( int iID )
{
	// make a new cloth object

	// make sure a cloth object with this ID does not already exist
	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Attempting to create a cloth object with an ID that is already being used", "", 0 );
		return;
	}

	// create memory for cloth
	PhysX::sCloth* pCloth = new PhysX::sCloth;

	// check the memory was allocated ok
	if ( !pCloth )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory for cloth object in phy make cloth", "", 0 );
		return;
	}

	pCloth->bFromObject = false;

	// store the ID of the cloth and set up default properties
	pCloth->iID 								= iID;

	pCloth->fWidth								= 8.0f;
	pCloth->fHeight								= 8.0f;
	pCloth->fDepth								= 0.2f;

	pCloth->desc.flags							|= NX_CLF_GRAVITY;
	pCloth->desc.flags							|= NX_CLF_BENDING;
	pCloth->desc.flags							|= NX_CLF_COLLISION_TWOWAY;

	pCloth->desc.thickness						= 0.2f;
	pCloth->desc.density						= 1.0f;
	pCloth->desc.bendingStiffness				= 1.0f;
	pCloth->desc.stretchingStiffness			= 1.0f;
	pCloth->desc.friction						= 0.5f;
	pCloth->desc.pressure						= 1.0f;
	pCloth->desc.tearFactor						= 1.5f;
	pCloth->desc.collisionResponseCoefficient	= 0.01f;//0.2f maks HW cloth jitter rapidly
	pCloth->desc.attachmentResponseCoefficient	= 0.01f;
	pCloth->desc.solverIterations				= 5;
	
	/*
	pCloth->desc.thickness						= 1.0f;
	pCloth->desc.density						= 0.05f;
	pCloth->desc.bendingStiffness				= 1.0f;
	pCloth->desc.stretchingStiffness			= 1.0f;
	pCloth->desc.collisionResponseCoefficient	= 0.4f;
	pCloth->desc.attachmentResponseCoefficient	= 0.4f;
	pCloth->desc.solverIterations				= 5;
	*/

	pCloth->iGroup								= 0;
	pCloth->pCloth								= NULL;

	pCloth->matRotation.zero ( );

	// send the cloth into the list
	PhysX::pClothList.push_back ( pCloth );
}

void dbPhyMakeClothFromObject ( int iID, int iObject )
{
	dbPhyMakeCloth ( iID );

	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID ) )
	{
		pCloth->iObjectID = iObject;
		pCloth->bFromObject = true;
	}
}

void dbPhyMakeClothFromLimb ( int iID, int iObject, int iLimb )
{

}

void dbPhyDeleteCloth ( int iID )
{
	// delete the specified cloth, we check for the existence so an
	// error can be shown if the cloth does not exist

	if ( PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true ) )
	{
		dbPhyClearCloth ( iID );
	}
}

void dbPhyBuildCloth ( int iID )
{
	// build the specified cloth object

	// get the cloth object and show an error if it doesn't exist
	PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, true );

	// return if something went wrong
	if ( !pCloth )
		return;

	// set up the cooking
	NxInitCooking ( NULL, &PhysX::m_ErrorStream );
	
	// create new cloth object with the properties set earlier
	if ( pCloth->bFromObject == false )
	{
		pCloth->pCloth = new cClothObject ( PhysX::pScene, pCloth->desc, pCloth->fWidth, pCloth->fHeight, pCloth->fDepth, "" );
	}
	else
	{
		sObject* pObject = PhysX::GetObject ( pCloth->iObjectID );

		//if ( pObject->pFrame )
		if ( pObject->iMeshCount )
		{
			//if ( pObject->pFrame->pMesh )
			{
				//PhysX::ConvertLocalMeshToTriList ( pObject->pFrame->pMesh );
				PhysX::ConvertLocalMeshToTriList ( pObject->ppMeshList [ 0 ] );
				pCloth->pCloth = new cClothObject ( pObject, PhysX::pScene, pCloth->desc, "", 1.0f );
			}
		}
		
		
	}

	

	// check if something went wrong
	if ( !pCloth->pCloth )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create cloth object in phy build cloth", "", 0 );
		return;
	}

	// set up variables for the basic 3d object
	sObject* pObject		= NULL;
	DWORD	 dwVertexCount	= pCloth->pCloth->mReceiveBuffers.maxVertices;
	//DWORD	 dwIndexCount	= pCloth->pCloth->mReceiveBuffers.maxIndices * 3;
	DWORD	 dwIndexCount	= pCloth->pCloth->mReceiveBuffers.maxIndices;

	// create the object with 1 mesh
	PhysX::CreateNewObject ( iID, "cloth", 1 );
	
	// now get the object
	pObject = PhysX::GetObject ( iID );

	// if the object doesn't exist something went wrong
	if ( !pObject )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create object in phy build cloth", "", 0 );
		return;
	}

	// check the index count is within limits
	if ( dwIndexCount > 65000 )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cloth size is too large when attempting to build cloth", "", 0 );
		return;
	}

	//char szInfo [ 256 ] = "";
	//sprintf ( szInfo, "%i, %i", dwVertexCount, dwIndexCount );
	//MessageBox ( NULL, szInfo, "info", MB_OK );

	// now set up the mesh
	PhysX::SetupMeshFVFData ( pObject->pFrame->pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount );

	// set up properties for object
	pObject->bTransparentObject              = true;
	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = ( int ) dwVertexCount;
	//pObject->pFrame->pMesh->iDrawVertexCount = 0;
	pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) dwIndexCount / 3;
	//pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) 0;
	//pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) pCloth->pCloth->mReceiveBuffers.numIndicesPtr / 3;

	//pObject->pFrame->pMesh->iDrawVertexCount = 0;
	//pObject->pFrame->pMesh->iDrawPrimitives	 = 0;

		//char szInfo [ 256 ] = "";
		//sprintf ( szInfo, "%i, %i", pObject->pFrame->pMesh->iDrawVertexCount, pObject->pFrame->pMesh->iDrawPrimitives );
		//MessageBox ( NULL, szInfo, "info", MB_OK );

	/*
	{
		for ( int iIndex = 0; iIndex < ( int ) pObject->pFrame->pMesh->iDrawPrimitives; iIndex++ )
			pObject->pFrame->pMesh->pIndices [ iIndex ] = iIndex;

		pObject->pFrame->pMesh->bVBRefreshRequired = true;

		PhysX::sClothVertex* pVertex = ( PhysX::sClothVertex* ) pObject->pFrame->pMesh->pVertexData;

		for ( int iVertex = 0; iVertex < pObject->pFrame->pMesh->iDrawVertexCount; iVertex++ )
		{
			pVertex [ iVertex ].x   = 0.0f;
			pVertex [ iVertex ].y   = 0.0f;
			pVertex [ iVertex ].z   = 0.0f;
			pVertex [ iVertex ].nx  = 0.0f;
			pVertex [ iVertex ].ny  = 0.0f;
			pVertex [ iVertex ].nz  = 0.0f;
			pVertex [ iVertex ].tu  = 0.0f;
			pVertex [ iVertex ].tv  = 0.0f;
		}
	}
	*/
	
	pObject->pFrame->pMesh->bCull			 = false;

	// calculate the bounds and finish off some set up
	PhysX::CalculateMeshBounds		   ( pObject->pFrame->pMesh );
	PhysX::SetNewObjectFinalProperties ( iID, 0.0f );
	PhysX::SetTexture                  ( iID, 0 );

	// we're done with cooking now
	NxCloseCooking ( );
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////