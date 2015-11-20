//
// GameFX
//

// Includes
#include "GameFX.h"

// Global data
GlobStruct*					g_pGlob = NULL;

// B3D Function Addresses
tCreateNodeTree B3D_CreateNodeTree;
tAddNodeTreeObject B3D_AddNodeTreeObject;
tAddNodeTreeLimb B3D_AddNodeTreeLimb;
tRemoveNodeTreeObject  B3D_RemoveNodeTreeObject ;
tDeleteNodeTree B3D_DeleteNodeTree;
tSetNodeTreeWireframeOn B3D_SetNodeTreeWireframeOn;
tSetNodeTreeWireframeOff B3D_SetNodeTreeWireframeOff;
tMakeNodeTreeCollisionBox B3D_MakeNodeTreeCollisionBox;
tSetNodeTreeTextureMode B3D_SetNodeTreeTextureMode;
tDisableNodeTreeOcclusion B3D_DisableNodeTreeOcclusion;
tEnableNodeTreeOcclusion B3D_EnableNodeTreeOcclusion;
tSaveNodeTreeObjects B3D_SaveNodeTreeObjects;
tSetNodeTreeEffectTechnique B3D_SetNodeTreeEffectTechnique;
tLoadNodeTreeObjects B3D_LoadNodeTreeObjects;
tAttachObjectToNodeTree B3D_AttachObjectToNodeTree;
tDetachObjectFromNodeTree B3D_DetachObjectFromNodeTree;
tSetNodeTreePortalsOn B3D_SetNodeTreePortalsOn;
tSetNodeTreePortalsOff B3D_SetNodeTreePortalsOff;
tSetNodeTreeCulling B3D_SetNodeTreeCulling;
tBuildNodeTreePortals B3D_BuildNodeTreePortals;
tSetNodeTreeScorchTexture B3D_SetNodeTreeScorchTexture;
tAddNodeTreeScorch B3D_AddNodeTreeScorch;
tAddNodeTreeLight B3D_AddNodeTreeLight;
tGetStaticHit B3D_GetStaticHit;
tGetStaticLineOfSight B3D_GetStaticLineOfSight;
tGetStaticRayCast B3D_GetStaticRayCast;
tGetStaticVolumeCast B3D_GetStaticVolumeCast;
tGetStaticX B3D_GetStaticX;
tGetStaticY B3D_GetStaticY;
tGetStaticZ B3D_GetStaticZ;
tGetStaticFloor B3D_GetStaticFloor;
tGetStaticColCount B3D_GetStaticColCount;
tGetStaticColValue B3D_GetStaticColValue;
tGetStaticLineOfSightX B3D_GetStaticLineOfSightX;
tGetStaticLineOfSightY B3D_GetStaticLineOfSightY;
tGetStaticLineOfSightZ B3D_GetStaticLineOfSightZ;
tPeformCSGUnion B3D_PeformCSGUnion;
tPeformCSGDifference B3D_PeformCSGDifference;
tPeformCSGIntersection B3D_PeformCSGIntersection;
tPeformCSGClip  B3D_PeformCSGClip ;
tPeformCSGUnionOnVertexData B3D_PeformCSGUnionOnVertexData;
tPeformCSGDifferenceOnVertexData  B3D_PeformCSGDifferenceOnVertexData ;
tPeformCSGIntersectionOnVertexData  B3D_PeformCSGIntersectionOnVertexData ;
tObjectBlocking  B3D_ObjectBlocking ;
tReduceMesh B3D_ReduceMesh;
tMakeLODFromMesh	 B3D_MakeLODFromMesh	;
tAddObjectToLightMapPool B3D_AddObjectToLightMapPool;
tAddLimbToLightMapPool  B3D_AddLimbToLightMapPool ;
tAddStaticObjectsToLightMapPool B3D_AddStaticObjectsToLightMapPool;
tAddLightMapLight B3D_AddLightMapLight;
tFlushLightMapLights B3D_FlushLightMapLights;
tCreateLightMaps B3D_CreateLightMaps;
tSetGlobalShadowsOn B3D_SetGlobalShadowsOn;
tSetGlobalShadowsOff B3D_SetGlobalShadowsOff;
tSetShadowPosition B3D_SetShadowPosition;
tSetShadowColor B3D_SetShadowColor;
tSetShadowShades B3D_SetShadowShades;
tAddLODToObject B3D_AddLODToObject;

//
// CONSTRUCTOR FUNCTIONS
//

void Constructor ( void )
{
}

