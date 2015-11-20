#ifndef _CENHANCEDX_H_
#define _CENHANCEDX_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#pragma comment ( lib, "d3dx8.lib"  )
//#pragma comment ( lib, "d3dxof.lib" )
//#pragma comment ( lib, "winmm.lib"  )

//#pragma comment ( lib, "dxguid.lib" )
//#pragma comment ( lib, "d3d8.lib"   )
//#pragma comment ( lib, "dxerr8.lib" )
//#include <d3d8.h>
#include <math.h>
//#include <D3DX8.h>

#define WIN32_LEAN_AND_MEAN
#include < windows.h >



#include "cdatac.h"
#include "sMesh.h"
#include "sFrame.h"
#include "sAnimation.h"
#include "sAnimationSet.h"
#include "cAnimation.h"
#include "dxfile.h"

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define GXRELEASE(_p) do       { if ((_p) != NULL) {(_p)->Release(); (_p) = NULL;} } while (0)
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// these functions are still exported in the command table, they are not meant for
// DB Pro though, they are used for communication between this DLL and the object
// manager, mainly so we can access internal data.
DARKSDK ID3DXMesh*          GetMeshData     ( int iID );
DARKSDK LPDIRECT3DTEXTURE8* GetTextureData  ( int iID );
DARKSDK DWORD               GetNumMaterials ( int iID );
DARKSDK D3DMATERIAL8*	    GetMaterialData ( int iID );

DARKSDK void				GetFVF ( int iID, DWORD** pdwFVF );
DARKSDK void				GetFVFSize ( int iID, DWORD** pdwFVFSize );
DARKSDK DWORD				GetNumberOfMeshes ( int iID );
DARKSDK void				GetMeshDataFromMeshList ( int iID, int iIndex, LPD3DXMESH** pData );
DARKSDK void				GetMeshAttribFromMeshList ( int iID, int iIndex, D3DXATTRIBUTERANGE*** pData );
DARKSDK void				ChangeToNewDeclaration ( int iID, DWORD* pdwDeclaration, DWORD dwOrFVF );

DARKSDK bool				MakeFromMesh ( int iID, DWORD dwInFVF, DWORD dwInFVFSize,
							float* pInMesh, DWORD dwInNumPoly, DWORD dwInNumVert, DWORD dwInPrimType );
DARKSDK void				NewObjectFromMesh  ( int iObjectID, int iMeshID );
DARKSDK void				NewObjectFromLimb ( int iNewObjectID, int iSrcObjectID, int iLimbID );

DARKSDK IDirect3DTexture8*	GetLimbTexturePtr ( int iID, int iLimbID );

DARKSDK D3DXMATRIX			GetCombined    ( int iID, int iFrame );
DARKSDK D3DXMATRIX			GetTransformed ( int iID, int iFrame );
DARKSDK D3DXMATRIX			GetOriginal    ( int iID, int iFrame );

DARKSDK void				SetVertexShaderOn ( int iID, DWORD dwShader );
DARKSDK void				SetVertexShaderOff ( int iID );
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct tagModelData
{
	char*				m_szFilename;			// model filename
	char				m_szPath [ 256 ];
	
	D3DXMATRIX*			m_matObject;			// positional data

	LPDIRECT3DTEXTURE8*	lpTexture;				// pointer to array of textures

	bool				m_bAnimPlaying;
	//bool				m_bAnimLoop;
	int					m_iAnimDirection;
	int					m_iAnimMode;
	DWORD				m_dwThisTime;			// current time
	DWORD				m_dwLastTime;			// previous saved time
	float				m_fAnimSpeed;			// speed increment
	float				m_fFrame;				// current frame
	int					m_iStartFrame;
	int					m_iEndFrame;
	int					m_iFrameCount;			// number of frames
	int					m_iFrameJump;			// number of frames to jump

	int					m_iChangedTexture;

	float				fX;						// x position
	float				fY;						// y position
	float				fZ;						// z position

	#if DEBUG_MODE
		LPD3DXMESH     m_pMeshBox;
		D3DXMATRIX     m_matBox;
	#endif

	sObjectData			m_Object;
};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// setup
DARKSDK void Constructor       ( LPDIRECT3DDEVICE8 lpDevice );
DARKSDK void Destructor        ( void );
DARKSDK void Update            ( int iID, D3DXMATRIX* matrix );
DARKSDK void UpdateTimer       ( void );
DARKSDK void RefreshD3D			( int iMode );

DARKSDK tagModelData* Load              ( int iID, char* szFilename );				// loads an x file
DARKSDK bool Delete            ( int iID );
DARKSDK bool AppendAnimationFromFile ( int iID, char* szFilename, int iFrame );

DARKSDK void Draw			   ( int iID, int iBaseTextureStage );
DARKSDK bool Clone             ( int iSourceID, int iDestinationID );		// clone an object - helps to reduce memory requirements
DARKSDK void SetSphereRadius   ( int iID, float fRadius );

// textures
DARKSDK void Color             ( int iID, int iR, int iG, int iB );
DARKSDK void Fade              ( int iID, float FPercentage );
DARKSDK void SetTexture        ( int iID, LPDIRECT3DTEXTURE8 pTexture );	// sets the current texture
DARKSDK void ScrollTexture     ( int iID, float fU, float fV );				// scrolls the texture
DARKSDK void ScaleTexture      ( int iID, float fU, float fV );				// scales the texture

