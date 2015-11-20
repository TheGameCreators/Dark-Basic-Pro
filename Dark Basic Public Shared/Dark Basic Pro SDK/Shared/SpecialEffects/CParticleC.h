#ifndef _CPARTICLE_H_
#define _CPARTICLE_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "d3d9.lib"  )
#pragma comment ( lib, "d3dx9.lib" )

#include ".\..\data\cdatac.h"
#include "cpositionc.h"
#include "cparticlemanagerc.h"

#include <d3d9.h>
#include <math.h>
#include <D3DX9.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h>
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

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
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// internal

DARKSDK void Constructor			( HINSTANCE hSetup, HINSTANCE hImage );
	DARKSDK void Destructor				( void );
	DARKSDK void SetErrorHandler		( LPVOID pErrorHandlerPtr );
	DARKSDK void PassCoreData			( LPVOID pGlobPtr );
	DARKSDK void RefreshD3D ( int iMode );
	DARKSDK void Update					( void );

#ifdef DARKSDK_COMPILE
	void ConstructorParticles			( HINSTANCE hSetup, HINSTANCE hImage );
	void DestructorParticles				( void );
	void SetErrorHandlerParticles		( LPVOID pErrorHandlerPtr );
	void PassCoreDataParticles			( LPVOID pGlobPtr );
	void RefreshD3DParticles ( int iMode );
	void UpdateParticles					( void );
#endif

bool UpdateParticlePtr ( int iID );

DARKSDK bool Create					( int iID, DWORD dwFlush, DWORD dwDiscard, float fRadius, int iImage );
DARKSDK void Delete					( int iID );
DARKSDK void SetColor				( int iID, int iRed, int iGreen, int iBlue );
DARKSDK void SetTime				( int iID, float fTime );

// commands

DARKSDK void CreateEx				( int iID, int iImageIndex, int maxParticles, float fRadius );
DARKSDK void DeleteEx				( int iID );

DARKSDK void SetSecondsPerFrame		( int iID, float fTime );
DARKSDK void SetNumberOfEmmissions	( int iID, int iNumber );
DARKSDK void SetVelocity			( int iID, float fVelocity );
DARKSDK void SetPosition			( int iID, float fX, float fY, float fZ );
DARKSDK void SetEmitPosition		( int iID, float fX, float fY, float fZ );
DARKSDK void SetColorEx				( int iID, int iRed, int iGreen, int iBlue );
DARKSDK void SetRotation			( int iID, float fX, float fY, float fZ );
DARKSDK void SetGravity				( int iID, float fGravity );
DARKSDK void SetChaos				( int iID, float fChaos );
DARKSDK void SetLife				( int iID, int iLifeValue );
DARKSDK void SetMask				( int iID, int iCameraMask );
DARKSDK void EmitSingleParticle		( int iID, float fX, float fY, float fZ );
DARKSDK void SetFloor				( int iID, int iFlag );
DARKSDK void GhostOn				( int iID, int iMode );
DARKSDK void GhostOff				( int iID );
DARKSDK void Ghost					( int iID, int iMode, float fPercentage );

DARKSDK void Hide					( int iID );
DARKSDK void Show					( int iID );

DARKSDK void CreateSnowEffect		( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth );
DARKSDK void CreateFireEffect		( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth );

DARKSDK void SetPositionVector3		( int iID, int iVector );
DARKSDK void GetPositionVector3		( int iVector, int iID );
DARKSDK void SetRotationVector3		( int iID, int iVector );
DARKSDK void GetRotationVector3		( int iVector, int iID );

// expressions

DARKSDK SDK_BOOL GetExist			( int iID );
DARKSDK SDK_FLOAT GetPositionXEx	( int iID );
DARKSDK SDK_FLOAT GetPositionYEx	( int iID );
DARKSDK SDK_FLOAT GetPositionZEx	( int iID );

#ifdef DARKSDK_COMPILE
	bool dbMake							( int iID, DWORD dwFlush, DWORD dwDiscard, float fRadius, int iImage );
	void dbDelete						( int iID );
	void dbSetColor						( int iID, int iRed, int iGreen, int iBlue );
	void dbSetTime						( int iID, float fTime );

	void dbMakeParticles				( int iID, int iImageIndex, int maxParticles, float fRadius );
	void dbDeleteParticles				( int iID );

	void dbSetParticleSpeed				( int iID, float fTime );
	void dbSetParticleEmmissions		( int iID, int iNumber );
	void dbSetParticleVelocity			( int iID, float fVelocity );
	void dbPositionParticles			( int iID, float fX, float fY, float fZ );
	void dbPositionParticleEmmissions	( int iID, float fX, float fY, float fZ );
	void dbColorParticles				( int iID, int iRed, int iGreen, int iBlue );
	void dbRotateParticles				( int iID, float fX, float fY, float fZ );
	void dbSetParticleGravity			( int iID, float fGravity );
	void dbSetParticleChaos				( int iID, float fChaos );
	void dbSetParticleLife				( int iID, int iLifeValue );
	void dbSetParticleFloor				( int iID, int iFlag );
	void dbGhostParticlesOn				( int iID, int iMode );
	void dbGhostParticlesOff			( int iID );
	void dbGhostParticlesOn				( int iID, int iMode, float fPercentage );

	void dbHideParticles				( int iID );
	void dbShowParticles				( int iID );

	void dbMakeSnowParticles			( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth );
	void dbMakeFireParticles			( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth );

	void dbPositionParticles				( int iID, int iVector );
	void dbSetVector3ToParticlesPosition	( int iVector, int iID );
	void dbRotateParticles					( int iID, int iVector );
	void dbSetVector3ToParticlesRotation	( int iVector, int iID );

	bool dbParticlesExist					( int iID );
	float dbParticlesPositionX				( int iID );
	float dbParticlesPositionY				( int iID );
	float dbParticlesPositionZ				( int iID );

	// lee - 300706 - GDK fixes
	void dbSetParticleEmissions				( int iID, int iNumber );
	void dbPositionParticleEmissions		( int iID, float fX, float fY, float fZ );

#endif

//////////////////////////////////////////////////////////////////////////////////


#endif _CPARTICLE_H_