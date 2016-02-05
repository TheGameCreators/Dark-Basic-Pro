
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodymesh.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*

	PHY MAKE RIGID BODY STATIC MESH%L%?dbPhyMakeRigidBodyStaticMesh@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC MESHES%L%?dbPhyMakeRigidBodyStaticMeshes@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC MESH%LL%?dbPhyMakeRigidBodyStaticMesh@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC MESHES%LL%?dbPhyMakeRigidBodyStaticMeshes@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESH%L%?dbPhyMakeRigidBodyDynamicMesh@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESHES%L%?dbPhyMakeRigidBodyDynamicMeshes@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESH%LL%?dbPhyMakeRigidBodyDynamicMesh@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESHES%LL%?dbPhyMakeRigidBodyDynamicMeshes@@YAXHH@Z%

	PHY MAKE RIGID BODY STATIC MESH%LS%?dbPhyMakeRigidBodyStaticMesh@@YAXHPAD@Z%
	PHY MAKE RIGID BODY STATIC MESHES%LS%?dbPhyMakeRigidBodyStaticMeshes@@YAXHPAD@Z%
	PHY MAKE RIGID BODY STATIC MESH%LSL%?dbPhyMakeRigidBodyStaticMesh@@YAXHPADH@Z%
	PHY MAKE RIGID BODY STATIC MESHES%LSL%?dbPhyMakeRigidBodyStaticMeshes@@YAXHPADH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESH%LS%?dbPhyMakeRigidBodyDynamicMesh@@YAXHPAD@Z%
	PHY MAKE RIGID BODY DYNAMIC MESHES%LS%?dbPhyMakeRigidBodyDynamicMeshes@@YAXHPAD@Z%
	PHY MAKE RIGID BODY DYNAMIC MESH%LSL%?dbPhyMakeRigidBodyDynamicMesh@@YAXHPADH@Z%
	PHY MAKE RIGID BODY DYNAMIC MESHES%LSL%?dbPhyMakeRigidBodyDynamicMeshes@@YAXHPADH@Z%

	PHY LOAD RIGID BODY STATIC MESH%LS%?dbPhyLoadRigidBodyStaticMesh@@YAXHPAD@Z%
	PHY LOAD RIGID BODY STATIC MESHES%LS%?dbPhyLoadRigidBodyStaticMeshes@@YAXHPAD@Z%
	PHY LOAD RIGID BODY STATIC MESH%LSL%?dbPhyLoadRigidBodyStaticMesh@@YAXHPADH@Z%
	PHY LOAD RIGID BODY STATIC MESHES%LSL%?dbPhyLoadRigidBodyStaticMeshes@@YAXHPADH@Z%
	PHY LOAD RIGID BODY DYNAMIC MESH%LS%?dbPhyLoadRigidBodyDynamicMesh@@YAXHPAD@Z%
	PHY LOAD RIGID BODY DYNAMIC MESHES%LS%?dbPhyLoadRigidBodyDynamicMeshes@@YAXHPAD@Z%
	PHY LOAD RIGID BODY DYNAMIC MESH%LSL%?dbPhyLoadRigidBodyDynamicMesh@@YAXHPADH@Z%
	PHY LOAD RIGID BODY DYNAMIC MESHES%LSL%?dbPhyLoadRigidBodyDynamicMeshes@@YAXHPADH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticMesh ( int iObject )
{
	//dbPhyMakeRigidBodyStaticMesh ( iObject, "", -1 );
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyMesh, PhysX::eStatic, -1 );
}

void dbPhyMakeRigidBodyStaticMeshes ( int iObject )
{

}

void dbPhyMakeRigidBodyStaticMesh ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyMesh, PhysX::eStatic, iMaterial );
}

void dbPhyMakeRigidBodyStaticMeshes ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicMesh ( int iObject )
{
	dbPhyMakeRigidBodyDynamicMesh ( iObject, "", -1 );
}

void dbPhyMakeRigidBodyDynamicMeshes ( int iObject )
{

}

void dbPhyMakeRigidBodyDynamicMesh ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyPMAP, PhysX::eDynamic, iMaterial, 1 );
}

void dbPhyMakeRigidBodyDynamicMeshes ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyStaticMesh ( int iObject, char* szFile )
{
	dbPhyMakeRigidBodyStaticMesh ( iObject, szFile, -1 );
}

