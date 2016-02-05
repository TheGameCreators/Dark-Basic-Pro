// LightMapper.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>

#include "Thread.h"

#include "DBOData.h"

//#define DARKSDK_COMPILE 1

#ifdef DARKSDK_COMPILE
	#define DLLEXPORT
	
	#include <mmsystem.h>
	#include "DarkSDK.h"

	sObject* dbGetObject ( int iObject );
	void	 MakePlane ( int iID, float fWidth, float fHeight, int extraParam );
#else
	#define DLLEXPORT __declspec(dllexport)
#endif

#include "DBPro Functions.h"
#include "CollisionTree.h"
#include "LMObject.h"
#include "LMTexture.h"
#include "Light.h"
#include "SharedData.h"
#include "Thread.h"
#include "LightMapperThread.h"
#include "LMPolyGroup.h"
#include "TreeFace.h"

#include <process.h>

SharedData *g_pShared = NULL;
int g_iLightmapFileFormat = 0;	//0=png, 1=dds, 2=bmp

LPDIRECT3DDEVICE9 g_pD3D = NULL;
GlobStruct *g_pGlob = NULL;

GetObjectPFN					g_GetObject							= NULL;
ObjectExistPFN					g_ObjectExist						= NULL;
CalcObjectWorldPFN				g_CalcObjectWorld					= NULL;
CalculateAbsoluteWorldMatrixPFN	g_CalculateAbsoluteWorldMatrix		= NULL;
ConvertLocalMeshToVertsOnlyPFN	g_ConvertLocalMeshToVertsOnly		= NULL;
ConvertLocalMeshToFVFPFN		g_ConvertLocalMeshToFVF				= NULL;
GetDirect3DDevicePFN			g_GetDirect3DDevice					= NULL;
SetBlendMappingPFN				g_SetBlendMapping					= NULL;
HideLimbPFN						g_HideLimb							= NULL;
GetFVFOffsetMapPFN				g_GetFVFOffsetMap					= NULL;
SmoothNormalsPFN				g_SmoothNormals						= NULL;
PositionObjectPFN				g_PositionObject					= NULL;
ConvertToFVFPFN					g_ConvertToFVF						= NULL;
CloneMeshToNewFormatPFN			g_CloneMeshToNewFormat				= NULL;

CollisionTree cColTree;
TreeFace *pFaceList = 0;
int iNumFaces = 0;
int iTotalLMObjects = 0;
int iBlendMode = D3DTOP_MODULATE;
char szLightmapName[256] = "";
char szLightmapFolderName[256] = "lightmaps\\";

LMObject *pLMObjectList = 0;
LMTexture *pLMTextureList = 0;
Light *pLightList = 0;

LightMapperThread *pLMThread = 0;

bool bInitialised = false;
bool bLightmapInProgress = false;
char errStr[256];

void InvalidPointer ( void )
{
	MessageBox ( NULL, "Include At Least One Object Command In Your Code To Use Lightmap Commands", "Light Mapping Error", 0 );
	exit ( 1 );
}

void CheckLMInit( )
{
	if ( !bInitialised )
	{
		MessageBox( NULL, "You Must Call LMStart Before Other Light Map Commands", "Light Mapping Error", 0 );
		exit(1);
	}
}

bool CheckInProgress( )
{
	if ( bLightmapInProgress )
	{
		//MessageBox( NULL, "Cannot Call Lightmap Command Whilst Lightmapping Is In Progress", "Light Mapping Warning", 0 );
	}

	return bLightmapInProgress;
}

