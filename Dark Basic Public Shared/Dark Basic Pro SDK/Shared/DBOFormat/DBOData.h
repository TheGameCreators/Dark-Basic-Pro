
#ifndef _DBODATA_H_
#define _DBODATA_H_

//////////////////////////////////////////////////////////////////////////////////////
// COMMON INCLUDES ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "..\global.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/*
#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define SAFE_MEMORY(p)			{ if ( p == NULL ) return false; }
#define MAX_STRING				256
*/

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

struct sCollisionData;		// collision data
struct sObject;				// dbo object
struct sMesh;				// mesh
struct sFrame;				// frame
struct sTexture;			// texture
struct sMeshGroup;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// TYPEDEFS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// delete callback - used for external DLLs
typedef void ( *ON_OBJECT_DELETE_CALLBACK ) ( int Id, int userData );

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// SPECIAL EFFECTS ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

struct sEffectConstantData
{
	// effect data

	D3DXMATRIX		matWorld;
	D3DXMATRIX		matView;
	D3DXMATRIX		matProj;
	D3DXMATRIX		matWorldView;
	D3DXMATRIX		matViewProj;
	D3DXMATRIX		matWorldViewProj;
	D3DXMATRIX		matWorldViewInv;
	D3DXMATRIXA16	matWorldInv;
	D3DXMATRIXA16	matViewInv;

	D3DXMATRIX		matWorldT;
	D3DXMATRIX		matViewT;
	D3DXMATRIX		matProjT;
	D3DXMATRIX		matWorldViewT;
	D3DXMATRIX		matViewProjT;
	D3DXMATRIX		matWorldViewProjT; // 270515 - reclassifying this to ViewProjInv (as matWorldViewProjT is never used?!)
	D3DXMATRIX		matWorldViewInvT;
	D3DXMATRIXA16	matWorldInvT;
	D3DXMATRIXA16	matViewInvT;

	D3DXVECTOR4		vecLightDir;
	D3DXVECTOR4		vecLightDirInv;
	D3DXVECTOR4		vecLightPos;
	float			fLightRange;

	D3DXVECTOR4		vecEyePos;
	D3DXVECTOR4		vecCameraPosition;

	float			fAlphaOverride;

	float			fBoneCount;
	D3DXMATRIX		matBoneMatrixPalette[256];

	D3DXVECTOR4		vecUVScaling;

	DWORD			dwReserved1;
	DWORD			dwReserved2;
	DWORD			dwReserved3;
	DWORD			dwReserved4;
	DWORD			dwReserved5;
	DWORD			dwReserved6;
	DWORD			dwReserved7;
	DWORD			dwReserved8;
};

class cSpecialEffect
{
	// base class for special effects

	public:
		LPD3DXEFFECT		m_pEffect;
		char				m_pEffectName [ MAX_STRING ];

		D3DXEFFECT_DESC		m_EffectDesc;
		bool				m_bTranposeToggle;

		D3DXHANDLE			m_MatWorldEffectHandle;
		D3DXHANDLE			m_MatViewEffectHandle;
		D3DXHANDLE			m_MatProjEffectHandle;
		D3DXHANDLE			m_MatWorldViewEffectHandle;
		D3DXHANDLE			m_MatViewProjEffectHandle;
		D3DXHANDLE			m_MatWorldViewProjEffectHandle;
		D3DXHANDLE			m_MatWorldInverseEffectHandle;
		D3DXHANDLE			m_MatViewInverseEffectHandle;
		D3DXHANDLE			m_MatWorldViewInverseEffectHandle;

		D3DXHANDLE			m_MatWorldTEffectHandle;
		D3DXHANDLE			m_MatViewTEffectHandle;
		D3DXHANDLE			m_MatProjTEffectHandle;
		D3DXHANDLE			m_MatWorldViewTEffectHandle;
		D3DXHANDLE			m_MatViewProjTEffectHandle;
		D3DXHANDLE			m_MatWorldViewProjTEffectHandle;
		D3DXHANDLE			m_MatWorldInverseTEffectHandle;
		D3DXHANDLE			m_MatViewInverseTEffectHandle;
		D3DXHANDLE			m_MatWorldViewInverseTEffectHandle;

		D3DXHANDLE			m_LightDirHandle;
		D3DXHANDLE			m_LightDirInvHandle;
		D3DXHANDLE			m_LightPosHandle;
		D3DXHANDLE			m_VecCameraPosEffectHandle;
		D3DXHANDLE			m_VecEyePosEffectHandle;
		D3DXHANDLE			m_AlphaOverrideHandle;

		D3DXHANDLE			m_TimeEffectHandle;
		D3DXHANDLE			m_SinTimeEffectHandle;
		//D3DXHANDLE		m_fMeshRadius; // gave it's life for m_VecClipPlaneEffectHandle
		D3DXHANDLE			m_VecClipPlaneEffectHandle; // 270111, then 140411 (this added 4 bytes to DBO, messes up all plugins)

		D3DXHANDLE			m_BoneCountHandle;
		D3DXHANDLE			m_BoneMatrixPaletteHandle;

		D3DXHANDLE			m_DeltaTimeEffectHandle;
		D3DXHANDLE			m_UVScalingHandle;
		//D3DXHANDLE			m_ReservedHandle3;
		//D3DXHANDLE			m_ReservedHandle4;
		//D3DXHANDLE			m_ReservedHandle5;
		//D3DXHANDLE			m_ReservedHandle6;
		D3DXHANDLE			m_ColorTone[4];
		D3DXHANDLE			m_ReservedHandle7;
		D3DXHANDLE			m_ReservedHandle8;

		LPSTR				m_pDefaultXFile;
		BOOL				m_bGenerateNormals;

		// flags that specify what the effect requires
		BOOL				m_bUsesNormals;
		BOOL				m_bUsesDiffuse;
		BOOL				m_bUsesTangents;
		BOOL				m_bUsesBinormals;
		BOOL				m_bUsesBoneData;

