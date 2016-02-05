
#include "globals.h"
#include "rigidbodyragdoll.h"
#include "stream.h"
#include "error.h"
#include "rigidbodycreation.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE RAG DOLL%LL%?dbPhyMakeRagdoll@@YAXHH@Z%
	PHY ASSIGN RAG DOLL PART%LLL%?dbPhyAssignRagdollPart@@YAXHHHH@Z%

	void dbPhyMakeRagdoll       ( int iRagdollID, int iObject );
	void dbPhyAssignRagdollPart ( int iRagdollID, int iBone, int iObject );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRagdoll ( int iRagdollID, int iObject, int iPhysicsObject )
{
	// create a rag doll object

	// get the object first
	sObject* pObject = PhysX::GetObject ( iObject );

	// check object is valid
	if ( !pObject )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid object specificed in 'phy make ragdoll'", "", 0 );
		return;
	}

	// now set up rag doll memory
	PhysX::sRagdoll* pRagdoll = new PhysX::sRagdoll;

	// check memory was allocated
	if ( !pRagdoll )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory in 'phy make ragdoll'", "", 0 );
		return;
	}
	
	// store details
	pRagdoll->pObject = pObject;
	pRagdoll->iID     = iObject;
	pRagdoll->pActor  = NULL;
	pRagdoll->type    = PhysX::eRigidBodyRagdoll;

	// add this into the list
	PhysX::pRagdollList.push_back ( pRagdoll );

	// and add first phyiscs objects
	dbPhyAssignRagdollPart ( iRagdollID, 0, 0, iPhysicsObject );
}

void dbPhyAssignRagdollPart ( int iRagdollID, int iMesh, int iBone, int iObject )
{
	// get the rag doll
	if ( PhysX::sRagdoll* pRagdoll = dbPhyGetRagdoll ( iRagdollID ) )
	{
		// bone structure
		PhysX::sBone bone;

		// store links to mesh, bone and object
		bone.iMesh   = iMesh;
		bone.iBone   = iBone;
		bone.iObject = iObject;

		// send to back of list
		pRagdoll->boneList.push_back ( bone );
	}
}

PhysX::sRagdoll* dbPhyGetRagdoll ( int iID )
{
	// function to get rag doll object

	// run through list
	for ( DWORD dwObject = 0; dwObject < PhysX::pRagdollList.size ( ); dwObject++ )
	{
		// see if ID matches
		if ( PhysX::pRagdollList [ dwObject ]->iID == iID )
		{
			// return object
			return PhysX::pRagdollList [ dwObject ];
		}
	}

	// none found
	return NULL;
}

void dbPhyUpdateRagdoll ( void )
{
	// update all ragdoll objects

	// local variables
	PhysX::sPhysXObject*	pObject;
	PhysX::sRagdoll*		pRagdoll;
	PhysX::sBone			bone;
	DWORD					dwRagdoll;
	DWORD					dwBone;
	NxMat34					matPose;
	float					glmat [ 16 ];
	NxMat33					matOrient;
	NxVec3					vecPos;

	// run through the rag doll list
	for ( dwRagdoll = 0; dwRagdoll < PhysX::pRagdollList.size ( ); dwRagdoll++ )
	{
		pRagdoll = PhysX::pRagdollList [ dwRagdoll ];

		// go through each bone in the object
		for ( dwBone = 0; dwBone < pRagdoll->boneList.size ( ); dwBone++ )
		{
			bone = pRagdoll->boneList [ dwBone ];

			// get the associated object
			if ( pObject = dbPhyGetObject ( bone.iObject ) )
			{
				// check we have some actors to work with
				if ( pObject->pActorList.size ( ) )
				{
					// only go off 1st actor in list
					if ( pObject->pActorList [ 0 ] )
					{
						// get the global pose
						matPose   = pObject->pActorList [ 0 ]->getGlobalPose ( );
						matOrient = matPose.M;
						vecPos    = matPose.t;	

						// get a gl matrix from the orientation matrix
						matOrient.getColumnMajorStride4 ( &( glmat [ 0 ] ) );
						vecPos.get ( &( glmat [ 12 ] ) );

						// wipe parts of the matrix we don't need
						glmat [  3 ] = 0.0f;
						glmat [  7 ] = 0.0f;
						glmat [ 11 ] = 0.0f;
						glmat [ 15 ] = 1.0f;

						// check mesh is within range
						if ( bone.iMesh < pRagdoll->pObject->iMeshCount )
						{
							// check bone is within range
							if ( bone.iBone < ( int ) pRagdoll->pObject->ppMeshList [ bone.iMesh ]->dwBoneCount )
							{
								if ( dwBone==0 )
								{
									// the first bone must move the whole object model to the worldpos of the first physics object 
//									D3DXMatrixIdentity ( (D3DXMATRIX*)&glmat );
									memcpy ( &pRagdoll->pObject->position.matWorld, &glmat, sizeof ( D3DXMATRIX ) );
									pRagdoll->pObject->position.bCustomWorldMatrix = true;
								}
								else
								{
									// deduct influence of world matrix, leaving relative world transforms
									float fDet = 0.0f;
									D3DXMATRIX matDeductInfluence;
									D3DXMatrixInverse ( &matDeductInfluence, &fDet, &pRagdoll->pObject->position.matWorld );

									// physics capsule matrix (centered) CONVERT TO
									D3DXMATRIX matTranslateShiftAlongCapsuleLength;
									D3DXMatrixTranslation ( &matTranslateShiftAlongCapsuleLength, pObject->pObject->collision.vecMin.x, pObject->pObject->collision.vecMin.y, pObject->pObject->collision.vecMin.z );
									float fXX = fabs ( matTranslateShiftAlongCapsuleLength._41 );
									float fYY = fabs ( matTranslateShiftAlongCapsuleLength._42 );
									float fZZ = fabs ( matTranslateShiftAlongCapsuleLength._43 );
									if ( fXX > fYY && fXX > fZZ ) { matTranslateShiftAlongCapsuleLength._42=0; matTranslateShiftAlongCapsuleLength._43=0; }
									if ( fYY > fXX && fYY > fZZ ) { matTranslateShiftAlongCapsuleLength._41=0; matTranslateShiftAlongCapsuleLength._43=0; }
									if ( fZZ > fXX && fZZ > fYY ) { matTranslateShiftAlongCapsuleLength._41=0; matTranslateShiftAlongCapsuleLength._42=0; }

									// bone matrix (point at one end of capsule)
									D3DXMATRIX matBoneMatrix;
									D3DXMatrixMultiply ( &matBoneMatrix, &matTranslateShiftAlongCapsuleLength, (D3DXMATRIX*)&glmat );

									// matrix multiply new bone matrix to deduct world pos influence
									D3DXMatrixMultiply ( pRagdoll->pObject->ppMeshList [ bone.iMesh ]->pFrameMatrices [ bone.iBone ], &matBoneMatrix, &matDeductInfluence );

									//D3DXMATRIX scale;
									//D3DXMatrixScaling ( &scale, 70.5, 7.5, 7.5 );
									//D3DXMatrixMultiply ( pRagdoll->pObject->ppMeshList [ bone.iMesh ]->pFrameMatrices [ bone.iBone ], pRagdoll->pObject->ppMeshList [ bone.iMesh ]->pFrameMatrices [ bone.iBone ], &scale );

									// all other physics objects connected to it then manipulate the bone matrices
									pRagdoll->pObject->position.bCustomBoneMatrix = true; // lee - 150606 - new U62 DBO structure addition
								}
							}
						}
					}
				}
			}
		}
	}
}