DLLEXPORT void LMStart ( )
{
	if ( bInitialised ) return;

	#ifndef DARKSDK_COMPILE
	HINSTANCE CoreHandle = LoadLibrary("DBProCore.dll");
    GlobStruct* (*GetGlobPtr)(void) = ( GlobStruct* (*)(void) ) GetProcAddress( CoreHandle, "?GetGlobPtr@@YAKXZ" );
    FreeLibrary( CoreHandle );

	if ( !GetGlobPtr )
	{
		MessageBox( NULL, "Could Not Get GlobStruct Function Pointer", "Light Mapping Error", 0 );
		exit(1);
	}

	CoreHandle = LoadLibrary("DBProSetupDebug.dll");
	g_GetDirect3DDevice	= ( GetDirect3DDevicePFN ) GetProcAddress ( CoreHandle, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	FreeLibrary( CoreHandle );

	if ( !g_GetDirect3DDevice )
	{
		MessageBox( NULL, "Could Not Get Direct3D Function Pointer", "Light Mapping Error", 0 );
		exit(1);
	}

	g_pGlob = GetGlobPtr( );
	g_pD3D = g_GetDirect3DDevice( );
	#endif

	//get function pointers
	#ifdef DARKSDK_COMPILE
		g_GetObject							= dbGetObject;
		g_ObjectExist						= dbObjectExist;
		g_CalcObjectWorld					= dbCalcObjectWorld;
		g_CalculateAbsoluteWorldMatrix		= dbCalculateAbsoluteWorldMatrix;
		g_ConvertLocalMeshToVertsOnly		= dbConvertLocalMeshToVertsOnly;
		g_ConvertLocalMeshToFVF				= dbConvertLocalMeshToFVF;
		g_SetBlendMapping					= dbSetBlendMapping;
	#else
		if ( g_pGlob->g_Basic3D )
		{
			g_GetObject							= ( GetObjectPFN )						GetProcAddress ( g_pGlob->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
			g_ObjectExist						= ( ObjectExistPFN )					GetProcAddress ( g_pGlob->g_Basic3D, "?GetExist@@YAHH@Z" );
			g_CalcObjectWorld					= ( CalcObjectWorldPFN )				GetProcAddress ( g_pGlob->g_Basic3D, "?CalcObjectWorld@@YA_NPAUsObject@@@Z" );
			g_CalculateAbsoluteWorldMatrix		= ( CalculateAbsoluteWorldMatrixPFN )	GetProcAddress ( g_pGlob->g_Basic3D, "?CalculateAbsoluteWorldMatrix@@YAXPAUsObject@@PAUsFrame@@PAUsMesh@@@Z" );
			g_ConvertLocalMeshToVertsOnly		= ( ConvertLocalMeshToVertsOnlyPFN )	GetProcAddress ( g_pGlob->g_Basic3D, "?ConvertLocalMeshToVertsOnly@@YAXPAUsMesh@@@Z" );
			g_ConvertLocalMeshToFVF				= ( ConvertLocalMeshToFVFPFN )			GetProcAddress ( g_pGlob->g_Basic3D, "?ConvertLocalMeshToFVF@@YAXPAUsMesh@@K@Z" );
			g_SetBlendMapping					= ( SetBlendMappingPFN )				GetProcAddress ( g_pGlob->g_Basic3D, "?SetBlendMap@@YAXHHHHH@Z" );
			g_HideLimb							= ( HideLimbPFN )						GetProcAddress ( g_pGlob->g_Basic3D, "?HideLimb@@YAXHH@Z" );
			g_GetFVFOffsetMap					= ( GetFVFOffsetMapPFN )				GetProcAddress ( g_pGlob->g_Basic3D, "?GetFVFOffsetMap@@YA_NPAUsMesh@@PAUsOffsetMap@@@Z" );
			g_SmoothNormals						= ( SmoothNormalsPFN )					GetProcAddress ( g_pGlob->g_Basic3D, "?SmoothNormals@@YAXPAUsMesh@@M@Z" );
			g_PositionObject					= ( PositionObjectPFN )					GetProcAddress ( g_pGlob->g_Basic3D, "?Position@@YAXHMMM@Z" );
			g_ConvertToFVF						= ( ConvertToFVFPFN )					GetProcAddress ( g_pGlob->g_Basic3D, "?ConvertToFVF@@YAXPAUsMesh@@K@Z" );
			g_CloneMeshToNewFormat				= ( CloneMeshToNewFormatPFN )			GetProcAddress ( g_pGlob->g_Basic3D, "?CloneMeshToNewFormat@@YAXHK@Z" );
		}
		else
		{
			g_GetObject							= ( GetObjectPFN )						InvalidPointer;
			g_ObjectExist						= ( ObjectExistPFN )					InvalidPointer;
			g_CalcObjectWorld					= ( CalcObjectWorldPFN )				InvalidPointer;
			g_CalculateAbsoluteWorldMatrix		= ( CalculateAbsoluteWorldMatrixPFN )	InvalidPointer;
			g_ConvertLocalMeshToVertsOnly		= ( ConvertLocalMeshToVertsOnlyPFN )	InvalidPointer;
			g_ConvertLocalMeshToFVF				= ( ConvertLocalMeshToFVFPFN )			InvalidPointer;
			g_SetBlendMapping					= ( SetBlendMappingPFN )				InvalidPointer;
			g_GetFVFOffsetMap					= ( GetFVFOffsetMapPFN )				InvalidPointer;
			g_HideLimb							= ( HideLimbPFN )						InvalidPointer;
			g_SmoothNormals						= ( SmoothNormalsPFN )					InvalidPointer;
			g_PositionObject					= ( PositionObjectPFN )					InvalidPointer;
			g_ConvertToFVF						= ( ConvertToFVFPFN )					InvalidPointer;
			g_CloneMeshToNewFormat				= ( CloneMeshToNewFormatPFN )			InvalidPointer;
		}
	#endif

	bInitialised = true;
}

DLLEXPORT void LMClearLights( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	Light *pTempLight;
	while ( pLightList )
	{
		pTempLight = pLightList;
		pLightList = pLightList->pNextLight;
		delete pTempLight;
	}
}

DLLEXPORT void LMClearCollisionObjects( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	cColTree.Reset( );
	
	TreeFace *pTempFace;
	while ( pFaceList )
	{
		pTempFace = pFaceList;
		pFaceList = pFaceList->nextFace;
		delete pTempFace;
	}

	TransparentFace::TextureClass* pStoredTexture = 0;

	while ( TransparentFace::pTextureList )
	{
		pStoredTexture = TransparentFace::pTextureList;
		TransparentFace::pTextureList = TransparentFace::pTextureList->pNextTexture;
		delete pStoredTexture;
	}

	iNumFaces = 0;
	pFaceList = 0;
}

DLLEXPORT void LMClearLightMapObjects( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	LMObject *pTempObject;
	while ( pLMObjectList )
	{
		pTempObject = pLMObjectList;
		pLMObjectList = pLMObjectList->pNextObject;
		delete pTempObject;
	}

	pLMObjectList = 0;
	iTotalLMObjects = 0;
}

void LMClearTextures( )
{
	if ( pLMTextureList ) delete pLMTextureList;
	
	/*
	LMTexture *pTempTexture;
	while ( pLMTextureList )
	{
		pTempTexture = pLMTextureList;
		pLMTextureList = pLMTextureList->pNextLMTex;
		delete pTempTexture;
	}
	*/

	pLMTextureList = 0;
}

DLLEXPORT void LMSetMode( int iMode )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	//mode = 0 : combined (extend valid, clamp invalid) (default)
	//mode = 1 : extend all edges
	//mode = 2 : clamp all edges
	//mode = 3 : colour with normals

	if ( iMode < 0 || iMode > 3 ) iMode = 0;
	LMPolyGroup::iMode = iMode;
}

DLLEXPORT void LMSetShadowPower( float fPower )
{
	CheckLMInit( );

	MessageBox( NULL, "'LM Set Shadow Power' command has been removed to fix a bluring bug", "Lightmapper Error", 0 );
	exit(-1);

	if ( CheckInProgress( ) ) return;

	//higher powers reduce the area of shadows
	if ( fPower < 0 ) fPower = 0;
	LMPolyGroup::fShadowPower = fPower;
}

DLLEXPORT void LMBoostCurvedSurfaceQuality( float fMaxSize, float fBoost )
{
	if ( fBoost < 0.0001f ) fBoost = 0.0001f;
	LMPolyGroup::fCurvedBoost = fBoost;

	if ( fMaxSize < -1 ) fMaxSize = -1;
	LMPolyGroup::fMaxCurvedBoostSize = fMaxSize;
}

DLLEXPORT void LMSetBlendMode( int iNewMode )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( iNewMode < 1 ) iNewMode = 1;
	if ( iNewMode > 26 ) iNewMode = 26;
	iBlendMode = iNewMode;
}