void dbPhyMakeRigidBodyStaticMeshes ( int iObject, char* szFile )
{

}

void dbPhyMakeRigidBodyStaticMesh ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyMesh, PhysX::eStatic, iMaterial, 0, szFile );
}

void dbPhyMakeRigidBodyStaticMeshes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicMesh ( int iObject, char* szFile )
{
	dbPhyMakeRigidBodyDynamicMesh ( iObject, szFile, -1 );
}

void dbPhyMakeRigidBodyDynamicMeshes ( int iObject, char* szFile )
{

}

void dbPhyMakeRigidBodyDynamicMesh ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyPMAP, PhysX::eDynamic, iMaterial, 0, szFile );
}

void dbPhyMakeRigidBodyDynamicMeshes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyStaticMesh ( int iObject, char* szFile )
{
	dbPhyLoadRigidBodyStaticMesh ( iObject, szFile, -1 );
}

void dbPhyLoadRigidBodyStaticMeshes	( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyStaticMesh ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyMesh, PhysX::eStatic, iMaterial, 1, szFile );
}

void dbPhyLoadRigidBodyStaticMeshes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyDynamicMesh ( int iObject, char* szFile )
{
	dbPhyLoadRigidBodyDynamicMesh ( iObject, szFile, -1 );
}

void dbPhyLoadRigidBodyDynamicMeshes ( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyDynamicMesh ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyPMAP, PhysX::eDynamic, iMaterial, 1, szFile );
}

void dbPhyLoadRigidBodyDynamicMeshes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhySetupHardwareMesh ( NxActor* pActor )
{
	if ( !pActor )
		return;

	NxShape* const* shapes = pActor->getShapes ( );

	int i = pActor->getNbShapes ( );

	for ( unsigned int s = 0; s < pActor->getNbShapes ( ); ++s )
	{
		NxShape* shape = ( NxShape* ) ( shapes [ s ] );

		if ( shape->is ( NX_SHAPE_MESH ) )
		{
			NxTriangleMeshShape*	triMeshShape = ( NxTriangleMeshShape* )shape;
			NxTriangleMesh&			triMesh	     = triMeshShape->getTriangleMesh ( );
			int						pageCnt      = triMesh.getPageCount ( );

			for ( int t = 0; t < pageCnt; ++t )
			{
				if ( !triMeshShape->isPageInstanceMapped ( t ) )
				{
					bool ok = triMeshShape->mapPageInstance ( t );
					assert ( ok );
				}
			}
		}
	}
}

D3DXMATRIX dbPhyGetMatrix ( int iObject, int iLimb )
{
	sObject* pObject = PhysX::GetObject ( iObject );
	return pObject->position.matWorld * pObject->ppFrameList [ iLimb ]->matCombined;
}

