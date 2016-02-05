
#include "SoftBody.h"
#include "SoftBodyUtility.h"
#include "globals.h"
#include "..\..\..\..\Dark Basic Pro SDK\Shared\Camera\CCameraDataC.h"

#include "objmesh.h"
#include "mysoftbody.h"
#include "cooking.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY LOAD SOFT BODY%LSSFFF%?dbPhyLoadSoftBody@@YAXHPAD0MMM@Z%

	PHY ATTACH SOFT BODY CAPSULE%LFFFFFFFFF%?dbPhyAttachSoftBodyCapsule@@YAXHMMMMMMMMM@Z%
	PHY ATTACH SOFT BODY SPHERE%LFFFFF%?dbPhyAttachSoftBodySphere@@YAXHMMMMM@Z%
	PHY ATTACH SOFT BODY BOX%LFFFFFFF%?dbPhyAttachSoftBodyBox@@YAXHMMMMMMM@Z%

	DARKSDK void dbPhyAttachSoftBodyBox     ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fDensity );

	PHY DELETE SOFT BODY%L%?dbPhyDeleteSoftBody@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

NxQuat AnglesToQuat(const NxVec3& angles);

namespace PhysX
{
	struct sSoftBodyObject
	{
		int						iID;
		NxArray < MySoftBody* >	pBodies;
		NxArray < ObjMesh*    >	pMeshes;
		NxArray < NxActor*    > pActors;
	};

	std::vector < sSoftBodyObject* >	pSoftBodyList;
}

struct sSoftBodyVertex
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
};

PhysX::sSoftBodyObject* dbPhyGetSoftBody ( int iID )
{
	for ( DWORD i = 0; i < PhysX::pSoftBodyList.size ( ); i++ )
	{
		if ( PhysX::pSoftBodyList [ i ]->iID == iID )
			return PhysX::pSoftBodyList [ i ];
	}

	return NULL;
}

void dbPhySetupSoftBody ( void )
{
	InitCooking ( );
}

ObjMesh* pMemoryCloneMesh = NULL;

