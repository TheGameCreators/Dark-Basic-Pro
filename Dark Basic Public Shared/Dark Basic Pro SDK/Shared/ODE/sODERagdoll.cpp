#include "ODEPhysics.h"
#include "sODERagdoll.h"
#include "Mmsystem.h"

// Global to track ragdoll creation (for ragdoll freeze feature)
int g_iRagdollCreationCounter = 0;
extern int g_iODERagdollMode;

void strlower( char* szStr )
{
	if ( !szStr ) return;

	while( *szStr )
	{
		*szStr = tolower( *szStr );

		szStr++;
	}
}

sODERagdoll* sODERagdoll::pODERagdollList = NULL;

sODERagdoll::sODERagdoll( )
{
	pObject = 0;
	pODEBones = 0;
	pODEJoints = new sODEJoint* [ 50 ];
	dwNumBones = 0;
	dwNumJoints = 0;
	iID = 0;
	iCenterBone = -1;

	pNextRagdoll = 0;
	pPrevRagdoll = 0;
}

sODERagdoll::~sODERagdoll( )
{
	if ( pObject )
	{
		//reset frame matrices to point away from matrices about to be deleted
		if ( pObject->ppMeshList )
		{
			for ( int i = 0; i < pObject->iMeshCount; i++ )
			{
				sMesh *pMesh = pObject->ppMeshList [ i ];
				for ( DWORD j = 0; j < pMesh->dwBoneCount; j++ )
				{
					if ( pMesh->pFrameRef [ j ] ) pMesh->pFrameMatrices [ j ] = &pMesh->pFrameRef [ j ]->matCombined;
					else pMesh->pFrameMatrices [ j ] = NULL;
				}
			}
		}
		pObject->position.bCustomWorldMatrix = false;
	}

	if ( pODEBones ) delete [] pODEBones;

	if ( pODEJoints )
	{
		for ( DWORD i = 0; i < dwNumJoints; i++ )
		{
			if ( pODEJoints [ i ] ) sODEJoint::RemoveJoint( pODEJoints [ i ] );
		}

		delete [] pODEJoints;
	}
}

sODEBone* sODERagdoll::FindBoneFromOwner( int iFrame )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		if ( pODEBones [ i ].GetOwner( ) == iFrame )
		{
			return pODEBones + i;
		}
	}

	return 0;
}

void sODERagdoll::AdjustMass ( float fNewMass )
{
	// adjust by percentage (0-1) of original ragdoll mass
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		dMass mass;
		dBodyID BoneBody = pODEBones [ i ].GetBody();
		dMassSetBox  ( &mass, 1, pODEBones [ i ].fSExtentX * fNewMass, pODEBones [ i ].fSExtentY * fNewMass, pODEBones [ i ].fSExtentZ * fNewMass );
		dBodySetMass ( BoneBody, &mass );
	}
}

void sODERagdoll::SetGravity ( int iGravityMode )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		dBodyID BoneBody = pODEBones [ i ].GetBody();
		dBodySetGravityMode ( BoneBody, iGravityMode );
		if ( iGravityMode==0 )
			dBodyDisable ( BoneBody );
		else
			dBodyEnable ( BoneBody );
	}
}

void sODERagdoll::SetActive ( int iNudgeIntoActivityMode )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		dBodyID BoneBody = pODEBones [ i ].GetBody();
		if ( iNudgeIntoActivityMode==0 )
			dBodyDisable ( BoneBody );
		else
			dBodyEnable ( BoneBody );
	}
}

void sODERagdoll::SetLinearVelocity ( float fX, float fY, float fZ )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		dBodyID BoneBody = pODEBones [ i ].GetBody();
		dBodyEnable ( BoneBody );

		// 310707 - do not apply forces to certain bones ( to prevent geometry penetration )
		if ( pODEBones [ i ].bResponsiveToLinearVelocity==true )
			dBodySetLinearVel ( BoneBody, fX, fY, fZ );
	}
}