//transparent type: 0=opaque, 1=black, 2=alpha
DLLEXPORT void LMAddCollisionObject( sObject *pObject, int iTransparent )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !pObject ) return;
	g_CalcObjectWorld ( pObject );

	//FILE* pDataFile = fopen( "Object Data.txt","w" );

	sObject *pPosObject = pObject;

	if ( pObject->pInstanceOfObject ) pObject = pObject->pInstanceOfObject;
	
	int iNumFrames = pObject->iFrameCount;

	//sprintf(errStr,"Frames: %d\n",iNumFrames);
	//fputs(errStr,pDataFile);

	for (int iFrame = 0; iFrame < iNumFrames; iFrame++ )
	{
		if ( !pObject->ppFrameList [ iFrame ] ) continue;
		sMesh *pMesh = pObject->ppFrameList [ iFrame ]->pMesh;
		if ( !pMesh ) continue;

		g_CalculateAbsoluteWorldMatrix ( pPosObject, pObject->ppFrameList [ iFrame ], pObject->ppFrameList [ iFrame ]->pMesh );

		int iPrimitiveType = pMesh->iPrimitiveType;
		if ( iPrimitiveType == 5 ) 
		{
			//MessageBox ( NULL, "Unhandled Primitive Type (5)", "Light Mapping Error", 0 );
			//continue;
			g_ConvertLocalMeshToVertsOnly( pMesh );
		}

		DWORD dwFVFSize = pMesh->dwFVFSize;
		int iNumVertices = pMesh->dwVertexCount;
		int iNumIndices = pMesh->dwIndexCount;

		BYTE *pVertexData = pMesh->pVertexData;
		WORD *pIndices = pMesh->pIndices;

		if ( dwFVFSize < 12 ) continue;
		if ( iNumVertices <= 0 ) continue;
		//if ( iNumIndices > 0 && iPrimitiveType != 4 ) continue;

		int iMax = iNumIndices>0 ? iNumIndices : iNumVertices;
		Point p1,p2,p3;
		float u1,u2,u3;
		float v1,v2,v3;
		int dwIndex;
		D3DXVECTOR3 vecVert;

		if ( iTransparent > 0 )
		{
			if ( pMesh->dwTextureCount < 1 ) 
			{
				//MessageBox ( NULL, "Not A Valid Transparent Collision Object, Must Have A Stage 0 Texture", "Lightmapper Warning", 0 );
				iTransparent = 0;
			}
		}

		sOffsetMap	pOffsetMap;
		g_GetFVFOffsetMap ( pMesh, &pOffsetMap );

		//sprintf(errStr,"Indices: %d Vertices: %d\n\n",iNumIndices,iNumVertices);
		//fputs(errStr,pDataFile);

		for ( int i = 0; i < iMax; i+=3 )
		{
			if ( iNumIndices > 0 ) dwIndex = *(pIndices + i)*dwFVFSize; else dwIndex = i*dwFVFSize;

			vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
			vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
			vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
			u1 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTU[0]*4 ) );
			v1 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTV[0]*4 ) );
			//u1 = *( (float*) ( pVertexData + dwIndex + 24 ) );
			//v1 = *( (float*) ( pVertexData + dwIndex + 28 ) );

			D3DXVec3TransformCoord( &vecVert, &vecVert, &pObject->ppFrameList [ iFrame ]->matAbsoluteWorld );
			p1.x = vecVert.x; p1.y = vecVert.y; p1.z = vecVert.z;

			if ( iNumIndices > 0 ) dwIndex = *(pIndices + i + 1)*dwFVFSize; else dwIndex = (i+1)*dwFVFSize;

			vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
			vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
			vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
			u2 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTU[0]*4 ) );
			v2 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTV[0]*4 ) );

			D3DXVec3TransformCoord( &vecVert, &vecVert, &pObject->ppFrameList [ iFrame ]->matAbsoluteWorld );
			p2.x = vecVert.x; p2.y = vecVert.y; p2.z = vecVert.z;

			if ( iNumIndices > 0 ) dwIndex = *(pIndices + i + 2)*dwFVFSize; else dwIndex = (i+2)*dwFVFSize;

			vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
			vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
			vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
			u3 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTU[0]*4 ) );
			v3 = *( (float*) ( pVertexData + dwIndex + pOffsetMap.dwTV[0]*4 ) );

			D3DXVec3TransformCoord( &vecVert, &vecVert, &pObject->ppFrameList [ iFrame ]->matAbsoluteWorld );
			p3.x = vecVert.x; p3.y = vecVert.y; p3.z = vecVert.z;

			TreeFace* aFace;
			bool bValid;
			
			if ( iTransparent > 0 ) 
			{
				//sprintf_s(errStr, "Tex: %p, u1: %3.3f, v1: %3.3f, u2: %3.3f, v2: %3.3f, u3: %3.3f, v3: %3.3f", pMesh->pTextures[0].pTexturesRef, u1,v1,u2,v2,u3,v3 );
				//MessageBox ( NULL, errStr, "Info", 0 );
				aFace = new TransparentFace( );
				bValid = ((TransparentFace*)aFace)->MakeTransparentFace( &p1, &p2, &p3, iTransparent - 1, u1, v1, u2, v2, u3, v3, (pMesh->pTextures[0].pTexturesRef) );
			}
			else 
			{
				aFace = new TreeFace( );
				bValid = aFace->MakeFace( &p1, &p2, &p3 );
			}
			
			//zero area polygons will have an undefined normal, which will cause problems later
			//so if any exist remove them
			if (!bValid) { delete aFace; continue; }
	        
			aFace->nextFace = pFaceList;
			pFaceList = aFace;

			iNumFaces++;

			/*
			sprintf(errStr,"X: %3.3f Y:%3.3f Z:%3.3f\n",p1.x,p1.y,p1.z);
			fputs(errStr,pDataFile);
			sprintf(errStr,"X: %3.3f Y:%3.3f Z:%3.3f\n",p2.x,p2.y,p2.z);
			fputs(errStr,pDataFile);
			sprintf(errStr,"X: %3.3f Y:%3.3f Z:%3.3f\n",p3.x,p3.y,p3.z);
			fputs(errStr,pDataFile);
			*/

			//sprintf(errStr,"Make Object Triangle %d, %3.3f, %3.3f, %3.3f, %3.3f, %3.3f, %3.3f, %3.3f, %3.3f, %3.3f\n",iNumFaces,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z);
			//fputs(errStr,pDataFile);
		}
	}

	//fclose(pDataFile);
}