		// store texture ptrs from loaded effect
		bool				m_bUseShaderTextures;
		DWORD				m_dwTextureCount;
		int					m_iParamOfTexture[32];
		sMesh*				m_pXFileMesh;

		DWORD				m_dwRTTexCount;
		LPDIRECT3DBASETEXTURE9 m_pRTTex[32];							// leeadd - 080414 - Needed to track allocations made by shader (so can free them later)

		// reserved members
		DWORD				m_bDoNotGenerateExtraData;					// leeadd - 050906 - Flag passed in to control if FX data auto-generated (tangents/binormals/normals)
		DWORD				m_dwUseDynamicTextureMask;					// leeadd - 180107 - Whether dynamic effect from TEXTURE OBJECT should apply to an effect that is using its own textures
		bool				m_bUsesAtLeastOneRT;						// leeadd - 200310 - support for RTs in shader
		bool				m_bPrimaryEffectForCascadeShadowMapping;	// leeadd - 250913 - cascade shadow mapping (texture slots 4,5,6,7 overwritten)
		//bool				m_bRes3c;									// reserved - maintain plugin compat.
		bool				m_bCharacterCreatorTonesOn;
		bool				m_bRes3d;									// reserved - maintain plugin compat.
		DWORD				m_dwCreatedRTTextureMask;					// leeadd - 200310 - support for RTs in shader

		// additional information on shader
		DWORD				m_dwEffectIndex;
		D3DXHANDLE			m_GlowIntensityHandle;						// leeadd - 170315 - holds the param for any GlowIntensity param							

		// DAVE, this is a MAJOR NONO, you changed the size and offset of this structure which means EVERY DBP DLL everywhere becomes wrong!
		// and I am moving these allocations into pre-created reserve members (above and below) to fix the new LM crash issue...
		//D3DXHANDLE			m_ColorTone[4];								// daveadd - 080515 - holds the param for character creator color tones
		//D3DXHANDLE			m_ToneMix[4];								// daveadd - 080515 - holds the param for character creator color tone mixes
		//bool				m_bCharacterCreatorTonesOn;					// daveadd - 090515 - so we only need to turn the effect off once rather than every mesh

		// reserved members
		//DWORD				m_dwReserved3;								
		//DWORD				m_dwReserved4;								// reserved
		//DWORD				m_dwReserved5;								// reserved
		//DWORD				m_dwReserved6;								// reserved
		D3DXHANDLE			m_ToneMix[4];
		DWORD				m_dwReserved7;								// reserved
		DWORD				m_dwReserved8;								// reserved

		// virtual functions
		cSpecialEffect ( );
		virtual ~cSpecialEffect ( );
		virtual bool	Load					( LPSTR pEffectFile, bool bUseXFile, bool bUseTextures );
		virtual bool	Setup					( sMesh* pMesh );
		virtual void	Mesh					( sMesh* pMesh );
		virtual DWORD	Start					( sMesh* pMesh, D3DXMATRIX matObject );
		virtual void	End						( void );

		// base functions
		bool			AssignValueHookCore		( LPSTR pName, D3DXHANDLE hParam, DWORD dwClass, bool bRemove );
		bool			AssignValueHook			( LPSTR pName, D3DXHANDLE hParam );
		bool			CorrectFXFile			( LPSTR pFile, LPSTR pModifiedFile );
		bool			ParseEffect				( bool bUseEffectXFile, bool bUseEffectTextures );
		void			ApplyEffect				( sMesh* pMesh );
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

enum eCollisionType
{
	// collision types

			COLLISION_NONE,		// none
			COLLISION_SPHERE,	// sphere
			COLLISION_BOX,		// box
			COLLISION_POLYGON	// polygon
};

enum eRotationOrders
{
	// order of rotation

			ROTORDER_XYZ,		// x y z
			ROTORDER_ZYX,		// z y x
			ROTORDER_YZX,		// y z x
			ROTORDER_ZXY		// z x y
};

struct sOffsetMap
{
	// stores a list of offsets to each part of fvf data

	DWORD	dwSize,				// size of data
			dwByteSize,			// size in bytes
			dwFVF,				// actual fvf
			dwX,				// x offset
			dwY,				// y offset
			dwZ,				// z offset
			dwRWH,				// rhw offset
			dwBlend1,			// blend 1 offset
			dwBlend2,			// blend 2 offset
			dwBlend3,			// blend 3 offset
			dwNX,				// normal x offset
			dwNY,				// normal y offset
			dwNZ,				// normal z offset
			dwPointSize,		// point size offset
			dwDiffuse,			// diffuse offset
			dwSpecular,			// specular offset
			dwTU[8],			// tu offsets
			dwTV[8],			// tv offsets
			dwTZ[8],			// tz offsets
			dwTW[8];			// tw offsets
};

struct sBone
{
	// stores bone data
	char							szName [ MAX_STRING ];						// bone name
	DWORD							dwNumInfluences;							// how many vertices does this affect
	DWORD*							pVertices;									// affect vertices - stores indices to link to
	float*							pWeights;									// bone weights
	D3DXMATRIX						matTranslation;								// translation matrix

	// reserved members
	DWORD							dwReservedB1;								// reserved - maintain plugin compat.
	DWORD							dwReservedB2;								// reserved - maintain plugin compat.
	DWORD							dwReservedB3;								// reserved - maintain plugin compat.
	