void sODERagdoll::SetAngularVelocity ( float fX, float fY, float fZ )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		dBodyID BoneBody = pODEBones [ i ].GetBody();
		dBodyEnable ( BoneBody );
		dBodySetAngularVel ( BoneBody, fX, fY, fZ );
	}
}

sODERagdoll* sODERagdoll::AddRagdoll( int id, sObject *pObject, float fAdjust, int iDebugObject )
{
	sODERagdoll *pRagdoll = new sODERagdoll( );

	pRagdoll->fContactMU = 1000.0f;
	pRagdoll->fContactSoftERP = 0.2f;
	pRagdoll->fContactSoftCFM = 0.0002f;
	pRagdoll->iResponseMode = 0;
	pRagdoll->iFreezeDelay = 0;
	pRagdoll->fContactBounce = 0.0f;
	pRagdoll->fContactBounceVelocity = 0.0f;
	pRagdoll->fContactMotion1 = 0.0f;
	pRagdoll->fContactMotion2 = 0.0f;
	pRagdoll->fContactSlip1 = 0.0f;
	pRagdoll->fContactSlip2 = 0.0f;

	pRagdoll->pNextRagdoll = pODERagdollList;
	if ( pODERagdollList ) pODERagdollList->pPrevRagdoll = pRagdoll;
	pODERagdollList = pRagdoll;

	int iMeshCount = pObject->iMeshCount;
	int iBoneCount = 0;

	for( int i = 0; i < iMeshCount; i++ )
	{
		for ( int bone = 0; bone < (int) pObject->ppMeshList [ i ]->dwBoneCount; bone++ )
		{
			//if ( pObject->ppMeshList [ i ]->pBones [ bone ].dwNumInfluences ) iBoneCount++;
			//if ( !strstr( pObject->ppMeshList [ i ]->pBones [ bone ].szName, "Clavicle" ) ) iBoneCount++;
			iBoneCount++;
		}
	}

	pRagdoll->pODEBones = new sODEBone [ iBoneCount ];
	pRagdoll->dwNumBones = iBoneCount;
	iBoneCount = 0;

	for( int i = 0; i < iMeshCount; i++ )
	{
		sMesh *pMesh = pObject->ppMeshList [ i ];
		BYTE* pVertPtr = pMesh->pOriginalVertexData;
		if ( pVertPtr == NULL ) pVertPtr = pMesh->pVertexData;	//if pOriginalVertexData has not been filled, assume pVertexData contains original vertices

		//cycle through all bones
		for( int bone = 0; bone < (int) pMesh->dwBoneCount; bone++ )
		{
			sBone *pBone = pMesh->pBones + bone;

			//values to store the bounds of the bone
			float minx=0,miny=0,minz=0;
			float maxx=0,maxy=0,maxz=0;

			//for all vertices, calculate the bounding box
			for ( int vert = 0; vert < ( int ) pBone->dwNumInfluences; vert++ )
			{
				int iOffset = pBone->pVertices [ vert ];
				float fWeight = pBone->pWeights [ vert ];

				if ( fWeight > 0.3 )
				{
					float* pVertexBase = (float*)(pVertPtr + ( pMesh->dwFVFSize * iOffset ));

					D3DXVECTOR3 vec = D3DXVECTOR3 ( *(pVertexBase+0), *(pVertexBase+1), *(pVertexBase+2) );
					
					//translate vertex onto x-axis using the bone translation and check bounds
					D3DXVECTOR3 vecFinal;
					vecFinal.x = vec.x * pBone->matTranslation._11 + vec.y * pBone->matTranslation._21 + vec.z * pBone->matTranslation._31 + pBone->matTranslation._41;
					vecFinal.y = vec.x * pBone->matTranslation._12 + vec.y * pBone->matTranslation._22 + vec.z * pBone->matTranslation._32 + pBone->matTranslation._42;
					vecFinal.z = vec.x * pBone->matTranslation._13 + vec.y * pBone->matTranslation._23 + vec.z * pBone->matTranslation._33 + pBone->matTranslation._43;

					if ( vecFinal.x < minx || vert == 0 ) minx = vecFinal.x;
					if ( vecFinal.y < miny || vert == 0 ) miny = vecFinal.y;
					if ( vecFinal.z < minz || vert == 0 ) minz = vecFinal.z;

					if ( vecFinal.x > maxx || vert == 0 ) maxx = vecFinal.x;
					if ( vecFinal.y > maxy || vert == 0 ) maxy = vecFinal.y;
					if ( vecFinal.z > maxz || vert == 0 ) maxz = vecFinal.z;
				}
			}

			// V109 - 070109 - if bone too small, will corrupt ragdoll, so make minimum size!
			if ( minx > - 5 ) minx = -5;
			if ( miny > - 5 ) miny = -5;
			if ( minz > - 5 ) minz = -5;
			if ( maxx < 5 ) maxx = 5;
			if ( maxy < 5 ) maxy = 5;
			if ( maxz < 5 ) maxz = 5;

			//calculate the center of the bounding box, this is the offset from te bone's center of rotation
			float fCenterX = (minx+maxx) / 2.0f;
			float fCenterY = (miny+maxy) / 2.0f;
			float fCenterZ = (minz+maxz) / 2.0f;

			//adjust bounding box to be smaller than the actual bone, may need tweaking.
			//float fAdjust = 0.85f;
			float fExtentX = (maxx-minx) * fAdjust;
			float fExtentY = (maxy-miny) * fAdjust;
			float fExtentZ = (maxz-minz) * fAdjust;
			
			//float fExtentX = (maxx-minx) * 0.8f;
			//float fExtentY = (maxy-miny) * 0.1f;
			//float fExtentZ = (maxz-minz) * 0.7f;

			if ( fExtentX < 0.1f ) fExtentX = 0.1f;
			if ( fExtentY < 0.1f ) fExtentY = 0.1f;
			if ( fExtentZ < 0.1f ) fExtentZ = 0.1f;

			char szName [ 256 ];
			strcpy_s( szName, 256, pBone->szName );
			strlower( szName );

			bool bNeck = strstr( szName, "neck" ) ? true : false;
			bool bSpine = strstr( szName, "spine" ) ? true : false;
			bool bClavicle = strstr( szName, "clavicle" ) ? true : false;
			bool bHead = strstr( szName, "head" ) ? true : false;
			bool bPelvis = strstr( szName, "pelvis" ) ? true : false;
			bool bThigh = strstr( szName, "thigh" ) ? true : false;
			bool bUpperArm = strstr( szName, "upper" ) && strstr( szName, "arm" ) ? true : false;
			bool bMain = strcmp( szName, "bip01_spine" ) ? true : false;

			if ( bMain ) pRagdoll->iCenterBone = iBoneCount;

			int iGroup = 0;
			if ( bSpine || bClavicle || bPelvis || bNeck /*|| bUpperArm || bThigh*/ || bHead ) iGroup = 1;

			/*
			// V109 - 110209 - reduce the input bones by the objects scaling factor
			D3DXVECTOR3 vecStorePos = D3DXVECTOR3 ( pMesh->pFrameMatrices [ bone ]->_41, pMesh->pFrameMatrices [ bone ]->_42, pMesh->pFrameMatrices [ bone ]->_43 );
			vecStorePos.x -= pObject->position.matWorld._41;
			vecStorePos.y -= pObject->position.matWorld._42;
			vecStorePos.z -= pObject->position.matWorld._43;
			vecStorePos.x *= pObject->position.vecScale.x;
			vecStorePos.y *= pObject->position.vecScale.y;
			vecStorePos.z *= pObject->position.vecScale.z;
			vecStorePos.x += pObject->position.matWorld._41;
			vecStorePos.y += pObject->position.matWorld._42;
			vecStorePos.z += pObject->position.matWorld._43;
			D3DXMatrixMultiply ( pMesh->pFrameMatrices [ bone ], pMesh->pFrameMatrices [ bone ], &pObject->position.matScale );
			pMesh->pFrameMatrices [ bone ]->_41 = vecStorePos.x;
			pMesh->pFrameMatrices [ bone ]->_42 = vecStorePos.y;
			pMesh->pFrameMatrices [ bone ]->_43 = vecStorePos.z;
			fCenterX *= pObject->position.vecScale.x;
			fCenterY *= pObject->position.vecScale.y;
			fCenterZ *= pObject->position.vecScale.z;
			*/
			fExtentX *= pObject->position.vecScale.x;
			fExtentY *= pObject->position.vecScale.y;
			fExtentZ *= pObject->position.vecScale.z;

			pRagdoll->pODEBones [ iBoneCount ].CreateBone( &pObject->position.matWorld, pMesh, bone, fCenterX, fCenterY, fCenterZ, fExtentX, fExtentY, fExtentZ, iGroup, iDebugObject );
			iBoneCount++;
			if ( iDebugObject > 0 ) iDebugObject++;
		}
	}

	if ( pRagdoll->iCenterBone < 0 ) 
		pRagdoll->iCenterBone = 0;

	//bones created, now make the joints, use frame structure
	pRagdoll->pObject = pObject;
	pRagdoll->AttachBones( NULL, pObject->pFrame );
	
	pRagdoll->iID = id;

	// 040707 - assign ragdoll total count and increment it
	pRagdoll->m_iRagdollCounterNumber = g_iRagdollCreationCounter;
	pRagdoll->m_dwRagdollCounterTime = timeGetTime();
	g_iRagdollCreationCounter++;

	pObject->position.bCustomWorldMatrix = true;
	D3DXMatrixIdentity( &pObject->position.matWorld );

	return pRagdoll;
}