void dbPhyMakeRigidBodyMesh ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile, PhysX::sPhysXObject* pPhysXObject )
{
	if ( iSaveMode == 0 )
	{
		// create and save
		for ( int iMesh = 0; iMesh < pObject->iFrameCount; iMesh++ )
		{
			if ( !pObject->ppFrameList [ iMesh ]->pMesh )
				continue;

			int iLimb = iMesh;
			int iObject = pPhysXObject->iID;

			D3DXMATRIX matWorld ;//= dbPhyGetMatrix ( iObject, iMesh );

			D3DXMATRIX a = pObject->position.matWorld;
			D3DXMATRIX b = pObject->ppFrameList [ iMesh ]->matCombined;
			D3DXMATRIX c = b * a;

			//matWorld = pObject->position.matWorld * pObject->ppFrameList [ iMesh ]->matCombined;
			matWorld = c;

			NxVec3* pVertices		= new NxVec3 [ pObject->ppFrameList [ iMesh ]->pMesh->dwVertexCount ];
			int		iVertexCount	= pObject->ppFrameList [ iMesh ]->pMesh->dwVertexCount;

			int*	iTriangles		= NULL;
			int		iTriangleCount	= 0;

			if ( pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount != 0 )
			{
				iTriangles		= new int [ pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount ];
				iTriangleCount	= pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount;
			}

			sOffsetMap offsetMap;
			PhysX::GetFVFOffsetMap ( pObject->ppFrameList [ iMesh ]->pMesh, &offsetMap );


			for ( int i = 0; i < iVertexCount; i++ )
			{
				BYTE* pVertex = pObject->ppFrameList [ iMesh ]->pMesh->pVertexData;
				D3DXVECTOR3 vecPos = D3DXVECTOR3 ( 
														*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
														*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
												);

				D3DXVec3TransformCoord ( &vecPos, &vecPos, &matWorld );

				pVertices [ i ].x = vecPos.x;
				pVertices [ i ].y = vecPos.y;
				pVertices [ i ].z = vecPos.z;
			}

			for ( int i = 0; i < iTriangleCount; i++ )
			{
				iTriangles [ i ] = pObject->ppFrameList [ iMesh ]->pMesh->pIndices [ i ];
			}
			
			NxTriangleMeshDesc levelDesc;
			levelDesc.numVertices			= iVertexCount;
			levelDesc.pointStrideBytes		= sizeof ( NxVec3 );
			levelDesc.triangleStrideBytes   = 3 * sizeof ( int );
			levelDesc.points				= pVertices;

			if ( pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount != 0 )
			{
				levelDesc.numTriangles			= iTriangleCount / 3;
				levelDesc.triangles				= iTriangles;
			}
			
			if ( PhysX::iSimulationType == 0 )
				levelDesc.flags				= NX_CF_COMPUTE_CONVEX;
			else
				levelDesc.flags				= NX_MF_HARDWARE_MESH | NX_CF_COMPUTE_CONVEX;
			
			NxTriangleMeshShapeDesc* pLevelShapeDesc = new NxTriangleMeshShapeDesc;
			MemoryWriteBuffer buf;

			char szFileName [ 256 ] = "";
			bool bDelete = false;

			if ( strlen ( szFile ) > 0 )
				strcpy ( szFileName, szFile );
			else
			{
				strcpy ( szFileName, "temp.mesh" );
				bDelete = true;
			}
			
			bool bStatus = false;
			
			bStatus = NxCookTriangleMesh ( levelDesc, UserStream ( szFileName, false ) );
			
			if ( bStatus )
			{
				pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream ( szFileName, true ) );

				if ( pMaterial )
					pLevelShapeDesc->materialIndex = pMaterial->matIndex;
				
				//pActorDesc->shapes.pushBack ( pLevelShapeDesc );

				NxActorDesc actorDesc;
				actorDesc.shapes.pushBack ( pLevelShapeDesc );

				actorDesc.globalPose.t = NxVec3 ( 0.0f, 0.0f, 0.0f );

				NxActor* pActor = PhysX::pScene->createActor ( actorDesc );

				if ( PhysX::iSimulationType == 1 )
					dbPhySetupHardwareMesh ( pActor );

				pActor->userData = ( void* ) pPhysXObject;

				pPhysXObject->type = PhysX::eRigidBodyMesh;

				pPhysXObject->pActorList.push_back ( pActor );

				if ( bDelete )
					DeleteFile ( szFileName );
			}
		}
	}

	if ( iSaveMode == 1 )
	{
		NxActorDesc actorDesc;
		NxTriangleMeshShapeDesc meshShapeDesc;

		meshShapeDesc.meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream ( szFile, true ) );
		actorDesc.shapes.pushBack ( &meshShapeDesc );

		NxActor* pActor = PhysX::pScene->createActor ( actorDesc );

		if ( PhysX::iSimulationType == 1 )
			dbPhySetupHardwareMesh ( pActor );

		pActor->userData = ( void* ) pPhysXObject;

		pPhysXObject->type = PhysX::eRigidBodyMesh;

		pPhysXObject->pActorList.push_back ( pActor );

	}
	
	return;

		/*
		NxTriangleMeshDesc terrainDesc;
		NxTriangleMeshShapeDesc terrainShapeDesc;

		//bool status = NxCookTriangleMesh(terrainDesc, UserStream(szFile, false));
		terrainShapeDesc.meshData = PhysX::pPhysicsSDK->createTriangleMesh(UserStream(szFile, true));

		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&terrainShapeDesc);
		NxActor* actor = PhysX::pScene->createActor(actorDesc);
		*/

	{
		
		return;
	}
	


	
}