void dbPhyLoadSoftBody ( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ, int iCreationMode )
{
	//iCreationMode: 0-normal, 1-clone from last loaded, 2-very spongey!

	PhysX::sSoftBodyObject*	pSoftBodyObject		= NULL;
	ObjMesh*				pMesh				= NULL;
	MySoftBody*				pSoftBody			= NULL;
	PhysX::sPhysXObject*	pPhysXObject		= new PhysX::sPhysXObject;
	NxSoftBodyDesc			softBodyDesc;

	if ( pSoftBodyObject = dbPhyGetSoftBody ( iID ) )
	{
		return;
	}

	pSoftBodyObject	= new PhysX::sSoftBodyObject;

	// load or clone
	if ( pMemoryCloneMesh==NULL || iCreationMode!=1 )
	{
		// loads new data
		pMesh			= new ObjMesh;
		pMesh->loadFromObjFile ( szMesh );

		// create copy as clone
		if ( iCreationMode==1 )
		{
			pMemoryCloneMesh = new ObjMesh;
			pMemoryCloneMesh->clone ( pMesh );
		}
	}
	else
	{
		// make clone from copy
		pMesh = new ObjMesh;
		pMesh->clone ( pMemoryCloneMesh );
	}

	pSoftBodyObject->iID = iID;
	pSoftBodyObject->pMeshes.push_back ( pMesh );

	softBodyDesc.globalPose.t					= NxVec3 ( fX, fY, fZ );
	softBodyDesc.particleRadius					= 0.36f;
	softBodyDesc.volumeStiffness				= 0.97f;
	softBodyDesc.stretchingStiffness			= 0.97f;
	softBodyDesc.friction						= 0.9f;
	softBodyDesc.collisionResponseCoefficient	= 0.0001f;
	softBodyDesc.attachmentResponseCoefficient	= 0.0001f;
	softBodyDesc.dampingCoefficient				= 0.25f;
	softBodyDesc.solverIterations				= 5;
	if ( iCreationMode==2 )
	{
		// ensure soft body behaves itself at high speeds
		softBodyDesc.flags							|= NX_SBF_SELFCOLLISION;
//		softBodyDesc.flags							|= NX_SBF_VALIDBOUNDS; // NOT HARDWARE COMPATIBLE
	}

	//TC - June 19th added hw/sw support based on simulation flag setting
	if (PhysX::iSimulationType == 1){
		softBodyDesc.flags							|= NX_SBF_HARDWARE;
	} else {
		softBodyDesc.flags							&= ~NX_SBF_HARDWARE;
	}

	// Want a more solid soft body
	softBodyDesc.flags							|= NX_SBF_VOLUME_CONSERVATION;
	softBodyDesc.flags							|= NX_SBF_COLLISION_TWOWAY;

	pSoftBody = new MySoftBody ( PhysX::pScene, softBodyDesc, szTetrahedralMesh, pMesh );

	pSoftBodyObject->pBodies.push_back ( pSoftBody );

	PhysX::CreateNewObject ( iID, "soft body", 1 );

	sObject* pObject = PhysX::GetObject ( iID );

	// leefix - 160409 - vertex only as textures can differ from vertex to vertex (OBJ)
	DWORD dwVertexCount  = pSoftBody->mSurfaceMesh->getNumTriangles ( ) * 3;
	PhysX::SetupMeshFVFData ( pObject->pFrame->pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, 0 );
	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = dwVertexCount;
	pObject->pFrame->pMesh->iDrawPrimitives  = dwVertexCount / 3;

	// leefix - 160409 - go through each OBJ triangle, and find texture ref index (different from vertex index)
	DWORD dwTriangleCount = pSoftBody->mSurfaceMesh->getNumTriangles ( );
	float* pVertex = ( float* ) pObject->pFrame->pMesh->pVertexData;
	for ( DWORD t = 0; t < dwTriangleCount; t++ )
	{
		for ( DWORD s=0; s<3; s++ )
		{
			DWORD vi = pSoftBody->mSurfaceMesh->mTriangles[t].vertexNr[s];
			DWORD ni = pSoftBody->mSurfaceMesh->mTriangles[t].normalNr[s];
			DWORD ti = pSoftBody->mSurfaceMesh->mTriangles[t].texCoordNr[s];
			*( ( float* ) pVertex + 0 ) = pSoftBody->mSurfaceMesh->mVertices[vi].x;
			*( ( float* ) pVertex + 1 ) = pSoftBody->mSurfaceMesh->mVertices[vi].y;
			*( ( float* ) pVertex + 2 ) = pSoftBody->mSurfaceMesh->mVertices[vi].z;
			*( ( float* ) pVertex + 3 ) = pSoftBody->mSurfaceMesh->mNormals[ni].x;
			*( ( float* ) pVertex + 4 ) = pSoftBody->mSurfaceMesh->mNormals[ni].y;
			*( ( float* ) pVertex + 5 ) = pSoftBody->mSurfaceMesh->mNormals[ni].z;
			*( ( float* ) pVertex + 6 ) = pSoftBody->mSurfaceMesh->mTexCoords[ti].u;
			*( ( float* ) pVertex + 7 ) = pSoftBody->mSurfaceMesh->mTexCoords[ti].v*-1;
			pVertex+=8;
		}
	}
	pObject->pFrame->pMesh->Collision.fRadius = 0.0f;

	PhysX::CalculateMeshBounds			( pObject->pFrame->pMesh );
	PhysX::SetNewObjectFinalProperties	( iID, 0.0f );
	PhysX::SetTexture					( iID, 0 );

	PhysX::pSoftBodyList.push_back ( pSoftBodyObject );

	pPhysXObject->pScene  = PhysX::pScene;
	pPhysXObject->iID     = iID;
	pPhysXObject->state	  = PhysX::eDynamic;
}

void dbPhyLoadSoftBody ( int iID, char* szTetrahedralMesh, char* szMesh, float fX, float fY, float fZ )
{
	dbPhyLoadSoftBody ( iID, szTetrahedralMesh, szMesh, fX, fY, fZ, 0 );
}