void sODERagdoll::AttachBones( sFrame *pParentFrame, sFrame *pFrame )
{
	if ( !pFrame ) return;

	char szParentName [ 256 ];
	if ( pParentFrame )
	{
		if ( pParentFrame->szName ) 
		{
			strcpy_s( szParentName, 256, pParentFrame->szName );
			strlower( szParentName );
		}
		else
		{
			strcpy_s( szParentName, 256, "" );
		}
	}
	else
		strcpy_s( szParentName, 256, "" );

	char szChildName [ 256 ];
	if ( pFrame->szName ) 
	{
		strcpy_s( szChildName, 256, pFrame->szName );
		strlower( szChildName );
	}
	else
		strcpy_s( szChildName, 256, "" );

	//bool bStop = (pFrame->szName && strstr( pFrame->szName, "Arm" )) || (pParentFrame->szName && strstr( pParentFrame->szName, "Arm" )) ? true : false;
	bool bClavicle = strstr( szChildName, "clavicle" ) ? true : false;
	bool bNeck = strstr( szChildName, "neck" ) ? true : false;
	bool bSpine = strstr( szChildName, "spine" ) ? true : false;
	bool bParentSpine = strstr( szParentName, "spine" ) ? true : false;

	bool bSkip = bClavicle || bNeck;

	sFrame *pSibling = pFrame;
	while ( pSibling )
	{
		//attach all siblings to the parent
		if ( pParentFrame ) CreateJoint( pParentFrame, pSibling );

		//attach child frames to this sibling
		if ( pSibling->pChild )
		{
			//skip over some bones, e.g. attach arms straight to body
			if ( !bSkip ) 
				AttachBones( pSibling, pSibling->pChild );
			else 
				AttachBones( pParentFrame, pSibling->pChild );
		}

		pSibling = pSibling->pSibling;
	}
}

