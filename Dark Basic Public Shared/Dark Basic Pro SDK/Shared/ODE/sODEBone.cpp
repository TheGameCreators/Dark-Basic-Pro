#include "ODEPhysics.h"
#include "sODEBone.h"

extern dWorldID				g_ODEWorld;
extern dSpaceID				g_ODESpace;
extern pfnMakeObjectBox		g_pfnMakeObjectBox;
extern pfnGetObject			g_pfnGetObject;
extern pfnPositionObject	g_pfnPositionObject;
extern pfnDeleteObject		g_pfnDeleteObject;

void ODE_AddObject ( dBodyID, dGeomID, int, int );

sODEBone::sODEBone( )
{
	fOffsetX = 0;
	fOffsetY = 0;
	fOffsetZ = 0;
	iFrameOwner = -1;
	iGroupID = 0;

	D3DXMatrixIdentity( &matWorld );
	bResponsiveToLinearVelocity = true;

	pDebugObject = 0;
	iDebugID = 0;
}

sODEBone::~sODEBone( )
{
	if ( body ) dBodyDestroy ( body );
	if ( geom ) dGeomDestroy ( geom );

	if ( pDebugObject )
	{
		g_pfnDeleteObject( iDebugID );
	}
}

void sODEBone::CreateBone( const D3DXMATRIX *pObjMatrix, const sMesh *pMesh, int iBoneID, float fCenterX, float fCenterY, float fCenterZ, float fExtentX, float fExtentY, float fExtentZ, int group, int iDebugObject )
{
	geom = dCreateBox  ( g_ODESpace, fExtentX, fExtentY, fExtentZ );
	body = dBodyCreate ( g_ODEWorld );

	dMass mass;
	dMassSetBox  ( &mass, 1, fExtentX, fExtentY, fExtentZ );
//	dMassAdjust  ( &mass, 1 ); // paul, this resets the mass, ruining the ragdoll proportions
	dBodySetMass ( body, &mass );
	
	dGeomSetBody     ( geom, body );

	D3DXMATRIX matOffset;
	D3DXMatrixTranslation( &matOffset, fCenterX, fCenterY, fCenterZ );

	fSExtentX = fExtentX;
	fSExtentY = fExtentY;
	fSExtentZ = fExtentZ;

	fOffsetX = fCenterX;
	fOffsetY = fCenterY;
	fOffsetZ = fCenterZ;
	iGroupID = group;
	
	//create position matrix for ODE box, offset from bone ceneter of rotation
	D3DXMatrixMultiply( &matWorld, &matOffset, pMesh->pFrameMatrices [ iBoneID ] );

	// paul210407	D3DXMatrixMultiply( &matWorld, &matWorld, pObjMatrix );
	// U75 - 210210 - this was commented out for special X10-instancing hack, but
	// has been returned as we want the bones at real world co-ordinates, not oriented at a 0,0,0 world position
	D3DXMatrixMultiply( &matWorld, &matWorld, pObjMatrix );

	float fPosX = matWorld._41;
	float fPosY = matWorld._42;
	float fPosZ = matWorld._43;

	dBodySetPosition ( body, fPosX, fPosY, fPosZ );

	fLastPositionX = fPosX;
	fLastPositionY = fPosY;
	fLastPositionZ = fPosZ;

	dMatrix3 matrix;
	matrix[0] = matWorld._11;
	matrix[1] = matWorld._21;
	matrix[2] = matWorld._31;
	matrix[3] = REAL(0.0);
	matrix[4] = matWorld._12;
	matrix[5] = matWorld._22;
	matrix[6] = matWorld._32;
	matrix[7] = REAL(0.0);
	matrix[8] = matWorld._13;
	matrix[9] = matWorld._23;
	matrix[10] = matWorld._33;
	matrix[11] = REAL(0.0);

	dBodySetRotation ( body, matrix );

	if ( iDebugObject > 0 )
	{
		//debug shapes
		g_pfnMakeObjectBox( iDebugObject, fExtentX, fExtentY, fExtentZ );
		//g_pfnPositionObject( iDebugObject, fPosX, fPosY, fPosZ );
		sObject *pObject = g_pfnGetObject( iDebugObject );
		pObject->position.bCustomWorldMatrix = true;
		pObject->position.matWorld = matWorld;
		pObject->position.vecPosition.x = fPosX;
		pObject->position.vecPosition.y = fPosY;
		pObject->position.vecPosition.z = fPosZ;
		//ODE_AddObject( body, geom, iDebugObject, 0 );
		pDebugObject = pObject;
		iDebugID = iDebugObject;
	}

	//copy original bone matrix back into local matrix
	matWorld = *(pMesh->pFrameMatrices [ iBoneID ]);
//paul210407	D3DXMatrixMultiply( &matWorld, &matWorld, pObjMatrix );
	D3DXMatrixMultiply( &matWorld, &matWorld, pObjMatrix );

	//replace bone matrix with local matrix
	pMesh->pFrameMatrices [ iBoneID ] = &matWorld;

	iFrameOwner = pMesh->pFrameRef [ iBoneID ]->iID;
}