	// constructor and destructor
	sBone  ( );
	~sBone ( );
};

struct sTexture
{
	// texture information
	char							pName [ MAX_STRING ];						// image filename
	int								iImageID;									// image id
	LPDIRECT3DTEXTURE9				pTexturesRef;								// reference to image texture
	LPDIRECT3DCUBETEXTURE9			pCubeTexture;								// cube texture reference
	DWORD							dwStage;									// can change stage order of texture
	DWORD							dwBlendMode;								// blending mode for multitexturing
	DWORD							dwBlendArg1;								// blend argument 1
	DWORD							dwBlendArg2;								// blend argument 2
	DWORD							dwAddressU;									// texture wrap mode settings
	DWORD							dwAddressV;									// texture wrap settings
	DWORD							dwMagState;									// texture filter and mipmap modes
	DWORD							dwMinState;									// min state
	DWORD							dwMipState;									// mip state
	DWORD							dwTexCoordMode;								// texture coordinate index mode
	int								iStartIndex;								// start index
	int								iPrimitiveCount;							// number of primitives

	// lerping
	DWORD							dwBlendArg0;								// U73 - 210309 - extra blend mapping support
	DWORD							dwBlendArgR;								// for lerping textures (arg0 and result)

	// reserved members
	DWORD							dwReservedT1;
	DWORD							dwReservedT2;
	DWORD							dwReservedT3;

	// constructor and destructor
	sTexture  ( );
	~sTexture ( );
};

struct sCollisionData
{
	// contains a list of all collision data ( each mesh will store one of these )
	D3DXVECTOR3						vecMin;										// origin bounding box min extents
	D3DXVECTOR3						vecMax;										// origin bounding box max extents
	D3DXVECTOR3						vecCentre;									// origin centre of object
	float							fRadius;									// origin sphere radius
	float							fScaledRadius;								// precomputed for main object radius
	float							fLargestRadius;								// precomputed all anim combos for culling
	float							fScaledLargestRadius;						// precomputed all anim combos for scaled culling

	// collision information for run-time processing
	bool							bActive;									// whether collision is on or off
	bool							bHasBeenMovedForResponse;					// flagged when entity is moved
	bool							bColCenterUpdated;							// collision center moves when object rotates
	D3DXVECTOR3						vecColCenter;								// realtime collision center

	// collision method
	DWORD							eCollisionType;								// collision type
	bool							bFixedBoxCheck;								// non-rotated box collision
	bool							bUseBoxCollision;							// flagged to use box collision
	int								iHitOverlapStore;							// stores when 'strike hit' occurs
	int								iResponseMode;								// how the collision produces a result
	bool							bBoundProduceResult;						// bound produces result???

	// special collision assist values
	DWORD							dwArbitaryValue;							// can use used to store a value (material ID for sound/ground effect)
	DWORD							dwPortalBlocker;							// assists in determining if mesh should block a created portal (cosmetic geometry)

	// reserved members
	DWORD							dwCollisionPropertyValue;					// state of collision behaviour (used externally to give object a collision prroperty)
	DWORD							dwReservedCD2;								// reserved - maintain plugin compat.
	DWORD							dwReservedCD3;								// reserved - maintain plugin compat.
	
	// constructor and destructor
	sCollisionData  ( );
	~sCollisionData ( );
};

struct sDrawBuffer
{
	// draw buffer data
	IDirect3DVertexBuffer9*			pVertexBufferRef;							// pointer to VB used
	IDirect3DIndexBuffer9*			pIndexBufferRef;							// pointer to IB used
	D3DPRIMITIVETYPE				dwPrimType;									// render prim type
	DWORD							dwVertexStart;								// start location for vertices in buffer
	DWORD							dwVertexCount;								// number of vertices used by this buffer
	DWORD							dwIndexStart;
	DWORD							dwPrimitiveCount;							// required to determine actual prims to draw
	DWORD							dwFVFSize;									// size of the stream source
	DWORD							dwBaseVertexIndex;							// used to offset vertex base index (setindices)
	LPVOID							pVBListEntryRef;							// ref to obj-manager VB list item
	LPVOID							pIBListEntryRef;							// ref to obj-manager IB list item
	DWORD							dwImmuneToForeColorWipe;					// U75 - 070410 - (was RES1) set by SET OBJECT MASK to allow objects to escape being wiped by COLOR BACKDROP c,b,FORECOLOR

	// reserved members
	DWORD							dwReservedDB1;								// reserved - maintain plugin compat.
	DWORD							dwReservedDB2;								// reserved - maintain plugin compat.
	DWORD							dwReservedDB3;								// reserved - maintain plugin compat.
	
	// constructor
	sDrawBuffer ( );
};

struct sMultiMaterial
{
	// multiple materials
	char							pName [ MAX_STRING ];						// image filename
	LPDIRECT3DTEXTURE9				pTexturesRef;								// reference to image texture
	D3DMATERIAL9					mMaterial;									// unique material information
	DWORD							dwIndexStart;								// start of index data for this material
	DWORD							dwIndexCount;								// quantity of index data items
	DWORD							dwPolyCount;								// quantity of polygons to draw
};

struct sMeshMultipleAnimation
{
	// multiple animation information
	sFrame*							pSubFrameList;								// sub frames
	DWORD							dwSubMeshListCount;							// number of sub frames
	int								iCurrentFrame;								// current frame
	int								iNextFrame;									// next frame
	int								iLastFrame;									// last frame
	float							fLastInterp;								// last interpolation value
	float							fInterp;									// interpolation
	DWORD							dwLastTime;									// last time
	DWORD							dwThisTime;									// current time
	bool							bLinked;									// linked flag

	// reserved members
	DWORD							dwReservedMMA1;								// reserved - maintain plugin compat.
	DWORD							dwReservedMMA2;								// reserved - maintain plugin compat.
	DWORD							dwReservedMMA3;								// reserved - maintain plugin compat.
};

struct sMeshFVF
{
	// mesh flexible vertex format data
	DWORD							dwFVFOriginal;								// flexible vertex format original
	DWORD							dwFVF;										// flexible vertex format
	DWORD							dwFVFSize;									// size of flexible vertex format
};

struct sMeshDraw
{
	// mesh draw information
	int								iMeshType;									// put it to handle terrain scene culling (mesh visible flag)
	BYTE*							pOriginalVertexData;						// pointer to original vertex data
	BYTE*							pVertexData;								// pointer to vertex data
	WORD*							pIndices;									// index array
	DWORD							dwVertexCount;								// number of vertices
	DWORD							dwIndexCount;								// number of indices
	int								iPrimitiveType;								// primitive type
	int								iDrawVertexCount;							// number of vertices to be used when drawing
	int								iDrawPrimitives;							// number of actual primitives to draw
	sDrawBuffer*					pDrawBuffer;								// VB and IB buffers used by the mesh