DLLEXPORT void LMAddCollisionObject ( int iObjID )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !g_ObjectExist( iObjID ) ) 
	{
		sprintf_s( errStr, 255, "Object (%d) Does Not Exist", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	sObject *pObject = g_GetObject( iObjID );

	if ( !pObject )
	{
		sprintf_s( errStr, 255, "Could Not Get Object (%d) Pointer", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	LMAddCollisionObject( pObject, 0 );
}

DLLEXPORT void LMAddTransparentCollisionObject ( int iObjID, int iType )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !g_ObjectExist( iObjID ) ) 
	{
		sprintf_s( errStr, 255, "Object (%d) Does Not Exist", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	sObject *pObject = g_GetObject( iObjID );

	if ( !pObject )
	{
		sprintf_s( errStr, 255, "Could Not Get Object (%d) Pointer", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	if ( iType < 0 ) iType = 0;
	if ( iType > 2 ) iType = 2;

	LMAddCollisionObject( pObject, iType + 1 );
}

DLLEXPORT void LMBuildCollisionData( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !pFaceList )
	{
		//MessageBox( NULL, "No Collision Data To Build", "Light Mapping Warning", 0 );
		//exit(0);
		return;
	}

	cColTree.setFacesPerNode( 2 );
	cColTree.makeCollisionObject( iNumFaces, pFaceList );
	pFaceList = 0;
}

DLLEXPORT void LMAddLightMapObject( int iObjID, sObject *pObject, int iBaseStage, int iDynamicLight, int iShaded, int iFlatNormals )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !pObject ) return;
	g_CalcObjectWorld ( pObject );
	
	if ( pObject->pInstanceOfObject )
	{
		//MessageBox ( NULL, "Cannot Lightmap Instance Objects", "Light Mapping Warning", 0 );
		return;
	}

	if ( iBaseStage < 0 ) iBaseStage = 0;
	if ( iBaseStage > 8 ) iBaseStage = 8;

	int iNumRealFrames = 0;
	int iNumFrames = pObject->iFrameCount;
	int iNumMeshes = pObject->iMeshCount;
	if ( iNumFrames < 0 ) return;

	int iNumStages = -1;
	int iMaxStages = 0;
	bool bWarned = false;
	DWORD objFVF = 0;

	for (int iFrame = 0; iFrame < iNumFrames; iFrame++ )
	{
		if ( !pObject->ppFrameList [ iFrame ] ) continue;
		if ( !pObject->ppFrameList [ iFrame ]->pMesh ) continue;

		int iNumVertices = pObject->ppFrameList [ iFrame ]->pMesh->dwVertexCount;		
		
		DWORD dwFVF = pObject->ppFrameList [ iFrame ]->pMesh->dwFVF;

		// U75 - 051109 - can send part-shaded objects into the light mapper *FPSC universe*
		// so detect if a non-standard-vertex shader is being used by detecting FVF=0 (iShaded=2=mixed shader)
		// however, FPSCV1 was never designed to allow lightmaps and BUMP.FX in same scene, so 
		// remove for the moment and focus on shaders properly in V117 and beyond
		// cannot mix! if ( dwFVF==0 ) iShaded = 2;

		if ( !(dwFVF & D3DFVF_XYZ) ) continue;	

		if ( (iFlatNormals == 0) && !(dwFVF & D3DFVF_NORMAL) ) 
		{
			iFlatNormals = 1;

			if ( !bWarned )
			{
				char str [ 256 ];
				sprintf_s( str, 256, "Object: %d, one or more limbs do not contain vertex normals data. Defaulting to flat shading for these limbs", iObjID );
				//MessageBox( NULL, str, "Light Mapping Warning", 0 );
				bWarned = true;
			}
		}
		if ( !iShaded && ((dwFVF & D3DFVF_TEXCOUNT_MASK) < (((DWORD)iBaseStage+1) << D3DFVF_TEXCOUNT_SHIFT)) ) 
		{
			iBaseStage = -1;
		}

		objFVF = dwFVF;

		if ( iShaded )
		{
			//check LMstage exists
			if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) < (((DWORD)iBaseStage+1) << D3DFVF_TEXCOUNT_SHIFT) )
			{
				char str [ 256 ];
				sprintf_s( str, 256, "Object: %d does not contain texture coordinates for stage %d, shaded lightmapping requires used stages to be set", iObjID, iBaseStage );
				MessageBox( NULL, str, "Lightmapping Error", 0 );
				exit(-1);
			}
		}
		if ( iNumVertices <= 0 ) continue;

		int iStages = pObject->ppFrameList [ iFrame ]->pMesh->dwTextureCount;
		if ( iNumStages < 0 || iStages < iNumStages ) iNumStages = iStages;
		if ( iStages > iMaxStages ) iMaxStages = iStages;

		if ( !iShaded && iBaseStage >= iStages ) iBaseStage = -1;
	}

	// U75 - 051109 - if mixed shaded, assume lightmapping at second stage
	// cannot mix! if ( iShaded==2 ) iBaseStage=1;

	// U75 - 051109 - note, only CHANGES FVF for non-shaded, not shaded or mixed-shaded (universe)
	if ( iShaded == 0 )
	{
		if ( iNumStages < 2 )
		{
			g_CloneMeshToNewFormat( iObjID, (objFVF & ~D3DFVF_TEXCOUNT_MASK) | D3DFVF_TEX2 );
			g_SetBlendMapping( iObjID, 1, 0, 0, 0 );
		}
	}

	for (int iFrame = 0; iFrame < iNumFrames; iFrame++ )
	{
		if ( !pObject->ppFrameList [ iFrame ] ) continue;
		if ( !pObject->ppFrameList [ iFrame ]->pMesh ) continue;

		int iNumVertices = pObject->ppFrameList [ iFrame ]->pMesh->dwVertexCount;		
		DWORD dwFVFSize = pObject->ppFrameList [ iFrame ]->pMesh->dwFVFSize;

		if ( dwFVFSize < 12 ) continue;
		if ( iNumVertices <= 0 ) continue;

		int iNumIndices = pObject->ppFrameList [ iFrame ]->pMesh->dwIndexCount;
		if ( iNumIndices >= 65535 )
		{
			sprintf_s( errStr, 255, "Object (%d) Limb (%d) Contains More Than 21844 Polygons, This Is Likely To Crash. If So Reduce The Number Of Polygons Per Limb", iObjID, iFrame );
		}

		iNumRealFrames++;

		LMObject *pNewObject = new LMObject( pObject, pObject->ppFrameList [ iFrame ], pObject->ppFrameList [ iFrame ]->pMesh );
		
		if ( iShaded == 0 ) pNewObject->SetBaseStage( iBaseStage );
		else pNewObject->SetShaded( iBaseStage );

		pNewObject->SetDynamicLightMode( iDynamicLight );
		pNewObject->pNextObject = pLMObjectList;
		pNewObject->iID = iObjID;
		pNewObject->iLimbID = iFrame;
		
		pNewObject->BuildPolyList( iFlatNormals == 1 );

		pLMObjectList = pNewObject;

		iTotalLMObjects++;
	}
}