void sODEBone::UpdateBone( bool bRagdollMoving, float fScaleY )
{
	// 130607 - dampen position and rotation of ragdoll
	// does not stop jiggling, the jiggle is not cumilative, its instant from somewhere
	if ( 0 )
	{
		const dReal* pBB = dBodyGetLinearVel ( body );
		dBodySetLinearVel ( body, pBB[0]*0.5f, pBB[1]*0.5f, pBB[2]*0.5f );
		const dReal* pAA = dBodyGetAngularVel ( body );
		dBodySetAngularVel ( body, pAA[0]*0.5f, pAA[1]*0.5f, pAA[2]*0.5f );
	}

	//translate bone matrix to match ODE matrix, offset as necessary
	const dReal*	fPosition	= dBodyGetPosition ( body );
	const dReal*	fRotate		= dBodyGetRotation ( body );

	// calculate travel distance per update
	fTravelDistancePerUpdateX = fabs ( fPosition[0] - fLastPositionX );
	fTravelDistancePerUpdateY = fabs ( fPosition[1] - fLastPositionY );
	fTravelDistancePerUpdateZ = fabs ( fPosition[2] - fLastPositionZ );
	fLastPositionX = fPosition[0];
	fLastPositionY = fPosition[1];
	fLastPositionZ = fPosition[2];
	
	iDebugID;

	if ( bRagdollMoving==true )
	{
		matWorld._11 = fRotate [ 0 ];
		matWorld._21 = fRotate [ 1 ];
		matWorld._31 = fRotate [ 2 ];
		matWorld._41 = fPosition [ 0 ];

		matWorld._12 = fRotate [ 4 ];
		matWorld._22 = fRotate [ 5 ];
		matWorld._32 = fRotate [ 6 ];
		matWorld._42 = fPosition [ 1 ];
		
		matWorld._13 = fRotate [ 8 ];
		matWorld._23 = fRotate [ 9 ];
		matWorld._33 = fRotate [ 10 ];
		matWorld._43 = fPosition [ 2 ];

		matWorld._14 = 0;
		matWorld._24 = 0;
		matWorld._34 = 0;
		matWorld._44 = 1;

		// reduce scale here when updating bone matrix (scale not interfere with rotation)
		matWorld._11 *= fScaleY;
		matWorld._12 *= fScaleY;
		matWorld._13 *= fScaleY;
		matWorld._21 *= fScaleY;
		matWorld._22 *= fScaleY;
		matWorld._23 *= fScaleY;
		matWorld._31 *= fScaleY;
		matWorld._32 *= fScaleY;
		matWorld._33 *= fScaleY;

		D3DXMATRIX matOffset;
		D3DXMatrixTranslation( &matOffset, -fOffsetX, -fOffsetY, -fOffsetZ );

		D3DXMatrixMultiply( &matWorld, &matOffset, &matWorld );

		if ( pDebugObject )
		{
			float fPosX = matWorld._41;
			float fPosY = matWorld._42;
			float fPosZ = matWorld._43;
			pDebugObject->position.matWorld = matWorld;
			pDebugObject->position.vecPosition.x = fPosX;
			pDebugObject->position.vecPosition.y = fPosY;
			pDebugObject->position.vecPosition.z = fPosZ;
		}
	}
}

dBodyID sODEBone::GetBody( )
{
	return body;
}

int sODEBone::GetOwner( )
{
	return iFrameOwner;
}

D3DXMATRIX* sODEBone::GetMatrix( )
{
	return &matWorld;
}

int sODEBone::GetGroupID( )
{
	return iGroupID;
}