/*
	ODE PHYSICS COMMANDs
*/

// Includes
#include "ODEPhysics.h"
#include ".\..\Objects\Universe.h"
#include <stack>
#include "sODEJoint.h"
#include "sODERagdoll.h"

// Defines
#define SMALLESTSIZE 2.01f			// 280907 - from 10 and 2.01 - with new ray cast for small objects, can reduce these limits
#define SMALLESTVERTSIZE 2.01f
#define STACKOVERFLOWCAP	2000	// not ideal solution, as static collisions can build up!

// Typedefs
typedef void		( *pfnGetUniverseMeshList	)	( vector < sMesh* > *pMeshList );

// INTERNAL FUNCTIONS //////////////////////////////////////////////
void convertMatrix(D3DXMATRIX &t, const dReal *bMat);
#ifdef DARKSDK_COMPILE
	sObject*	GetObject				( int iID );
	bool		GetFVFOffsetMap			( sMesh* pMesh, sOffsetMap* psOffsetMap );
	bool		CalculateMeshBounds		( sMesh* pMesh );
	bool		CalcObjectWorld			( sObject* pObject );
	bool		MakeMeshFromOtherMesh	( bool bCreateNew, sMesh* pMesh, sMesh* pOtherMesh, D3DXMATRIX* pmatWorld );
	void		GetUniverseMeshList		( vector < sMesh* > *pMeshList );
#endif
void TransformVertices	( sObject* pObject, sMesh* pMesh, D3DXMATRIX matrix );

// EXPORTED FUNCTIONS //////////////////////////////////////////////
DARKSDK void ReceiveCoreDataPtr ( LPVOID pCore );
DARKSDK void Destructor         ( void );

// Globals
GlobStruct* g_pGlob = NULL;
char errStr [ 256 ];

// External global to track ragdoll creation (for ragdoll freeze feature)
extern int g_iRagdollCreationCounter;

void HandleError( char* szMessage )
{
	MessageBox( NULL, szMessage, "ODE Error", 0 );
	exit(0);
}

int ODE_FindStaticBodyID ( dGeomID geom );

////////////////////////////////////////////////////////////////////
// GLOBALS /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

GlobStruct*						g_pGlobal					= NULL;
pfnGetObject					g_pfnGetObject				= NULL;
pfnGetFVFOffsetMap				g_pfnGetFVFOffsetMap		= NULL;
pfnCalculateMeshBounds			g_pfnCalculateMeshBounds	= NULL;
pfnCalcObjectWorld				g_pfnCalcObjectWorld		= NULL;
pfnMakeMeshFromOtherMesh		g_pfnMakeMeshFromOtherMesh	= NULL;
pfnPositionObject				g_pfnPositionObject			= NULL;
pfnPositionObject				g_pfnRotateObject			= NULL;
pfnGetUniverseMeshList			g_pfnGetUniverseMeshList	= NULL;
pfnMakeObjectBox				g_pfnMakeObjectBox			= NULL;
pfnDeleteObject					g_pfnDeleteObject			= NULL;

dWorldID						g_ODEWorld;
dSpaceID						g_ODESpace;
dJointGroupID					g_ODEContactGroup;
std::vector < sODEObject >		g_ODEObjectList;
std::vector < sODEObject >		g_ODEStaticObjectList;
std::vector < sODEBody >		g_ODEBodyList;
float							g_fODEStep;
int								g_iODEMode;
int								g_iODERagdollMode = 1;
sODECar							g_ODECarList [ 64 ];
std::stack < sODECollision >	g_ODECollision;
sODECollision					g_ODECollisionMessage;
int								g_iCountDynamicEntityJoints = 0;
int								g_iCountDynamicEntityJointsMax = STACKOVERFLOWCAP; // any higher and too many joints are created - crash!

__int64 i64TimeFreq;
__int64 i64CurrentTime;
__int64 i64LastTime;
float fTimeDelta;

dVector3 vCollidePos, vCollideNormal;
float fCollideDepth = 0;;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

int	ODE_CollisionMessageExists ( void )
{
	memset ( &g_ODECollisionMessage, 0, sizeof ( g_ODECollisionMessage ) );
	if ( g_ODECollision.size ( ) )
		return 1;

	return 0;
}

void ODE_CollisionGetMessage ( void )
{
	g_iCountDynamicEntityJoints=0;
	g_ODECollisionMessage = g_ODECollision.top ( );
	g_ODECollision.pop ( );
}

int	ODE_GetObjectA ( void )
{
	return g_ODECollisionMessage.iObjectA;
}

int	ODE_GetObjectB ( void )
{
	return g_ODECollisionMessage.iObjectB;
}

DWORD ODE_GetObjectAContact ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAContact;
}

DWORD ODE_GetObjectAVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityX;
}

DWORD ODE_GetObjectAVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityY;
}

DWORD ODE_GetObjectAVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAVelocityZ;
}

DWORD ODE_GetObjectAAngularVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityX;
}

DWORD ODE_GetObjectAAngularVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityY;
}

DWORD ODE_GetObjectAAngularVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectAAngularVelocityZ;
}

DWORD ODE_GetObjectBContact ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBContact;
}

DWORD ODE_GetObjectBVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityX;
}

DWORD ODE_GetObjectBVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityY;
}

DWORD ODE_GetObjectBVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBVelocityZ;
}

DWORD ODE_GetObjectBAngularVelocityX ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityX;
}

DWORD ODE_GetObjectBAngularVelocityY ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityY;
}

DWORD ODE_GetObjectBAngularVelocityZ ( void )
{
	return *(DWORD*)&g_ODECollisionMessage.fObjectBAngularVelocityZ;
}

void ODE_CreateStaticUniverse ( void )
{
	// create a static triangle mesh from the universe

	// local variable declarations
	cUniverse*			pUniverse = NULL;			// universe pointer
	vector < sMesh* >	pMeshList;					// mesh list
	DWORD				dwVertexCount	= 0;		// total number of vertices
	DWORD				dwIndexCount	= 0;		// total number of indices
	int					iMesh			= 0;		// temporary mesh counter
	float*				vertices		= NULL;		// vertex list
	int					vertexCount		= 0;		// vertex count
	int*				triangles		= NULL;		// triangle list
	int					triangleCount	= 0;		// triangle count
	int					iPos			= 0;
	dTriMeshDataID		triangleData	= 0;
	dBodyID				body;
	dGeomID				geom;
	int					iTriPos			= 0;
	int					iTriOffset		= 0;
	dMass				mass;
	sOffsetMap			offsetMap;

	// check the get universe function pointer is valid
	if ( !g_pfnGetUniverseMeshList )
	{
		return;
	}
	
	// make sure the list is cleared first
	pMeshList.clear ( );

	// attempt to get the master mesh list,
	g_pfnGetUniverseMeshList ( &pMeshList );

	// check to see if we have some meshes
	if ( pMeshList.size ( ) < 1 )
		return;

	// find the total number of vertices and indices
	for ( iMesh = 0; iMesh < (int)pMeshList.size ( ); iMesh++ )
	{
		dwVertexCount += pMeshList [ iMesh ]->dwVertexCount;
		dwIndexCount  += pMeshList [ iMesh ]->dwIndexCount;
	}

	// allocate arrays for vertices and indices
	vertices		= new float [ dwVertexCount * 3 ];
	vertexCount		= dwVertexCount;
	triangles		= new int [ dwIndexCount ];
	triangleCount	= dwIndexCount;
	iPos			= 0;
	triangleData	= 0;
	
	// check pointers are valid
	if ( !vertices || !triangles )
	{
		// memory allocation failed
		return;
	}

	// check the offset map function as a precaution
	if ( !g_pfnGetFVFOffsetMap )
	{
		// invalid function pointer
		return;
	}
	
	// go through each mesh in the list
	for ( iMesh = 0; iMesh < (int)pMeshList.size ( ); iMesh++ )
	{
		// get the current mesh
		sMesh* pMesh = pMeshList [ iMesh ];

		// check the mesh is valid
		if ( !pMesh )
			continue;
		
		// get the offset map
		g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
		
		// copy each vertex position across to the new vertex array
		for ( int i = 0; i < (int)pMesh->dwVertexCount; i++ )
		{
			BYTE* pVertex = pMesh->pVertexData;

			D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
														*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
												);

			vertices [ iPos++ ] = vecPosition.x;
			vertices [ iPos++ ] = vecPosition.y;
			vertices [ iPos++ ] = vecPosition.z;
		}

		//paul missing "int i" declaration
		// copy each index value across to the new index array
		for ( int i = 0; i < (int)pMesh->dwIndexCount; i++ )
			triangles [ iTriPos++ ] = pMesh->pIndices [ i ] + iTriOffset;

		// increment the triangle offset
		iTriOffset += pMesh->dwIndexCount;
	}

	// now create a rigid body
	body = dBodyCreate ( g_ODEWorld );

	// create the triangle mesh data
	triangleData = dGeomTriMeshDataCreate ( );

	// pass in the data
	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									dwVertexCount,
									( int* ) &triangles [ 0 ],
									dwIndexCount,
									3 * sizeof ( int )
								);
	
	// finally create the big triangle mesh
	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );
}

void ReceiveCoreDataPtr ( LPVOID pCore )
{
	g_pGlobal = ( GlobStruct* ) pCore;
	g_pGlob = g_pGlobal;

	if ( !g_pGlobal )
	{

	}
}

