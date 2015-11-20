#ifndef _CCUSTOMBSP_H_
#define _CCUSTOMBSP_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "dxerr9.lib" )

#include <d3d9.h>
#include <d3dx9.h>

#include <windows.h> 
#include <windowsx.h>
#include <stdio.h>

#include "CCollisionManager.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// function pointer types ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef IDirect3DDevice9*   ( *RetD3DFunctionPointerPFN  ) ( ... );
typedef void                ( *CAMERA_Int1Float3PFN ) ( int, float, float, float );
typedef void                ( *CAMERA_Int1Float1PFN ) ( int, float );
typedef float               ( *CAMERA_GetFloatPFN   ) ( int );

extern HINSTANCE					g_Setup;
extern RetD3DFunctionPointerPFN		g_Setup_GetDirect3DDevice;
extern HINSTANCE					g_Camera;
extern CAMERA_Int1Float3PFN			g_Camera_Position;
extern CAMERA_GetFloatPFN			g_Camera_GetXPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetYPosition;
extern CAMERA_GetFloatPFN			g_Camera_GetZPosition;
extern CAMERA_Int1Float3PFN			g_Object_Position;
extern CAMERA_GetFloatPFN			g_Object_GetXPosition;
extern CAMERA_GetFloatPFN			g_Object_GetYPosition;
extern CAMERA_GetFloatPFN			g_Object_GetZPosition;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void	RenderTree				( D3DXVECTOR3 pos );
void	DrawTree				( long leaf );
BOOL	Get_Intersect			( D3DXVECTOR3* linestart, D3DXVECTOR3* lineend, D3DXVECTOR3* vertex, D3DXVECTOR3* normal, D3DXVECTOR3* intersection, float* percentage );
int		ClassifyPoint			( D3DXVECTOR3* pos, PLANE* Plane );
BOOL	LineOfSight				( D3DXVECTOR3* Start, D3DXVECTOR3* End, long Node );
void	DeletePolygonArray		( void );
void	DeletePolygon			( POLYGON* Poly );
BOOL	CollideSphere			( D3DXVECTOR3* SphereCenter, float SphereRadius, long Node );
void	ExtractFrustumPlanes	( PLANE2* Planes );
BOOL	LeafInFrustum			( long Leaf );
void    LoadTextures            ( void );
void    UpdateViewpos ( void );
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
DARKSDK void Destructor  ( void );
DARKSDK void PassCoreData( LPVOID pGlobPtr );

DARKSDK bool Load        ( char* szFilename );
DARKSDK void Update      ( void );
DARKSDK void DeleteEx			  ( void );

DARKSDK void SetupCameraCollision ( int iID, int iType, float fRadius, int iResponse );
DARKSDK void SetupObjectCollision ( int iID, int iType, float fRadius, int iResponse );
DARKSDK void SetupCullingCamera ( int iID );
DARKSDK void SetupCollisionOff ( int iID );
DARKSDK void SetCollisionThreshhold ( int iID, float fSensitivity );
DARKSDK void SetCollisionHeightAdjustment ( int iID, float fHeight );

DARKSDK void SetCameraCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ );
DARKSDK void SetObjectCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ );

DARKSDK int GetCollisionResponse ( int iID );
DARKSDK float GetCollisionX ( int iID );
DARKSDK float GetCollisionY ( int iID );
DARKSDK float GetCollisionZ ( int iID );

DARKSDK void Start                ( void );
DARKSDK void ProcessCollision	  ( int iTemp );
DARKSDK void End	              ( void );
//////////////////////////////////////////////////////////////////////////////////

#endif _CCUSTOMBSP_H_