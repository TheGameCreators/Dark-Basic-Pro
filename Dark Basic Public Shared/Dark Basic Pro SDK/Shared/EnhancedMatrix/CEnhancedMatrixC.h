#ifndef _CENHANCEDMATRIX_H_
#define _CENHANCEDMATRIX_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "d3d9.lib"   )
#pragma comment ( lib, "d3dx9.lib"  )
#pragma comment ( lib, "dxerr9.lib" )

#include ".\..\data\cdatac.h"
#include "cenhancedmatrixdatac.h"
#include "cenhancedmatrixmanagerc.h"

#include ".\modes\cmodec.h"
#include ".\modes\quadtrees\cquadtreesc.h"

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
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

// SDK DEFINES (DBPRO/SDK)
#define DB_PRO 1
#if DB_PRO
 #define SDK_BOOL int
 #define SDK_FLOAT DWORD
 #define SDK_LPSTR DWORD
 #define SDK_RETFLOAT(f) *(DWORD*)&f 
 #define SDK_RETSTR DWORD pDestStr,
#else
 #define SDK_BOOL bool
 #define SDK_FLOAT float
 #define SDK_LPSTR char*
 #define SDK_RETFLOAT(f) f
 #define SDK_RETSTR 
#endif

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bool UpdatePtr ( int iID );
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK void Constructor			( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera );
DARKSDK void Destructor				( void );												// cleanup
DARKSDK void RefreshD3D				( int iMode );
DARKSDK void PassCoreData			( LPVOID pGlobPtr );
DARKSDK void SetErrorHandler		( LPVOID pErrorHandlerPtr );

// internal Functions
DARKSDK bool Make					( int iID, char* szHeightMap, int iSize );
DARKSDK bool Make					( int iID, char* szHeightMap, int iSize, int iPoolSize, float fMinResolution, float fMultScale );

DARKSDK void Update					( void );												// update all terrains
DARKSDK void SetTextureDivide		( int iID, int iX, int iY );							// set texture divide on terrains

// command functions
DARKSDK void Make					( int iID, SDK_LPSTR szHeightMap );
DARKSDK void Make					( int iID, SDK_LPSTR szHeightMap, int iPoolSize, float fMinResolution, float fMultScale );
DARKSDK void Delete					( int iID );			
DARKSDK void SetPosition			( int iID, float fX, float fY, float fZ );
DARKSDK void SetTexture				( int iID, int iImageIndex );
DARKSDK void SetTextureDivideEx		( int iID, float fX, float fY );

// expression functions
DARKSDK SDK_BOOL  GetExist			( int iID );
DARKSDK SDK_FLOAT GetPositionX		( int iID );
DARKSDK SDK_FLOAT GetPositionY		( int iID );
DARKSDK SDK_FLOAT GetPositionZ		( int iID );
DARKSDK SDK_FLOAT GetHeight			( int iID, float fX, float fZ );
DARKSDK SDK_FLOAT GetTotalHeight	( int iID );

//////////////////////////////////////////////////////////////////////////////////
#endif _CENHANCEDMATRIX_H_