DLLEXPORT void LMAddLightMapObject ( int iObjID, int iBaseStage, int iDynamicLight, int iFlatShaded )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !g_ObjectExist( iObjID ) ) 
	{
		sprintf_s( errStr, 255, "Object (%d) Does Not Exist", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	sObject *pObject = g_GetObject( iObjID );

	if ( !pObject )
	{
		sprintf_s( errStr, 255, "Could Not Get Object (%d) Pointer", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	LMAddLightMapObject( iObjID, pObject, iBaseStage, iDynamicLight, 0, iFlatShaded );
}

DLLEXPORT void LMAddLightMapObject ( int iObjID )
{
	LMAddLightMapObject( iObjID, 0, 0, 0 );
}

DLLEXPORT void LMAddLightMapObject ( int iObjID, int iBaseStage )
{
	LMAddLightMapObject( iObjID, iBaseStage, 0, 0 );
}

DLLEXPORT void LMAddLightMapObject ( int iObjID, int iBaseStage, int iDynamicLight )
{
	LMAddLightMapObject( iObjID, iBaseStage, iDynamicLight, 0 );
}

DLLEXPORT void LMAddShadedLightMapObject ( int iObjID, int iLightMapStage, int iFlatShaded )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( !g_ObjectExist( iObjID ) ) 
	{
		sprintf_s( errStr, 255, "Object (%d) Does Not Exist", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	sObject *pObject = g_GetObject( iObjID );

	if ( !pObject )
	{
		sprintf_s( errStr, 255, "Could Not Get Object (%d) Pointer", iObjID );
		MessageBox( NULL, errStr, "Light Mapping Error", 0 );
		exit(1);
		return;
	}

	LMAddLightMapObject( iObjID, pObject, iLightMapStage, 0, 1, iFlatShaded );
}

DLLEXPORT void LMAddShadedLightMapObject ( int iObjID, int iLightMapStage )
{
	LMAddShadedLightMapObject ( iObjID, iLightMapStage, 0 );
}

DLLEXPORT void LMAddPointLight( float posX, float posY, float posZ, float radius, float red, float green, float blue )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	PointLight *pNewLight = new PointLight ( posX, posY, posZ, radius, radius, 16/(radius*radius), red, green, blue );	
	pNewLight->pNextLight = pLightList;
	pLightList = (Light*) pNewLight;
}

DLLEXPORT void LMAddCustomPointLight( float posX, float posY, float posZ, float radius, float attenuation, float attenuation2, float red, float green, float blue )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	PointLight *pNewLight = new PointLight ( posX, posY, posZ, radius, attenuation, attenuation2, red, green, blue );	
	pNewLight->pNextLight = pLightList;
	pLightList = (Light*) pNewLight;
}

DLLEXPORT void LMAddDirectionalLight( float dirX, float dirY, float dirZ, float red, float green, float blue )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	DirLight *pNewLight = new DirLight ( dirX, dirY, dirZ, red, green, blue );	
	pNewLight->pNextLight = pLightList;
	pLightList = (Light*) pNewLight;
}

DLLEXPORT void LMAddSpotLight( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float ang1, float ang2, float range, float red, float green, float blue )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	SpotLight *pNewLight = new SpotLight ( posX, posY, posZ, dirX, dirY, dirZ, ang1, ang2, range, red, green, blue );	
	pNewLight->pNextLight = pLightList;
	pLightList = (Light*) pNewLight;
}