void dbPhyMakeRigidBodyMesh121207 ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile, PhysX::sPhysXObject* pPhysXObject )
{
	{
		NxTriangleMeshDesc terrainDesc;
		NxTriangleMeshShapeDesc terrainShapeDesc;

		//bool status = NxCookTriangleMesh(terrainDesc, UserStream(szFile, false));
		terrainShapeDesc.meshData = PhysX::pPhysicsSDK->createTriangleMesh(UserStream(szFile, true));

		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&terrainShapeDesc);
		NxActor* actor = PhysX::pScene->createActor(actorDesc);
	
		return;
	}

	
	for ( int iMesh = 0; iMesh < pObject->iFrameCount; iMesh++ )
	{
		if ( !pObject->ppFrameList [ iMesh ]->pMesh )
			continue;

		int iLimb = iMesh;
		int iObject = pPhysXObject->iID;

		D3DXMATRIX matWorld ;//= dbPhyGetMatrix ( iObject, iMesh );

		D3DXMATRIX a = pObject->position.matWorld;
		D3DXMATRIX b = pObject->ppFrameList [ iMesh ]->matCombined;
		D3DXMATRIX c = b * a;

		//matWorld = pObject->position.matWorld * pObject->ppFrameList [ iMesh ]->matCombined;
		matWorld = c;

		NxVec3* pVertices		= new NxVec3 [ pObject->ppFrameList [ iMesh ]->pMesh->dwVertexCount ];
		int		iVertexCount	= pObject->ppFrameList [ iMesh ]->pMesh->dwVertexCount;

		int*	iTriangles		= NULL;
		int		iTriangleCount	= 0;

		if ( pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount != 0 )
		{
			iTriangles		= new int [ pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount ];
			iTriangleCount	= pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount;
		}

		sOffsetMap offsetMap;
		PhysX::GetFVFOffsetMap ( pObject->ppFrameList [ iMesh ]->pMesh, &offsetMap );


		for ( int i = 0; i < iVertexCount; i++ )
		{
			BYTE* pVertex = pObject->ppFrameList [ iMesh ]->pMesh->pVertexData;
			D3DXVECTOR3 vecPos = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * i ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * i ) )
											);

			D3DXVec3TransformCoord ( &vecPos, &vecPos, &matWorld );

			pVertices [ i ].x = vecPos.x;
			pVertices [ i ].y = vecPos.y;
			pVertices [ i ].z = vecPos.z;
		}

		for ( int i = 0; i < iTriangleCount; i++ )
		{
			iTriangles [ i ] = pObject->ppFrameList [ iMesh ]->pMesh->pIndices [ i ];
		}
		
		NxTriangleMeshDesc levelDesc;
		levelDesc.numVertices			= iVertexCount;
		levelDesc.pointStrideBytes		= sizeof ( NxVec3 );
		levelDesc.triangleStrideBytes   = 3 * sizeof ( int );
		levelDesc.points				= pVertices;

		if ( pObject->ppFrameList [ iMesh ]->pMesh->dwIndexCount != 0 )
		{
			levelDesc.numTriangles			= iTriangleCount / 3;
			levelDesc.triangles				= iTriangles;
		}
		
		if ( PhysX::iSimulationType == 0 )
			levelDesc.flags				= NX_CF_COMPUTE_CONVEX;
		else
			levelDesc.flags				= NX_MF_HARDWARE_MESH | NX_CF_COMPUTE_CONVEX;
		
		NxTriangleMeshShapeDesc* pLevelShapeDesc = new NxTriangleMeshShapeDesc;
		//NxInitCooking ( );
		MemoryWriteBuffer buf;

		char szFileName [ 256 ] = "";
		bool bDelete = false;

		if ( strcmp ( szFile, "" ) == 0 )
		{
			strcpy ( szFileName, "temp.mesh" );
			bDelete = true;
		}
		else
		{
			strcpy ( szFileName, szFile );
		}

		bool bStatus = false;
		
		if ( iSaveMode == 0 )
			bStatus = NxCookTriangleMesh(levelDesc, UserStream(szFileName, false));
		else
			bStatus = true;
			
		
		if ( bStatus )
		{
			pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream(szFileName, true) );

			if ( pMaterial )
				pLevelShapeDesc->materialIndex = pMaterial->matIndex;
			
			//pActorDesc->shapes.pushBack ( pLevelShapeDesc );

			NxActorDesc actorDesc;
			actorDesc.shapes.pushBack ( pLevelShapeDesc );

			actorDesc.globalPose.t = NxVec3 ( 0.0f, 0.0f, 0.0f );

			NxActor* pActor = PhysX::pScene->createActor ( actorDesc );

			if ( PhysX::iSimulationType == 1 )
				dbPhySetupHardwareMesh ( pActor );

			pActor->userData = ( void* ) pPhysXObject;

			pPhysXObject->type = PhysX::eRigidBodyMesh;

			pPhysXObject->pActorList.push_back ( pActor );

			if ( bDelete == true )
			{
				DeleteFile ( szFileName );
			}
		}
	}
	

}
static NxU32 getFileSize(const char* name)
{
	#ifndef SEEK_END
	#define SEEK_END 2
	#endif
	
	FILE* File = fopen(name, "rb");
	if(!File)
		return 0;

	fseek(File, 0, SEEK_END);
	NxU32 eof_ftell = ftell(File);
	fclose(File);
	return eof_ftell;
}