	// reserved members
	DWORD							dwReservedMD1;								// reserved - maintain plugin compat.
	DWORD							dwReservedMD2;								// reserved - maintain plugin compat.
	DWORD							dwReservedMD3;								// reserved - maintain plugin compat.
};

struct sMeshShader
{
	// shader data
	bool							bUseVertexShader;							// flag to control vertex shader
	bool							bOverridePixelShader;						// pixel shader on or off
	bool							bVertexShaderEffectRefOnly;
	D3DVERTEXELEMENT9				pVertexDeclaration [ MAX_FVF_DECL_SIZE ];	// for custom FVFs
	LPDIRECT3DVERTEXSHADER9			pVertexShader;								// vertex shader
	LPDIRECT3DVERTEXDECLARATION9	pVertexDec;									// vertex shader dec
	LPDIRECT3DPIXELSHADER9			pPixelShader;								// pixel shader handle
	cSpecialEffect*					pVertexShaderEffect;						// built-in shader effect ptr
	char							pEffectName [ MAX_STRING ];
	DWORD							dwForceCPUAnimationMode;					// 200111 - force CPU to take over bone animation (for ray cast intersect detection even if GPU using bone animation)

	// reserved members
	float							fUVScalingU;								// dwReservedMS1 - maintain plugin compat.
	float							fUVScalingV;								// dwReservedMS2 - maintain plugin compat.
	DWORD							dwReservedMS3;								// reserved - maintain plugin compat.
};

struct sMeshBones
{
	// mesh bones
	sBone*							pBones;										// array of bones for mesh
	DWORD							dwBoneCount;								// number of bones in mesh
	sFrame**						pFrameRef;									// stores reference to original frame representing bone (used for limb-based bone collision)
	D3DXMATRIX**					pFrameMatrices;								// list of all matrices used for this mesh ( in conjunction with bones )
};

struct sMeshTexture
{
	// mesh texture
	bool							bUsesMaterial;								// flag to skip material use
	bool							bAlphaOverride;								// flag to alpha override
	bool							bUseMultiMaterial;							// for multi-material models (bone based type for now)
	DWORD							dwTextureCount;								// number of textures in list
	DWORD							dwMultiMaterialCount;						// size of multimaterial array
	DWORD							dwAlphaOverride;							// tfactor alpha override (true object transparency)
	sTexture*						pTextures;									// texture list
	sMultiMaterial*					pMultiMaterial;								// multimaterial array
	D3DXMATERIAL*					pMaterialBank;								// temp store for all materials of a single mesh
	D3DMATERIAL9					mMaterial;									// unique material information
	DWORD*							pAttributeWorkData;							// stores work data when a mesh has multiple materials

	// reserved members
	DWORD							dwReservedMT1;								// reserved - maintain plugin compat.
	DWORD							dwReservedMT2;								// reserved - maintain plugin compat.
	DWORD							dwReservedMT3;								// reserved - maintain plugin compat.
};

struct sMeshInternalProperties
{
	// object maintainance and management
	bool							bAddObjectToBuffers;						// flagged when mesh needs to be added to buffers
	bool							bVBRefreshRequired;							// flagged when vertex data modified
	bool							bMeshHasBeenReplaced;						// shaders can change a mesh (buffer would be replaced if so)
	bool							bVertexTransform;							// have vertices been transformed
	DWORD							dwDrawSignature;							// draw information
	bool							bShaderBoneSkinning;						// indicates if Vertex Shader has taken over bone animation work
	int								iSolidForVisibility;						// filled when mesh used as part of nodetree visibility system
	int								iCastShadowIfStatic;						// occluder shadow value (written by ADD STATIC)
	union
	{
		DWORD						dwMeshID;
	};

	// reserved members
	DWORD							dwReservedMIP1;								// reserved - maintain plugin compat.
	DWORD							dwReservedMIP2;								// reserved - maintain plugin compat.
	DWORD							dwReservedMIP3;								// reserved - maintain plugin compat.
};

struct sMeshExternalProperties
{
	// these store the individual render states of the mesh
	bool							bWireframe;									// render state flags for each mesh
	bool							bLight;										// lighting on / off
	bool							bCull;										// culling on / off
	bool							bFog;										// fog on / off
	bool							bAmbient;									// respond to ambient
	bool							bTransparency;								// transparency on / off
	DWORD							dwAlphaTestValue;							// used to restrict what is rendered by alpha value
	bool							bGhost;										// is ghosting on
	bool							bVisible;									// is object visible
	bool							bZRead;										// z buffer
	bool							bZWrite;									// z write
	int								iGhostMode;									// ghost mode index
	bool							bZBiasActive;								// active when using some zbias
	float							fZBiasSlopeScale;							// how much of object to add to Z (0.0-1.0)
	float							fZBiasDepth;								// definitively add a value to Z
	int								iCullMode;									// 1-CCW 2-CW - lee - 040306 - u6rc5 - solve import issue for some CW models
	bool							bDrawBounds;								// to hide/show the bounds geometry - mike 160505 - added ability to draw bounds of an individual mesh
	float							fMipMapLODBias;								// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis

