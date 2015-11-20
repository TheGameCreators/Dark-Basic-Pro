#ifndef _CQ2BSP_H_
#define _CQ2BSP_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "d3d9.lib"   )
#pragma comment ( lib, "d3dx9.lib"  )
#pragma comment ( lib, "dxerr9.lib" )

#include < d3d9.h >
#include < Dxerr9.h >
#include < D3dx9tex.h >
#include < D3dx9core.h >
#include < basetsd.h >
#include < stdio.h >
#include < math.h >
#include < D3DX9.h >
#include < d3d9types.h >

#define WIN32_LEAN_AND_MEAN
#include <windows.h>  
#include <windowsx.h>

#include "Q2Data.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// FILE FORMAT ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// a Q2 BSP map consists of several sections, the first block is a header which
// contains the version number and then an array of other data. this other data
// holds information about textures, models etc.

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bool SetupWorld ( byte* data );
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject );
DARKSDK void Destructor  ( void );
DARKSDK void RefreshD3D ( int iMode );
DARKSDK void PassCoreData( LPVOID pGlobPtr );

DARKSDK bool Load        ( char* szFilename, char* szMap );
DARKSDK void Update      ( void );
DARKSDK void DeleteEx			  ( void );

DARKSDK void SetupCameraCollision ( int iID, int iType, float fRadius, int iResponse );
DARKSDK void SetupObjectCollision ( int iID, int iType, float fRadius, int iResponse );
DARKSDK void SetupCullingCamera ( int iID );
DARKSDK void SetupCollisionOff ( int iID );
DARKSDK void SetCollisionThreshhold ( int iID, float fSensitivity );

DARKSDK void SetCameraCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ );
DARKSDK void SetObjectCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ );

DARKSDK void Start                ( void );
DARKSDK void End	              ( void );

DARKSDK void SetHardwareMultiTexturingOn  ( void );
DARKSDK void SetHardwareMultiTexturingOff ( void );

// mike - 240604
DARKSDK int GetCollisionResponse		( int iID );
DARKSDK float GetCollisionX			( int iID );
DARKSDK float GetCollisionY			( int iID );
DARKSDK float GetCollisionZ			( int iID );
//////////////////////////////////////////////////////////////////////////////////

#endif _CQ2BSP_H_