DLLEXPORT void LMSetAmbientLight( float red, float green, float blue )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	Light::fAmbientR = red;
	Light::fAmbientG = green;
	Light::fAmbientB = blue;
}

DLLEXPORT void LMSetAmbientOcclusionOn( int iIterations, float fRayDist, int iPattern )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	srand( (unsigned int)time(0));

	LMPolyGroup::SetAmbientOcclusionOn( iIterations, fRayDist, iPattern );
}

DLLEXPORT void LMSetAmbientOcclusionOff( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	LMPolyGroup::SetAmbientOcclusionOff( );
}

DLLEXPORT void LMSetLightMapName ( DWORD pInString )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;
	if ( !pInString ) return;

#pragma warning ( disable : 4312 )
	strcpy_s( szLightmapName, 256, (char*) pInString );
#pragma warning ( default : 4312 )
}

DLLEXPORT void LMSetLightMapFileFormat ( int iFormat )
{
	if ( iFormat < 0 || iFormat > 2 ) iFormat = 0;
	g_iLightmapFileFormat = iFormat;
}

DLLEXPORT void LMSetLightMapFolder ( DWORD pInString )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;
	if ( !pInString ) return;

#pragma warning ( disable : 4312 )
	strcpy_s( szLightmapFolderName, 256, (char*) pInString );
#pragma warning ( default : 4312 )
}