	// reserved members
	DWORD							dwReservedMEP1;								// reserved - maintain plugin compat.
	DWORD							dwReservedMEP2;								// reserved - maintain plugin compat.
	DWORD							dwReservedMEP3;								// reserved - maintain plugin compat.
	DWORD							dwReservedMEP4;								// reserved - maintain plugin compat.
	DWORD							dwReservedMEP5;								// reserved - maintain plugin compat.
};

struct sMesh : public sMeshFVF,
					  sMeshDraw,
					  sMeshShader,
					  sMeshBones,
					  sMeshTexture,
					  sMeshInternalProperties,
					  sMeshExternalProperties,
					  sMeshMultipleAnimation
{
	// contains all data for a mesh
	sCollisionData					Collision;									// collision information

	// reserved members
	DWORD							dwTempFlagUsedDuringUniverseRayCast;		// V119 to fix bugged universe ray cast of large meshes
	DWORD							dwReservedM2;								// reserved - maintain plugin compat.
	DWORD							dwReservedM3;								// reserved - maintain plugin compat.

	// constructor and destructor
	sMesh  ( );
	~sMesh ( );
};

struct sMeshGroup
{
	sMesh* pMesh;
};

struct sFrameID
{
	// frame ID
	char							szName [ MAX_STRING ];						// name
	int								iID;										// local index of frame in build list
};

struct sFrameLinks
{
	// frame links - parent, child and sibling
	sFrame*							pParent;									// parent
	sFrame*							pChild;										// child
	sFrame*							pSibling;									// sibling
};

struct sFrameTransforms
{
	// frame transforms
	D3DXMATRIX						matOriginal;								// original matrix
	D3DXMATRIX						matTransformed;								// realtime transformed matrix
	D3DXMATRIX						matCombined;								// realtime combined matrix
	D3DXMATRIX						matAbsoluteWorld;							// includes object world (absolute world)

	// mike 170505 - new matrix for completely custom, physics needs this for implementing it's own matrix
	bool							bOverride;									// flag to override
	D3DXMATRIX						matOverride;								// override matrix

	// reserved members
	DWORD							dwReservedFT1;								// reserved - maintain plugin compat.
	DWORD							dwReservedFT2;								// reserved - maintain plugin compat.
	DWORD							dwReservedFT3;								// reserved - maintain plugin compat.
};

struct sFramePosition
{
	// frame position data
	D3DXMATRIX						matUserMatrix;								// local frame matrix
	D3DXVECTOR3						vecOffset;									// local frame orientation
	D3DXVECTOR3						vecScale;									// scale
	D3DXVECTOR3						vecRotation;								// rotation
	D3DXVECTOR3						vecPosition;								// realtime update
	D3DXVECTOR3						vecDirection;								// realtime update
	bool							bVectorsCalculated;							// realtime update flag

	// reserved members
	D3DXVECTOR3						vecReservedFP1;								// reserved - maintain plugin compat.
	D3DXVECTOR3						vecReservedFP2;								// reserved - maintain plugin compat.
	D3DXVECTOR3						vecReservedFP3;								// reserved - maintain plugin compat.
};

struct sFrame : public sFrameID,
					   sFrameLinks,
					   sFrameTransforms,
					   sFramePosition
{
	// base meshes
	sMesh*							pMesh;										// basic mesh data for frame (optional)	
	sMesh*							pShadowMesh;								// created to render a shadow (optional)	
	sMesh*							pBoundBox;									// created to view bound box (optional)	
	sMesh*							pBoundSphere;								// created to view bound sphere (optional)
	sMesh*							pLOD [ 2 ];									// created to hold LOD[0] and LOD[1] model
	bool							bExcluded;									// 301007 - new exclude limb feature

	// reserved bools
	bool							bReservedFa;								// 290808 - added these 3 BOOLS in (as Bool=1 & DWORD=4)
	bool							bReservedFb;								// 
	bool							bReservedFc;								// 

	// quad handing
	DWORD							dwStatusBits;								// 211008 - u71 - stores 32 bits of info ( %1-shift object bounds by frame zero )
	sMesh*							pLODForQUAD;								// 061208 - U71 - quad at end of LOD chain

	// reserved members
	D3DXMATRIX*						pmatBoneLocalTransform;						// reserved - maintain plugin compat.
	DWORD							dwReservedF2;								// reserved - maintain plugin compat.
	DWORD							dwReservedF3;								// reserved - maintain plugin compat.
	DWORD							dwReservedF4;								// reserved - maintain plugin compat.
	DWORD							dwReservedF5;								// reserved - maintain plugin compat.

	// constructor and destructor
	sFrame  ( );
	~sFrame ( );
};

struct sXFileRotateKey
{
	// x file rotate key
	DWORD							dwTime;										// time value
	DWORD							dwFloats;									// floats
	float							w;											// w rotate
	float							x;											// x rotate
	float							y;											// y rotate
	float							z;											// z rotate
};

struct sXFileScaleKey
{
	// x file scale key
	DWORD							dwTime;										// time value
	DWORD							dwFloats;									// floats
	D3DXVECTOR3						vecScale;									// scale value
};

struct sXFilePositionKey
{
	// x file position key
	DWORD							dwTime;										// time value
	DWORD							dwFloats;									// floats
	D3DXVECTOR3						vecPos;										// position
};

struct sXFileMatrixKey
{
	// x file matrix key
	DWORD							dwTime;										// time
	DWORD							dwFloats;									// floats
	D3DXMATRIX						matMatrix;									// matrix to be applied
};

struct sRotateKey
{
	// rotate key
	DWORD							dwTime;										// time value
	D3DXQUATERNION					Quaternion;									// quaternion
};

struct sPositionKey
{
	// position key
	DWORD     						dwTime;										// time value
	D3DXVECTOR3						vecPos;										// position
	D3DXVECTOR3						vecPosInterpolation;						// interpolation
};

struct sScaleKey
{
	// scale key
	DWORD							dwTime;										// time value
	D3DXVECTOR3						vecScale;									// scale
	D3DXVECTOR3						vecScaleInterpolation;						// interpolation
};

struct sMatrixKey
{
	// matrix key
	DWORD							dwTime;										// time
	D3DXMATRIX						matMatrix;									// matrix
	D3DXMATRIX						matInterpolation;							// interpolation
};

struct sAnimationID
{
	// animation ID
	char							szName [ MAX_STRING ];						// name
	sFrame*							pFrame;										// pointer to frame
};

struct sAnimationProperties
{
	// animation properties
	BOOL							bLoop;										// is animation looping
	BOOL							bLinear;									// is animation linear
};

struct sAnimationKeys
{
	// animation keys
	DWORD							dwNumPositionKeys;							// number of position keys
	DWORD							dwNumRotateKeys;							// number of rotation keys
	DWORD							dwNumScaleKeys;								// number of scale keys
	DWORD							dwNumMatrixKeys;							// number of matrix keys
	sPositionKey*					pPositionKeys;								// position keys
	sRotateKey*						pRotateKeys;								// rotate keys
	sScaleKey*						pScaleKeys;									// scale keys
	sMatrixKey*						pMatrixKeys;								// and finally matrix keys