void dbPhyAttachSoftBodyCapsule ( int iID, float fX, float fY, float fZ, float fHeight, float fRadius, float fDensity, float fAngleX, float fAngleY, float fAngleZ )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;

	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iID ) ) )
		return;

	NxActor* pCapsule = CreateCapsule ( NxVec3 ( fX, fY, fZ ), fHeight, fRadius, fDensity );

	pCapsule->setGlobalOrientationQuat ( AnglesToQuat ( NxVec3 ( fAngleX, fAngleY, fAngleZ ) ) );

	pSoftBodyObject->pBodies [ 0 ]->getNxSoftBody ( )->attachToShape ( pCapsule->getShapes ( ) [ 0 ], NX_SOFTBODY_ATTACHMENT_TWOWAY );

	pSoftBodyObject->pActors.push_back ( pCapsule );
}

void dbPhyAttachSoftBodyBox ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fDensity )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;

	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iID ) ) )
		return;


	NxActor* pBox = CreateBox ( NxVec3 ( fX, fY, fZ ), NxVec3 ( fSizeX, fSizeY, fSizeZ ), fDensity );

	pSoftBodyObject->pBodies [ 0 ]->getNxSoftBody ( )->attachToShape ( pBox->getShapes ( ) [ 0 ], NX_SOFTBODY_ATTACHMENT_TWOWAY );

	pSoftBodyObject->pActors.push_back ( pBox );
}

void dbPhyAttachSoftBodySphere ( int iID, float fX, float fY, float fZ, float fRadius, float fDensity )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;

	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iID ) ) )
		return;

	NxActor* pSphere = CreateSphere ( NxVec3 ( fX, fY, fZ ), fRadius, fDensity );

	pSoftBodyObject->pBodies [ 0 ]->getNxSoftBody ( )->attachToShape ( pSphere->getShapes ( ) [ 0 ], NX_SOFTBODY_ATTACHMENT_TWOWAY );

	pSoftBodyObject->pActors.push_back ( pSphere );
}

void dbPhyDeleteSoftBody ( int iID, int iUseClone )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;

	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iID ) ) )
		return;

	for ( DWORD i = 0; i < pSoftBodyObject->pActors.size ( ); i++ )
	{
		PhysX::pScene->releaseActor ( *pSoftBodyObject->pActors [ i ] );
	}

	for ( DWORD i = 0; i < pSoftBodyObject->pMeshes.size ( ); i++ )
	{
		pSoftBodyObject->pMeshes [ i ]->clear ( );
		delete pSoftBodyObject->pMeshes [ i ];
	}

	for ( DWORD i = 0; i < pSoftBodyObject->pBodies.size ( ); i++ )
	{
		PhysX::pScene->releaseSoftBody ( *pSoftBodyObject->pBodies [ i ]->getNxSoftBody ( ) );
	}

	PhysX::DeleteObject ( iID );

	for ( DWORD dwObject = 0; dwObject < PhysX::pSoftBodyList.size ( ); dwObject++ )
	{
		if ( PhysX::pSoftBodyList [ dwObject ]->iID == iID )
		{
			// get a pointer to our object
			PhysX::sSoftBodyObject* pObject = PhysX::pSoftBodyList [ dwObject ];

			PhysX::pSoftBodyList.erase ( PhysX::pSoftBodyList.begin ( ) + dwObject );

			delete pObject;
		}
	}

}

void dbPhyDeleteSoftBody ( int iID )
{
	dbPhyDeleteSoftBody ( iID, 0 );
}