DLLEXPORT void LMBuildLightMaps( int iTexSize, float fQuality, int iBlur, int iNumThreads )
{
	CheckLMInit( );
	if ( CheckInProgress( ) ) return;
	bLightmapInProgress = true;

	if ( iBlur < 0 ) iBlur = 0;
	if ( iBlur > 3 ) iBlur = 3;

	LMPolyGroup::SetPixelBorder( iBlur + 1 );

	if ( g_pShared ) g_pShared->SetComplete( false );

	int iCurrentObject = 0;
	char pInfoStr[256];

	if ( g_pShared ) g_pShared->SetStatus( "Starting Light Mapper...", 0 );

	if ( fQuality < 0.00001f ) fQuality = 0.00001f;
	if ( fQuality > 10000.0f ) fQuality = 10000.0f;
	fQuality = 1 / fQuality;

	if ( iTexSize < 32 ) iTexSize = 32;
	if ( pLMTextureList ) LMClearTextures( );

	pLMTextureList = new LMTexture( iTexSize, iTexSize );
	pLMTextureList->pNextLMTex = 0;

	//fit light map objects to textures
	LMObject *pLMObject = pLMObjectList;
	bool bSucceed = true;

	while ( pLMObject )
	{
		iCurrentObject++;

		if ( g_pShared ) 
		{
			sprintf_s( pInfoStr, 255, "Mapping Object [%d/%d]", iCurrentObject, iTotalLMObjects );
			g_pShared->SetStatus( pInfoStr, (48.0f*iCurrentObject)/iTotalLMObjects );
			if ( g_pShared->GetTerminate( ) ) 
			{
				bLightmapInProgress = false;
				g_pShared->SetComplete( true );
				return;
			}
		}

		pLMObject->GroupPolys( fQuality );
		pLMObject->SortGroups( );

		if ( pLMObject->GetNumPolys( ) == 0 ) 
		{
			pLMObject = pLMObject->pNextObject;
			continue;
		}

		float fNewQuality = fQuality;
		while ( !pLMTextureList->AddLMObject( pLMObject ) && fNewQuality < 1000 )
		{
			bSucceed = false;

			fNewQuality *= 1.5;
			pLMObject->ReScaleGroups( fNewQuality );

			while ( !pLMObject->WillFit( iTexSize, iTexSize ) && fNewQuality < 1000 )
			{
				fNewQuality *= 1.5;
				pLMObject->ReScaleGroups( fNewQuality );
			}
		}

		pLMObject = pLMObject->pNextObject;
	}

	if ( !bSucceed )
	{
		char fileStr[256];
		FILE *pQualityChangeFile;
		fopen_s( &pQualityChangeFile, "Quality Change.txt","w" );
		fputs( "Listed below are object limbs that were too large to fit onto a texture and had their quality reduced\n", pQualityChangeFile);
		fputs( "To allow the original quality to be used try splitting your object up into limbs and splitting large polygons into several smaller ones\n\n", pQualityChangeFile);

		pLMObject = pLMObjectList;
		while ( pLMObject )
		{
			if ( fabs( pLMObject->GetQuality( ) - fQuality ) > 0.0001 && pLMObject->GetNumPolys( ) > 0 )
			{
				if ( pLMObject->pLMTexture )
				{
					sprintf_s( fileStr, 255, "Object Number: %d, Limb Number: %d, New Quality: %f\n", pLMObject->iID, pLMObject->iLimbID, 1.0f/pLMObject->GetQuality( ) );
				}
				else
				{
					sprintf_s( fileStr, 255, "Object Number: %d, Limb Number: %d, FAILED\n", pLMObject->iID, pLMObject->iLimbID );
				}

				fputs( fileStr, pQualityChangeFile );
			}

			pLMObject = pLMObject->pNextObject;
		}

		fclose( pQualityChangeFile );
	}

	iCurrentObject = 0;
	pLMObject = pLMObjectList;

	if ( iNumThreads > 0 )
	{
		//Multi Threaded
		HANDLE pSemaphore = CreateSemaphore( NULL, iNumThreads, iNumThreads, NULL );
		while ( pLMObject )
		{
			iCurrentObject++;

			if ( g_pShared ) 
			{
				sprintf_s( pInfoStr, 255, "Calculating Object Light [%d/%d]", iCurrentObject, iTotalLMObjects );
				g_pShared->SetStatus( pInfoStr, (48.0f*iCurrentObject)/iTotalLMObjects + 48 );
				if ( g_pShared->GetTerminate( ) ) 
				{
					bLightmapInProgress = false;
					g_pShared->SetComplete( true );
					return;
				}
			}

			if ( pLMObject->pLMTexture )
			{
				DWORD dwResult = WaitForSingleObject( pSemaphore, INFINITE );
				if ( dwResult != WAIT_OBJECT_0 )
				{
					MessageBox( NULL, "Multithreading Error", "Lightmapping Error", 0 );
					exit(-1);
				}

				pLMObject->SetLocalValues( pLightList, &cColTree, iBlur, pSemaphore );
				pLMObject->Start( );
			}

			pLMObject = pLMObject->pNextObject;
		}

		iCurrentObject = 0;
		pLMObject = pLMObjectList;

		while ( pLMObject )
		{
			iCurrentObject++;

			if ( g_pShared ) 
			{
				sprintf_s( pInfoStr, 255, "Waiting For Threads To Finish [%d/%d]", iCurrentObject, iTotalLMObjects );
				g_pShared->SetStatus( pInfoStr, (3.0f*iCurrentObject)/iTotalLMObjects + 96 );
				if ( g_pShared->GetTerminate( ) ) 
				{
					bLightmapInProgress = false;
					g_pShared->SetComplete( true );
					return;
				}
			}

			if ( pLMObject->pLMTexture )
			{
				pLMObject->Join( );
			}

			pLMObject = pLMObject->pNextObject;
		}

		CloseHandle( pSemaphore );
	}
	else
	{
		//Single Threaded
		while ( pLMObject )
		{
			iCurrentObject++;

			if ( g_pShared ) 
			{
				sprintf_s( pInfoStr, 255, "Calculating Object Light [%d/%d]", iCurrentObject, iTotalLMObjects );
				g_pShared->SetStatus( pInfoStr, (48.0f*iCurrentObject)/iTotalLMObjects + 48 );
				if ( g_pShared->GetTerminate( ) ) 
				{
					bLightmapInProgress = false;
					g_pShared->SetComplete( true );
					return;
				}
			}

			if ( pLMObject->pLMTexture )
			{
				pLMObject->CalculateLight( pLightList, &cColTree, iBlur, 0 );
				pLMObject->ApplyToTexture( );
			}

			pLMObject = pLMObject->pNextObject;
		}
	}

	//fill the textures with light
	LMTexture *pLMTexture = pLMTextureList;
	int iTexNum = 0;
	char filename[256];

	char szRoot [ 256 ];
	GetCurrentDirectory( 256, szRoot );

	char szFolder [ 64 ];

	char* szLast = szLightmapFolderName;
	char* szSlash = strchr( szLightmapFolderName, '\\' );

	while ( szSlash )
	{
		strncpy_s( szFolder, 64, szLast, (szSlash - szLast) );
		
		if ( !SetCurrentDirectory( szFolder ) )
		{
			CreateDirectory( szFolder, NULL );
			if ( !SetCurrentDirectory( szFolder ) ) 
			{
				MessageBox( NULL, "Invalid lightmap folder location, could not create directory", "Lightmapper Error", 0 );
				return;
			}
		}

		szLast = szSlash+1;
		szSlash = strchr( szLast, '\\' );
	}

	SetCurrentDirectory( szRoot );

	pLMTexture = pLMTextureList;

	char szExtension [ 5 ];
	switch( g_iLightmapFileFormat )
	{
		case 0: strcpy_s( szExtension, 5, "png" ); break;
		case 1: strcpy_s( szExtension, 5, "dds" ); break;
		case 2: strcpy_s( szExtension, 5, "bmp" ); break;
		default: strcpy_s( szExtension, 5, "png" ); break;
	}

	while ( pLMTexture )
	{
		if ( !pLMTexture->IsEmpty( ) )
		{
			if ( g_pShared ) 
			{
				sprintf_s( pInfoStr, 255, "Saving Textures [%d]", iTexNum );
				g_pShared->SetStatus( pInfoStr, 99 );
				if ( g_pShared->GetTerminate( ) ) 
				{
					bLightmapInProgress = false;
					g_pShared->SetComplete( true );
					return;
				}
			}

			//calculate Lumel values
			pLMTexture->CopyToTexture( );
			sprintf_s( filename, 255, "%s%s%d.%s", szLightmapFolderName, szLightmapName, iTexNum, szExtension );
			iTexNum++;
			pLMTexture->SaveTexture( filename );

			// U75 - FPGC - 121209 - surely we can now remove the DDS from managed memory if the file has been created
			// otherwise we build up potentially hundreds of DDS textures in managed memory!!
			SAFE_RELEASE ( pLMTexture->pTextureDDS );
		}

		pLMTexture = pLMTexture->pNextLMTex;
	}

	//apply the textures to the dbpro objects
	pLMObject = pLMObjectList;
	while ( pLMObject )
	{
		pLMObject->CalculateVertexUV( iTexSize, iTexSize );
		pLMObject->UpdateObject( iBlendMode );
		pLMObject = pLMObject->pNextObject;
	}
	bLightmapInProgress = false;
	if ( g_pShared ) g_pShared->SetComplete( true );
}

