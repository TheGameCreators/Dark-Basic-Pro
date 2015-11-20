#ifndef _CSOUND_H_
#define _CSOUND_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#pragma comment ( lib, "d3dx9.lib" )
#include <D3DX9.h>

#include "csoundmanagerc.h"
#include "csounddatac.h"
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

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor		( HWND hWnd );
DARKSDK void Destructor			( void );
DARKSDK void SetErrorHandler	( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData		( LPVOID pGlobPtr );
DARKSDK void RefreshD3D			( int iMode );
DARKSDK void Update				( void );

#ifdef DARKSDK_COMPILE
	void ConstructorSound		( HWND hWnd );
	void DestructorSound		( void );
	void SetErrorHandlerSound	( LPVOID pErrorHandlerPtr );
	void PassCoreDataSound		( LPVOID pGlobPtr );
	void RefreshD3DSound		( int iMode );
	void UpdateSound			( void );
#endif

DARKSDK bool UpdatePtr ( int iID );
DARKSDK sSoundData* GetSound( int iID );

// loading commands
DARKSDK void LoadSound			   ( LPSTR szFilename, int iID );			// load sound file
DARKSDK void LoadSound			   ( LPSTR szFilename, int iID, int iFlag );
DARKSDK void LoadSound			   ( LPSTR szFilename, int iID, int iFlag, int iSilentFail );
DARKSDK void CloneSound			   ( int iDestination, int iSource );		// clone sound id
DARKSDK void DeleteSound		   ( int iID );								// delete sound

// playing commands
DARKSDK void PlaySound			   ( int iID );								// play sound
DARKSDK void PlaySoundOffset	   ( int iID, int iOffset );				// play sound at offset position
DARKSDK void LoopSound			   ( int iID );								// loop sound
DARKSDK void LoopSound			   ( int iID, int iStart );
DARKSDK void LoopSound			   ( int iID, int iStart, int iEnd );
DARKSDK void LoopSound			   ( int iID, int iStart, int iEnd, int iInitialPos );

DARKSDK void StopSound			   ( int iID );								// stop sound
DARKSDK void ResumeSound		   ( int iID );								// resume sound
DARKSDK void PauseSound			   ( int iID );								// pause sound

// sound properties commands
DARKSDK void SetSoundPan           ( int iID, int iPan );					// set sound pan
DARKSDK void SetSoundSpeed		   ( int iID, int iFrequency );				// set sound speed / frequency
DARKSDK void SetSoundVolume		   ( int iID, int iVolume );				// set sound volume

// recording commands
DARKSDK void RecordSound           ( int iID );								// record sound
DARKSDK void RecordSound		   ( int iID, int iCaptureDuration );
DARKSDK void StopRecordingSound    ( void );								// stop recording session
DARKSDK void SaveSound             ( LPSTR szFilename, int iID );			// save sound to disk

// 3D sound commands
DARKSDK void Load3DSound           ( LPSTR szFilename, int iID );			// load a sound in 3D space
DARKSDK void Load3DSound           ( LPSTR szFilename, int iID, int iSilentFail );
DARKSDK void PositionSound         ( int iID, float fX, float fY, float fZ );	// position sound
DARKSDK void PositionListener	   ( float fX, float fY, float fZ );	// position sound listener
DARKSDK void RotateListener		   ( float fX, float fY, float fZ );		// rotate the listening point
DARKSDK void ScaleListener		   ( float fScale );
DARKSDK void SetEAX				   ( int iEffect ) ;

// get properties commands
DARKSDK int  GetSoundExist         ( int iID );								// does sound exist?
DARKSDK int  GetSoundType          ( int iID );								// get sound type - normal or 3D
DARKSDK int  GetSoundPlaying       ( int iID );								// is sound playing?
DARKSDK int  GetSoundLooping       ( int iID );								// is sound looping?
DARKSDK int  GetSoundPaused        ( int iID );								// is sound paused?
DARKSDK int  GetSoundPan           ( int iID );								// get pan
DARKSDK int  GetSoundSpeed         ( int iID );								// get speed
DARKSDK int  GetSoundVolume        ( int iID );								// get volume

// get 3D properties commands
DARKSDK DWORD  GetSoundPositionXEx     ( int iID );								// get x position
DARKSDK DWORD  GetSoundPositionYEx     ( int iID );								// get y position
DARKSDK DWORD  GetSoundPositionZEx     ( int iID );								// get z position
DARKSDK DWORD  GetListenerPositionXEx  ( void );								// get listener x position
DARKSDK DWORD  GetListenerPositionYEx  ( void );								// get listener y position
DARKSDK DWORD  GetListenerPositionZEx  ( void );								// get listener z position
DARKSDK DWORD  GetListenerAngleXEx     ( void );								// get listener x angle position
DARKSDK DWORD  GetListenerAngleYEx     ( void );								// get listener y angle position
DARKSDK DWORD  GetListenerAngleZEx     ( void );								// get listener z angle position

// SDK Functions

DARKSDK float  GetSoundPositionX     ( int iID );								// get x position
DARKSDK float  GetSoundPositionY     ( int iID );								// get y position
DARKSDK float  GetSoundPositionZ     ( int iID );								// get z position
//DARKSDK float  GetListenerPositionX  ( int iID );								// get listener x position
//DARKSDK float  GetListenerPositionY  ( int iID );								// get listener y position
//DARKSDK float  GetListenerPositionZ  ( int iID );								// get listener z position
//DARKSDK float  GetListenerAngleX     ( int iID );								// get listener x angle position
//DARKSDK float  GetListenerAngleY     ( int iID );								// get listener y angle position
//DARKSDK float  GetListenerAngleZ     ( int iID );								// get listener z angle position

DARKSDK void GetSoundData( int iID, DWORD* dwBitsPerSecond, DWORD* Frequency, DWORD* Duration, LPSTR* pData, DWORD* dwDataSize, bool bLockData, WAVEFORMATEX* wfx );
DARKSDK void SetSoundData( int iID, DWORD dwBitsPerSecond, DWORD Frequency, DWORD Duration, LPSTR pData, DWORD dwDataSize, WAVEFORMATEX wfx );
DARKSDK LPDIRECTSOUND8 GetSoundInterface ( void );
DARKSDK IDirectSound3DBuffer8* GetSoundBuffer ( int iID );

#ifdef DARKSDK_COMPILE
		void dbLoadSound				( LPSTR szFilename, int iID );
		void dbLoadSound				( LPSTR szFilename, int iID, int iFlag );
		void dbCloneSound				( int iDestination, int iSource );
		void dbDeleteSound				( int iID );

		void dbPlaySound				( int iID );
		void dbPlaySound				( int iID, int iOffset );
		void dbLoopSound				( int iID );
		void dbLoopSound				( int iID, int iStart );
		void dbLoopSound				( int iID, int iStart, int iEnd );
		void dbLoopSound				( int iID, int iStart, int iEnd, int iInitialPos );

		void dbStopSound				( int iID );
		void dbResumeSound				( int iID );
		void dbPauseSound				( int iID );

		void dbSetSoundPan				( int iID, int iPan );
		void dbSetSoundSpeed			( int iID, int iFrequency );
		void dbSetSoundVolume			( int iID, int iVolume );
		void dbSetSoundVolumeEx			( int iID, int iDecibels );

		void dbRecordSound				( int iID );
		void dbRecordSound				( int iID, int iCaptureDuration );
		void dbStopRecordingSound		( void );
		void dbSaveSound				( LPSTR szFilename, int iID );

		void dbLoad3DSound				( LPSTR szFilename, int iID );
		void dbPositionSound			( int iID, float fX, float fY, float fZ );
		void dbPositionListener			( float fX, float fY, float fZ );
		void dbRotateListener			( float fX, float fY, float fZ );
		void dbScaleListener			( float fScale );
		void dbSetEAX					( int iEffect );

		int  dbSoundExist         		( int iID );
		int  dbSoundType          		( int iID );
		int  dbSoundPlaying       		( int iID );
		int  dbSoundLooping       		( int iID );
		int  dbSoundPaused        		( int iID );
		int  dbSoundPan           		( int iID );
		int  dbSoundSpeed				( int iID );
		int  dbSoundVolume				( int iID );

		float  dbSoundPositionX			( int iID );
		float  dbSoundPositionY			( int iID );
		float  dbSoundPositionZ			( int iID );
		float  dbListenerPositionX		( void );
		float  dbListenerPositionY		( void );
		float  dbListenerPositionZ		( void );
		float  dbListenerAngleX			( void );
		float  dbListenerAngleY			( void );
		float  dbListenerAngleZ			( void );

		float  dbGetSoundPositionX		( int iID );
		float  dbGetSoundPositionY		( int iID );
		float  dbGetSoundPositionZ		( int iID );
		float  dbGetListenerPositionX	( int iID );
		float  dbGetListenerPositionY	( int iID );
		float  dbGetListenerPositionZ	( int iID );
		float  dbGetListenerAngleX		( int iID );
		float  dbGetListenerAngleY		( int iID );
		float  dbGetListenerAngleZ		( int iID );

		void   dbGetSoundData			( int iID, DWORD* dwBitsPerSecond, DWORD* Frequency, DWORD* Duration, LPSTR* pData, DWORD* dwDataSize, bool bLockData, WAVEFORMATEX* wfx );
		void   dbSetSoundData			( int iID, DWORD dwBitsPerSecond, DWORD Frequency, DWORD Duration, LPSTR pData, DWORD dwDataSize, WAVEFORMATEX wfx );
#endif



#endif _CMATRIX_H_

