
#include "ClothUpdate.h"
#include "globals.h"
#include "clothobject.h"
#include "clothstructure.h"
#include "error.h"

void dbPhyUpdateCloth ( void )
{
	// update all cloth objects in the simulation

//	return;
//
	cClothObject*			pCloth	= NULL;
	sObject*				pObject	= NULL;
	PhysX::sClothVertex*	pVertex = NULL;

	// run through the list
	for ( DWORD dwCloth = 0; dwCloth < PhysX::pClothList.size ( ); dwCloth++ )
	{
		// check the internal cloth data
		if ( !PhysX::pClothList [ dwCloth ]->pCloth )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid cloth data used in cloth update", "", 0 );
			continue;
		}

		// get the cloth
		pCloth = PhysX::pClothList [ dwCloth ]->pCloth;
		
		// get the object
		if ( !( pObject = PhysX::GetObject ( PhysX::pClothList [ dwCloth ]->iID ) ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid object used in cloth update", "", 0 );
			continue;
		}

		// check the frame
		if ( !pObject->pFrame )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid frame data used in cloth update", "", 0 );
			continue;
		}

		// check the mesh
		if ( !pObject->pFrame->pMesh )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid mesh data used in cloth update", "", 0 );
			continue;
		}

		//char szInfo [ 256 ] = "";
		//sprintf ( szInfo, "%i, %i", pObject->pFrame->pMesh->iDrawVertexCount, pObject->pFrame->pMesh->iDrawPrimitives );
		//MessageBox ( NULL, szInfo, "info", MB_OK );
		

		// update draw data
		pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
		pObject->pFrame->pMesh->iDrawVertexCount = ( int ) *pCloth->mReceiveBuffers.numVerticesPtr;
		pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) *pCloth->mReceiveBuffers.numIndicesPtr / 3;
		//pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) *pCloth->mReceiveBuffers.numIndicesPtr;

		//char szInfo [ 256 ] = "";
		//sprintf ( szInfo, "%i, %i", pObject->pFrame->pMesh->iDrawVertexCount, pObject->pFrame->pMesh->iDrawPrimitives );
		//MessageBox ( NULL, szInfo, "info", MB_OK );
		//exit ( 0 );

		//pObject->pFrame->pMesh->iDrawVertexCount = 12;
		//pObject->pFrame->pMesh->iDrawPrimitives  = 12;

		//pObject->pFrame->pMesh->iDrawPrimitives /= 3;
		
		// set up pointer to vertex data
		PhysX::sClothVertex* pVertex = ( PhysX::sClothVertex* ) pObject->pFrame->pMesh->pVertexData;
		
		// update vertices
		for ( int iVertex = 0, iOffset = 0; iVertex < pObject->pFrame->pMesh->iDrawVertexCount; iVertex++, iOffset += 3 )
		{
			float* pVertexArray = ( float* ) pCloth->mReceiveBuffers.verticesPosBegin;
			float* pNormalArray = ( float* ) pCloth->mReceiveBuffers.verticesNormalBegin;

			pVertex [ iVertex ].x   = pVertexArray [ iOffset + 0 ];
			pVertex [ iVertex ].y   = pVertexArray [ iOffset + 1 ];
			pVertex [ iVertex ].z   = pVertexArray [ iOffset + 2 ];
			
			pVertex [ iVertex ].nx  = pNormalArray [ iOffset + 0 ];
			pVertex [ iVertex ].ny  = pNormalArray [ iOffset + 1 ];
			pVertex [ iVertex ].nz  = pNormalArray [ iOffset + 2 ];

			// mike - 130307 - comment out for now
			pVertex [ iVertex ].tu  = pCloth->mTexCoords [ 2 * iVertex     ];
			pVertex [ iVertex ].tv  = pCloth->mTexCoords [ 2 * iVertex + 1 ];
		}

		// update indices
		//for ( int iIndex = 0, iOffset = 0; iIndex < ( int ) pObject->pFrame->pMesh->iDrawPrimitives / 3; iIndex++ )
		for ( int iIndex = 0, iOffset = 0; iIndex < ( int ) pCloth->mReceiveBuffers.maxIndices / 3; iIndex++ )
		{
			unsigned int* pArray = ( unsigned int* ) pCloth->mReceiveBuffers.indicesBegin;

			pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
			pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
			pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
		}

		// signal vb refresh
		pObject->pFrame->pMesh->bVBRefreshRequired = true;

		// 240808 - cloth is not visibly culled - no culling each update
		pObject->collision.fScaledLargestRadius = 0.0f;
		pObject->pFrame->pMesh->Collision.fRadius = 0.0f;
	}
}