void sODERagdoll::CreateJoint( sFrame *pParentLimb, sFrame *pChildLimb )
{
	if ( !pParentLimb ) return;
	if ( !pChildLimb ) return;

	int iParent = pParentLimb->iID;
	int iChild = pChildLimb->iID;

	sODEBone *pParentBone = FindBoneFromOwner( iParent );
	sODEBone *pChildBone = FindBoneFromOwner( iChild );

	char szParentName [ 256 ];
	if ( pParentLimb )
	{
		if ( pParentLimb->szName )
		{
			strcpy_s( szParentName, 256, pParentLimb->szName );
			strlower( szParentName );
		}
		else
			strcpy_s( szParentName, 256, "" );
	}
	else
		strcpy_s( szParentName, 256, "" );

	char szChildName [ 256 ];
	if ( pChildLimb->szName )
	{
		strcpy_s( szChildName, 256, pChildLimb->szName );
		strlower( szChildName );
	}
	else
	{
		strcpy_s( szChildName, 256, "" );
	}

	bool bClavicle = strstr( szChildName, "clavicle" ) ? true : false;
	bool bNeck = strstr( szChildName, "neck" ) ? true : false;
	bool bChildSpine = strstr( szChildName, "spine" ) ? true : false;
	bool bParentSpine = strstr( szParentName, "spine" ) ? true : false;
	bool bUpperArm = strstr( szChildName, "upper" ) && strstr( szChildName, "arm" ) ? true : false;
	bool bForeArm = strstr( szChildName, "fore" ) && strstr( szChildName, "arm" ) ? true : false;
	bool bToe = strstr( szChildName, "toe" ) ? true : false;
	bool bFinger = strstr( szChildName, "finger" ) ? true : false;
	bool bHand = strstr( szChildName, "hand" ) ? true : false;
	bool bFoot = strstr( szChildName, "foot" ) ? true : false;
	bool bCalf = strstr( szChildName, "calf" ) ? true : false;
	bool bThigh = strstr( szChildName, "thigh" ) ? true : false;
	bool bHead = strstr( szChildName, "head" ) ? true : false;

	if ( !pParentBone ) return;
	if ( !pChildBone ) return;

	D3DXVECTOR3 vecChildX( pChildBone->GetMatrix()->_11, pChildBone->GetMatrix()->_12, pChildBone->GetMatrix()->_13 );
	D3DXVECTOR3 vecChildY( pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23 );
	D3DXVECTOR3 vecChildZ( pChildBone->GetMatrix()->_31, pChildBone->GetMatrix()->_32, pChildBone->GetMatrix()->_33 );
	D3DXVECTOR3 vecParentX( pParentBone->GetMatrix()->_11, pParentBone->GetMatrix()->_12, pParentBone->GetMatrix()->_13 );
	D3DXVECTOR3 vecParentY( pParentBone->GetMatrix()->_21, pParentBone->GetMatrix()->_22, pParentBone->GetMatrix()->_23 );
	D3DXVECTOR3 vecParentZ( pParentBone->GetMatrix()->_31, pParentBone->GetMatrix()->_32, pParentBone->GetMatrix()->_33 );

	if ( D3DXVec3Dot( &vecChildY, &vecParentY ) < 0 ) vecParentY = -vecParentY;

	D3DXVECTOR3 vecNewX = vecChildX;
	D3DXVec3Cross( &vecNewX, &vecParentY, &vecChildZ );

	if ( bThigh || bUpperArm ) vecParentX = -vecParentX;

	float fChildXDotParentX = D3DXVec3Dot( &vecNewX, &vecParentX );
	float fAngle = acos( fChildXDotParentX );

	float fChildXDotParentZ = D3DXVec3Dot( &vecNewX, &vecParentZ );
	if ( fChildXDotParentZ < 0 )
	{
		fAngle = -fAngle;
	}

	// lee - 310707 - determine if bone should respond to external forces
	if ( bClavicle | bChildSpine | bParentSpine | bForeArm | bUpperArm | bHead )
	{
		pParentBone->bResponsiveToLinearVelocity = true;
		pChildBone->bResponsiveToLinearVelocity = true;
	}
	else
	{
		pParentBone->bResponsiveToLinearVelocity = false;
		pChildBone->bResponsiveToLinearVelocity = false;
	}

	// lee - 081107 - for 12+ and TEEN rating, ragdoll does not react much as a dead body should
	bool bNoRagdollReactionForTEENRating = true;
	if ( g_iODERagdollMode==1 ) bNoRagdollReactionForTEENRating = false;

	sODEJoint* pJoint = 0;
	if ( 1 )
	{
		// U75 - 230210 - replaces legacy X10 ragdoll with more forgiving system for model not 'designed' with ragdoll in mind
		D3DXVECTOR3 vecPos;
		vecPos.x = pChildBone->fLastPositionX;
		vecPos.y = pChildBone->fLastPositionY;
		vecPos.z = pChildBone->fLastPositionZ;
		if ( bCalf )
		{
			// knees bend 80 degrees or so, typical collapsing human legs
			pJoint = sODEJoint::AddHingeJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ), 
												   pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23, //axis
												   vecPos.x, vecPos.y, vecPos.z ); //anchor
			pJoint->SetMovingHingeLimit( 0.1f, 0, -0.6f, 0, 3.0f );
		}
		else if ( bForeArm )
		{
			pJoint = sODEJoint::AddHingeJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ), 
												   pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23, //axis
												   vecPos.x, vecPos.y, vecPos.z ); //anchor
			pJoint->SetHingeLimit( 0.0f, 1.3f );
		}
		else if ( bHead || bNeck )
		{
			pJoint = sODEJoint::AddHingeJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ), 
												   pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23, //axis
												   vecPos.x, vecPos.y, vecPos.z ); //anchor
			pJoint->SetHingeLimit( -1.5f, 0.0f );
		}
		else
		{
			// all other joints fixed and rigid to prevent ugly vertex tears
			pJoint = sODEJoint::AddFixedJoint( dwNumJoints, pParentBone->GetBody( ), pChildBone->GetBody( ) );
		}
	}
	else
	{
		// 091107 - TEEN Rating ragdoll
		if ( bNoRagdollReactionForTEENRating==true )
		{
			if ( bHead )
			{
				// allow the head to loll
				D3DXVECTOR3 vecPos;
				vecPos.x = pChildBone->fLastPositionX;
				vecPos.y = pChildBone->fLastPositionY;
				vecPos.z = pChildBone->fLastPositionZ;
				pJoint = sODEJoint::AddHingeJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ), 
													   pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23, //axis
													   vecPos.x, vecPos.y, vecPos.z ); //anchor
				pJoint->SetHingeLimit( -0.8f - fAngle, 0.8f - fAngle );
			}
			else
			{
				// rigid joint prevents ragdoll collapse
				pJoint = sODEJoint::AddFixedJoint( dwNumJoints, pParentBone->GetBody( ), pChildBone->GetBody( ) );
			}
		}
		else
		{
			//030707 - added more since if ( bClavicle || bNeck || bChildSpine )
			if ( bClavicle || bNeck || bChildSpine || bFinger || bToe || bFoot || bHand )
			{
				// 030707 - prevents small bone warping and looks like riga setting in / stiffening of character
				pJoint = sODEJoint::AddFixedJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ) );
			}
			else
			{
				D3DXVECTOR3 vecHingePos;
				vecHingePos.x = pChildBone->fLastPositionX;
				vecHingePos.y = pChildBone->fLastPositionY;
				vecHingePos.z = pChildBone->fLastPositionZ;
				pJoint = sODEJoint::AddHingeJoint( -1, pParentBone->GetBody( ), pChildBone->GetBody( ), 
													   pChildBone->GetMatrix()->_21, pChildBone->GetMatrix()->_22, pChildBone->GetMatrix()->_23, //axis
													   vecHingePos.x, vecHingePos.y, vecHingePos.z ); //anchor
				if ( bFinger || bToe || bFoot )
					pJoint->SetHingeLimit( -0.2f, 0.2f );
				else if ( bHand )
					pJoint->SetHingeLimit( -0.3f, 0.3f );
				else if ( bCalf )
					pJoint->SetMovingHingeLimit( -0.2f - fAngle, 0.01f - fAngle, -2.2f - fAngle, 0.01f - fAngle, 3.0f );
				else if ( bForeArm )
					pJoint->SetHingeLimit( -2.4f - fAngle, 0.1f - fAngle );
				else if ( bThigh ) 
					pJoint->SetMovingHingeLimit( -0.30f - fAngle, 0.30f - fAngle, -0.30f - fAngle, 1.50f - fAngle, 3.0f );
				else if ( bHead ) 
					pJoint->SetHingeLimit( -0.8f - fAngle, 0.8f - fAngle );
				else 
					pJoint->SetHingeLimit( -1.5f, 1.5f );
			}
		}
	}

	// U75 - 230210 - ensure no two bones are unconnected, it looks messy!
	//hmmnotwork if ( pJoint==0 ) pJoint = sODEJoint::AddFixedJoint( dwNumJoints, pParentBone->GetBody( ), pChildBone->GetBody( ) );

	if ( pJoint )
	{
		pODEJoints [ dwNumJoints ] = pJoint;
		dwNumJoints++;
	}
}