DLLEXPORT void LMBuildLightMaps( int iTexSize, float fQuality, int iBlur )
{
	LMBuildLightMaps( iTexSize, fQuality, iBlur, 0 );
}

DLLEXPORT void LMBuildLightMapsThread( int iTexSize, float fQuality, int iBlur, int iNumThreads )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	if ( g_pShared ) delete g_pShared;
	g_pShared = new SharedData( );

	if ( pLMThread ) delete pLMThread;

	if ( iNumThreads < 0 )
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo( &sysInfo );
		iNumThreads = sysInfo.dwNumberOfProcessors + 1;
	}

	pLMThread = new LightMapperThread( );
	pLMThread->iTexSize = iTexSize;
	pLMThread->fQuality = fQuality;
	pLMThread->iBlur = iBlur;
	pLMThread->iNumThreads = iNumThreads;

	pLMThread->Start( );
}

DLLEXPORT void LMBuildLightMapsThread( int iTexSize, float fQuality, int iBlur )
{
	LMBuildLightMapsThread( iTexSize, fQuality, iBlur, 0 );
}

DLLEXPORT void LMTerminateThread( )
{
	CheckLMInit( );

	if ( !g_pShared )
	{
		//MessageBox( NULL, "Cannot Terminate, Thread Not In Progress", "Light Mapping Warning", 0 );
		return;
	}

	g_pShared->SetTerminate( true );
}

DLLEXPORT void LMUpdateObjects( )
{
	CheckLMInit( );

	LMTexture *pLMTexture = pLMTextureList;
//	int iTexNum = 0;
//	char filename[256];
/*
	while ( pLMTexture )
	{
		if ( !pLMTexture->IsEmpty( ) )
		{
			pLMTexture->CopyToTexture( );
			sprintf_s( filename, 255, "test-%d.bmp", iTexNum );
			iTexNum++;
			pLMTexture->SaveTexture( filename );
		}

		pLMTexture = pLMTexture->pNextLMTex;
	}
*/
	LMObject *pLMObject = pLMObjectList;

	while ( pLMObject )
	{
		pLMObject->UpdateObject( iBlendMode );

		pLMObject = pLMObject->pNextObject;
	}
}

DLLEXPORT void LMReset( )
{
	CheckLMInit( );

	if ( CheckInProgress( ) ) return;

	LMClearCollisionObjects( );
	LMClearLightMapObjects( );
	LMClearLights( );
	LMClearTextures( );

	if ( pLMThread ) delete pLMThread;
	pLMThread = 0;

	LMSetAmbientLight( 0,0,0 );
	LMSetAmbientOcclusionOff( );
	LMSetBlendMode( 4 );
	LMSetMode( 0 );
	//LMSetShadowPower( 1.0 );
}

DLLEXPORT DWORD LMGetStatus( DWORD szReturnString )
{
	CheckLMInit( );

#pragma warning ( disable : 4312 )	//convert DWORD to 'char *'

	g_pGlob->CreateDeleteString ( (DWORD*) &szReturnString, 0 );
	
	char szNewString[256];
	bool bOK = g_pShared && g_pShared->GetStatus( szNewString );
	if ( !bOK ) sprintf_s( szNewString, 255, "Could Not Get Status" );
	
	DWORD dwSize = (DWORD) strlen ( (char*) szNewString );

	g_pGlob->CreateDeleteString ( (DWORD*) &szReturnString, dwSize+1 );
	
	strcpy_s ( (char*) szReturnString, dwSize+1, szNewString);

#pragma warning ( default : 4312 )
	
	return (DWORD) szReturnString;
}

DLLEXPORT DWORD LMGetPercent( )
{
	CheckLMInit( );

	float fValue = -1;
	if ( g_pShared ) 
	{
		fValue = g_pShared->GetPercent( );
	}

	return *(DWORD*)&fValue;
}

DLLEXPORT int LMGetComplete( )
{
	CheckLMInit( );

	if ( g_pShared ) 
	{
		return g_pShared->GetComplete( ) ? 1 : 0;
	}

	return true;
}

/*
DLLEXPORT int LMIntersects( float x, float y, float z, float x2, float y2, float z2 )
{
	CheckLMInit( );

	Point pntStart( x,y,z );
	Vector vecDir( x2-x, y2-y, z2-z );
	Vector vecDirI( 1.0f/(x2-x), 1.0f/(y2-y), 1.0f/(z2-z) );

	return cColTree.intersects( &pntStart, &vecDir, &vecDirI, 0, 0 ) ? 1 : 0;
}
*/

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