void Destructor ( void )
{
}

void PreDestructor ( void )
{
	// pre destructor
}

void ReceiveCoreDataPtr ( LPVOID pCore )
{
	// Get Core Data Pointer here
	g_pGlob = (GlobStruct*)pCore;

	// Fill B3D function addresses
	if( !g_pGlob->g_Basic3D ) return;
	B3D_CreateNodeTree						= ( tCreateNodeTree )						GetProcAddress ( g_pGlob->g_Basic3D, "?CreateNodeTree@@YAXMMM@Z" );
	B3D_AddNodeTreeObject					= ( tAddNodeTreeObject )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddNodeTreeObject@@YAXHHHHH@Z" );
	B3D_AddNodeTreeLimb						= ( tAddNodeTreeLimb )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddNodeTreeLimb@@YAXHHHHHH@Z" );
	B3D_RemoveNodeTreeObject				= ( tRemoveNodeTreeObject )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFRemoveNodeTreeObject@@YAXH@Z" );
	B3D_DeleteNodeTree						= ( tDeleteNodeTree )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFDeleteNodeTree@@YAXXZ" );
	B3D_SetNodeTreeWireframeOn				= ( tSetNodeTreeWireframeOn )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeWireframeOn@@YAXXZ" );
	B3D_SetNodeTreeWireframeOff				= ( tSetNodeTreeWireframeOff )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeWireframeOff@@YAXXZ" );
	B3D_MakeNodeTreeCollisionBox			= ( tMakeNodeTreeCollisionBox )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFMakeNodeTreeCollisionBox@@YAXMMMMMM@Z" );
	B3D_SetNodeTreeTextureMode				= ( tSetNodeTreeTextureMode )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeTextureMode@@YAXH@Z" );
	B3D_DisableNodeTreeOcclusion			= ( tDisableNodeTreeOcclusion )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFDisableNodeTreeOcclusion@@YAXXZ" );
	B3D_EnableNodeTreeOcclusion				= ( tEnableNodeTreeOcclusion )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFEnableNodeTreeOcclusion@@YAXXZ" );
	B3D_SaveNodeTreeObjects					= ( tSaveNodeTreeObjects )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSaveNodeTreeObjects@@YAXPAD@Z" );
	B3D_SetNodeTreeEffectTechnique			= ( tSetNodeTreeEffectTechnique )			GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeEffectTechnique@@YAXPAD@Z" );
	B3D_LoadNodeTreeObjects					= ( tLoadNodeTreeObjects )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFLoadNodeTreeObjects@@YAXPADH@Z" );
	B3D_AttachObjectToNodeTree				= ( tAttachObjectToNodeTree )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFAttachObjectToNodeTree@@YAXH@Z" );
	B3D_DetachObjectFromNodeTree			= ( tDetachObjectFromNodeTree )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFDetachObjectFromNodeTree@@YAXH@Z" );
	B3D_SetNodeTreePortalsOn				= ( tSetNodeTreePortalsOn )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreePortalsOn@@YAXXZ" );
	B3D_SetNodeTreePortalsOff				= ( tSetNodeTreePortalsOff )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreePortalsOff@@YAXXZ" );
	B3D_SetNodeTreeCulling					= ( tSetNodeTreeCulling )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeCulling@@YAXH@Z" );
	B3D_BuildNodeTreePortals				= ( tBuildNodeTreePortals )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFBuildNodeTreePortals@@YAXXZ" );
	B3D_SetNodeTreeScorchTexture			= ( tSetNodeTreeScorchTexture )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetNodeTreeScorchTexture@@YAXHHH@Z" );
	B3D_AddNodeTreeScorch					= ( tAddNodeTreeScorch )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddNodeTreeScorch@@YAXMH@Z" );
	B3D_AddNodeTreeLight					= ( tAddNodeTreeLight )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddNodeTreeLight@@YAXHMMMM@Z" );
	B3D_GetStaticHit						= ( tGetStaticHit )							GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticHit@@YAHMMMMMMMMMMMM@Z" );
	B3D_GetStaticLineOfSight				= ( tGetStaticLineOfSight )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticLineOfSight@@YAHMMMMMMMM@Z" );
	B3D_GetStaticRayCast					= ( tGetStaticRayCast )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticRayCast@@YAHMMMMMM@Z" );
	B3D_GetStaticVolumeCast					= ( tGetStaticVolumeCast )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticVolumeCast@@YAHMMMMMMM@Z" );
	B3D_GetStaticX							= ( tGetStaticX )							GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticX@@YAKXZ" );
	B3D_GetStaticY							= ( tGetStaticY )							GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticY@@YAKXZ" );
	B3D_GetStaticZ							= ( tGetStaticZ )							GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticZ@@YAKXZ" );
	B3D_GetStaticFloor						= ( tGetStaticFloor )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticFloor@@YAHXZ" );
	B3D_GetStaticColCount					= ( tGetStaticColCount )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticColCount@@YAHXZ" );
	B3D_GetStaticColValue					= ( tGetStaticColValue )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticColValue@@YAHXZ" );
	B3D_GetStaticLineOfSightX				= ( tGetStaticLineOfSightX )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticLineOfSightX@@YAKXZ" );
	B3D_GetStaticLineOfSightY				= ( tGetStaticLineOfSightY )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticLineOfSightY@@YAKXZ" );
	B3D_GetStaticLineOfSightZ				= ( tGetStaticLineOfSightZ )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFGetStaticLineOfSightZ@@YAKXZ" );
	B3D_PeformCSGUnion						= ( tPeformCSGUnion )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGUnion@@YAXHH@Z" );
	B3D_PeformCSGDifference					= ( tPeformCSGDifference )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGDifference@@YAXHH@Z" );
	B3D_PeformCSGIntersection				= ( tPeformCSGIntersection )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGIntersection@@YAXHH@Z" );
	B3D_PeformCSGClip						= ( tPeformCSGClip )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGClip@@YAXHH@Z" );
	B3D_PeformCSGUnionOnVertexData			= ( tPeformCSGUnionOnVertexData )			GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGUnionOnVertexData@@YAXH@Z" );
	B3D_PeformCSGDifferenceOnVertexData		= ( tPeformCSGDifferenceOnVertexData )		GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGDifferenceOnVertexData@@YAXH@Z" );
	B3D_PeformCSGIntersectionOnVertexData	= ( tPeformCSGIntersectionOnVertexData )	GetProcAddress ( g_pGlob->g_Basic3D, "?GFPeformCSGIntersectionOnVertexData@@YAXH@Z" );
	B3D_ObjectBlocking						= ( tObjectBlocking )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFObjectBlocking@@YAHHMMMMMM@Z" );
	B3D_ReduceMesh							= ( tReduceMesh )							GetProcAddress ( g_pGlob->g_Basic3D, "?GFReduceMesh@@YAXHHHHHH@Z" );
	B3D_MakeLODFromMesh						= ( tMakeLODFromMesh )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFMakeLODFromMesh@@YAXHHH@Z" );
	B3D_AddObjectToLightMapPool				= ( tAddObjectToLightMapPool )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddObjectToLightMapPool@@YAXH@Z" );
	B3D_AddLimbToLightMapPool				= ( tAddLimbToLightMapPool )				GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddLimbToLightMapPool@@YAXHH@Z" );
	B3D_AddStaticObjectsToLightMapPool		= ( tAddStaticObjectsToLightMapPool )		GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddStaticObjectsToLightMapPool@@YAXXZ" );
	B3D_AddLightMapLight					= ( tAddLightMapLight )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFAddLightMapLight@@YAXMMMMMMMMH@Z" );
	B3D_FlushLightMapLights					= ( tFlushLightMapLights )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFFlushLightMapLights@@YAXXZ" );
	B3D_CreateLightMaps						= ( tCreateLightMaps )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFCreateLightMaps@@YAXHHPAD@Z" );
	B3D_SetGlobalShadowsOn					= ( tSetGlobalShadowsOn )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetGlobalShadowsOn@@YAXXZ" );
	B3D_SetGlobalShadowsOff					= ( tSetGlobalShadowsOff )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetGlobalShadowsOff@@YAXXZ" );
	B3D_SetShadowPosition					= ( tSetShadowPosition )					GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetShadowPosition@@YAXHMMM@Z" );
	B3D_SetShadowColor						= ( tSetShadowColor )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetShadowColor@@YAXHHHH@Z" );
	B3D_SetShadowShades						= ( tSetShadowShades )						GetProcAddress ( g_pGlob->g_Basic3D, "?GFSetShadowShades@@YAXH@Z" );
	B3D_AddLODToObject						= ( tAddLODToObject )						GetProcAddress ( g_pGlob->g_Basic3D, "?AddLODToObject@@YAXHHHM@Z" );
}