void ODE_Start ( void )
{
	#ifndef DARKSDK_COMPILE
#ifdef X10
		g_pfnGetObject				= ( pfnGetObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetObjectW@@YAPAUsObject@@H@Z" );
#else
		g_pfnGetObject				= ( pfnGetObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
#endif
		g_pfnGetFVFOffsetMap		= ( pfnGetFVFOffsetMap			) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetFVFOffsetMap@@YA_NPAUsMesh@@PAUsOffsetMap@@@Z" );
		g_pfnCalculateMeshBounds	= ( pfnCalculateMeshBounds		) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalculateMeshBounds@@YA_NPAUsMesh@@@Z" );
		g_pfnCalcObjectWorld		= ( pfnCalcObjectWorld			) GetProcAddress ( g_pGlobal->g_Basic3D, "?CalcObjectWorld@@YA_NPAUsObject@@@Z" );
		g_pfnMakeMeshFromOtherMesh	= ( pfnMakeMeshFromOtherMesh	) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeMeshFromOtherMesh@@YA_N_NPAUsMesh@@1PAUD3DXMATRIX@@@Z" );
		g_pfnPositionObject			= ( pfnPositionObject			) GetProcAddress ( g_pGlobal->g_Basic3D, "?Position@@YAXHMMM@Z" );
		g_pfnGetUniverseMeshList	= ( pfnGetUniverseMeshList		) GetProcAddress ( g_pGlobal->g_Basic3D, "?GetUniverseMeshList@@YAXPAV?$vector@PAUsMesh@@V?$allocator@PAUsMesh@@@std@@@std@@@Z" );
		g_pfnMakeObjectBox			= ( pfnMakeObjectBox			) GetProcAddress ( g_pGlobal->g_Basic3D, "?MakeBox@@YAXHMMM@Z" );
		g_pfnDeleteObject			= ( pfnDeleteObject				) GetProcAddress ( g_pGlobal->g_Basic3D, "?DeleteEx@@YAXH@Z" );
	#else
		g_pfnGetObject				= GetObject;
		g_pfnGetFVFOffsetMap		= GetFVFOffsetMap;
		g_pfnCalculateMeshBounds	= CalculateMeshBounds;
		g_pfnCalcObjectWorld		= CalcObjectWorld;
		g_pfnMakeMeshFromOtherMesh	= MakeMeshFromOtherMesh;
		g_pfnPositionObject			= dbPositionObject;
		g_pfnRotateObject			= dbRotateObject;
		g_pfnGetUniverseMeshList	= GetUniverseMeshList;
	#endif

	g_ODEWorld = dWorldCreate ( );

	// type of space to detect collision within
	if ( 1 )
	{
		// I think some optimisation potential here (a big flaky once in my proto-staticobj miss)
		// ie such as the circle windows (WHEN STATIC GEOM) of size 60,60,5 which could not be detected!
		g_ODESpace = dHashSpaceCreate  ( 0 );
	}
	else
	{
		// Absolutely no collision culling, everything Vs everything approach (not for final release)
		g_ODESpace = dSimpleSpaceCreate  ( 0 );
	}

	g_ODEContactGroup = dJointGroupCreate ( 0 );
	g_iODEMode		  = 0;
	g_fODEStep		  = 0.05f;

	// world defaults
	dWorldSetGravity ( g_ODEWorld, 0, -9.81f, 0 );
	ODE_SetAutoDisableFlag ( 1 );
// 140307 - ragdolls jitter and do not stop with this;	ODE_SetAutoDisableLinearThreshold ( 0.2f );
//	ODE_SetAutoDisableAngularThreshold ( 0.2f );
// 240407 - big alien jittering
	ODE_SetAutoDisableLinearThreshold ( 3.0f );
	ODE_SetAutoDisableAngularThreshold ( 3.0f );
	ODE_SetAutoDisableSteps( 20 );

	QueryPerformanceFrequency ( (LARGE_INTEGER*) &i64TimeFreq );
    QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
    i64LastTime = i64CurrentTime;
}

void ODE_End ( void )
{
	// X10 - 051008 - delete any ragdolls still in the scene
	sODERagdoll* pRagdoll = sODERagdoll::pODERagdollList;
	while ( pRagdoll )
	{
		sODERagdoll* pNextOne = pRagdoll->pNextRagdoll;
		sODERagdoll::RemoveRagdoll( pRagdoll );
		pRagdoll = pNextOne;
	}
	sODERagdoll::pODERagdollList = NULL;

	// X10 - 051008 - delete any objects still in the scene
	for ( int iObjectID=1; iObjectID<65535; iObjectID++ )
		ODE_DestroyObject ( iObjectID );
	g_ODEObjectList.clear();

	// close ode
	dCloseODE();
}

void ODE_DestroyObject ( int iObject )
{
	// scan dynamic objects
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].iID == iObject )
		{
			sObject* pObject = g_pfnGetObject ( g_ODEObjectList [ i ].iID );
			if ( pObject ) pObject->position.bCustomWorldMatrix = false;

			if ( g_ODEObjectList [ i ].body )
				dBodyDestroy ( g_ODEObjectList [ i ].body );

			if ( g_ODEObjectList [ i ].geom )
				dGeomDestroy ( g_ODEObjectList [ i ].geom );

			if ( g_ODEObjectList [ i ].geom2 )
				dGeomDestroy ( g_ODEObjectList [ i ].geom2 );

			//paull - erase() needs an index
			g_ODEObjectList.erase ( g_ODEObjectList.begin() + i );
		}
	}

	// scan static objects
	for ( int i = 0; i < (int)g_ODEStaticObjectList.size ( ); i++ )
	{
		if ( g_ODEStaticObjectList [ i ].iID == iObject )
		{
			if ( g_ODEStaticObjectList [ i ].geom )
				dGeomDestroy ( g_ODEStaticObjectList [ i ].geom );

			if ( g_ODEStaticObjectList [ i ].geom2 )
				dGeomDestroy ( g_ODEStaticObjectList [ i ].geom2 );

			//paul - erase() needs an index
			g_ODEStaticObjectList.erase ( g_ODEStaticObjectList.begin() + i );
		}
	}
}

void MakeRotationMatrix ( const dReal* fRotate, D3DXMATRIX* pMatrix )
{
	pMatrix->_11 = fRotate [  0 ]; 
	pMatrix->_12 = fRotate [  4 ]; 
	pMatrix->_13 = fRotate [  8 ]; 
	pMatrix->_14 = 0; 

	pMatrix->_21 = fRotate [  1 ]; 
	pMatrix->_22 = fRotate [  5 ]; 
	pMatrix->_23 = fRotate [  9 ]; 
	pMatrix->_24 = 0; 

	pMatrix->_31 = fRotate [  2 ]; 
	pMatrix->_32 = fRotate [  6 ]; 
	pMatrix->_33 = fRotate [ 10 ]; 
	pMatrix->_34 = 0;
	
	pMatrix->_41 = 0;
	pMatrix->_42 = 0;
	pMatrix->_43 = 0;
	pMatrix->_44 = 1;
}

void DBPAnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
	float m00 = pmatMatrix->_11;
	float m01 = pmatMatrix->_12;
	float m02 = pmatMatrix->_13;
	float m12 = pmatMatrix->_23;
	float m22 = pmatMatrix->_33;
	float heading = (float)atan2(m01,m00);
	float attitude = (float)atan2(m12,m22);
	float bank = (float)asin(-m02);

	// check for gimbal lock
	if ( fabs ( m02 ) > 1.0f )
	{
		// looking straight up or down
		float PI = D3DX_PI / 2.0f;
		pVecAngles->x = 0.0f;
		pVecAngles->y = D3DXToDegree ( PI * m02 );
		pVecAngles->z = 0.0f;
	}
	else
	{
		pVecAngles->x = D3DXToDegree ( attitude );
		pVecAngles->y = D3DXToDegree ( bank );
		pVecAngles->z = D3DXToDegree ( heading );
	}
}

int ODE_RayCast( int iObject, float sx, float sy, float sz, float ex, float ey, float ez )
{
	float diffx = ex - sx;
	float diffy = ey - sy;
	float diffz = ez - sz;
	float length = sqrt( diffx*diffx + diffy*diffy + diffz*diffz );

	dGeomID ray = dCreateRay( g_ODESpace, length );
	dGeomRaySet( ray, sx,sy,sz, diffx,diffy,diffz );

	fCollideDepth = -1;

	try
	{
		if ( iObject > 0 )
		{
			dContact contact;//dContactGeom
			sODEObject *pObj= ODE_FindID( iObject );

			if ( !pObj ) throw 0;

			if ( dCollide( ray, pObj->geom, 1, &contact.geom, sizeof(contact) ) )
			{
				vCollidePos [ 0 ] = contact.geom.pos [ 0 ];
				vCollidePos [ 1 ] = contact.geom.pos [ 1 ];
				vCollidePos [ 2 ] = contact.geom.pos [ 2 ];

				vCollideNormal [ 0 ] = contact.geom.normal [ 0 ];
				vCollideNormal [ 1 ] = contact.geom.normal [ 1 ];
				vCollideNormal [ 2 ] = contact.geom.normal [ 2 ];
				
				fCollideDepth = contact.geom.depth;
				throw 1;
			}
		}
		else
		{
			dContact contact [ 10 ];//dContactGeom
			int objs = dSpaceGetNumGeoms( g_ODESpace );

			for (int i = 0; i < objs; i++ ) 
			{
				dGeomID geom = dSpaceGetGeom( g_ODESpace, i );
				int numContacts = dCollide( ray, geom, 10, &contact [ 0 ].geom, sizeof(dContact) );//dContactGeom
				for ( int c = 0; c < numContacts && c < 10; c++ )
				{
					if ( fCollideDepth < 0 || contact [ c ].geom.depth < fCollideDepth )
					{
						vCollidePos [ 0 ] = contact [ c ].geom.pos [ 0 ];
						vCollidePos [ 1 ] = contact [ c ].geom.pos [ 1 ];
						vCollidePos [ 2 ] = contact [ c ].geom.pos [ 2 ];

						vCollideNormal [ 0 ] = contact [ c ].geom.normal [ 0 ];
						vCollideNormal [ 1 ] = contact [ c ].geom.normal [ 1 ];
						vCollideNormal [ 2 ] = contact [ c ].geom.normal [ 2 ];

						fCollideDepth = contact [ c ].geom.depth;
					}
				}
			}
		}
	}
	catch( int num )
	{
		dSpaceRemove( g_ODESpace, ray );
		dGeomDestroy( ray ); 
		return num;
	}

	dSpaceRemove( g_ODESpace, ray );
	dGeomDestroy( ray ); 

	return fCollideDepth >= 0;
}