	DWORD							dwLastPositionKey;							// keep track of last actual key used for each type
	DWORD							dwLastRotateKey;
	DWORD							dwLastScaleKey;
	DWORD							dwLastMatrixKey;
};

struct sAnimationExtraBones
{
	// extra bone data for certain types of animations, will be removed in future
	// and replaced
	DWORD							bBoneType;									// type of bone
	int*							piBoneOffsetList;							// offset list
	int								iBoneOffsetListCount;						// count of offset list
	D3DXMATRIX**					ppBoneFrames;								// matrix frame
	int								iBoneFrameA;								// index a
	int								iBoneFrameB;								// index b
};

struct sAnimation : public sAnimationID,
						   sAnimationProperties,
						   sAnimationKeys,
						   sAnimationExtraBones
{
	// final animation structure
	sAnimation*						pSharedReadAnim;							// if erase any anim data (for clone-shared use), this points to clone anim data
	sAnimation*						pNext;										// pointer to next animation block

	// reserved members
	DWORD							dwReservedA1;								// reserved - maintain plugin compat.
	DWORD							dwReservedA2;								// reserved - maintain plugin compat.
	DWORD							dwReservedA3;								// reserved - maintain plugin compat.

	// constructor and destructor
	sAnimation  ( );
	~sAnimation ( );
};

struct sAnimationSet
{
	// stores a list of animations
	char							szName [ MAX_STRING ];						// animation name
	sAnimation*						pAnimation;									// pointer to data
	DWORD							ulLength;									// length of animation
	sAnimationSet*					pNext;										// next set

	// extra data to store dynamic boundboxes from animation
	D3DXVECTOR3*					pvecBoundMin;								// pre-calculated bound boxes per anim frame
	D3DXVECTOR3*					pvecBoundMax;
	D3DXVECTOR3*					pvecBoundCenter;
	float*							pfBoundRadius;

	// reserved members
	DWORD							dwReservedAS1;								// reserved - maintain plugin compat.
	DWORD							dwReservedAS2;								// reserved - maintain plugin compat.
	DWORD							dwReservedAS3;								// reserved - maintain plugin compat.

	// constructor and destructor
	sAnimationSet  ( );
	~sAnimationSet ( );
};

struct sPositionMatrices
{
	// matrices for position
	D3DXMATRIX						matTranslation,								// translation ( position )
									matRotation,								// final rotation matrix
									matRotateX,									// x rotation
									matRotateY,									// y rotation
									matRotateZ,									// z rotation
									matScale,									// scale
									matObjectNoTran,							// final world without translation (collision)
									matWorld,									// final world matrix
									matFreeFlightRotate,						// free flight matrix rotation
									matPivot;									// pivot

	// reserved
	D3DXMATRIX						matReservedPM1;								// reserved - maintain plugin compat.
	D3DXMATRIX						matReservedPM2;								// reserved - maintain plugin compat.
	D3DXMATRIX						matReservedPM3;								// reserved - maintain plugin compat.
};

struct sPositionProperties
{
	// position properties
	bool							bFreeFlightRotation;						// flag for euler/freeflight
	bool							bApplyPivot;								// pivot
	bool							bGlued;										// glue
	bool							bCustomWorldMatrix;							// used for when world matrix is calculated manually
	int		 						iGluedToObj;								// glued to object
	int		 						iGluedToMesh;								// glued to mesh (mode 1 when negative)
	DWORD							dwRotationOrder;							// euler rotation order
	float							fCamDistance;								// used for depth sorting
	bool							bCustomBoneMatrix;							// used by darkphysics (ragdoll)
	bool							bParentOfInstance;							// used by instance command to flag parent (used to animate parent even if not visible)

