#include "DBOBlock.h"

// Externals for DBO/Manager relationship
#include <vector>
extern std::vector< sMesh* >		g_vRefreshMeshList;
extern std::vector< sObject* >		g_vAnimatableObjectList;
extern int							g_iSortedObjectCount;
extern sObject**					g_ppSortedObjectList;

sBone::sBone ( )
{
	// clear out structure
	memset ( this, 0, sizeof ( sBone ) );
	
	// set values to 0 and pointers to null
	dwNumInfluences = 0;
	pVertices       = NULL;
	pWeights        = NULL;
}

sBone::~sBone ( )
{
	SAFE_DELETE_ARRAY ( pVertices );	// delete vertex array
	SAFE_DELETE_ARRAY ( pWeights );		// delete weights array
}

sTexture::sTexture ( )
{
	// clear out the structure
	memset ( this, 0, sizeof ( sTexture ) );

	// default texture settings
	dwBlendMode		= D3DTOP_SELECTARG1;
	dwBlendArg1		= D3DTA_DIFFUSE;
	dwBlendArg2		= D3DTA_DIFFUSE;
	dwAddressU		= D3DTADDRESS_WRAP;
	dwAddressV		= D3DTADDRESS_WRAP;
	dwMagState		= D3DTEXF_LINEAR;
	dwMinState		= D3DTEXF_LINEAR;
	dwMipState		= D3DTEXF_LINEAR;

	// U73 - 210309 - extra blending args
	dwBlendArg0		= D3DTA_DIFFUSE;
	dwBlendArgR		= D3DTA_CURRENT;
}

sTexture::~sTexture ( )
{
	SAFE_RELEASE ( pCubeTexture );
}

sCollisionData::sCollisionData ( )
{
	// clear out the structure
	memset ( this, 0, sizeof ( sCollisionData ) );

	bActive					= true;				// default box collision
	eCollisionType			= COLLISION_BOX;
	bBoundProduceResult		= true;
}

sCollisionData::~sCollisionData ( )
{

}

sDrawBuffer::sDrawBuffer ( )
{
	memset ( this, 0, sizeof ( sDrawBuffer ) );
}

sMesh::sMesh ( )
{
	// clear out the structure
	memset ( this, 0, sizeof ( sMesh ) );

	iPrimitiveType			= 4;		// default to triangle list
	iDrawVertexCount		= 0;
	iDrawPrimitives			= 0;

	bWireframe				= false;	// fill mesh with default render flags
	bLight					= true;
	bCull					= true;
	iCullMode				= 0;
	bFog					= true;
	bAmbient				= true;
	bTransparency			= false;
	dwAlphaTestValue		= 0;
	bGhost					= false;
	iGhostMode				= 2;
	bVisible				= true;
	bZRead					= true;
	bZWrite					= true;
	bZBiasActive			= false;
	fZBiasSlopeScale		= 0.0f;
	fZBiasDepth				= 0.0f;

	// mike 160505 - added ability to draw bounds of an individual mesh
	bDrawBounds				= false;

	bUsesMaterial			= false;	// default settings in case we use it (ie fade object)
	mMaterial.Diffuse.r		= 1.0f;
	mMaterial.Diffuse.g		= 1.0f;
	mMaterial.Diffuse.b		= 1.0f;
	mMaterial.Diffuse.a		= 1.0f;
	mMaterial.Ambient.r		= 1.0f;
	mMaterial.Ambient.g		= 1.0f;
	mMaterial.Ambient.b		= 1.0f;
	mMaterial.Ambient.a		= 1.0f;

	bLinked                 = false;
	bVertexTransform		= false;

	// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
	fMipMapLODBias			= 0.0f;		// default value in DX is 0.0f
}