DWORD ODE_GetRayCollisionX( )
{
	float value = vCollidePos [ 0 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionY( )
{
	float value = vCollidePos [ 1 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayCollisionZ( )
{
	float value = vCollidePos [ 2 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalX( )
{
	float value = vCollideNormal [ 0 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalY( )
{
	float value = vCollideNormal [ 1 ];
	return *( (DWORD*) &value );
}

DWORD ODE_GetRayNormalZ( )
{
	float value = vCollideNormal [ 2 ];
	return *( (DWORD*) &value );
}

void Update ( float fStep )
{
	// 150607 - ragdolls using response mode are disabled each update
	sODERagdoll::DisableSilentRagdollsEachUpdate();

	//raycast check for small objects
	for ( int i = 0; i < (int)g_ODEObjectList.size(); i++ )
	{
		if ( g_ODEObjectList [ i ].bRayCast )
		{
			float fRadius = 1.0;
			if ( dGeomGetClass( g_ODEObjectList [ i ].geom ) == dSphereClass ) fRadius = dGeomSphereGetRadius( g_ODEObjectList [ i ].geom );
			if ( dGeomGetClass( g_ODEObjectList [ i ].geom ) == dBoxClass ) 
			{
				dVector3 vSize;
				dGeomBoxGetLengths( g_ODEObjectList [ i ].geom, vSize );
				fRadius = (vSize[0] + vSize[1] + vSize[2]) / 3.0f;
			}

			const dReal *pos = dGeomGetPosition( g_ODEObjectList [ i ].geom );
			const dReal *vel = dBodyGetLinearVel( g_ODEObjectList [ i ].body );
			dVector3 vGrav;
			dWorldGetGravity( g_ODEWorld, vGrav );
			float fNewVelX = (vel[0] + vGrav[0]*fStep)*0.99f;	//added gravity manually and damp speed
			float fNewVelY = (vel[1] + vGrav[1]*fStep)*0.99f;
			float fNewVelZ = (vel[2] + vGrav[2]*fStep)*0.99f;
			
			float fLength = sqrt( fNewVelX*fNewVelX + fNewVelY*fNewVelY + fNewVelZ*fNewVelZ );
			if ( fLength*fStep < 0.001f ) continue;

			dGeomID ray = dCreateRay( g_ODESpace, fLength*fStep );
			dGeomRaySet( ray, pos[0],pos[1],pos[2], fNewVelX/fLength,fNewVelY/fLength,fNewVelZ/fLength );

			fCollideDepth = -1;

			dContact contact [ 10 ];//dContactGeom
			int iNumGeoms = dSpaceGetNumGeoms( g_ODESpace );

			//check against all geoms in the space
			for (int obj = 0; obj < iNumGeoms; obj++ ) 
			{
				dGeomID geom = dSpaceGetGeom( g_ODESpace, obj );
				if ( geom == g_ODEObjectList [ i ].geom || geom == ray ) continue;

				//check ray against geom, record at most 10 contacts - unordered
				int numContacts = dCollide( ray, geom, 10, &contact[0].geom, sizeof(dContact) );//dContactGeom

				//find the closest contact and store it
				for ( int c = 0; c < numContacts && c < 10; c++ )
				{
					if ( fCollideDepth < 0 || (contact [ c ].geom.depth < fCollideDepth && contact [ c ].geom.depth > 0) )
					{
						vCollidePos [ 0 ] = contact [ c ].geom.pos [ 0 ];
						vCollidePos [ 1 ] = contact [ c ].geom.pos [ 1 ];
						vCollidePos [ 2 ] = contact [ c ].geom.pos [ 2 ];

						vCollideNormal [ 0 ] = contact [ c ].geom.normal [ 0 ];
						vCollideNormal [ 1 ] = contact [ c ].geom.normal [ 1 ];
						vCollideNormal [ 2 ] = contact [ c ].geom.normal [ 2 ];

						fCollideDepth = contact [ c ].geom.depth;
					}
				}
			}

			dSpaceRemove( g_ODESpace, ray );
			dGeomDestroy( ray );

			//object collided on route, place at closest collision
			//(small chance of error here due to only 10 contacts being checked)
			if ( fCollideDepth > 0 )
			{
				//add a collision message for 
				sODECollision collision = { 0 };
				collision.iObjectA = g_ODEObjectList [ i ].iID; // 201007 - paul, corrected from just 'i'
				const dReal* pA = dBodyGetLinearVel ( g_ODEObjectList [ i ].body );
				collision.fObjectAVelocityX = pA [ 0 ];
				collision.fObjectAVelocityY = pA [ 1 ];
				collision.fObjectAVelocityZ = pA [ 2 ];
				g_ODECollision.push ( collision );
						
				//dGeomSetPosition( g_ODEObjectList [ i ].geom, vCollidePos [ 0 ], vCollidePos [ 1 ], vCollidePos [ 2 ] );
				float fNewX = pos[0] + (fNewVelX/fLength)*(fCollideDepth-fRadius*0.9);
				float fNewY = pos[1] + (fNewVelY/fLength)*(fCollideDepth-fRadius*0.9);
				float fNewZ = pos[2] + (fNewVelZ/fLength)*(fCollideDepth-fRadius*0.9);
				dGeomSetPosition( g_ODEObjectList [ i ].geom, fNewX, fNewY, fNewZ );
				//dBodyDisable( g_ODEObjectList [ i ].body );

				float fNormLength = sqrt(vCollideNormal[0]*vCollideNormal[0] + vCollideNormal[1]*vCollideNormal[1] + vCollideNormal[2]*vCollideNormal[2]);
				vCollideNormal[0] = vCollideNormal[0]/fNormLength;
				vCollideNormal[1] = vCollideNormal[1]/fNormLength;
				vCollideNormal[2] = vCollideNormal[2]/fNormLength;

				float fBounce = 1.6f;	//This sets the bounciness of the object, 2.0 = perfect bounce (loses no height), 1.0 = no bounce

				float fDot = fNewVelX*vCollideNormal[0] + fNewVelY*vCollideNormal[1] + fNewVelZ*vCollideNormal[2];
				fNewVelX = fNewVelX - vCollideNormal[0]*fDot*fBounce;
				fNewVelY = fNewVelY - vCollideNormal[1]*fDot*fBounce;
				fNewVelZ = fNewVelZ - vCollideNormal[2]*fDot*fBounce;
				dBodySetLinearVel( g_ODEObjectList [ i ].body, fNewVelX, fNewVelY, fNewVelZ );
			}
			else
			{
				float fNewX = pos[0] + fNewVelX*fStep;
				float fNewY = pos[1] + fNewVelY*fStep;
				float fNewZ = pos[2] + fNewVelZ*fStep;
				dGeomSetPosition( g_ODEObjectList [ i ].geom, fNewX, fNewY, fNewZ );

				dBodySetLinearVel( g_ODEObjectList [ i ].body, fNewVelX, fNewVelY, fNewVelZ );
			}

			//completely taking control of the object
			dBodyDisable( g_ODEObjectList [ i ].body );
			dGeomDisable( g_ODEObjectList [ i ].geom );
		}
	}

	dSpaceCollide ( g_ODESpace, 0, &ODE_Callback );
	sODERagdoll::DisableSilentRagdollsEachUpdate();

	if ( g_iODEMode == 0 )
	{
		dWorldQuickStep ( g_ODEWorld, fStep );
	}
	else if ( g_iODEMode == 1 )
	{
		dWorldStep ( g_ODEWorld, fStep );
	}

	dJointGroupEmpty ( g_ODEContactGroup );

	sODERagdoll::UpdateRagdolls( fStep );

	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		// FPSC specific
		if ( dBodyIsEnabled  ( g_ODEObjectList [ i ].body ) )
		{
			// this stops crash, caused by angular velocity exponentially accelerating and objs going numb and failing ALL collision
			const dReal* pAA = dBodyGetAngularVel ( g_ODEObjectList [ i ].body );
			dBodySetAngularVel ( g_ODEObjectList [ i ].body, pAA[0]*0.9f, pAA[1]*0.9f, pAA[2]*0.9f );
			// this ensures objects never go TOO fast to penetrate geom
			/* another way in X10
			const dReal* pBB = dBodyGetLinearVel ( g_ODEObjectList [ i ].body );
			float fSpeed = abs(pBB[0]);
			if ( fSpeed<abs(pBB[1]) ) fSpeed = abs(pBB[1]);
			if ( fSpeed<abs(pBB[2]) ) fSpeed = abs(pBB[2]);
			if ( fSpeed>80.0f )
			{
				fSpeed=fSpeed/80.0f;
				float fNewX = pBB[0] / fSpeed;
				float fNewY = pBB[1] / fSpeed;
				float fNewZ = pBB[2] / fSpeed;
				//dBodySetLinearVel ( g_ODEObjectList [ i ].body, fNewX, fNewY, fNewZ );
			}
			*/
		}

		// X10 - 300707 - doors tend to move very slowly
		if ( g_ODEObjectList [ i ].iResponseMode==2 )
		{
			// ensures pseudo static are completely none-moving
			dBodySetLinearVel ( g_ODEObjectList [ i ].body, 0, 0, 0 );
		}
		
		D3DXMATRIX		matTranslation,
						matRotation,
						matRotate,
						matWorld;
		const dReal*	fPosition	= dBodyGetPosition ( g_ODEObjectList [ i ].body );
		const dReal*	fRotate		= dBodyGetRotation ( g_ODEObjectList [ i ].body );
		sObject*		pObject		= g_pfnGetObject ( g_ODEObjectList [ i ].iID );

		// make the rotation matrix
		MakeRotationMatrix ( fRotate, &matRotate );	

		// Use physics object position
		g_pfnPositionObject ( g_ODEObjectList [ i ].iID, fPosition [ 0 ], fPosition [ 1 ], fPosition [ 2 ] );
		D3DXMatrixTranslation ( &matTranslation, fPosition [ 0 ], fPosition [ 1 ], fPosition [ 2 ] );

		// Apply pivot if any
		matRotation = matRotate;
		if ( pObject->position.bApplyPivot )
		{
			// modify current rotation
			matRotation = pObject->position.matPivot * matRotation;
		}

		// final world matrix for physics object back to dbpro object
		matWorld    = pObject->position.matScale * matRotation * matTranslation;

		// verify object ptr
		sODEObject* pODEObject = ODE_FindID ( g_ODEObjectList [ i ].iID );
		if ( pODEObject )
		{
			// this info used if the adjustment values are read (or height value)
			// used for adjustment for small objects with boundbox margins and height for shadow calc.
			pODEObject->bNoNeedToRecalculateShape = false;
		}		
		
		// use ODE matrix, not regular DBPro matrix
		pObject->position.bCustomWorldMatrix = true;
		pObject->position.matWorld			 = matWorld;

		// also copy rotation matrix for orient command to use
		pObject->position.matRotation = matRotate;
		DBPAnglesFromMatrix ( &matRotate, &pObject->position.vecRotate );
	}
}

void ODE_Update ( void )
{
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	fTimeDelta = ( i64CurrentTime - i64LastTime ) / ( (float) i64TimeFreq );
	i64LastTime = i64CurrentTime;

	// if time step is too big objects will tunnel through each other.
	// lee - 170407 - objects drop out at 10fps, let alone 1fps - adjust max to Xfps
	float fMaxForStableSimulation = 1.0f / 10.0f;
	// multiplier because we multiply later on
	fMaxForStableSimulation /= 3.0f;
	// if time leap too great, bring it back to min
	if ( fTimeDelta > fMaxForStableSimulation ) fTimeDelta = fMaxForStableSimulation;
	if ( fTimeDelta < 0.0001f ) fTimeDelta=0.0001f;

	//on first update i64LastTime is undefined, step a fixed amount
	static bool bInitialUpdate = true;

	// skip intro setup delay
	if ( bInitialUpdate )
	{
		// startup update
		Update( 0.001f );
		bInitialUpdate = false;
	}
	else
	{
		fTimeDelta *= 3.0f;
		Update( fTimeDelta );
	}	
}

void ODE_Update ( float fManualStep )
{
	// X10 - 221007 - if call a manualy update of 0.001 (pause physics), then make all ragdolls static
	if ( fManualStep==0.001f )
	{
		// this ensures all ragdolls joints are destroyed (prevents the ragdoll jump and corrupt bug when pause physics)
		g_iRagdollCreationCounter += 10;
	}

	// performs  manual update, and also resets the i64LastTime to solve physics mid-run physics jump in time deltas
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	i64LastTime = i64CurrentTime;
	Update( fManualStep );
}

void ODE_SetContact ( int iID, int iMode, float fValue )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iID );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		switch ( iMode )
		{
			case 0: pRagdoll->fContactMU				= fValue; break;
			case 1: pRagdoll->fContactMU2				= fValue; break;
			case 2: pRagdoll->fContactBounce			= fValue; break;
			case 3: pRagdoll->fContactBounceVelocity	= fValue; break;
			case 4: pRagdoll->fContactSoftERP			= fValue; break;
			case 5: pRagdoll->fContactSoftCFM			= fValue; break;
			case 6: pRagdoll->fContactMotion1			= fValue; break;
			case 7: pRagdoll->fContactMotion2			= fValue; break;
			case 8: pRagdoll->fContactSlip1				= fValue; break;
			case 9: pRagdoll->fContactSlip1				= fValue; break;
		}
	}
	if ( pObject )
	{
		switch ( iMode )
		{
			case 0: pObject->fContactMU				= fValue; break;
			case 1: pObject->fContactMU2			= fValue; break;
			case 2: pObject->fContactBounce			= fValue; break;
			case 3: pObject->fContactBounceVelocity = fValue; break;
			case 4: pObject->fContactSoftERP		= fValue; break;
			case 5: pObject->fContactSoftCFM		= fValue; break;
			case 6: pObject->fContactMotion1		= fValue; break;
			case 7: pObject->fContactMotion2		= fValue; break;
			case 8: pObject->fContactSlip1			= fValue; break;
			case 9: pObject->fContactSlip1			= fValue; break;
		}
	}
}

void ODE_SetContactMu2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 1, fValue );
}

void ODE_SetContactFDir1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 0, fValue );
}

void ODE_SetContactBounce ( int iID, float fValue )
{
	ODE_SetContact ( iID, 2, fValue );
}

void ODE_SetContactBounceVelocity ( int iID, float fValue )
{
	ODE_SetContact ( iID, 3, fValue );
}

void ODE_SetContactSoftERP ( int iID, float fValue )
{
	ODE_SetContact ( iID, 4, fValue );
}

void ODE_SetContactSoftCFM ( int iID, float fValue )
{
	ODE_SetContact ( iID, 5, fValue );
}

void ODE_SetContactMotion1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 6, fValue );
}

void ODE_SetContactMotion2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 7, fValue );
}

void ODE_SetContactSlip1 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 8, fValue );
}

void ODE_SetContactSlip2 ( int iID, float fValue )
{
	ODE_SetContact ( iID, 9, fValue );
}

void ODE_SetSurfaceMode ( int iID, int iMode, int iState )
{
	int			iModeList [ ]	=	{ 
										dContactMu2,
										dContactFDir1,
										dContactBounce,
										dContactSoftERP,
										dContactSoftCFM,
										dContactMotion1,
										dContactMotion2,
										dContactSlip1,
										dContactSlip2,
										dContactApprox0,
										dContactApprox1_1,
										dContactApprox1_2,
										dContactApprox1
									};

	sODEObject* pObject			= ODE_FindID ( iID );

	if ( !pObject )
		return;
	
	if ( iState )
		pObject->iSurfaceMode |= iModeList [ iMode ];
	else
		pObject->iSurfaceMode &= ~iModeList [ iMode ];
}

void ODE_SetSurfaceModeContactMu2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 0, iState );
}

void ODE_SetSurfaceModeContactFDir1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 1, iState );
}

void ODE_SetSurfaceModeContactBounce ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 2, iState );
}

