//
// GameFX Functions called into Basic3D-Integrated-Functions
//

// Includes
#include "CommonC.h"
#include "CObjectsNewC.h"

// Static
DARKSDK void GFCreateNodeTree						( float fX, float fY, float fZ )
{
	CreateNodeTree ( fX, fY, fZ );
}

DARKSDK void GFAddNodeTreeObject					( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	AddNodeTreeObject ( iID, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

DARKSDK void GFAddNodeTreeLimb						( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	AddNodeTreeLimb ( iID, iLimb, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

DARKSDK void GFRemoveNodeTreeObject					( int iID )
{
	RemoveNodeTreeObject ( iID );
}

DARKSDK void GFDeleteNodeTree						( void )
{
	DeleteNodeTree();
}

DARKSDK void GFSetNodeTreeWireframeOn				( void )
{
	SetNodeTreeWireframeOn();
}

DARKSDK void GFSetNodeTreeWireframeOff				( void )
{
	SetNodeTreeWireframeOff();
}

DARKSDK void GFMakeNodeTreeCollisionBox				( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 )
{
	MakeNodeTreeCollisionBox ( fX1, fY1, fZ1, fX2, fY2, fZ2 );
}

DARKSDK void GFSetNodeTreeTextureMode				( int iMode )
{
	SetNodeTreeTextureMode ( iMode );
}

DARKSDK void GFDisableNodeTreeOcclusion				( void )
{
	DisableNodeTreeOcclusion();
}

DARKSDK void GFEnableNodeTreeOcclusion				( void )
{
	EnableNodeTreeOcclusion();
}

DARKSDK void GFSaveNodeTreeObjects					( LPSTR pFilename )
{
	SaveNodeTreeObjects	( (DWORD)pFilename );
}

DARKSDK void GFSetNodeTreeEffectTechnique			( LPSTR pFilename )
{
	SetNodeTreeEffectTechnique	( (DWORD)pFilename );
}

DARKSDK void GFLoadNodeTreeObjects					( LPSTR pFilename, int iDivideTextureSize )
{
	LoadNodeTreeObjects	( (DWORD)pFilename, iDivideTextureSize );
}

DARKSDK void GFAttachObjectToNodeTree				( int iID )
{
	AttachObjectToNodeTree ( iID );
}

DARKSDK void GFDetachObjectFromNodeTree				( int iID )
{
	DetachObjectFromNodeTree ( iID );
}

DARKSDK void GFSetNodeTreePortalsOn					( void )
{
	SetNodeTreePortalsOn();
}

DARKSDK void GFSetNodeTreeCulling				( int iFlag )
{
	SetNodeTreeCulling(iFlag);
}

DARKSDK void GFSetNodeTreePortalsOff				( void )
{
	SetNodeTreePortalsOff();
}

DARKSDK void GFBuildNodeTreePortals					( void )
{
	BuildNodeTreePortals();
}

DARKSDK void GFSetNodeTreeScorchTexture				( int iImageID, int iWidth, int iHeight )
{
	SetNodeTreeScorchTexture	( iImageID, iWidth, iHeight );
}

DARKSDK void GFAddNodeTreeScorch					( float fSize, int iType )
{
	AddNodeTreeScorch ( fSize, iType );
}

DARKSDK void GFAddNodeTreeLight						( int iLightIndex, float fX, float fY, float fZ, float fRange )
{
	AddNodeTreeLight ( iLightIndex, fX, fY, fZ, fRange );
}

// Static Expressions
DARKSDK int GFGetStaticHit ( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2   )
{
	return GetStaticHit ( fOldX1, fOldY1, fOldZ1, fOldX2, fOldY2, fOldZ2, fNX1, fNY1, fNZ1, fNX2, fNY2, fNZ2   );
}

DARKSDK int GFGetStaticLineOfSight ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy )
{
	return GetStaticLineOfSight ( fSx, fSy, fSz, fDx, fDy, fDz, fWidth, fAccuracy );
}

DARKSDK int GFGetStaticRayCast ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz )
{
	return GetStaticRayCast ( fSx, fSy, fSz, fDx, fDy, fDz );
}

DARKSDK int GFGetStaticVolumeCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize )
{
	return GetStaticVolumeCast ( fX, fY, fZ, fNewX, fNewY, fNewZ, fSize );
}

DARKSDK DWORD GFGetStaticX ( void )
{
	return  GetStaticX ( );
}

DARKSDK DWORD GFGetStaticY ( void )
{
	return  GetStaticY ( );
}

DARKSDK DWORD GFGetStaticZ ( void )
{
	return  GetStaticZ ( );
}

DARKSDK int GFGetStaticFloor ( void )
{
	return  GetStaticFloor ( );
}

DARKSDK int GFGetStaticColCount ( void )
{
	return  GetStaticColCount ( );
}

DARKSDK int GFGetStaticColValue ( void )
{
	return  GetStaticColValue ( );
}

DARKSDK DWORD GFGetStaticLineOfSightX ( void )
{
	return  GetStaticLineOfSightX ( );
}

DARKSDK DWORD GFGetStaticLineOfSightY ( void )
{
	return  GetStaticLineOfSightY ( );
}

DARKSDK DWORD GFGetStaticLineOfSightZ ( void )
{
	return  GetStaticLineOfSightZ ( );
}

// CSG Commands (CSG)

DARKSDK void GFPeformCSGUnion						( int iObjectA, int iObjectB )
{
	PeformCSGUnion ( iObjectA, iObjectB );
}

DARKSDK void GFPeformCSGDifference					( int iObjectA, int iObjectB )
{
	PeformCSGDifference ( iObjectA, iObjectB );
}

DARKSDK void GFPeformCSGIntersection				( int iObjectA, int iObjectB )
{
	PeformCSGIntersection ( iObjectA, iObjectB );
}

DARKSDK void GFPeformCSGClip						( int iObjectA, int iObjectB )
{
	PeformCSGClip ( iObjectA, iObjectB );
}

DARKSDK void GFPeformCSGUnionOnVertexData			( int iBrushMeshID )
{
	PeformCSGUnionOnVertexData ( iBrushMeshID );
}

DARKSDK void GFPeformCSGDifferenceOnVertexData		( int iBrushMeshID )
{
	PeformCSGDifferenceOnVertexData ( iBrushMeshID );
}

DARKSDK void GFPeformCSGIntersectionOnVertexData	( int iBrushMeshID )
{
	PeformCSGIntersectionOnVertexData ( iBrushMeshID );
}

DARKSDK int  GFObjectBlocking						( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
{
	return ObjectBlocking ( iID, X1, Y1, Z1, X2, Y2, Z2 );
}

DARKSDK void GFReduceMesh							( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ )
{
	ReduceMesh ( iMeshID, iBlockMode, iNearMode, iGX, iGY, iGZ );
}

DARKSDK void GFMakeLODFromMesh						( int iMeshID, int iVertNum, int iNewMeshID )
{
	MakeLODFromMesh	( iMeshID, iVertNum, iNewMeshID );
}

// Light Maps

DARKSDK void GFAddObjectToLightMapPool				( int iID )
{
	AddObjectToLightMapPool ( iID );
}

DARKSDK void GFAddLimbToLightMapPool				( int iID, int iLimb )
{
	AddLimbToLightMapPool ( iID, iLimb );
}

DARKSDK void GFAddStaticObjectsToLightMapPool		( void )
{
	AddStaticObjectsToLightMapPool();
}

DARKSDK void GFAddLightMapLight						( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, int bCastShadow )
{
	AddLightMapLight ( fX, fY, fZ, fRadius, fRed, fGreen, fBlue, fBrightness, bCastShadow );
}

DARKSDK void GFFlushLightMapLights					( void )
{
	FlushLightMapLights();
}

DARKSDK void GFCreateLightMaps						( int iLMSize, int iLMQuality, LPSTR dwPathForLightMaps )
{
	CreateLightMaps	( iLMSize, iLMQuality, (DWORD)dwPathForLightMaps );
}

// Shadows

DARKSDK void GFSetGlobalShadowsOn					( void )
{
	SetGlobalShadowsOn();
}

DARKSDK void GFSetGlobalShadowsOff					( void )
{
	SetGlobalShadowsOff();
}

DARKSDK void GFSetShadowPosition ( int iMode, float fX, float fY, float fZ )
{
	SetShadowPosition (  iMode,  fX,  fY,  fZ );
}

DARKSDK void GFSetShadowColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	SetShadowColor (  iRed,  iGreen,  iBlue,  iAlphaLevel );
}

DARKSDK void GFSetShadowShades ( int iShades )
{
	SetShadowShades (  iShades );
}

// Others

DARKSDK void GFAddLODToObject ( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD )
{
	AddLODToObject ( iCurrentID, iLODModelID, iLODLevel, fDistanceOfLOD );
}