sMesh::~sMesh ( )
{
	// ensure we remove this mesh from any refresh list as it wont be ther by the time we try to refresh it
	if ( !g_vRefreshMeshList.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < g_vRefreshMeshList.size(); ++iIndex )
        {
			// get mesh to refresh
            sMesh* pMesh = g_vRefreshMeshList [ iIndex ];
			if ( pMesh==this )
				g_vRefreshMeshList [ iIndex ] = NULL;
		}
	}

	// delete any shader effect
	if ( bVertexShaderEffectRefOnly==false )
	{
		SAFE_DELETE ( pVertexShaderEffect );

		// lee - 030114 - release vertex declaration if deleting mesh
		// leeconfused - 030715 - by this point the dec is invalid - commenting out solves LM crash issue
		//SAFE_RELEASE ( pVertexDec );
	}

	// delete all previously created memory
	SAFE_DELETE_ARRAY ( pOriginalVertexData );
	SAFE_DELETE_ARRAY ( pVertexData );
	SAFE_DELETE_ARRAY ( pIndices );
	SAFE_DELETE_ARRAY ( pBones );
	SAFE_DELETE_ARRAY ( pFrameRef );
	SAFE_DELETE_ARRAY ( pFrameMatrices );
	SAFE_DELETE_ARRAY ( pMaterialBank );
	SAFE_DELETE_ARRAY ( pTextures );

	SAFE_DELETE_ARRAY ( pMultiMaterial );
	SAFE_DELETE ( pDrawBuffer );
	SAFE_DELETE_ARRAY ( pAttributeWorkData );
}

sFrame::sFrame ( )
{
	// clear out structure
	memset ( this, 0, sizeof ( sFrame ) );

	// create identity matrices
	D3DXMatrixIdentity ( &matOriginal );
	D3DXMatrixIdentity ( &matUserMatrix );
	D3DXMatrixIdentity ( &matCombined );
	D3DXMatrixIdentity ( &matTransformed );
	D3DXMatrixIdentity ( &matAbsoluteWorld );

	// by default no bone assigned
	pmatBoneLocalTransform = NULL;

	// setup vectors and orientation data
	vecScale = D3DXVECTOR3 ( 1.0f, 1.0f, 1.f);

	// mike 170505 - new matrix for completely custom, physics needs this for implementing it's own matrix
	bOverride = false;
	D3DXMatrixIdentity ( &matOverride );

	// 201007 - each limb is not excluded by default
	bExcluded = false;

	// 211008 - u71 - clear state bits
	dwStatusBits = 0;
}

sFrame::~sFrame ( )
{
	// free all mesh data
	//SAFE_DELETE ( pMesh );
	//SAFE_DELETE ( pShadowMesh );
	//SAFE_DELETE ( pBoundBox );
	//SAFE_DELETE ( pBoundSphere );
	//SAFE_DELETE ( pLOD [ 0 ] );
	//SAFE_DELETE ( pLOD [ 1 ] );
	//SAFE_DELETE ( pLODForQUAD );
	// delete all pointers which will in turn delete all sub frames
	//SAFE_DELETE ( pChild );
	//SAFE_DELETE ( pSibling );

	// recursive delete causes stack overflow on large linklists, do old fashioned way
	sFrame* pThis = this;
	while ( pThis )
	{
		// free all mesh data
		SAFE_DELETE ( pThis->pMesh );
		SAFE_DELETE ( pThis->pShadowMesh );
		SAFE_DELETE ( pThis->pBoundBox );
		SAFE_DELETE ( pThis->pBoundSphere );
		SAFE_DELETE ( pThis->pLOD [ 0 ] );
		SAFE_DELETE ( pThis->pLOD [ 1 ] );
		SAFE_DELETE ( pThis->pLODForQUAD );

		// and next down to tackle children (if any)
		SAFE_DELETE ( pThis->pChild );

		// move to next one
		sFrame* pNext = pThis->pSibling;
		pThis->pSibling = NULL;
		if ( pThis!=this ) delete pThis;
		pThis = pNext;
	}
}