void ODE_SetSurfaceModeContactSoftERP ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 3, iState );
}

void ODE_SetSurfaceModeContactSoftCFM ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 4, iState );
}

void ODE_SetSurfaceModeContactMotion1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 5, iState );
}

void ODE_SetSurfaceModeContactMotion2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 6, iState );
}

void ODE_SetSurfaceModeContactSlip1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 7, iState );
}

void ODE_SetSurfaceModeContactSlip2 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 8, iState );
}

void ODE_SetSurfaceModeContactApprox0 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 9, iState );
}

void ODE_SetSurfaceModeContactApprox11 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 10, iState );
}

void ODE_SetSurfaceModeContactApprox12 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 11, iState );
}

void ODE_SetSurfaceModeContactApprox1 ( int iID, int iState )
{
	ODE_SetSurfaceMode ( iID, 12, iState );
}

static void ODE_Callback ( void* data, dGeomID o1, dGeomID o2 )
{
	// get bodies of this collision
	dBodyID b1 = dGeomGetBody ( o1 );
	dBodyID b2 = dGeomGetBody ( o2 );

	//if bodies are directly connected by a joint then ignore collisions
	//moved downif (b1 && b2 && dAreConnected (b1,b2)) return;

	// reset IDs
	int	iID = 0;//-1 dangerous id to default with 060307
	int	iID2 = 0; //-1

	// paul - add ragdoll ptr check
	sODERagdoll *pRagdoll1 = sODERagdoll::GetRagdoll( b1 );
	sODERagdoll *pRagdoll2 = sODERagdoll::GetRagdoll( b2 );
	if ( pRagdoll1 || pRagdoll2 )
	{
		//if bodies are directly connected by a joint then ignore collisions
		if (b1 && b2 && dAreConnected (b1,b2)) return;

		//if different limbs of the same ragdoll are colliding check if this will cause problems. e.g. spine1<->spine2 collision
		if ( pRagdoll1 == pRagdoll2 )
		{
			//check if bones are in the same group, ignore collision
			if ( pRagdoll1->CheckBoneGroups( b1, b2 ) ) return;
		}

		// get ID of colliding ragdoll
		if ( pRagdoll1 ) iID = pRagdoll1->GetRagdollID();
		if ( pRagdoll2 ) iID2 = pRagdoll2->GetRagdollID();

		// if body is a pseudo-static against a NULL, no collision required at all
		// this means if plr obj hits static, plr walks thourgh ragdoll pseudo static - no no 
		// if ( pRagdoll1 && pRagdoll2==NULL ) if ( pRagdoll1->iResponseMode==2 ) return;
		// if ( pRagdoll2 && pRagdoll1==NULL ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if pseudo static ragdoll body is colliding against no other body, go no further
		if ( pRagdoll1 && !b2 ) if ( pRagdoll1->iResponseMode==2 ) return;
		if ( pRagdoll2 && !b1 ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if body is a pseudo-static against another pseudo-static, no collision required at all
		if ( pRagdoll1 && pRagdoll2 ) if ( pRagdoll1->iResponseMode==2 && pRagdoll2->iResponseMode==2 ) return;
		if ( pRagdoll1 && pRagdoll2 ) if ( pRagdoll1->iResponseMode==2 && pRagdoll2->iResponseMode==2 ) return;

		// X10 - 141107 - if psuedo static used, so guns and plr can pass through ragdoll, ignore collision with non-ragdoll OBJ
		if ( pRagdoll1 && pRagdoll2==NULL && b1 && b2 ) if ( pRagdoll1->iResponseMode==2 ) return;
		if ( pRagdoll2 && pRagdoll1==NULL && b1 && b2 ) if ( pRagdoll2->iResponseMode==2 ) return;

		// if b2 NULL must be static geom - find it's id
		if ( !b2 )
		{
			// this actually gets the ObjectID back, 
			iID2 = ODE_FindStaticBodyID ( o2 );
		}

		// reset collision structure
		sODECollision collision = { 0 };
		collision.iObjectA = iID;
		collision.iObjectB = iID2;
		collision.fObjectAContact = 0;
		collision.fObjectBContact = 0;

		// accuracy of contact points (more the better)
		const int	N = 32;
		dContact	contacts [ N ];
		const int	n = dCollide ( o1, o2, N, &contacts [ 0 ].geom, sizeof ( dContact ) );

		// if obj1 present
		if ( b1 )
		{
			if ( n!=0 ) collision.fObjectAContact = 1.0f;
			const dReal* pA = dBodyGetLinearVel ( b1 );
			collision.fObjectAVelocityX = pA [ 0 ];
			collision.fObjectAVelocityY = pA [ 1 ];
			collision.fObjectAVelocityZ = pA [ 2 ];
			const dReal* pAA = dBodyGetAngularVel ( b1 );
			collision.fObjectAAngularVelocityX = pAA [ 0 ];
			collision.fObjectAAngularVelocityY = pAA [ 1 ];
			collision.fObjectAAngularVelocityZ = pAA [ 2 ];
		}

		// if obj2 present
		if ( b2 )
		{
			if ( n!=0 ) collision.fObjectBContact = 1.0f;
			const dReal* pB = dBodyGetLinearVel ( b2 );
			collision.fObjectBVelocityX = pB [ 0 ];
			collision.fObjectBVelocityY = pB [ 1 ];
			collision.fObjectBVelocityZ = pB [ 2 ];
			const dReal* pBB = dBodyGetAngularVel ( b2 );
			collision.fObjectBAngularVelocityX = pBB [ 0 ];
			collision.fObjectBAngularVelocityY = pBB [ 1 ];
			collision.fObjectBAngularVelocityZ = pBB [ 2 ];
		}

		// add to collision list (for getcollision loop in dbpro - which empties it)
		g_ODECollision.push ( collision );

		// determine if A-B connection, or if A/B is pseudo-static, connect other to static environment
		dBodyID bTreatBodyAsStatic = 0;
		if ( pRagdoll1 != pRagdoll2 )
		{
			// ragdoll is pseudo static, only when a dynamic object is moving at it
			if ( pRagdoll1 )
				if ( pRagdoll1->iResponseMode==2 )
					if ( b2 )
						bTreatBodyAsStatic = b1;

			if ( pRagdoll2 )
				if ( pRagdoll2->iResponseMode==2 )
					if ( b1 )
						bTreatBodyAsStatic = b2;
		}

		// ragdoll could brush against a pseudo-static like a door, make the door SOLID (NULL)
		int iObjID = 0;
		int iObjID2 = 0;
		if ( b1 ) iObjID = ODE_FindBodyID ( b1 );
		if ( b2 ) iObjID2 = ODE_FindBodyID ( b2 );
		sODEObject* pObject = ODE_FindID ( iObjID );
		sODEObject* pObject2 = ODE_FindID ( iObjID2 );
		if ( pObject ) if ( pObject->iResponseMode==2 ) bTreatBodyAsStatic = b1;
		if ( pObject2 ) if ( pObject2->iResponseMode==2 ) bTreatBodyAsStatic = b2;

		// go through all contact points for this collision, add to contactgroup list
		sODERagdoll *pRagdoll = pRagdoll1;
		if ( pRagdoll==NULL ) pRagdoll = pRagdoll2;
		for ( int i = 0; i < n; ++i )
		{
			if ( 0 )
			{
				// pauls fixed - slides about
				contacts [ i ].surface.mode			= 0x00000010;
				contacts [ i ].surface.mu			= 150.0f;
				contacts [ i ].surface.mu2			= 0.0f;
				contacts [ i ].surface.bounce		= 0;
				contacts [ i ].surface.bounce_vel	= 0;
				contacts [ i ].surface.soft_erp		= 0;
				contacts [ i ].surface.soft_cfm		= 0;
				contacts [ i ].surface.motion1		= 0;
				contacts [ i ].surface.motion2		= 0;
				contacts [ i ].surface.slip1		= 0;
				contacts [ i ].surface.slip2		= 0;
			}
			else
			{
				contacts [ i ].surface.mode			= dContactSoftERP | dContactSoftCFM | 0x00000010;
				//contacts [ i ].surface.mu			= pRagdoll->fContactMU;
				//contacts [ i ].surface.soft_erp	= pRagdoll->fContactSoftERP;
				//contacts [ i ].surface.soft_cfm	= pRagdoll->fContactSoftCFM;
				contacts [ i ].surface.mu			= 10000.0f;
				contacts [ i ].surface.soft_erp		= 0.3f;
				contacts [ i ].surface.soft_cfm		= 0.0001f;
				contacts [ i ].surface.mu2			= 0.0f;
				contacts [ i ].surface.bounce		= pRagdoll->fContactBounce;
				contacts [ i ].surface.bounce_vel	= pRagdoll->fContactBounceVelocity;
				contacts [ i ].surface.motion1		= pRagdoll->fContactMotion1;
				contacts [ i ].surface.motion2		= pRagdoll->fContactMotion2;
				contacts [ i ].surface.slip1		= pRagdoll->fContactSlip1;
				contacts [ i ].surface.slip2		= pRagdoll->fContactSlip2;
			}
			dJointID c = dJointCreateContact ( g_ODEWorld, g_ODEContactGroup, &contacts [ i ] );
			dBodyID bodyA = dGeomGetBody ( contacts [ i ].geom.g1 );
			dBodyID bodyB = dGeomGetBody ( contacts [ i ].geom.g2 );
			if ( bodyA==bTreatBodyAsStatic ) bodyA=0;
			if ( bodyB==bTreatBodyAsStatic ) bodyB=0;

			// add joint to physics pool for resolving
			if ( g_iCountDynamicEntityJoints < g_iCountDynamicEntityJointsMax || iID==65534 || iID2==65534 )
			{
				dJointAttach ( c, bodyA, bodyB );
				g_iCountDynamicEntityJoints++;
			}
			/*
			if ( g_iCountDynamicEntityJoints < g_iCountDynamicEntityJointsMax || iID==65534 || iID2==65534 )
			{
				dJointAttach ( c, bodyA, bodyB );
				g_iCountDynamicEntityJoints++;
			}
			else
				if ( bodyA==0 || bodyB==0 )
					dJointAttach ( c, bodyA, bodyB );
			*/
		}
	}
	else
	{
		// get obj1 and 2 ptr
		sODEObject* pObject = ODE_FindObjPtr ( b1 );
		sODEObject* pObject2 = ODE_FindObjPtr ( b2 );
		if ( !pObject ) return;

		// if objects excluded because they are totally disabled, ignore this contact
		if ( pObject ) if ( pObject->bEnabled==false ) return;
		if ( pObject2 ) if ( pObject2->bEnabled==false ) return;

		// get IDs
		if ( b1 ) iID = ODE_FindBodyID ( b1 );
		if ( b2 ) iID2 = ODE_FindBodyID ( b2 );

		// leelee
		if ( iID==10102 || iID2==10102 )
		{
			int ibarrelcollidingwithsomething=42;
		}

		// if body is a pseudo-static against a NULL, no collision required at all
		if ( pObject && pObject2==NULL ) if ( pObject->iResponseMode==2 ) return;
		if ( pObject2 && pObject==NULL ) if ( pObject->iResponseMode==2 ) return;

		// if body is a pseudo-static against another pseudo-static, no collision required at all
		if ( pObject && pObject2 ) if ( pObject->iResponseMode==2 && pObject2->iResponseMode==2 ) return;
		if ( pObject && pObject2 ) if ( pObject->iResponseMode==2 && pObject2->iResponseMode==2 ) return;

		// if b2 NULL must be static geom - find it's id
		if ( !b2 )
		{
			// this actually gets the ObjectID back, 
			iID2 = ODE_FindStaticBodyID ( o2 );

			// if the response of objA is to ignore static one (the universe mesh), ignore this callback
			if ( pObject->iResponseMode!=0)
			{
				// is it universe mesh (body zero)
				if ( iID2==0 )
					return;
			}
		}

		// reset collision structure
		sODECollision collision = { 0 };
		collision.iObjectA = iID;
		collision.iObjectB = iID2;
		collision.fObjectAContact = 0;
		collision.fObjectBContact = 0;

		// accuracy of contact points (more the better)
		const int	N = 32;
		dContact	contacts [ N ];
		const int	n = dCollide ( o1, o2, N, &contacts [ 0 ].geom, sizeof ( dContact ) );

		// if obj1 present
		if ( b1 )
		{
			if ( n!=0 ) collision.fObjectAContact = 1.0f;
			const dReal* pA = dBodyGetLinearVel ( b1 );
			collision.fObjectAVelocityX = pA [ 0 ];
			collision.fObjectAVelocityY = pA [ 1 ];
			collision.fObjectAVelocityZ = pA [ 2 ];
			const dReal* pAA = dBodyGetAngularVel ( b1 );
			collision.fObjectAAngularVelocityX = pAA [ 0 ];
			collision.fObjectAAngularVelocityY = pAA [ 1 ];
			collision.fObjectAAngularVelocityZ = pAA [ 2 ];
		}

		// if obj2 present
		if ( b2 )
		{
			if ( n!=0 ) collision.fObjectBContact = 1.0f;
			const dReal* pB = dBodyGetLinearVel ( b2 );
			collision.fObjectBVelocityX = pB [ 0 ];
			collision.fObjectBVelocityY = pB [ 1 ];
			collision.fObjectBVelocityZ = pB [ 2 ];
			const dReal* pBB = dBodyGetAngularVel ( b2 );
			collision.fObjectBAngularVelocityX = pBB [ 0 ];
			collision.fObjectBAngularVelocityY = pBB [ 1 ];
			collision.fObjectBAngularVelocityZ = pBB [ 2 ];
		}

		// add to collision list (for getcollision loop in dbpro - which empties it)
		g_ODECollision.push ( collision );

		// determine if A-B connection, or if A/B is pseudo-static, connect other to static environment
		dBodyID bTreatBodyAsStatic = 0;
		if ( pObject  )	if ( pObject->iResponseMode==2 ) bTreatBodyAsStatic = b1;
		if ( pObject2 )	if ( pObject2->iResponseMode==2 ) bTreatBodyAsStatic = b2;

		// go through all contact points for this collision, add to contactgroup list
		for ( int i = 0; i < n; ++i )
		{
			contacts [ i ].surface.mu			= pObject->fContactMU;
			contacts [ i ].surface.mu2			= 0.0f;
			contacts [ i ].surface.bounce		= pObject->fContactBounce;
			contacts [ i ].surface.bounce_vel	= pObject->fContactBounceVelocity;

			// lee - 160307 - resolve this properly at some point (ERP, sheesh!)
			if ( iID==65534 || iID2==65534 )
			{
				// X10 hack - player borble needs to have very quick ERP (correct error quickly to avoid going through floor)
				contacts [ i ].surface.mode			= dContactSoftERP | pObject->iSurfaceMode;

				// U76 - 290710 - when hit surface that will push borble DOWN, it's not a great character controlling move
				// so as we're already hacking 65534, we will add a hack to ensure we cannot be pushed down through a floor
//				contacts [ i ].surface.soft_erp		= 2.0f; // was0.8 but could run into table and fall through floor
				contacts [ i ].surface.soft_erp		= 1.1f;
				if ( contacts [ i ].geom.normal[1] < 0.0f )
				{
					// and to help smooth things out, we even off a slight upward force so we can climb it
					contacts [ i ].geom.normal[1] = 0.001f;
				}
			}
			else
			{
				contacts [ i ].surface.mode			= pObject->iSurfaceMode;
				contacts [ i ].surface.soft_erp		= pObject->fContactSoftERP;
			}
			contacts [ i ].surface.soft_cfm		= pObject->fContactSoftCFM;
			contacts [ i ].surface.motion1		= pObject->fContactMotion1;
			contacts [ i ].surface.motion2		= pObject->fContactMotion2;
			contacts [ i ].surface.slip1		= pObject->fContactSlip1;
			contacts [ i ].surface.slip2		= pObject->fContactSlip2;

			dJointID c = dJointCreateContact ( g_ODEWorld, g_ODEContactGroup, &contacts [ i ] );
			dBodyID bodyA = dGeomGetBody ( contacts [ i ].geom.g1 );
			dBodyID bodyB = dGeomGetBody ( contacts [ i ].geom.g2 );
			if ( bodyA==bTreatBodyAsStatic ) bodyA=0;
			if ( bodyB==bTreatBodyAsStatic ) bodyB=0;

			// add joint to physics pool for resolving
			if ( g_iCountDynamicEntityJoints < g_iCountDynamicEntityJointsMax || iID==65534 || iID2==65534 )
			{
				dJointAttach ( c, bodyA, bodyB );
				g_iCountDynamicEntityJoints++;
			}
		}
	}
}

void ODE_AddObject ( dBodyID body, dGeomID geom, int iID, int iFrame )
{
	sODEObject object = { 0 };

	object.bEnabled = true;
	object.body = body;
	object.geom = geom;
	object.iID  = iID;
	object.iFrame  = iFrame;

	g_ODEObjectList.push_back ( object );
}

void ODE_AddObject ( sODEObject object, int iID, int iFrame )
{
	object.bEnabled = true;
	object.iID = iID;
	object.iFrame = iFrame;
	g_ODEObjectList.push_back ( object );
}

sODEObject* ODE_FindID ( int iID )
{
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].iID == iID )
			return &g_ODEObjectList [ i ];
	}

	return NULL;
}