// taken from B3D on 160409 - need to get correct elements within whatever FVF is (shader on soft body)
bool GetFVFValueOffsetMap ( DWORD dwFVF, sOffsetMap* psOffsetMap )
{
	SAFE_MEMORY ( psOffsetMap );

	memset ( psOffsetMap, 0, sizeof ( sOffsetMap ) );

	int iOffset   = 0;
	int iPosition = 0;

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( dwFVF );

	if ( dwFVF & D3DFVF_XYZ )
	{
		psOffsetMap->dwX         = iOffset + 0;
		psOffsetMap->dwY         = iOffset + 1;
		psOffsetMap->dwZ         = iOffset + 2;
		iOffset += 3;
	}

	if ( dwFVF & D3DFVF_XYZRHW )
	{
		psOffsetMap->dwRWH = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_NORMAL )
	{
		psOffsetMap->dwNX        = iOffset + 0;
		psOffsetMap->dwNY        = iOffset + 1;
		psOffsetMap->dwNZ        = iOffset + 2;
		iOffset += 3;
	}

	if ( dwFVF & D3DFVF_PSIZE )
	{
		psOffsetMap->dwPointSize = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_DIFFUSE )
	{
		psOffsetMap->dwDiffuse   = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_SPECULAR )
	{
		psOffsetMap->dwSpecular   = iOffset + 0;
		iOffset += 1;
	}

	DWORD dwTexCount = 0;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX1 ) dwTexCount=1;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX2 ) dwTexCount=2;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX3 ) dwTexCount=3;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX4 ) dwTexCount=4;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX5 ) dwTexCount=5;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX6 ) dwTexCount=6;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX7 ) dwTexCount=7;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX8 ) dwTexCount=8;
	for ( DWORD dwTexCoordSet=0; dwTexCoordSet<dwTexCount; dwTexCoordSet++ )
	{
		DWORD dwTexCoord = dwFVF & D3DFVF_TEXCOORDSIZE1(dwTexCoordSet);
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE1(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			iOffset += 1;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE2(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			iOffset += 2;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE3(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			//psOffsetMap->dwTZ[0] = iOffset + 2;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			psOffsetMap->dwTZ[dwTexCoordSet] = iOffset + 2;
			iOffset += 3;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE4(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			//psOffsetMap->dwTZ[0] = iOffset + 2;
			//psOffsetMap->dwTW[0] = iOffset + 3;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			psOffsetMap->dwTZ[dwTexCoordSet] = iOffset + 2;
			psOffsetMap->dwTW[dwTexCoordSet] = iOffset + 3;
			iOffset += 4;
		}
	}
	
	// calculate byte offset
	psOffsetMap->dwByteSize = sizeof ( DWORD ) * iOffset;

	// store number of offsets
	psOffsetMap->dwSize = iOffset;

	// check if matches byte size of actual FVF
	if ( dwFVFSize != psOffsetMap->dwByteSize )
	{
		// Offsets not being calculated correctly!
		return false;
	}

	// complete
	return true;
}

DARKSDK_DLL bool GetFVFOffsetMap ( sMesh* pMesh, sOffsetMap* psOffsetMap )
{
	// clear to begin with
	memset ( psOffsetMap, 0, sizeof(sOffsetMap) );

	// FVF or declaration
	if ( pMesh->dwFVF==0 )
	{
		// Define end declaration token
		D3DVERTEXELEMENT9 End = D3DDECL_END();

		// Find Offsets
		for( int iElem=0; pMesh->pVertexDeclaration[iElem].Stream != End.Stream; iElem++ )
		{   
			int iIndex = pMesh->pVertexDeclaration[iElem].UsageIndex;
			int iElementOffset = pMesh->pVertexDeclaration[iElem].Offset / sizeof(DWORD);
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_POSITION )
			{
				psOffsetMap->dwX = iElementOffset + 0;
				psOffsetMap->dwY = iElementOffset + 1;
				psOffsetMap->dwZ = iElementOffset + 2;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_POSITIONT )
			{
				psOffsetMap->dwX = iElementOffset + 0;
				psOffsetMap->dwY = iElementOffset + 1;
				psOffsetMap->dwZ = iElementOffset + 2;
				psOffsetMap->dwRWH = iElementOffset + 3;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_PSIZE )
			{
				psOffsetMap->dwPointSize = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_NORMAL )
			{
				psOffsetMap->dwNX = iElementOffset + 0;
				psOffsetMap->dwNY = iElementOffset + 1;
				psOffsetMap->dwNZ = iElementOffset + 2;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_COLOR && iIndex==0 )
			{
				psOffsetMap->dwDiffuse = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_COLOR && iIndex==1 )
			{
				psOffsetMap->dwSpecular = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_TEXCOORD )
			{
				psOffsetMap->dwTU[iIndex] = iElementOffset + 0;
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT2)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
				}
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT3)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
					psOffsetMap->dwTZ[iIndex] = iElementOffset + 2;
				}
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT4)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
					psOffsetMap->dwTZ[iIndex] = iElementOffset + 2;
					psOffsetMap->dwTW[iIndex] = iElementOffset + 3;
				}
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_TANGENT )
			{
				psOffsetMap->dwTU[1] = iElementOffset + 0;
				psOffsetMap->dwTV[1] = iElementOffset + 1;
				psOffsetMap->dwTZ[1] = iElementOffset + 2;
				psOffsetMap->dwTW[1] = iElementOffset + 3;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_BINORMAL )
			{
				psOffsetMap->dwTU[2] = iElementOffset + 0;
				psOffsetMap->dwTV[2] = iElementOffset + 1;
				psOffsetMap->dwTZ[2] = iElementOffset + 2;
				psOffsetMap->dwTW[2] = iElementOffset + 3;
			}
		}

		// calculate byte offset
		psOffsetMap->dwByteSize = pMesh->dwFVFSize;

		// store number of offsets
		psOffsetMap->dwSize = pMesh->dwFVFSize/sizeof(DWORD);

		// complete
		return true;
	}
	else
	{
		return GetFVFValueOffsetMap ( pMesh->dwFVF, psOffsetMap );
	}
}