// animation
DARKSDK void Play              ( int iID );
DARKSDK void Play              ( int iID, int iStart );
DARKSDK void Play              ( int iID, int iStart, int iEnd );
DARKSDK void Loop              ( int iID );
DARKSDK void Loop              ( int iID, int iStart );
DARKSDK void Loop              ( int iID, int iStart, int iEnd );
DARKSDK void Stop              ( int iID );

DARKSDK void ClearAllKeyFrames ( int iID );
DARKSDK void ClearKeyFrame     ( int iID, int iFrame );
DARKSDK void SetKeyFrame       ( int iID, int iFrame );

DARKSDK void SetAnimationMode  ( int iID, int iMode );

DARKSDK void SetSpeed         ( int iID, int iSpeed );
DARKSDK void SetFrame         ( int iID, int iFrame );
DARKSDK void SetFrame		  ( int iID, float fFrame );
DARKSDK void SetInterpolation ( int iID, int iJump );
DARKSDK int  GetAnimationCount ( int iID );									// get the number of animations
DARKSDK void SetAnimation      ( int iID, int iAnim );						// set the current animation
//aboveDARKSDK void Color             ( int iID, int iR, int iG, int iB );



// limb commands
DARKSDK void PerformCheckListForLimbs ( void );
DARKSDK void HideLimb                 ( int iID, int iLimbID );
DARKSDK void ShowLimb                 ( int iID, int iLimbID );
DARKSDK void OffsetLimb               ( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void RotateLimb               ( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void ScaleLimb                ( int iID, int iLimbID, float fX, float fY, float fZ );
DARKSDK void AddLimb                  ( int iID, int iLimbID, int iMeshID );
DARKSDK void RemoveLimb               ( int iID, int iLimbID );
DARKSDK void LinkLimb                 ( int iID, int iLimbID, int iParentID );
DARKSDK void TextureLimb              ( int iID, int iLimbID, int iImageID );
DARKSDK void ColorLimb                ( int iID, int iLimbID, DWORD dwColor );
DARKSDK void ScrollLimbTexture        ( int iID, int iLimbID, float fU, float fV );
DARKSDK void ScaleLimbTexture         ( int iID, int iLimbID, float fU, float fV );


// information commands
DARKSDK float GetMinX ( int iID );
DARKSDK float GetMinY ( int iID );
DARKSDK float GetMinZ ( int iID );
DARKSDK float GetMaxX ( int iID );
DARKSDK float GetMaxY ( int iID );
DARKSDK float GetMaxZ ( int iID );

DARKSDK float GetRadius        ( int iID );
DARKSDK int   GetFrames        ( int iID );
DARKSDK float GetXSize         ( int iID );
DARKSDK float GetYSize         ( int iID );
DARKSDK float GetZSize         ( int iID );
DARKSDK bool  GetPlaying       ( int iID );
DARKSDK bool  GetLooping       ( int iID );
DARKSDK int   GetFrame         ( int iID );
DARKSDK int   GetSpeed         ( int iID );
DARKSDK int   GetInterpolation ( int iID );

DARKSDK bool  GetLimbExist       ( int iID, int iLimbID );
DARKSDK float GetLimbOffsetX     ( int iID, int iLimbID );
DARKSDK float GetLimbOffsetY     ( int iID, int iLimbID );
DARKSDK float GetLimbOffsetZ     ( int iID, int iLimbID );
DARKSDK float GetLimbAngleX      ( int iID, int iLimbID );
DARKSDK float GetLimbAngleY      ( int iID, int iLimbID );
DARKSDK float GetLimbAngleZ      ( int iID, int iLimbID );
DARKSDK float GetLimbXPosition   ( int iID, int iLimbID );
DARKSDK float GetLimbYPosition   ( int iID, int iLimbID );
DARKSDK float GetLimbZPosition   ( int iID, int iLimbID );
DARKSDK float GetLimbXDirection  ( int iID, int iLimbID );
DARKSDK float GetLimbYDirection  ( int iID, int iLimbID );
DARKSDK float GetLimbZDirection  ( int iID, int iLimbID );
DARKSDK int   GetLimbTexture     ( int iID, int iLimbID );
DARKSDK bool  GetLimbVisible     ( int iID, int iLimbID );
DARKSDK char* GetLimbTextureName ( int iID, int iLimbID );
DARKSDK char* GetLimbName        ( int iID, int iLimbID );
DARKSDK int   GetLimbCount		 ( int iID );
DARKSDK float GetLimbScaleX      ( int iID, int iLimbID );
DARKSDK float GetLimbScaleY      ( int iID, int iLimbID );
DARKSDK float GetLimbScaleZ      ( int iID, int iLimbID );
DARKSDK bool  GetLimbLink        ( int iID, int iLimbID );

DARKSDK void LoadMesh           ( int iID, char* szFilename );
DARKSDK void DeleteMesh         ( int iID );
DARKSDK void SaveMesh			( int iMeshID, char* szFilename );
DARKSDK void ChangeMesh			( int iObjectID, int iLimbID, int iMeshID );
DARKSDK void MakeMeshFromObject ( int iID, int iObjectID );
DARKSDK int  GetMeshExist		( int iID );
//////////////////////////////////////////////////////////////////////////////////

// memblock support
DARKSDK void				GetMeshData( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
DARKSDK void				SetMeshData( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax );

#endif _CENHANCEDX_H_