void ODE_SetWorldGravity ( float fX, float fY, float fZ )
{
	dWorldSetGravity ( g_ODEWorld, fX, fY, fZ );
}

void ODE_SetWorldERP ( float fValue )
{
//	dWorldSetERP ( g_ODEWorld, 0.2f );
	dWorldSetERP ( g_ODEWorld, fValue );
}

void ODE_SetWorldCFM ( float fValue )
{
//	dWorldSetCFM ( g_ODEWorld, 0.00001f ); // 130607 - removes ragdoll jiggle
	dWorldSetCFM ( g_ODEWorld, fValue );
}

void ODE_SetWorldStep ( float fStep )
{
	//U75 - 150210 - interferes with FPSC compile process
	//static bool bWarned = false;
	static bool bWarned = true;
	if ( !bWarned ) MessageBox( NULL, "The command 'ODE Set World Step' is no longer supported, use 'ODE Update <step value>' instead", "Warning", 0 );
	bWarned = true;

	g_fODEStep = fStep;
}

void ODE_SetWorldQuickStepNumIterations ( int iValue )
{
	dWorldSetQuickStepNumIterations ( g_ODEWorld, iValue );
}

void ODE_SetStepMode ( int iMode )
{
	g_iODEMode = iMode;
}

void ODE_SetRagdollMode	( int iOneIsOn )
{
	// 201107 - so we can control TEEN rating ragdoll freeze in here
	g_iODERagdollMode = iOneIsOn;
}

float ODE_GetWorldGravityX ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 0 ];
}

float ODE_GetWorldGravityY ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 1 ];
}

float ODE_GetWorldGravityZ ( void )
{
	dVector3 gravity;

	dWorldGetGravity ( g_ODEWorld, gravity );

	return gravity [ 2 ];
}

float ODE_GetWorldERP ( void )
{
	return dWorldGetERP ( g_ODEWorld );
}

float ODE_GetWorldCFM ( void )
{
	return dWorldGetCFM ( g_ODEWorld );
}

float ODE_GetWorldStep ( void )
{
	return g_fODEStep;
}

int ODE_GetWorldQuickStepNumIterations ( void )
{
	return dWorldGetQuickStepNumIterations ( g_ODEWorld );
}

void ODE_SetAutoDisableFlag ( int iFlag )
{
	dWorldSetAutoDisableFlag ( g_ODEWorld, iFlag );
}

void ODE_SetAutoDisableLinearThreshold ( float fThreshold )
{
	dWorldSetAutoDisableLinearThreshold ( g_ODEWorld, fThreshold );
}

void ODE_SetAutoDisableAngularThreshold ( float fThreshold )
{
	dWorldSetAutoDisableAngularThreshold ( g_ODEWorld, fThreshold );
}

void ODE_SetAutoDisableSteps ( int iSteps )
{
	dWorldSetAutoDisableSteps ( g_ODEWorld, iSteps );
}

void ODE_SetAutoDisableTime ( float fTime )
{
	dWorldSetAutoDisableTime ( g_ODEWorld, fTime );
}

int ODE_GetAutoDisableFlag ( void )
{
	return dWorldGetAutoDisableFlag ( g_ODEWorld );
}

float ODE_GetAutoDisableLinearThreshold ( void )
{
	return dWorldGetAutoDisableLinearThreshold ( g_ODEWorld );
}

float ODE_GetAutoDisableAngularThreshold ( void )
{
	return dWorldGetAutoDisableAngularThreshold ( g_ODEWorld );
}

int ODE_GetAutoDisableSteps ( void )
{
	return dWorldGetAutoDisableSteps ( g_ODEWorld );
}

float ODE_GetAutoDisableTime ( void )
{
	return dWorldGetAutoDisableTime ( g_ODEWorld );
}

void ODE_SetWorldContactSurfaceLayer ( float fDepth )
{
	dWorldSetContactSurfaceLayer ( g_ODEWorld, fDepth );
}

float ODE_GetWorldContactSurfaceLayer ( void )
{
	return dWorldGetContactSurfaceLayer ( g_ODEWorld );
}

sODEBody* ODE_FindBodyID ( int iID )
{
	for ( int i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			return &g_ODEBodyList [ i ];
		}
	}

	return NULL;
}

int ODE_FindBodyID ( dBodyID body )
{
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].body == body )
		{
			return g_ODEObjectList [ i ].iID;
		}
	}
	return 0;
}

sODEObject* ODE_FindObjPtr ( dBodyID body )
{
	for ( int i = 0; i < (int)g_ODEObjectList.size ( ); i++ )
	{
		if ( g_ODEObjectList [ i ].body == body )
		{
			return &(g_ODEObjectList [ i ]);
		}
	}
	return 0;
}

int ODE_FindStaticBodyID ( dGeomID geom )
{
	for ( int i = 0; i < (int)g_ODEStaticObjectList.size ( ); i++ )
	{
		if ( g_ODEStaticObjectList [ i ].geom == geom )
		{
			return g_ODEStaticObjectList [ i ].iID;
		}
	}
	return 0;
}

void ODE_BodyCreate	( int iID )
{
	sODEBody	body    = { 0 };
	int			i		= 0;

	body.iID     = iID;
	body.dBodyID = dBodyCreate ( g_ODEWorld );

	for ( i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			// error - id already exists
		}
	}

	g_ODEBodyList.push_back ( body );
}

void ODE_BodyDestroy ( int iID )
{
	//paul - erase() needs an index 
	for ( int i = 0; i < (int)g_ODEBodyList.size ( ); i++ )
	{
		if ( g_ODEBodyList [ i ].iID == iID )
		{
			g_ODEBodyList.erase( g_ODEBodyList.begin() + i );
		}
	}

	/*
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	g_ODEBodyList.erase ( pBody );	
	*/
}

void ODE_SetBodyPosition ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	dBodySetPosition ( pBody->dBodyID, fX, fY, fZ );
}

void ODE_SetBodyResponse ( int iID, int iResponseMode )
{
	// ode object
	sODEObject* pObject = ODE_FindID ( iID );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		// set respnse (0-default,1-ignorestatic,2-isstatic)
		pRagdoll->iResponseMode = iResponseMode;
	}
	if ( pObject )
	{
		// set respnse (0-default,1-ignorestatic,2-isstatic)
		pObject->iResponseMode = iResponseMode;
	}
}