void dbPhyUpdateSoftBody ( void )
{
	for ( DWORD i = 0; i < PhysX::pSoftBodyList.size ( ); i++ )
	{
		PhysX::sSoftBodyObject*	pSoftBodyObject	= PhysX::pSoftBodyList [ i ];
		pSoftBodyObject->pBodies [ 0 ]->simulateAndDraw ( true, false, false );
		sObject* pObject = PhysX::GetObject ( pSoftBodyObject->iID );

		// get the offset map
		sOffsetMap offsetMap;
		if ( pObject->ppMeshList ) GetFVFOffsetMap ( pObject->ppMeshList[0], &offsetMap );

		//  offset
		NxVec3 off = NxVec3(0,0,0);

		// leefix - 160409 - go through each OBJ triangle, and find texture ref index (different from vertex index)
		DWORD dwTriangleCount = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->getNumTriangles ( );
		float* pVertex = ( float* ) pObject->pFrame->pMesh->pVertexData;
		for ( DWORD t = 0; t < dwTriangleCount; t++ )
		{
			for ( DWORD s=0; s<3; s++ )
			{
				DWORD vi = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].vertexNr[s];
				DWORD ni = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].normalNr[s];
				DWORD ti = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].texCoordNr[s];
				*( ( float* ) pVertex + offsetMap.dwX ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].x - off.x;
				*( ( float* ) pVertex + offsetMap.dwY ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].y - off.y;
				*( ( float* ) pVertex + offsetMap.dwZ ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].z - off.z;
				*( ( float* ) pVertex + offsetMap.dwNX ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].x;
				*( ( float* ) pVertex + offsetMap.dwNY ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].y;
				*( ( float* ) pVertex + offsetMap.dwNZ ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].z;
				*( ( float* ) pVertex + offsetMap.dwTU[0] ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTexCoords[ti].u;
				*( ( float* ) pVertex + offsetMap.dwTV[0] ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTexCoords[ti].v*-1;
				pVertex+=offsetMap.dwSize;
			}
		}
		pObject->pFrame->pMesh->bVBRefreshRequired = true;

		// 160409 - update any shadow mesh associated with soft body
		if ( pObject->pFrame->pShadowMesh )
		{
			sOffsetMap offsetMap;
			if ( pObject->ppMeshList ) GetFVFOffsetMap ( pObject->pFrame->pShadowMesh, &offsetMap );
			DWORD dwTriangleCount = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->getNumTriangles ( );
			float* pVertex = ( float* ) pObject->pFrame->pShadowMesh->pVertexData;
			for ( DWORD t = 0; t < dwTriangleCount; t++ )
			{
				for ( DWORD s=0; s<3; s++ )
				{
					DWORD vi = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].vertexNr[s];
					DWORD ni = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].normalNr[s];
					DWORD ti = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mTriangles[t].texCoordNr[s];
					*( ( float* ) pVertex + offsetMap.dwX ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].x - off.x;
					*( ( float* ) pVertex + offsetMap.dwY ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].y - off.y;
					*( ( float* ) pVertex + offsetMap.dwZ ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices[vi].z - off.z;
					*( ( float* ) pVertex + offsetMap.dwNX ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].x;
					*( ( float* ) pVertex + offsetMap.dwNY ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].y;
					*( ( float* ) pVertex + offsetMap.dwNZ ) = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mNormals[ni].z;
					pVertex+=offsetMap.dwSize;
				}
			}
			pObject->pFrame->pShadowMesh->bVBRefreshRequired = true;
		}

		// 390808 - soft bodies are not visibly culled - no culling each update
		pObject->collision.fScaledLargestRadius = 0.0f;
		pObject->pFrame->pMesh->Collision.fRadius = 0.0f;

		// override world position so position can be used to hold actual sft body vector
		pObject->bDisableTransform = true;

		// 240908 - get actor position and feed it back into Object Pos so we know where it is
		NxVec3 vec = pSoftBodyObject->pActors [ 0 ]->getGlobalPosition();
		pObject->position.vecPosition.x = vec.x;
		pObject->position.vecPosition.y = vec.y;
		pObject->position.vecPosition.z = vec.z;
	}
}