	// reserved
	DWORD							dwReservedPP1;								// reserved - maintain plugin compat.
	DWORD							dwReservedPP2;								// reserved - maintain plugin compat.
	DWORD							dwReservedPP3;								// reserved - maintain plugin compat.
};

struct sPositionVectors
{
	// stores position data
	D3DXVECTOR3						vecPosition,								// main position
									vecRotate,									// euler rotation
									vecScale,									// main scale
									vecLook,									// look vector
									vecUp,										// up vector
									vecRight,									// right vector
									vecLastPosition,							// last position used by auto-collision
									vecLastRotate;								// autocol uses for rotation restoration
	D3DXMATRIX						matLastFreeFlightRotate;					// lee - 240306 - u6b4 - for automatic collision rotation freeze
};

struct sPositionData : public sPositionVectors,
                              sPositionProperties,
							  sPositionMatrices
{
	// constructor and destructor
	sPositionData  ( );
	~sPositionData ( );
};

struct sObjectProperties
{
	// object properties
	bool							bVisible;									// for hide/show
	bool							bUniverseVisible;							// internal systems can cull visibility (like universe)
	bool							bNoMeshesInObject;							// sometimes all meshes can be removed!
	bool							bUpdateOverallBounds;						// flag when need to update overall object bounds
	bool							bUpdateOnlyCurrentFrameBounds;				// true by default, when true it ONLY does bound for current animation frame (set object frame o,f,X)
	bool							bOverlayObject;								// flagged if special overlay object (ie lock,ghost)
	bool							bGhostedObject;								// flagged if a ghosted object
	bool							bTransparentObject;							// flagged if a transparent object
	bool							bTransparencyWaterLine;						// new: transparency water line (for above/below water)
	bool							bNewZLayerObject;							// flagged if requires zbuffer clear
	bool							bLockedObject;								// flagged if requires a locked camera
	bool							bStencilObject;								// flagged if object uses the stencil layer
	bool							bReflectiveObject;							// flagged if object renders a reflection
	bool							bReflectiveClipping;						// flagged if user clipping detected and available
	bool							bHadLODNeedCamDistance;						// flagged if use ADD LOD TO OBJECT
	bool							bDrawBounds;								// to hide/show the bounds geometry
	bool							bStatic;									// flagged if a static object ( won't go into the draw list )
	bool							bUsesItsOwnBuffers;							// flagged to get best speed with some drivers
	bool							bReplaceObjectFromBuffers;					// flagged when object must be removed from buffers
	bool							bCastsAShadow;								// flagged if will cast a shadow
	LPVOID							pShadowMesh;								// holds the pointer to the shadow mesh
	bool							bHideShadow;								// flagged if temp. hide shadow
	bool							bExcluded;									// flagged if object is excluded
	bool							bDisableTransform;							// disable transforms - for external control
	float							fFOV;										// per-object FOV control
	int								iInsideUniverseArea;						// -1=no, otherwise areabox index (work)
	float							fLODDistance [ 2 ];							// store distance of LOD transitions
	int								iUsingWhichLOD;								// using 0-normal,1-lod[0] and 2-lod[1]

	// reserved members
	bool							bVeryEarlyObject;							// U71 - object can be rendered even before stencilstart (and matrix, terrain, world, etc), ideal for skyboxes
	float*							pfAnimLimbFrame;							// U75 - mem ptr for limb based animation frames
	DWORD							dwObjectNumber;								// was dwCameraMaskBits, 301007 - now objid
	DWORD							dwCameraMaskBits;							// reserved - maintain plugin compat.
	union
	{
		int							iUsingOldLOD;								// U71 - alpha fade
		int							iLOD0LimbIndex;
	};
	union
	{
		float						fLODTransition;								// U71 - alpha fade
		int							iLOD1LimbIndex;
	};
	union
	{
		float						fLODDistanceQUAD;							// store distance of final LOD transitions
		int							iLOD2LimbIndex;
	};

	// reserved
	DWORD							dwReservedPR1;
	DWORD							dwReservedPR2;
	DWORD							dwReservedPR3;
	DWORD							dwReservedPR4;
	DWORD							dwReservedPR5;
	DWORD							dwReservedPR6;
	DWORD							dwReservedPR7;
	DWORD							dwReservedPR8;
};

struct sObjectAnimationProperties
{
	// animation properties of an object
	bool							bAnimPlaying;								// is anim playing
	bool							bAnimLooping;								// is looping
	bool							bAnimUpdateOnce;							// used when change limb (can affect boned models)
	float							fAnimFrame;									// frame we're on
	float							fAnimLastFrame;								// last frame
	float							fAnimSpeed;									// speed of animation
	float							fAnimLoopStart;								// start loop point
	float							fAnimFrameEnd;								// end frame
	float							fAnimTotalFrames;							// total number of frame
	bool							bAnimManualSlerp;							// slerp animation
	float							fAnimSlerpStartFrame;						// start frame
	float							fAnimSlerpEndFrame;							// end frame
	float							fAnimSlerpLastTime;							// used so we do not repeat anim calculations on object (quickreject)
	float							fAnimSlerpTime;								// time
	float							fAnimInterp;								// interpolation

	// reserved
	DWORD							dwReservedAN1;
	DWORD							dwReservedAN2;
	DWORD							dwReservedAN3;
};

struct sObjectInstance
{
	// object instance
	sObject*						pInstanceOfObject;							// used to mark object as a mere instance of object at ptr
	bool*							pInstanceMeshVisible;						// used to store limb visibility data
	bool							bInstanceAlphaOverride;						// used to apply alpha factor to instance obj
	DWORD							dwInstanceAlphaOverride;					// alpha override
};

struct sObjectData
{
	// object data
	int								iMeshCount;									// number of meshes in model
	int								iFrameCount;								// number of frames in the model
	sMesh**							ppMeshList;									// mesh list
	sFrame**						ppFrameList;								// frame list
	sFrame*							pFrame;										// main frame ( contained in a hiearachy )
	sAnimationSet*					pAnimationSet;								// list of all animations
	sCollisionData					collision;									// global collision data ( each mesh within a frame also has collision data - but local to itself )
	sPositionData					position;									// global positioning data

	// reserved
	DWORD							dwReservedR1;
	DWORD							dwReservedR2;
	DWORD							dwReservedR3;
};

struct sObjectDelete
{
	// delete object - for adding in your own delete function e.g. external DLL

	struct sDelete
	{
		ON_OBJECT_DELETE_CALLBACK	onDelete;
		int							userData;

		sDelete ( )
		{
			onDelete = NULL;
			userData = -1;
		}
	};

	sDelete*						pDelete;
	int								iDeleteCount;
	int								iDeleteID;

	// 310305 - mike - destructor needed
	~sObjectDelete ( );
};

struct sObjectCustom
{
	// 280305 - used when objects want to store custom data for example when
	//        - they save and want to save out this data

	DWORD							dwCustomSize;
	void*							pCustomData;

	sObjectCustom ( );