void ODE_SetBodyRotation ( int iID, float fX, float fY, float fZ )
{
	// ode object
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return;

	// internal clearing value (for hacked Yrotation solution)
	if ( fX==-1.0f && fY==-1.0f && fZ==-1.0f )
	{
		pObject->iItemCarryGrab=0;
		return;
	}

	// vars needed
	dMatrix3 y, w;
	D3DXMATRIX matY, matW, matOLDW;

	// ode matrices
	dRSetIdentity ( y );
	dRSetIdentity ( w );

	// get ode W rotation
	if ( pObject->iItemCarryGrab==0 )
	{	
 		// store 'held' position
		pObject->iItemCarryGrab=1;
		const dReal* pRot = dBodyGetRotation ( pObject->body );
		if ( pRot )
		{
			for ( int i=0; i<12; i++ )
			{
				w[i] = pRot[i];
				pObject->wItemCarryGrab[i] = w[i];
			}
		}
	}
	else
	{
		// restore grabbed W
		for ( int i=0; i<12; i++ )
			w[i] = pObject->wItemCarryGrab[i];
	}

	// convert odeW to dxW
	convertMatrix ( matOLDW, w );

	// dx matrix for Y only
	dRFromEulerAngles( y, 0, D3DXToRadian ( -fY ), 0);

	// ode to dx Y only
	convertMatrix ( matY, y );

	// work out final DX matrux
	matW = matY * matOLDW;

	// dx to ode
	w [  0 ] = matW._11;
	w [  1 ] = matW._12;
	w [  2 ] = matW._13;
	w [  4 ] = matW._21;
	w [  5 ] = matW._22;
	w [  6 ] = matW._23;
	w [  8 ] = matW._31;
	w [  9 ] = matW._32;
	w [ 10 ] = matW._33;

	// final rotation
	dBodySetRotation(pObject->body, w);

	// done for temp solution.
	return;
}

void ODE_SetBodyLinearVelocity ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	// not the DBPro one!
	dBodySetLinearVel ( pBody->dBodyID, fX, fY, fZ );
}

void ODE_SetBodyAngularVelocity ( int iID, float fX, float fY, float fZ )
{
	sODEBody* pBody = ODE_FindBodyID ( iID );

	if ( !pBody )
		return;

	dBodySetAngularVel ( pBody->dBodyID, fX, fY, fZ );
}

DWORD ODE_GetBodyTravelDistanceX ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateX();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

DWORD ODE_GetBodyTravelDistanceY ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateY();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

DWORD ODE_GetBodyTravelDistanceZ ( int iID )
{
	// X10 use this to determine movement of a ragdoll
	if ( iID > 0 )
	{
		sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
		if ( pRagdoll )
		{
			float fReturnDistance = pRagdoll->GetTravelDistancePerUpdateZ();
			return *(DWORD*)&fReturnDistance;
		}
		else
			return 0;
	}
	else
		return 0;
}

float ODE_GetBodyPositionX ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyPositionY ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyPositionZ ( int iID )
{
	return 0.0f;
}

float ODE_GetBodyRotationX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fXAngle;
}

float ODE_GetBodyRotationY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fYAngle;
}

float ODE_GetBodyRotationZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0.0f;

	return pObject->fZAngle;
}

DWORD ODE_GetBodyLinearVelocityX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateX();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 0 ];
	}
}

DWORD ODE_GetBodyLinearVelocityY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateY();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 1 ];
	}
}

DWORD ODE_GetBodyLinearVelocityZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iID );
	if ( pRagdoll )
	{
		const float fValue = pRagdoll->GetTravelDistancePerUpdateZ();
		return *(DWORD*)&fValue;
	}
	else
	{
		if ( !pObject ) return 0;
		const dReal* pA = dBodyGetLinearVel ( pObject->body );
		return *(DWORD*)&pA [ 2 ];
	}
}

DWORD ODE_GetBodyAngularVelocityX ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 0 ];
}

DWORD ODE_GetBodyAngularVelocityY ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 1 ];
}

DWORD ODE_GetBodyAngularVelocityZ ( int iID )
{
	sODEObject* pObject = ODE_FindID ( iID );

	if ( !pObject )
		return 0;

	const dReal* pA = dBodyGetAngularVel ( pObject->body );

	return *(DWORD*)&pA [ 2 ];
}

void CalculateShapeOfObjAfterRotation ( sODEObject* pObject )
{
	// Work out if need to recalculate
	if ( pObject->bNoNeedToRecalculateShape==false )
	{
		// Object rotation matrix
		float fRev=0;
		D3DXMATRIX matObjRot;
		const dReal* fRotate = dBodyGetRotation ( pObject->body );
		MakeRotationMatrix ( fRotate, &matObjRot );
		D3DXMatrixInverse ( &matObjRot, &fRev, &matObjRot );

		// Default ground direction
		D3DXVECTOR3 vecDir = D3DXVECTOR3( 0, -1, 0 );
		D3DXVec3TransformNormal ( &vecDir, &vecDir, &matObjRot );

		// Default size of object
		sObject* pActualObject = g_pfnGetObject ( pObject->iID );
		float fOrigXSize = ( pActualObject->collision.vecMax.x - pActualObject->collision.vecMin.x ) * pActualObject->position.vecScale.x;
		float fOrigYSize = ( pActualObject->collision.vecMax.y - pActualObject->collision.vecMin.y ) * pActualObject->position.vecScale.y;
		float fOrigZSize = ( pActualObject->collision.vecMax.z - pActualObject->collision.vecMin.z ) * pActualObject->position.vecScale.z;
		D3DXVECTOR3 vecBodySize = D3DXVECTOR3 ( pObject->fXSize, pObject->fYSize, pObject->fZSize );

		// Margins of object
		D3DXVECTOR3 vecMargin = vecBodySize - D3DXVECTOR3 ( fOrigXSize, fOrigYSize, fOrigZSize );
		if ( vecMargin.x < 0.0f ) vecMargin.x = 0.0f; else vecMargin.x/=2.0f;
		if ( vecMargin.y < 0.0f ) vecMargin.y = 0.0f; else vecMargin.y/=2.0f;
		if ( vecMargin.z < 0.0f ) vecMargin.z = 0.0f; else vecMargin.z/=2.0f;
		pObject->fActualMarginX = vecMargin.x * vecDir.x;
		pObject->fActualMarginY = vecMargin.y * vecDir.y;
		pObject->fActualMarginZ = vecMargin.z * vecDir.z;

		// After used size above, rotate against obj to get current body height
		D3DXVec3TransformCoord ( &vecBodySize, &vecBodySize, &matObjRot );
		pObject->fActualHeight = fabs ( (double)vecBodySize.y );

		// recalculated
		pObject->bNoNeedToRecalculateShape=true;
	}
}

DWORD ODE_GetBodyHeight ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get height of current body
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualHeight;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentX ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginX;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentY ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginY;
	return *(DWORD*)&fValue;
}

DWORD ODE_GetBodyAdjustmentZ ( int iID )
{
	// verify object ptr
	sODEObject* pObject = ODE_FindID ( iID );
	if ( !pObject ) return 0;

	// get adjustment to floor (due to margins caused by min obj size of 6x6x6)
	CalculateShapeOfObjAfterRotation ( pObject );
	float fValue = pObject->fActualMarginZ;
	return *(DWORD*)&fValue;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void convertMatrix ( D3DXMATRIX &t, const dReal *bMat )
{ 

   t._11 = bMat[0];
   t._12 = bMat[1];
   t._13 = bMat[2];
   t._14 = 0;
   t._21 = bMat[4];
   t._22 = bMat[5];
   t._23 = bMat[6];
   t._24 = 0;
   t._31 = bMat[8];
   t._32 = bMat[9];
   t._33 = bMat[10];
   t._34 = 0;


   t._41 = 0;
   t._42 = 0;
   t._43 = 0;
   t._44 = 0;
} 

void ODE_CreateStaticBox ( int iObject )
{
	// create ODE box
	sODEObject box     = { 0 };

	// ensure actual object exists
	sObject*   pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// obtain sizes, position and rotation
	float fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	float fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	float fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	float fXRot	 = D3DXToRadian ( pObject->position.vecRotate.x );
	float fYRot	 = D3DXToRadian ( pObject->position.vecRotate.y );
	float fZRot	 = D3DXToRadian ( pObject->position.vecRotate.z );
	//if ( fXSize < 5.0f ) fXSize = 5.0f;
	//if ( fYSize < 5.0f ) fYSize = 5.0f;
	//if ( fZSize < 5.0f ) fZSize = 5.0f;

	// create box geom and position ODE object geom
	box.geom = dCreateBox ( g_ODESpace, fXSize, fYSize, fZSize );
	dGeomSetPosition  ( box.geom, fXPos, fYPos, fZPos );

	// convey rotation to geom too
	dMatrix3 matrix;
	dRFromEulerAngles ( matrix, fXRot, fYRot, fZRot );
	dGeomSetRotation  ( box.geom, matrix );

	// assign actualobjectID to ODEobject for reference
	box.bEnabled = true;
	box.iID = iObject;

	// add this ODE object to list
	g_ODEStaticObjectList.push_back ( box );
}

void ODE_CreateStaticTriangleMesh ( int iObject )
{
	// get object 
	sObject* pObject = g_pfnGetObject ( iObject );

	// only accept Vertex only meshes (not indexed as they are not big enough in 16bit)
	DWORD dwVertexCount = 0;
	for ( int i = 0; i < pObject->iMeshCount; i++ )
		dwVertexCount += pObject->ppMeshList [ i ]->dwVertexCount;

	float*			vertices		= new float [ dwVertexCount * 3 ];
	int				vertexCount		= dwVertexCount;
	int*			triangles		= new int [ dwVertexCount * 3 ];
	int				triangleCount	= dwVertexCount * 3;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	sOffsetMap		offsetMap;
	dBodyID			body;
	dGeomID			geom;

	//paul missing "int i" declaration
	// collect triangles
	int iTriPos = 0;
	for ( int i = 0; i < pObject->iMeshCount; i++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ i ];

		// transform FVF with frame matrix
		g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
		TransformVertices ( pObject, pMesh, pObject->ppFrameList [ i ]->matTransformed );

		// go through verts of mesh
		BYTE* pVertex = pMesh->pVertexData;
		for ( int k = 0; k < (int)pMesh->dwVertexCount; k+=3 )
		{
			// get vertex data
			D3DXVECTOR3 vec0 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+0) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+0) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+0) ) ) );
			D3DXVECTOR3 vec1 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+1) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+1) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+1) ) ) );
			D3DXVECTOR3 vec2 = D3DXVECTOR3 (		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+2) ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * (k+2) ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * (k+2) ) ) );

			vertices [ iPos++ ] = vec0.x;
			vertices [ iPos++ ] = vec0.y;
			vertices [ iPos++ ] = vec0.z;
			vertices [ iPos++ ] = vec1.x;
			vertices [ iPos++ ] = vec1.y;
			vertices [ iPos++ ] = vec1.z;
			vertices [ iPos++ ] = vec2.x;
			vertices [ iPos++ ] = vec2.y;
			vertices [ iPos++ ] = vec2.z;

			// test and see poly size
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+0) ) ) = vec0;
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+1) ) ) = vec1;
			*(D3DXVECTOR3*)( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * (k+2) ) ) = vec2;
		}

		//paul missing "int k" declaration
		// simple incremental indexes
		for ( int k = 0; k < (int)pMesh->dwVertexCount; k++ )
		{
			triangles [ iTriPos ] = iTriPos;
			iTriPos++;
		}
	
		// ODE controls transform now
		pObject->bDisableTransform = true;
	}

	// construct ODE static body
	body = dBodyCreate ( g_ODEWorld );
	triangleData = dGeomTriMeshDataCreate ( );
	dGeomTriMeshDataBuildSingle (	triangleData,
									// vertices
									&vertices [ 0 ],
									3 * sizeof ( float ),
									dwVertexCount,
									// faces
									( int* ) &triangles [ 0 ],
									dwVertexCount,
									3 * sizeof ( int )
									);

	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );
}