extern bool g_bIgnoreErrors;

void dbPhyMakeRigidBodyPMAPNewtest ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile )
{
	

	if ( iSaveMode == 0 )
	{
		for ( int iMesh = 0; iMesh < pObject->iFrameCount; iMesh++ )
		{
			if ( !pObject->ppFrameList [ iMesh ]->pMesh )
				continue;

			sMesh* pMesh = pObject->ppFrameList [ iMesh ]->pMesh;

			NxVec3* pVertices		= new NxVec3 [ pMesh->dwVertexCount ];
			int		iVertexCount	= pMesh->dwVertexCount;
			int*	iTriangles		= new int [ pMesh->dwIndexCount ];
			int		iTriangleCount	= pMesh->dwIndexCount;

			sOffsetMap offsetMap;
			PhysX::GetFVFOffsetMap ( pMesh, &offsetMap );

			
			D3DXMATRIX matWorld ;//= dbPhyGetMatrix ( iObject, iMesh );

			D3DXMATRIX a = pObject->position.matWorld;
			//D3DXMATRIX b = pObject->ppFrameList [ iMesh + 1 ]->matCombined;
			D3DXMATRIX b = pObject->ppFrameList [ iMesh ]->matCombined;
			//D3DXMATRIX c = b * a;
			D3DXMATRIX c = b;

			matWorld = c;


			for ( int i = 0; i < iVertexCount; i++ )
			{
				//pVertices [ i ].x = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * i ) );
				//pVertices [ i ].y = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * i ) );
				//pVertices [ i ].z = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * i ) );

				pVertices [ i ].x = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * i ) );
				pVertices [ i ].y = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * i ) );
				pVertices [ i ].z = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * i ) );

				//pVertices [ i ].x *= pObject->position.vecScale.x;
				//pVertices [ i ].y *= pObject->position.vecScale.y;
				//pVertices [ i ].z *= pObject->position.vecScale.z;

				D3DXVECTOR3 vecPos = D3DXVECTOR3 ( pVertices [ i ].x, pVertices [ i ].y, pVertices [ i ].z );

				D3DXVec3TransformCoord ( &vecPos, &vecPos, &matWorld );

				pVertices [ i ].x = vecPos.x;
				pVertices [ i ].y = vecPos.y;
				pVertices [ i ].z = vecPos.z;
			}

			for ( int i = 0; i < iTriangleCount; i++ )
				iTriangles [ i ] = pMesh->pIndices [ i ];
				//iTriangles [ i ] = pObject->ppMeshList [ iMesh ]->pIndices [ i ];
			
			NxTriangleMeshDesc levelDesc;
			levelDesc.numVertices			= iVertexCount;
			levelDesc.numTriangles			= iTriangleCount / 3;
			levelDesc.pointStrideBytes		= sizeof ( NxVec3 );
			levelDesc.triangleStrideBytes   = 3 * sizeof ( int );
			levelDesc.points				= pVertices;
			levelDesc.triangles				= iTriangles;
			//levelDesc.flags					= NX_CF_COMPUTE_CONVEX;
			levelDesc.flags					= 0;
			
			
			NxTriangleMeshShapeDesc* pLevelShapeDesc = new NxTriangleMeshShapeDesc;
			MemoryWriteBuffer buf;

			//bool status = NxCookTriangleMesh ( levelDesc, buf );

			bool status = NxCookTriangleMesh(levelDesc, UserStream("winkle.phy", false));
			
			if ( status )
			{
				

				//pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( MemoryReadBuffer ( buf.data ) );
				pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream("winkle.phy", true) );

				if ( pMaterial )
					pLevelShapeDesc->materialIndex = pMaterial->matIndex;
				
				
				// Initialize PMap
				NxPMap bunnyPMap;
				bunnyPMap.dataSize	= 0;
				bunnyPMap.data		= NULL;


				char szFileName [ 256 ] = "";
				bool bDelete = false;

				if ( strcmp ( szFile, "" ) == 0 )
				{
					strcpy ( szFileName, "temp.mesh" );
					bDelete = true;
				}
				else
				{
					strcpy ( szFileName, szFile );
				}

				//bool status = NxCookTriangleMesh ( levelDesc, buf );
				bool bStatus = false;
			
				//if ( iSaveMode == 0 )
				//	bStatus = true;
				

				FILE* fp = fopen(szFileName, "rb");
				//if(!fp)
				//if ( iSaveMode == 1 )	
				{
					// Not found => create PMap
					printf("please wait while precomputing pmap...\n");
					if(NxCreatePMap(bunnyPMap, *pLevelShapeDesc->meshData, 64))
					{
						// The pmap has successfully been created, save it to disk for later use
						
						char PMapFilename[512];
					//	GetTempFilePath(PMapFilename);
						strcpy(PMapFilename, szFileName);
						fp = fopen(PMapFilename, "wb+");
						if(fp)
						{
							fwrite(bunnyPMap.data, bunnyPMap.dataSize, 1, fp);
							fclose(fp);
							fp = NULL;
						}

						//assign pmap to mesh
						pLevelShapeDesc->meshData->loadPMap(bunnyPMap);

						// sdk created data => sdk deletes it
 						NxReleasePMap(bunnyPMap);
					}
					printf("...done\n");

					if ( bDelete )
						DeleteFile ( szFileName );
				}
				/*
				else if ( iSaveMode == 0 )
				{
					if ( fp == NULL )
					{
						// report an error
						PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to load dynamic mesh - file does not exist", "", 0 );
						return;
					}

					// Found pmap file
					bunnyPMap.dataSize	= getFileSize(szFileName);
					bunnyPMap.data		= new NxU8[bunnyPMap.dataSize];
					fread(bunnyPMap.data, bunnyPMap.dataSize, 1, fp);
					fclose(fp);

					//assign pmap to mesh
					pLevelShapeDesc->meshData->loadPMap(bunnyPMap);

					//we created data => we delete it
					delete bunnyPMap.data;
				}
				*/

				pActorDesc->shapes.pushBack ( pLevelShapeDesc );
			}
		}
	}

	//return;

	if ( iSaveMode == 1 )
	{
		NxActorDesc actorDesc;
		NxTriangleMeshShapeDesc meshShapeDesc;

		meshShapeDesc.meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream ( "winkle.phy", true ) );
		actorDesc.shapes.pushBack ( &meshShapeDesc );

		// Initialize PMap
		NxPMap bunnyPMap;
		bunnyPMap.dataSize	= 0;
		bunnyPMap.data		= NULL;

		FILE* fp = fopen(szFile, "rb");

		// Found pmap file
		bunnyPMap.dataSize	= getFileSize(szFile);
		bunnyPMap.data		= new NxU8[bunnyPMap.dataSize];
		fread(bunnyPMap.data, bunnyPMap.dataSize, 1, fp);
		fclose(fp);

		//assign pmap to mesh
		meshShapeDesc.meshData->loadPMap(bunnyPMap);

		//we created data => we delete it
		delete bunnyPMap.data;

		pActorDesc->shapes.pushBack ( &meshShapeDesc );

		int d = 0;
	}


	return;

	g_bIgnoreErrors = true;

	

	g_bIgnoreErrors = false;
}