	// 310305 - destructor
	~sObjectCustom ( );
};

struct sObjectCharacterCreator
{
	// 070515 - Dave added to store character creator tones
	float ColorTone[4][3];
	float ToneMix[4];
};

struct sObject : public sObjectData,
					    sObjectProperties,
					    sObjectAnimationProperties,
						sObjectInstance,
						sObjectDelete,
						sObjectCustom
{
	// constructor and destructor
	sObject  ( );
	~sObject ( );

    // Ideally, these should go into sObjectInstance, but
    // doing so would break internal and third-party plug-ins

	// Set this to point to the object that this object is dependent upon.
	// (ie, Instance, and clone with shared animation data)
    sObject*                        pObjectDependency;

	// Increment this when another object depends on this object.
    DWORD                           dwDependencyCount;

	// Dave - added for character creator
	sObjectCharacterCreator*		pCharacterCreator;

};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

struct sInstanceStampTileData;

#define INSTANCESTAMPCAMERAMAX 35

struct sInstanceStampBuffer
{
	bool bVisible;
	bool bDirty;
	DWORD dwLODLevel;

	DWORD dwRefPosX;
	DWORD dwRefPosY;
	DWORD dwRefPosZ;
	sMesh* pMeshFromParent;
	DWORD dwCurrentMaxVBSize;
	DWORD dwCurrentMaxIBSize;
	IDirect3DVertexBuffer9* pVBRef;
	IDirect3DIndexBuffer9* pIBRef;
	D3DPRIMITIVETYPE dwPrimType;
	DWORD dwBaseVertexIndex;
	DWORD dwVertexStart;
	DWORD dwVertexCount;
	DWORD dwIndexStart;
	DWORD dwPrimitiveCount;
	DWORD dwVBOffset;
	DWORD dwIBOffset;
	DWORD dwMinX;
	DWORD dwMinY;
	DWORD dwMinZ;
	DWORD dwMaxX;
	DWORD dwMaxY;
	DWORD dwMaxZ;

	D3DXVECTOR3 vecMin; // vector bounds for culling/occlusion
	D3DXVECTOR3 vecMax;
	D3DXVECTOR3 vecCentre;
	D3DXVECTOR3 vecSize;
	bool bOccluded[INSTANCESTAMPCAMERAMAX];
	int iOccludedCounter[INSTANCESTAMPCAMERAMAX];
	int iQueryBusyStage[INSTANCESTAMPCAMERAMAX];
	DWORD dwQueryValue[INSTANCESTAMPCAMERAMAX];
	LPDIRECT3DQUERY9 d3dQuery[INSTANCESTAMPCAMERAMAX];

	int iQuadTexIndexCount;
	IDirect3DTexture9* d3dTex[INSTANCESTAMPCAMERAMAX];
	float fDistanceFromCamera[INSTANCESTAMPCAMERAMAX];
	float fAngleFromCamera[INSTANCESTAMPCAMERAMAX];
	bool bQuadTextureGenerated[INSTANCESTAMPCAMERAMAX];
	float fQuadTextureLastAngleFromCamera[INSTANCESTAMPCAMERAMAX];
	float fQuadTextureLastHeightFromCamera[INSTANCESTAMPCAMERAMAX];

	sInstanceStampTileData* pFirstLOD2Item; // item reference for quad texture traversal (read only)

	sInstanceStampBuffer* pNext; // next item in buffer chain from ref pos
	sInstanceStampBuffer* pActiveListNext; // next item in active list to know which buffers are being rendered
	sInstanceStampBuffer* pInertListNext; // next item in the inert list to know which buffers are not being used
};

#define INSTANCESTAMPLODMAX 3 

#define NUM_BOX_VERTS 8

struct vertstype
{
	D3DXVECTOR3 pos;
};
struct sImposter
{
	D3DXVECTOR3 boundingBoxVerts[NUM_BOX_VERTS];
	float radius;
	D3DXVECTOR2 uvOffset;
	vertstype verts[6];
	D3DXVECTOR3 centre;
	D3DXVECTOR3 cameraDir;
	bool pAdjustUV;
};

struct sInstanceStampTileData
{
	DWORD dwParentObjNumber;
	float fPosX;
	float fPosY;
	float fPosZ;
	float fRotX;
	float fRotY;
	float fRotZ;
	float fScaleX;
	float fScaleY;
	float fScaleZ;
	DWORD dwMeshVisibility;
	bool bAddedToBuffer[INSTANCESTAMPLODMAX];
	int iQuadTexIndex[INSTANCESTAMPCAMERAMAX];
	DWORD dwImposterOffsetIntoBuffer[INSTANCESTAMPCAMERAMAX];
	sImposter* pImposter[INSTANCESTAMPCAMERAMAX];
	sInstanceStampTileData* pNext;
	sInstanceStampTileData* pLOD2BufferCollectionNext; // a chain of items representing a LOD2 buffer
};

struct sInstanceStamp
{
	// reserved header vars
	DWORD dwReserved1;
	DWORD dwReserved2;
	DWORD dwReserved3;
	DWORD dwReserved4;
	DWORD dwReserved5;
	DWORD dwReserved6;
	DWORD dwReserved7;
	DWORD dwReserved8;
	DWORD dwReserved9;
	DWORD dwReserved10;

	// map data
	DWORD dwXSize;
	DWORD dwYSize;
	DWORD dwZSize;

	// camera view zone
	DWORD dwViewXSize;
	DWORD dwViewYSize;
	DWORD dwViewZSize;

	// map tile matrix
	sInstanceStampTileData** map;

	// all vertex/index buffers (lod levels)
	sInstanceStampBuffer** buffer[INSTANCESTAMPLODMAX];
	sInstanceStampBuffer* pActiveBufferList[INSTANCESTAMPLODMAX];
	sInstanceStampBuffer* pInertBufferList[INSTANCESTAMPLODMAX];
};

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#endif _DBODATA_H_