sAnimation::sAnimation ( )
{
	memset ( szName, 0, sizeof ( szName ) );

	pFrame            = NULL;
	bLoop             = FALSE;
	bLinear           = TRUE;
	pPositionKeys     = NULL;
	dwNumPositionKeys = 0;
	pRotateKeys       = NULL;
	dwNumRotateKeys   = 0;
	pScaleKeys        = NULL;
	dwNumScaleKeys    = 0;
	pMatrixKeys       = NULL;
	dwNumMatrixKeys   = 0;
	pNext             = NULL;

	dwLastPositionKey		= 0;
	dwLastRotateKey			= 0;
	dwLastScaleKey			= 0;
	dwLastMatrixKey			= 0;
	
	bBoneType				= 1;
	piBoneOffsetList		= NULL;
	iBoneOffsetListCount	= 0;
	ppBoneFrames			= NULL;
	iBoneFrameA				= 0;
	iBoneFrameB				= 0;

	pSharedReadAnim			= NULL;
}

sAnimation::~sAnimation ( )
{
	SAFE_DELETE_ARRAY ( piBoneOffsetList );

	if ( ppBoneFrames )
	{
		for ( int iFrame = 0; iFrame < iBoneFrameA; iFrame++ )
		{
			SAFE_DELETE_ARRAY ( ppBoneFrames [ iFrame ] );
		}
	}

	SAFE_DELETE_ARRAY ( ppBoneFrames );

	SAFE_DELETE ( pPositionKeys );
	SAFE_DELETE ( pRotateKeys );
	SAFE_DELETE ( pScaleKeys );
	SAFE_DELETE ( pMatrixKeys );
	SAFE_DELETE ( pNext );
}

sAnimationSet::sAnimationSet ( )
{
	memset ( this, 0, sizeof ( sAnimationSet ) );
}

sAnimationSet::~sAnimationSet ( )
{
	SAFE_DELETE		  ( pvecBoundMin );
	SAFE_DELETE		  ( pvecBoundMax );
	SAFE_DELETE		  ( pvecBoundCenter );
	SAFE_DELETE		  ( pfBoundRadius );
	SAFE_DELETE       ( pAnimation );
	SAFE_DELETE       ( pNext );
}

sPositionData::sPositionData ( )
{
	memset ( this, 0, sizeof ( sPositionData ) );

	vecPosition	= D3DXVECTOR3 ( 0, 0, 0 );		// default settings
	vecRotate	= D3DXVECTOR3 ( 0, 0, 0 );
	vecScale    = D3DXVECTOR3 ( 1, 1, 1 );

	vecLook     = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
	vecUp       = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
	vecRight    = D3DXVECTOR3 ( 1, 0, 0 );		// right vector

	bFreeFlightRotation		= false;			// default euler rotation
	bApplyPivot				= false;
	bGlued					= false;
	dwRotationOrder			= ROTORDER_XYZ;
	D3DXMatrixIdentity		( &matFreeFlightRotate );

	// Required for immediate limb data extraction (uses this matrix)
	D3DXMatrixIdentity ( &matWorld );

	// used for when world matrix is calculated manually
	bCustomWorldMatrix = false;
	bCustomBoneMatrix = false;
	bParentOfInstance = false;
}

sPositionData::~sPositionData ( )
{

}