void dbPhySetSoftBodyPosition ( int iObject, float fX, float fY, float fZ, int iSmooth )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;
	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iObject ) ) )
		return;

	if ( pSoftBodyObject->pActors [ 0 ] )
	{
		NxVec3 pos = NxVec3 ( fX, fY, fZ );
		if ( iSmooth==1 )
		{
			NxVec3 oldpos = pSoftBodyObject->pBodies [ 0 ]->mSurfaceMesh->mVertices [ 0 ];
			pos = oldpos;
			pos.x += (fX-oldpos.x)/5.0f;
			pos.y += (fY-oldpos.y)/5.0f;
			pos.z += (fZ-oldpos.z)/5.0f;
		}
		pSoftBodyObject->pActors [ 0 ]->setGlobalPosition ( pos );
		pSoftBodyObject->pActors [ 0 ]->setLinearVelocity ( NxVec3 ( 0, 0, 0 ) );
		pSoftBodyObject->pActors [ 0 ]->setLinearMomentum(NxVec3 ( 0,0,0 ));
	}
}

void dbPhySetSoftBodyRotation ( int iObject, float fX, float fY, float fZ )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;
	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iObject ) ) )
		return;

	if ( pSoftBodyObject->pActors [ 0 ] )
	{
		pSoftBodyObject->pActors [ 0 ]->setAngularVelocity ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetSoftBodyLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;
	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iObject ) ) )
		return;

	if ( pSoftBodyObject->pActors [ 0 ] )
	{
		pSoftBodyObject->pActors [ 0 ]->setLinearVelocity ( NxVec3 ( fX, fY, fZ ) );


	}
}

void dbPhySetSoftBodyFrozen ( int iObject, int iAxis, int iState )
{
	PhysX::sSoftBodyObject*	pSoftBodyObject	= NULL;
	if ( !( pSoftBodyObject = dbPhyGetSoftBody ( iObject ) ) )
		return;

	if ( iState == 0 )
	{
		if ( iAxis == 0 ) pSoftBodyObject->pActors [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_X );
		if ( iAxis == 1 ) pSoftBodyObject->pActors [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_Y );
		if ( iAxis == 2 ) pSoftBodyObject->pActors [ 0 ]->clearBodyFlag ( NX_BF_FROZEN_POS_Z );
	}
	if ( iState == 1 )
	{
		if ( iAxis == 0 ) pSoftBodyObject->pActors [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_X );
		if ( iAxis == 1 ) pSoftBodyObject->pActors [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_Y );
		if ( iAxis == 2 ) pSoftBodyObject->pActors [ 0 ]->raiseBodyFlag ( NX_BF_FROZEN_POS_Z );
	}
}