void ODE_CreateDynamicSphere ( int iObject )
{
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	dMass		mass;

	object.body = dBodyCreate   ( g_ODEWorld );

	float fMinRadius = pObject->collision.fRadius;
	if ( fMinRadius < SMALLESTSIZE ) fMinRadius = SMALLESTSIZE;
	object.fXSize = fMinRadius * 1.0f;
	object.fYSize = fMinRadius * 1.0f;
	object.fZSize = fMinRadius * 1.0f;

	dMassSetSphere ( &mass, 5, fMinRadius * 1.0f );
	object.geom = dCreateSphere ( g_ODESpace, fMinRadius * 1.0f );
	dBodySetMass   ( object.body, &mass );
	dGeomSetBody   ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	//Rotation matrix needs to be copied, and transposed.
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	ODE_AddObject ( object, iObject, -1 );

	// lee - 170407
	pObject->position.bCustomWorldMatrix = true;
}

void ODE_SetRayCast( int iObject, int iMode )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	if ( !pObject ) return;

	dBodyDisable( pObject->body );

	pObject->bRayCast = iMode > 0;
/*	pObject->fOldXPos = dBodyGetPosition( pObject->body ) [ 0 ];
	pObject->fOldYPos = dBodyGetPosition( pObject->body ) [ 1 ];
	pObject->fOldZPos = dBodyGetPosition( pObject->body ) [ 2 ];*/
}

void ODE_SetBodyMass ( int iObject, float fMass )
{
	dMass		mass;
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->AdjustMass ( fMass );
	}
	if ( pObject )
	{
		dMassSetBox  ( &mass, 1, pObject->fXSize, pObject->fYSize, pObject->fZSize );
		dMassAdjust  ( &mass, fMass );
		dBodySetMass ( pObject->body, &mass );
	}
}

void ODE_CreateDynamicCylinder ( int iObject )
{
	// init object
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// total bound size
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	// modify radius of object to reflect new cylinder identity HACK (ODE needs z Z-axis tube)
	pObject->collision.fRadius = object.fXSize;
	if ( object.fYSize > pObject->collision.fRadius )
		pObject->collision.fRadius = object.fYSize;

	// radius and length
	int direction = 3; // z-axis
	float fRad = pObject->collision.fRadius / 2.0f;
	float flen = object.fZSize;

	// ALAS ODE CANNOT CHECK COLLISION OF CYLINDER AGAINST TRI-MESH (NOR TRIMESH vs TRIMESH) :(
//	object.geom   = dCreateCCylinder  ( g_ODESpace, fRad, flen );

	// Fake a cylinder using a composite of a sphere and a box
	float fBulgeRadius = (object.fXSize+object.fZSize)/4;
	object.geom = dCreateSphere  ( g_ODESpace, fBulgeRadius * 1.1f );
	object.geom2 = dCreateBox  ( g_ODESpace, object.fXSize*0.95f, object.fYSize, object.fZSize*0.95f );

	object.body   = dBodyCreate ( g_ODEWorld );
	dMassSetSphere  ( &mass, 1, object.fXSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	dGeomSetBody     ( object.geom, object.body );
	dGeomSetBody     ( object.geom2, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	// X10 - 131007 - PAULs rotation code from DBP to ODE
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	// add physics object
	ODE_AddObject ( object, iObject, -1 );

	// lee - 170407
	pObject->position.bCustomWorldMatrix = true;
}

void ODE_CreateDynamicBox ( int iObject )
{
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// work out size based on collision bounds
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	object.geom   = dCreateBox  ( g_ODESpace, object.fXSize, object.fYSize, object.fZSize );
	object.body   = dBodyCreate ( g_ODEWorld );

	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	//paul 190207 - ODE matrix is formed using XYZ order, DBPro matrices are ZYX order, dRFromEulerAngles is incorrectly interpreting the angles
	//Rotation matrix needs to be copied, and transposed.
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	ODE_AddObject ( object, iObject, -1 );

	pObject->position.bCustomWorldMatrix = true;
}

void ODE_CreateDynamicBox ( int iObject, float fXSize, float fYSize, float fZSize )
{
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;

	// work out size based on collision bounds
	object.fXSize = fXSize;
	object.fYSize = fYSize;
	object.fZSize = fZSize;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	object.geom   = dCreateBox  ( g_ODESpace, object.fXSize, object.fYSize, object.fZSize );
	object.body   = dBodyCreate ( g_ODEWorld );

	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );
	
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	//paul 190207 - ODE matrix is formed using XYZ order, DBPro matrices are ZYX order, dRFromEulerAngles is incorrectly interpreting the angles
	//Rotation matrix needs to be copied, and transposed.
	dMatrix3 matrix; 
	matrix[0] = pObject->position.matRotation._11;
	matrix[1] = pObject->position.matRotation._21;
	matrix[2] = pObject->position.matRotation._31;
	matrix[3] = REAL(0.0);
	matrix[4] = pObject->position.matRotation._12;
	matrix[5] = pObject->position.matRotation._22;
	matrix[6] = pObject->position.matRotation._32;
	matrix[7] = REAL(0.0);
	matrix[8] = pObject->position.matRotation._13;
	matrix[9] = pObject->position.matRotation._23;
	matrix[10] = pObject->position.matRotation._33;
	matrix[11] = REAL(0.0);
	dBodySetRotation ( object.body, matrix );

	ODE_AddObject ( object, iObject, -1 );
	pObject->position.bCustomWorldMatrix = true;

	/* above copied from main box create, but replacing size values (previosly did not account for ODE rotation)
	dMass		mass;
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	if ( !pObject ) return;
	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( fXSize<SMALLESTSIZE ) fXSize=SMALLESTSIZE;
	if ( fZSize<SMALLESTSIZE ) fZSize=SMALLESTSIZE;
	if ( fYSize<SMALLESTVERTSIZE ) fYSize=SMALLESTVERTSIZE;
	object.geom = dCreateBox  ( g_ODESpace, fXSize, fYSize, fZSize );
	object.body = dBodyCreate ( g_ODEWorld );
	dMassSetBox  ( &mass, 1, fXSize, fYSize, fZSize );
	dMassAdjust  ( &mass, 1 );
	dBodySetMass ( object.body, &mass );	
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );
	ODE_AddObject ( object.body, object.geom, iObject, -1 );
	*/
}

void ODE_CreateHingeJoint ( int id, int iObj1, int iObj2, float axisX, float axisY, float axisZ, float posX, float posY, float posZ )
{
	sODEObject *pODEObject1 = ODE_FindID( iObj1 );
	sODEObject *pODEObject2 = ODE_FindID( iObj2 );

	if ( !pODEObject1 )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - Object (%d) has not been created in ODE", iObj1 );
		HandleError( errStr );
	}

	if ( !pODEObject2 )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - Object (%d) has not been created in ODE", iObj2 );
		HandleError( errStr );
	}

	if ( sODEJoint::GetJoint( id ) )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - hinge id (%d) already exists", id );
		HandleError( errStr );
	}

	sODEJoint::AddHingeJoint( id, pODEObject1->body, pODEObject2->body, axisX, axisY, axisZ, posX, posY, posZ );
}

void ODE_CreateBallJoint ( int id, int iObj1, int iObj2, float posX, float posY, float posZ )
{
	sODEObject *pODEObject1 = ODE_FindID( iObj1 );
	sODEObject *pODEObject2 = ODE_FindID( iObj2 );

	if ( !pODEObject1 )
	{
		sprintf_s( errStr, 256, "Cannot create ball joint - Object (%d) has not been created in ODE", iObj1 );
		HandleError( errStr );
	}

	if ( !pODEObject2 )
	{
		sprintf_s( errStr, 256, "Cannot create ball joint - Object (%d) has not been created in ODE", iObj2 );
		HandleError( errStr );
	}

	if ( sODEJoint::GetJoint( id ) )
	{
		sprintf_s( errStr, 256, "Cannot create hinge - hinge id (%d) already exists", id );
		HandleError( errStr );
	}

	sODEJoint::AddBallJoint( id, pODEObject1->body, pODEObject2->body, posX, posY, posZ );
}

void ODE_SetHingeJointLimit ( int id, float low, float high )
{
	sODEJoint *pJoint = sODEJoint::GetJoint( id );
	if ( !pJoint )
	{
		sprintf_s( errStr, 256, "Cannot set hinge limit - hinge id (%d) does not exist", id );
		HandleError( errStr );
	}

	pJoint->SetHingeLimit( low, high );
}

void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale, int iDebugObject )
{
	sObject *pObject = g_pfnGetObject ( iObjID );

	if ( !pObject ) 
	{
		sprintf_s( errStr, 256, "Cannot create ragdoll - object (%d) does not exist", iObjID );
		HandleError( errStr );
	}

	if ( !pObject->ppMeshList )
	{
		sprintf_s( errStr, 256, "Cannot create ragdoll - object (%d) has no mesh", iObjID );
		HandleError( errStr );
	}

	// lee - 040707 - too many ragdolls overload the simulation and cause.
	// hmm, lets find out if its ragdoll joints or limbs in simulation

	// lee - 030307 - the rag doll uses the matCombined data of the object, but we
	// must update this at the point of rag doll conversion or the physical = 
	// achieved by set object frame with the update bounds parameter (,1)
	
	// now create the ragdoll
	sODERagdoll::AddRagdoll( iObjID, pObject, fScale, iDebugObject );

	// and now wipe out the objects local matrix as it is not inside the ragdoll limbs
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	pObject->position.vecPosition = D3DXVECTOR3(0,0,0);

	// 201007 - ensure no rotation is associated with object holding ragdoll (ragdoll uses custom matrix for each bone)
	for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
	{
		// find frame within object
		sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
		if ( pFrame ) D3DXMatrixIdentity ( &pFrame->matAbsoluteWorld );
	}

	// and ensure object is set as ragdoll (this skips the matCombined calculation normally done to animating/animframe objects)
	// 250607 - this also skips parts of the regular object processing for faster performance
	pObject->position.bCustomBoneMatrix = true;

	// U75 - 210210 - ensure cannot be culled
	pObject->collision.fRadius = 0;
	pObject->collision.fScaledRadius = 0;
	pObject->collision.fScaledLargestRadius = 0;

	// trigger an anim frame refresh (needed paul?)
	pObject->bAnimUpdateOnce = true;
}

void ODE_CreateRagdollFromBoneModel( int iObjID, float fScale )
{
	ODE_CreateRagdollFromBoneModel( iObjID, fScale, 0 );
}

void ODE_DestroyRagdoll( int iRagdollID )
{
	// X10 - 051007 - silent fail if no object
	sObject *pObject = g_pfnGetObject ( iRagdollID );
	if ( pObject==NULL )
		return;

	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iRagdollID );
	if ( pRagdoll==NULL )
		return;

	// ensure object restored from ragdoll (this reincludes the matCombined calculation normally done to animating/animframe objects)
	pObject->position.bCustomBoneMatrix = false;

	// reset matrices to wipe out old ragdoll influemces
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	D3DXMatrixIdentity ( &pObject->position.matTranslation );
	D3DXMatrixIdentity ( &pObject->position.matObjectNoTran );

	// and now wipe out the objects local matrix as it is not inside the ragdoll limbs
	D3DXMatrixIdentity ( &pObject->position.matWorld );
	pObject->position.vecPosition = D3DXVECTOR3(0,0,0);

	// remove from physics simulation
	pRagdoll->SetActive ( 0 );

	// and remove ragdoll
	sODERagdoll::RemoveRagdoll( pRagdoll );
}

void ODE_SetLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetLinearVelocity ( fX, fY, fZ );
	}
	if ( pObject )
	{
		dBodyEnable ( pObject->body );
		dBodySetLinearVel ( pObject->body, fX, fY, fZ );
	}
}

void ODE_SetAngularVelocity ( int iObject, float fX, float fY, float fZ )
{
	sODEObject* pObject = ODE_FindID ( iObject );
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetAngularVelocity ( fX, fY, fZ );
	}
	if ( pObject )
	{
		dBodyEnable ( pObject->body );
		dBodySetAngularVel ( pObject->body, fX, fY, fZ );
	}
}