void sODERagdoll::RemoveRagdoll( sODERagdoll* pDelRagdoll )
{
	if ( pDelRagdoll->pPrevRagdoll )
	{
		//assign the previous ragdoll to skip over this ragdoll in the list so it can be deleted
		pDelRagdoll->pPrevRagdoll->pNextRagdoll = pDelRagdoll->pNextRagdoll;
	}
	else
	{
		//else ragdoll must be at the head of the list, assign the head pointer to skip over it
		pODERagdollList = pDelRagdoll->pNextRagdoll;
	}

	if ( pDelRagdoll->pNextRagdoll )
	{
		//assign the next joint to skip over this joint when pointing backwards
		pDelRagdoll->pNextRagdoll->pPrevRagdoll = pDelRagdoll->pPrevRagdoll;
	}

	delete pDelRagdoll;
}

sODERagdoll* sODERagdoll::GetRagdoll( int iID )
{
	sODERagdoll* pRagdoll = pODERagdollList;

	//cycle through the external joint list to find the id
	while ( pRagdoll )
	{
		if ( pRagdoll->iID == iID ) return pRagdoll;			
		pRagdoll = pRagdoll->pNextRagdoll;
	}

	return 0;
}

void sODERagdoll::DisableSilentRagdollsEachUpdate( void )
{
	sODERagdoll *pRagdoll = pODERagdollList;
	while ( pRagdoll )
	{
		if ( pRagdoll->iResponseMode==2 ) pRagdoll->SetActive ( 0 );
		pRagdoll = pRagdoll->pNextRagdoll;
	}
}