void dbPhyMakeRigidBodyPMAP ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile )
{
	g_bIgnoreErrors = true;

	for ( int iMesh = 0; iMesh < pObject->iFrameCount; iMesh++ )
	{
		if ( !pObject->ppFrameList [ iMesh ]->pMesh )
			continue;

		sMesh* pMesh = pObject->ppFrameList [ iMesh ]->pMesh;
		NxVec3* pVertices		= new NxVec3 [ pMesh->dwVertexCount ];
		int		iVertexCount	= pMesh->dwVertexCount;
		int*	iTriangles		= new int [ pMesh->dwIndexCount ];
		int		iTriangleCount	= pMesh->dwIndexCount;

		sOffsetMap offsetMap;
		PhysX::GetFVFOffsetMap ( pMesh, &offsetMap );

		D3DXMATRIX matWorld;
		D3DXMATRIX a = pObject->position.matWorld;
		D3DXMATRIX b = pObject->ppFrameList [ iMesh ]->matCombined;
		D3DXMATRIX c = b;
		matWorld = c;

		for ( int i = 0; i < iVertexCount; i++ )
		{
			pVertices [ i ].x = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * i ) );
			pVertices [ i ].y = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * i ) );
			pVertices [ i ].z = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * i ) );
			D3DXVECTOR3 vecPos = D3DXVECTOR3 ( pVertices [ i ].x, pVertices [ i ].y, pVertices [ i ].z );
			D3DXVec3TransformCoord ( &vecPos, &vecPos, &matWorld );
			pVertices [ i ].x = vecPos.x;
			pVertices [ i ].y = vecPos.y;
			pVertices [ i ].z = vecPos.z;
		}

		for ( int i = 0; i < iTriangleCount; i++ )
			iTriangles [ i ] = pMesh->pIndices [ i ];
		
		NxTriangleMeshDesc levelDesc;
		levelDesc.numVertices			= iVertexCount;
		levelDesc.numTriangles			= iTriangleCount / 3;
		levelDesc.pointStrideBytes		= sizeof ( NxVec3 );
		levelDesc.triangleStrideBytes   = 3 * sizeof ( int );
		levelDesc.points				= pVertices;
		levelDesc.triangles				= iTriangles;
		levelDesc.flags					= 0;
		
		NxTriangleMeshShapeDesc* pLevelShapeDesc = new NxTriangleMeshShapeDesc;
		MemoryWriteBuffer buf;

		bool status = NxCookTriangleMesh ( levelDesc, buf );
		if ( status )
		{
			pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( MemoryReadBuffer ( buf.data ) );

			if ( pMaterial )
				pLevelShapeDesc->materialIndex = pMaterial->matIndex;
			
			// Initialize PMap
			NxPMap bunnyPMap;
			bunnyPMap.dataSize	= 0;
			bunnyPMap.data		= NULL;

			char szFileName [ 256 ] = "";
			bool bDelete = false;
			if ( strcmp ( szFile, "" ) == 0 )
			{
				strcpy ( szFileName, "temp.mesh" );
				bDelete = true;
			}
			else
			{
				strcpy ( szFileName, szFile );
			}
			bool bStatus = false;	

			FILE* fp = fopen(szFileName, "rb");
			if ( iSaveMode == 1 )	
			{
				// Not found => create PMap
				printf("please wait while precomputing pmap...\n");
				if(NxCreatePMap(bunnyPMap, *pLevelShapeDesc->meshData, 64))
				{
					// The pmap has successfully been created, save it to disk for later use
					char PMapFilename[512];
					strcpy(PMapFilename, szFileName);
					fp = fopen(PMapFilename, "wb+");
					if(fp)
					{
						fwrite(bunnyPMap.data, bunnyPMap.dataSize, 1, fp);
						fclose(fp);
						fp = NULL;
					}

					//assign pmap to mesh
					pLevelShapeDesc->meshData->loadPMap(bunnyPMap);

					// sdk created data => sdk deletes it
 					NxReleasePMap(bunnyPMap);
				}
				printf("...done\n");

				if ( bDelete )
					DeleteFile ( szFileName );
			}
			else if ( iSaveMode == 0 )
			{
				if ( fp == NULL )
				{
					// report an error
					PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to load dynamic mesh - file does not exist", "", 0 );
					return;
				}

				// Found pmap file
				bunnyPMap.dataSize	= getFileSize(szFileName);
				bunnyPMap.data		= new NxU8[bunnyPMap.dataSize];
				fread(bunnyPMap.data, bunnyPMap.dataSize, 1, fp);
				fclose(fp);

				//assign pmap to mesh
				pLevelShapeDesc->meshData->loadPMap(bunnyPMap);

				//we created data => we delete it
				delete bunnyPMap.data;
			}

			pActorDesc->shapes.pushBack ( pLevelShapeDesc );
		}
	}
	g_bIgnoreErrors = false;
}