void ODE_AddBodyForce ( int iObject, float fX, float fY, float fZ, float fPX, float fPY, float fPZ )
{
	sODEObject* pObject = ODE_FindID ( iObject );

	if ( !pObject )
		return;

	// dbpro.fpsc scaled for V1
	dBodyEnable ( pObject->body );
	dBodyAddForceAtPos ( pObject->body, fX*10000, fY*10000, fZ*10000, fPX, fPY, fPZ );
}

void ODE_SetGravity ( int iObject, int iMode )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetGravity ( iMode );
	}
	if ( pObject )
	{
		dBodySetGravityMode ( pObject->body, iMode );
		dBodyEnable ( pObject->body );
	}
}

void ODE_SetActive ( int iObject, int iMode )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
		pRagdoll->SetActive ( iMode );
	}
	if ( pObject )
	{
		if ( iMode==0 )
			dBodyDisable ( pObject->body );
		else
			dBodyEnable ( pObject->body );
	}
}

void ODE_SetEnabled	( int iObject, int iEnabled )
{
	// get object ptr
	sODEObject* pObject = ODE_FindID ( iObject );

	// is the object a ragdoll
	sODERagdoll *pRagdoll = sODERagdoll::GetRagdoll( iObject );
	if ( pRagdoll )
	{
	}
	if ( pObject )
	{
		if ( iEnabled==0 )
			pObject->bEnabled = false;
		else
			pObject->bEnabled = true;
	}
}

void TransformVertices ( sObject* pObject, sMesh* pMesh, D3DXMATRIX matrix )
{
	sOffsetMap	offsetMap;
	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	for ( int iVertex = 0; iVertex < (int)pMesh->dwVertexCount; iVertex++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											  );

		D3DXVec3TransformCoord ( &vecPosition, &vecPosition, &matrix );

		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
		*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
		*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
	}
}

void ODE_CreateDynamicTriangleMesh	( int iObject )
{
	sODEObject	object  = { 0 };
	sObject*	pObject = g_pfnGetObject ( iObject );
	dMass		mass;

	// work out size based on collision bounds
	object.fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
	object.fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
	object.fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;

	// by default, a dbpro box cannot be smaller than X (or it escapes geom)
	if ( object.fXSize<SMALLESTSIZE ) object.fXSize=SMALLESTSIZE;
	if ( object.fZSize<SMALLESTSIZE ) object.fZSize=SMALLESTSIZE;
	if ( object.fYSize<SMALLESTVERTSIZE ) object.fYSize=SMALLESTVERTSIZE;

	// GEOM creation
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];
	sOffsetMap		offsetMap;
	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );
	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;
		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);
		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ ) triangles [ i ] = pMesh->pIndices [ i ];
	triangleData = dGeomTriMeshDataCreate ( );
	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	object.geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );

	object.body   = dBodyCreate ( g_ODEWorld );
	dMassSetBox  ( &mass, 1, object.fXSize, object.fYSize, object.fZSize );
	dMassAdjust  ( &mass, 400 );
	dBodySetMass ( object.body, &mass );
	dGeomSetBody     ( object.geom, object.body );
	dBodySetPosition ( object.body, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	ODE_AddObject ( object, iObject, -1 );
	pObject->position.bCustomWorldMatrix = true;
}

bool GetODETriangleMesh ( sODECar* pCar, sObject* pObject, sMesh* pMesh )
{
	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	dReal			sides [ 3 ];
	sOffsetMap		offsetMap;

	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	TransformVertices ( pObject, pMesh, pObject->position.matWorld );

	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);

		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul - missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ )
		triangles [ i ] = pMesh->pIndices [ i ];

	pCar->bodies [ 0 ] = dBodyCreate ( g_ODEWorld );

	triangleData = dGeomTriMeshDataCreate ( );

	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	
	pCar->geom [ 0 ] = dCreateTriMesh ( 0, triangleData, 0, 0, 0 );

	sides [ 0 ] = 14;
	sides [ 1 ] = 5;
	sides [ 2 ] = 14;

	dGeomSetData ( pCar->geom [ 0 ], triangleData );
	dMassSetBox  ( &m, pCar->fMass, sides [ 0 ], sides [ 1 ], sides [ 2 ] );
	dGeomSetBody ( pCar->geom [ 0 ], pCar->bodies [ 0 ] );

	return true;
}

void ODE_CreateCar ( int iCar )
{
	memset ( &g_ODECarList [ iCar ], 0, sizeof ( sODECar ) );
}

void ODE_SetCarPosition ( int iCar, float fX, float fY, float fZ )
{
	g_ODECarList [ iCar ].fX = fX;
	g_ODECarList [ iCar ].fY = fY;
	g_ODECarList [ iCar ].fZ = fZ;
}

void ODE_SetCarMass ( int iCar, float fMass )
{
	g_ODECarList [ iCar ].fMass = fMass;
}

void ODE_SetCarMesh ( int iCar, int iObject, int iMode )
{
	sObject*	pObject = g_pfnGetObject ( iObject );
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];

	g_ODECarList [ iCar ].iID [ 0 ] = iObject;

	switch ( iMode )
	{
		case 0:
		{
			// sphere
		}
		break;

		case 1:
		{
			// triangle
			if ( ! GetODETriangleMesh ( &g_ODECarList [ iCar ], pObject, pMesh ) )
				return;
		}
		break;

		case 2:
		{
			// box
		}
		break;
	}
}

void ODE_SetCarWheel ( int iCar, int iWheel, int iObject, float fRadius )
{
	g_ODECarList [ iCar ].bodies [ iWheel + 1 ] = dBodyCreate ( g_ODEWorld );
	g_ODECarList [ iCar ].geom   [ iWheel + 1 ] = dCreateSphere ( 0, fRadius );
	g_ODECarList [ iCar ].iID    [ iWheel + 1 ] = iObject;
	g_ODECarList [ iCar ].fWheelRadius			= fRadius;

	dGeomSetBody ( g_ODECarList [ iCar ].geom [ iWheel + 1 ], g_ODECarList [ iCar ].bodies [ iWheel + 1 ] );

	sObject* pObject = g_pfnGetObject ( iObject );
	TransformVertices ( pObject, pObject->ppMeshList [ 0 ], pObject->position.matWorld );

}

void ODE_SetCarWheelMass ( int iCar, int iWheel, float fMass )
{
	dMass mass;

	dMassSetSphere ( &mass, 1, g_ODECarList [ iCar ].fWheelRadius );
	dMassAdjust	   ( &mass, fMass );
}

void ODE_SetCarWheelRotation ( int iCar, int iWheel, float fX, float fY, float fZ )
{
	dMatrix3 r;

	dRFromEulerAngles ( r, fX, fY, fZ );
	dBodySetRotation  ( g_ODECarList [ iCar ].bodies [ iWheel + 1 ], r );
}

void ODE_SetCarWheelPosition ( int iCar, int iWheel, float fX, float fY, float fZ )
{
	dBodySetPosition ( g_ODECarList [ iCar ].bodies [ iWheel + 1 ], fX, fY, fZ );
}

void ODE_SetCarDefaultJoints ( int iCar )
{
	for ( int i = 0; i < 4; ++i )
	{
		g_ODECarList [ iCar ].joints [ i ] = dJointCreateHinge2 ( g_ODEWorld, 0 );
		dJointAttach ( g_ODECarList [ iCar ].joints [ i ], g_ODECarList [ iCar ].bodies [ 0 ], g_ODECarList [ iCar ].bodies [ i + 1 ] );
		
		const dReal* const wPos = dBodyGetPosition ( g_ODECarList [ iCar ].bodies [ i + 1 ] );

		dJointSetHinge2Anchor ( g_ODECarList [ iCar ].joints [ i ], wPos [ 0 ], wPos [ 1 ], wPos [ 2 ] );
		dJointSetHinge2Axis1  ( g_ODECarList [ iCar ].joints [ i ], 0, 1, 0 );
		dJointSetHinge2Axis2  ( g_ODECarList [ iCar ].joints [ i ], 0, 0, ( ( i % 2 ) == 0 ) ? -1 : 1 );
		
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamLoStop, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamHiStop, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamFMax, 50 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamVel2, 0 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamFMax2, 80 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamSuspensionERP, 0.25 );
		dJointSetHinge2Param  ( g_ODECarList [ iCar ].joints [ i ], dParamSuspensionCFM, 0.004f * 50 );
	}
}


void ODE_UpdateCar ( int iCar )
{
	g_ODECarList [ iCar ].space = dHashSpaceCreate ( g_ODESpace );

	dSpaceSetCleanup ( g_ODECarList [ iCar ].space, 0 );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 0 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 1 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 2 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 3 ] );
	dSpaceAdd ( g_ODECarList [ iCar ].space, g_ODECarList [ iCar ].geom [ 4 ] );

	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 0 ], g_ODECarList [ iCar ].geom [ 0 ], g_ODECarList [ iCar ].iID [ 0 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 1 ], g_ODECarList [ iCar ].geom [ 1 ], g_ODECarList [ iCar ].iID [ 1 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 2 ], g_ODECarList [ iCar ].geom [ 2 ], g_ODECarList [ iCar ].iID [ 2 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 3 ], g_ODECarList [ iCar ].geom [ 3 ], g_ODECarList [ iCar ].iID [ 3 ], -1 );
	ODE_AddObject ( g_ODECarList [ iCar ].bodies [ 4 ], g_ODECarList [ iCar ].geom [ 4 ], g_ODECarList [ iCar ].iID [ 4 ], -1 );
}

void ODE_CreateStaticBox ( int iObject, float fXSize, float fYSize, float fZSize )
{
	sODEObject box     = { 0 };
	sObject*   pObject = g_pfnGetObject ( iObject );

	if ( !pObject )
		return;

	float fXPos	 = pObject->position.vecPosition.x;
	float fYPos	 = pObject->position.vecPosition.y;
	float fZPos	 = pObject->position.vecPosition.z;
	
	if ( fXSize == 0.0f ) fXSize = 0.01f;
	if ( fYSize == 0.0f ) fYSize = 0.01f;
	if ( fZSize == 0.0f ) fZSize = 0.01f;

	

	box.geom = dCreateBox ( g_ODESpace, fXSize, fYSize, fZSize );

	dGeomSetPosition ( box.geom, fXPos, fYPos, fZPos );	
}


void ODE_CreateStaticTriangleMesh ( int iObject, float fX, float fY, float fZ )
{
	sObject*	pObject = g_pfnGetObject ( iObject );
	sMesh*		pMesh   = pObject->ppMeshList [ 0 ];

	float*			vertices		= new float [ pMesh->dwVertexCount * 3 ];
	int				vertexCount		= pMesh->dwVertexCount;
	int*			triangles		= new int [ pMesh->dwIndexCount ];
	int				triangleCount	= pMesh->dwIndexCount;
	int				iPos			= 0;
	dTriMeshDataID	triangleData	= 0;
	dMass			m;
	sOffsetMap		offsetMap;

	dBodyID		body;
	dGeomID		geom;

	g_pfnGetFVFOffsetMap ( pMesh, &offsetMap );

	TransformVertices ( pObject, pMesh, pObject->position.matWorld );

	for ( int i = 0; i < vertexCount; i++ )
	{
		BYTE* pVertex = pMesh->pVertexData;

		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);

		vertices [ iPos++ ] = vecPosition.x;
		vertices [ iPos++ ] = vecPosition.y;
		vertices [ iPos++ ] = vecPosition.z;
	}

	//paul - missing "int i" declaration
	for ( int i = 0; i < triangleCount; i++ )
		triangles [ i ] = pMesh->pIndices [ i ];

	body = dBodyCreate ( g_ODEWorld );

	triangleData = dGeomTriMeshDataCreate ( );

	dGeomTriMeshDataBuildSingle ( 
									triangleData,
									&vertices [ 0 ],
									3 * sizeof ( float ),
									pMesh->dwVertexCount,
									( int* ) &triangles [ 0 ],
									pMesh->dwIndexCount,
									3 * sizeof ( int )
								);
	
	geom = dCreateTriMesh ( g_ODESpace, triangleData, 0, 0, 0 );

	pObject->bDisableTransform = true;
}