sObject::sObject ( )
{
	pDelete					= NULL;
	iDeleteCount			= 0;
	iDeleteID				= 0;
	bDisableTransform		= false;
	fFOV					= 0.0f;
	iInsideUniverseArea		= -1;

	pInstanceOfObject		= NULL;
	pInstanceMeshVisible	= NULL;
	bInstanceAlphaOverride	= false;
	dwInstanceAlphaOverride	= 0;

	pFrame					= NULL;
	pAnimationSet			= NULL;
	iMeshCount				= 0;
	iFrameCount				= 0;
	ppMeshList				= NULL;
	ppFrameList				= NULL;

	bVisible				= true;
	bUniverseVisible		= true;
	bNoMeshesInObject		= false;
	bUpdateOverallBounds	= false;
	bUpdateOnlyCurrentFrameBounds = true;		// false 290506 - u62 - restored pre-U6 behaviour of boundbox

	bDrawBounds				= false;
	bCastsAShadow			= false;
	pShadowMesh				= NULL;
	bExcluded				= false;

	bOverlayObject			= false;
	bGhostedObject			= false;
	bTransparentObject		= false;
	bNewZLayerObject		= false;
	bLockedObject			= false;
	bStencilObject			= false;
	bReflectiveObject		= false;
	bReflectiveClipping		= false;
	bHadLODNeedCamDistance	= false;

	bAnimPlaying			= false;
	bAnimLooping			= false;
	bAnimUpdateOnce			= false;
	fAnimFrame				= 0.0f;
	fAnimSpeed				= 1.0f;
	fAnimLoopStart			= 0.0f;
	fAnimFrameEnd			= 0.0f;
	fAnimTotalFrames		= 0.0f;

	bAnimManualSlerp		= false;
	fAnimSlerpStartFrame	= 0.0f;
	fAnimSlerpEndFrame		= 0.0f;
	fAnimSlerpTime			= 0.0f;
	fAnimInterp				= 1.0f;

	bStatic						= false;
	bUsesItsOwnBuffers			= true;			// can be changed in ObjSetup (bGlobalVBIBUsageFlag)
	bReplaceObjectFromBuffers	= false;

	bVeryEarlyObject			= false;		// leeadd - U71 - better for eary skybox drawing

	pfAnimLimbFrame				= NULL;			// U75 - 240909 - clear mem ptr to limb based animation control

	fLODDistance[0]				= 0;
	fLODDistance[1]				= 0;
	fLODDistanceQUAD			= 0;
	iUsingWhichLOD				= 0;
	iUsingOldLOD				= -1;
	fLODTransition				= 0.0f;

	dwCameraMaskBits			= 4294967295;	// lee - 211006 - u63 - for darkshader

	pObjectDependency			= NULL; // lee - 110310 - initialise IamM's dependency vars
	dwDependencyCount			= 0;

	pCharacterCreator			= NULL; // Dave 070515 - for character creator

	pCustomData					= NULL;
	dwCustomSize				= 0;
}

sObject::~sObject ( )
{
	// lee - 021014 - if removing an object, ensure it's also removed from any temp shortlist
	if ( g_ppSortedObjectList )
		for ( int iIndex = 0; iIndex < g_iSortedObjectCount; iIndex++ )
			if ( g_ppSortedObjectList[ iIndex ]==this )
				g_ppSortedObjectList[ iIndex ] = NULL;

	// lee - 290306 - u6rc3 - clear rogue meshes from meshlist
	// for each mesh reference, clear ptr if mesh found in frame list
	for ( int m=0; m<(int)iMeshCount; m++ )
		for ( int f=0; f<(int)iFrameCount; f++ )
			if ( ppFrameList [ f ]->pMesh==ppMeshList [ m ] )
				ppMeshList [ m ] = NULL;
	
	// delete any previously allocate memory
	SAFE_DELETE		  ( pInstanceMeshVisible );
	SAFE_DELETE       ( pFrame );
	SAFE_DELETE       ( pAnimationSet );
	SAFE_DELETE       ( pCharacterCreator );
	SAFE_DELETE_ARRAY       ( ppFrameList );

	// lee - 290306 - u6rc3 - command like MAKE OBJECT FROM LIMB can store meshes without frame links
	for ( int m=0; m<(int)iMeshCount; m++ )
		if ( ppMeshList [ m ] )
			SAFE_DELETE ( ppMeshList [ m ] );

	// finally clear mesh reference list
	SAFE_DELETE_ARRAY ( ppMeshList );

	// U75 - 240909 - free if reserved this during callto SET OBJET FRAME (limb)
	SAFE_DELETE ( pfAnimLimbFrame );
}

sObjectCustom::sObjectCustom ( )
{
	// 280305 - used when objects want to store custom data for example when
	//        - they save and want to save out this data

	dwCustomSize = 0;
	pCustomData  = NULL;

}

// 310305 - destructor
sObjectCustom::~sObjectCustom ( )
{
	SAFE_DELETE_ARRAY ( pCustomData );
}

// 310305 - mike - destructor needed
sObjectDelete::~sObjectDelete ( )
{
	SAFE_DELETE_ARRAY ( pDelete );
}