int GetAssociatedDLLs ( void )
{
	// return a code indicating which DLLs are required
	// 1=Basic3D (and friends)
	return 1;
}

int GetNumDependencies ( void )
{
	return 0;
}

const char * GetDependencyID ( int n )
{
	return "";
}

void ReceiveDependenciesHinstance ( LPSTR pDLLName, HINSTANCE hModule )
{
	// got the hinstance for this DLL
	HINSTANCE DLL = hModule;
}

//
// MAIN GAMEFX FUNCTIONS
//

// Static
void CreateNodeTree						( float fX, float fY, float fZ )
{
	B3D_CreateNodeTree ( fX, fY, fZ );
}

void AddNodeTreeObject					( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	B3D_AddNodeTreeObject ( iID, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

void AddNodeTreeLimb						( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	B3D_AddNodeTreeLimb ( iID, iLimb, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

void RemoveNodeTreeObject					( int iID )
{
	B3D_RemoveNodeTreeObject ( iID );
}

void DeleteNodeTree						( void )
{
	B3D_DeleteNodeTree();
}

void SetNodeTreeWireframeOn				( void )
{
	B3D_SetNodeTreeWireframeOn();
}

void SetNodeTreeWireframeOff				( void )
{
	B3D_SetNodeTreeWireframeOff();
}

void MakeNodeTreeCollisionBox				( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 )
{
	B3D_MakeNodeTreeCollisionBox ( fX1, fY1, fZ1, fX2, fY2, fZ2 );
}

void SetNodeTreeTextureMode				( int iMode )
{
	B3D_SetNodeTreeTextureMode ( iMode );
}

void DisableNodeTreeOcclusion				( void )
{
	B3D_DisableNodeTreeOcclusion();
}

void EnableNodeTreeOcclusion				( void )
{
	B3D_EnableNodeTreeOcclusion();
}

void SaveNodeTreeObjects					( DWORD dwFilename )
{
	B3D_SaveNodeTreeObjects	( (LPSTR)dwFilename );
}

void SetNodeTreeEffectTechnique					( DWORD dwFilename )
{
	B3D_SetNodeTreeEffectTechnique ( (LPSTR)dwFilename );
}

void LoadNodeTreeObjects					( DWORD dwFilename, int iDivideTextureSize )
{
	B3D_LoadNodeTreeObjects	( (LPSTR)dwFilename, iDivideTextureSize );
}

void AttachObjectToNodeTree				( int iID )
{
	B3D_AttachObjectToNodeTree ( iID );
}

void DetachObjectFromNodeTree				( int iID )
{
	B3D_DetachObjectFromNodeTree ( iID );
}

void SetNodeTreePortalsOn					( void )
{
	B3D_SetNodeTreePortalsOn();
}

void SetNodeTreePortalsOff				( void )
{
	B3D_SetNodeTreePortalsOff();
}

void SetNodeTreeCulling ( int iFlag )
{
	B3D_SetNodeTreeCulling(iFlag);
}

void BuildNodeTreePortals					( void )
{
	B3D_BuildNodeTreePortals();
}

void SetNodeTreeScorchTexture				( int iImageID, int iWidth, int iHeight )
{
	B3D_SetNodeTreeScorchTexture	( iImageID, iWidth, iHeight );
}

void AddNodeTreeScorch					( float fSize, int iType )
{
	B3D_AddNodeTreeScorch ( fSize, iType );
}

void AddNodeTreeLight						( int iLightIndex, float fX, float fY, float fZ, float fRange )
{
	B3D_AddNodeTreeLight ( iLightIndex, fX, fY, fZ, fRange );
}

// Static Expressions
DARKSDK int GetStaticHit ( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2   )
{
	return B3D_GetStaticHit ( fOldX1, fOldY1, fOldZ1, fOldX2, fOldY2, fOldZ2, fNX1, fNY1, fNZ1, fNX2, fNY2, fNZ2   );
}

DARKSDK int GetStaticLineOfSight ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy )
{
	return B3D_GetStaticLineOfSight ( fSx, fSy, fSz, fDx, fDy, fDz, fWidth, fAccuracy );
}

DARKSDK int GetStaticRayCast ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz )
{
	return B3D_GetStaticRayCast ( fSx, fSy, fSz, fDx, fDy, fDz );
}

DARKSDK int GetStaticVolumeCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize )
{
	return B3D_GetStaticVolumeCast ( fX, fY, fZ, fNewX, fNewY, fNewZ, fSize );
}

DARKSDK DWORD GetStaticX ( void )
{
	return B3D_GetStaticX ( );
}

DARKSDK DWORD GetStaticY ( void )
{
	return B3D_GetStaticY ( );
}

DARKSDK DWORD GetStaticZ ( void )
{
	return B3D_GetStaticZ ( );
}

DARKSDK int GetStaticFloor ( void )
{
	return B3D_GetStaticFloor ( );
}

DARKSDK int GetStaticColCount ( void )
{
	return B3D_GetStaticColCount ( );
}

DARKSDK int GetStaticColValue ( void )
{
	return B3D_GetStaticColValue ( );
}

DARKSDK DWORD GetStaticLineOfSightX ( void )
{
	return B3D_GetStaticLineOfSightX ( );
}

DARKSDK DWORD GetStaticLineOfSightY ( void )
{
	return B3D_GetStaticLineOfSightY ( );
}

DARKSDK DWORD GetStaticLineOfSightZ ( void )
{
	return B3D_GetStaticLineOfSightZ ( );
}

// CSG Commands (CSG)

void PeformCSGUnion						( int iObjectA, int iObjectB )
{
	B3D_PeformCSGUnion ( iObjectA, iObjectB );
}

void PeformCSGDifference					( int iObjectA, int iObjectB )
{
	B3D_PeformCSGDifference ( iObjectA, iObjectB );
}

void PeformCSGIntersection				( int iObjectA, int iObjectB )
{
	B3D_PeformCSGIntersection ( iObjectA, iObjectB );
}

void PeformCSGClip						( int iObjectA, int iObjectB )
{
	B3D_PeformCSGClip ( iObjectA, iObjectB );
}

void PeformCSGUnionOnVertexData			( int iBrushMeshID )
{
	B3D_PeformCSGUnionOnVertexData ( iBrushMeshID );
}

void PeformCSGDifferenceOnVertexData		( int iBrushMeshID )
{
	B3D_PeformCSGDifferenceOnVertexData ( iBrushMeshID );
}

void PeformCSGIntersectionOnVertexData	( int iBrushMeshID )
{
	B3D_PeformCSGIntersectionOnVertexData ( iBrushMeshID );
}

DARKSDK int  GFObjectBlocking						( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
{
	return B3D_ObjectBlocking ( iID, X1, Y1, Z1, X2, Y2, Z2 );
}

void ReduceMesh							( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ )
{
	B3D_ReduceMesh ( iMeshID, iBlockMode, iNearMode, iGX, iGY, iGZ );
}

void MakeLODFromMesh						( int iMeshID, int iVertNum, int iNewMeshID )
{
	B3D_MakeLODFromMesh	( iMeshID, iVertNum, iNewMeshID );
}

// Light Maps

void AddObjectToLightMapPool				( int iID )
{
	B3D_AddObjectToLightMapPool ( iID );
}

void AddLimbToLightMapPool				( int iID, int iLimb )
{
	B3D_AddLimbToLightMapPool ( iID, iLimb );
}

void AddStaticObjectsToLightMapPool		( void )
{
	B3D_AddStaticObjectsToLightMapPool();
}

void AddLightMapLight						( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, int bCastShadow )
{
	B3D_AddLightMapLight ( fX, fY, fZ, fRadius, fRed, fGreen, fBlue, fBrightness, bCastShadow );
}

void FlushLightMapLights					( void )
{
	B3D_FlushLightMapLights();
}

void CreateLightMaps						( int iLMSize, int iLMQuality, DWORD dwPathForLightMaps )
{
	B3D_CreateLightMaps	( iLMSize, iLMQuality, (LPSTR)dwPathForLightMaps );
}

// Shadows

void SetGlobalShadowsOn					( void )
{
	B3D_SetGlobalShadowsOn();
}

void SetGlobalShadowsOff					( void )
{
	B3D_SetGlobalShadowsOff();
}

void SetShadowPosition ( int iMode, float fX, float fY, float fZ )
{
	B3D_SetShadowPosition (  iMode,  fX,  fY,  fZ );
}

void SetShadowColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	B3D_SetShadowColor (  iRed,  iGreen,  iBlue,  iAlphaLevel );
}

void SetShadowShades ( int iShades )
{
	B3D_SetShadowShades (  iShades );
}

// Others

void AddLODToObject ( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD )
{
	B3D_AddLODToObject ( iCurrentID, iLODModelID, iLODLevel, fDistanceOfLOD );
}


// Explosions
void MakeExplosion ( int iExplosionID, float fX, float fY, float fZ )
{
	MessageBox ( NULL, "bang", "bang!", MB_OK );
}

// Water