void sODERagdoll::UpdateRagdolls( float fTimeDelta )
{
	sODERagdoll *pRagdoll = pODERagdollList;
	while ( pRagdoll )
	{
		// if ragdoll not set as pseudo static, keep ragdoll calculation active
		if ( pRagdoll->iResponseMode != 2 ) pRagdoll->Update( fTimeDelta );
		pRagdoll = pRagdoll->pNextRagdoll;
	}
}

void sODERagdoll::Update( float fTimeDelta )
{
	// 081107 - freeze the ragdoll almost right away to show reaction is artificial
	bool bFreezeForRatingsMode = false;
	if ( g_iODERagdollMode==0 )
		if ( iResponseMode != 2 )
			if ( m_dwRagdollCounterTime+250 > timeGetTime() )
				bFreezeForRatingsMode = true;

	// lee - 040707 - destroy joints if too may to avoid a crash in ODE 
	if ( g_iRagdollCreationCounter > m_iRagdollCounterNumber + 8 || bFreezeForRatingsMode==true ) 
	{
		// freeze
		iResponseMode = 2;

		// delete ragdoll joints if more than X ragdolls old
		if ( pODEJoints )
		{
			for ( DWORD i = 0; i < dwNumJoints; i++ )
			{
				if ( pODEJoints [ i ] ) sODEJoint::RemoveJoint( pODEJoints [ i ] );
			}
			delete [] pODEJoints;
			pODEJoints = NULL;
		}
	}

	// this triggers the engine to update the ragdoll mesh (via CPU or GPU)
	// ONLY if travel average is big enough
	bool bRagdollMoving = false;
	float fTreshholdBeforeUpdateBones = 1.0f; // 220210 - fully rested instead (was 3.0f)
	if ( fTravelDistancePerUpdateX + fTravelDistancePerUpdateY + fTravelDistancePerUpdateZ > fTreshholdBeforeUpdateBones ) 
	{
		// update visual of this ragdoll
		pObject->bAnimUpdateOnce = true;
		bRagdollMoving = true;
	}
	else
	{
		// after a while of being still, switch to pseudo static
		iFreezeDelay++;

		// turn ragdoll bodies to pseudo static
		if ( iFreezeDelay > 3 )
		{
			// freeze
			iResponseMode = 2;

			// destroy all joints so physics performance improves
			if ( pODEJoints )
			{
				for ( DWORD i = 0; i < dwNumJoints; i++ )
				{
					if ( pODEJoints [ i ] ) sODEJoint::RemoveJoint( pODEJoints [ i ] );
				}
				delete [] pODEJoints;
				pODEJoints = NULL;
			}
		}
	}

	// distance calc during bone update
	fTravelDistancePerUpdateX = 0.0f;
	fTravelDistancePerUpdateY = 0.0f;
	fTravelDistancePerUpdateZ = 0.0f;
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		// 130607 - if moving, update bones of ragdoll (visible matrix update)
		pODEBones [ i ].UpdateBone( bRagdollMoving, pObject->position.vecScale.y );
		AddTravelDistanceX ( pODEBones [ i ].GetTravelDistancePerUpdateX() );
		AddTravelDistanceY ( pODEBones [ i ].GetTravelDistancePerUpdateY() );
		AddTravelDistanceZ ( pODEBones [ i ].GetTravelDistancePerUpdateZ() );
	}

	// update to change joint limits over time
	if ( pODEJoints )
		for ( DWORD i = 0; i < dwNumJoints; i++ )
			pODEJoints [ i ]->UpdateJoint( fTimeDelta );

	// update translation matrix with world position of ragdoll (if it moves)
	// 201007 - this was commented out on 191007 - needed for other parts of engine
	pObject->position.matTranslation._41 = pODEBones [ 0 ].matWorld._41;
	pObject->position.matTranslation._42 = pODEBones [ 0 ].matWorld._42;
	pObject->position.matTranslation._43 = pODEBones [ 0 ].matWorld._43;

	// U75 - 230210 - also need to get POSITION back from ragdoll (for dragging rag doll around)
	pObject->position.vecPosition.x = pODEBones [ 0 ].matWorld._41;
	pObject->position.vecPosition.y = pODEBones [ 0 ].matWorld._42;
	pObject->position.vecPosition.z = pODEBones [ 0 ].matWorld._43;
}

sODERagdoll* sODERagdoll::GetRagdoll( dBodyID checkbody )
{
	sODERagdoll *pRagdoll = pODERagdollList;

	while ( pRagdoll )
	{
		if ( pRagdoll->IsBodyBone( checkbody ) ) return pRagdoll;

		pRagdoll = pRagdoll->pNextRagdoll;
	}

	return 0;
}

bool sODERagdoll::IsBodyBone( dBodyID checkbody )
{
	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		if ( pODEBones [ i ].GetBody( ) == checkbody ) return true;
	}

	return false;
}

bool sODERagdoll::CheckBoneGroups( dBodyID body1, dBodyID body2 )
{
	int iGroup1 = 0;
	int iGroup2 = 0;

	for ( DWORD i = 0; i < dwNumBones; i++ )
	{
		if ( pODEBones [ i ].GetBody( ) == body1 ) iGroup1 = pODEBones [ i ].GetGroupID( );
		if ( pODEBones [ i ].GetBody( ) == body2 ) iGroup2 = pODEBones [ i ].GetGroupID( );
	}

	//groups 0 is a free for all group, everything collides
	if ( iGroup1 == iGroup2 && iGroup1 != 0 ) return true;

	return false;
}