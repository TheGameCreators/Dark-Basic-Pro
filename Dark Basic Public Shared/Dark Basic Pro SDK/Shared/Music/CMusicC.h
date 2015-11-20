#ifndef _CMUSIC_H_
#define _CMUSIC_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <wtypes.h>

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
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor		( HWND hWnd );
DARKSDK void Destructor			( void );
DARKSDK void SetErrorHandler	( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData		( LPVOID pGlobPtr );
DARKSDK void RefreshD3D			( int iMode );
DARKSDK void Update				( void );

#ifdef DARKSDK_COMPILE
	void ConstructorMusic		( HWND hWnd );
	void DestructorMusic		( void );
	void SetErrorHandlerMusic	( LPVOID pErrorHandlerPtr );
	void PassCoreDataMusic		( LPVOID pGlobPtr );
	void RefreshD3DMusic		( int iMode );
	void UpdateMusic			( void );
#endif

DARKSDK bool UpdatePtr ( int iID );
	
// loading commands
DARKSDK void LoadMusic				( LPSTR szFilename, int iID );			// load midi file
DARKSDK void LoadCDMusic			( int iTrackNumber, int iID );			// load music file
DARKSDK void DeleteMusic			( int iID );								// delete music

// play commands
DARKSDK void PlayMusic				( int iID );								// play specified music
DARKSDK void LoopMusic				( int iID );								// loop the music
DARKSDK void StopMusic				( int iID );								// stop the music
DARKSDK void PauseMusic				( int iID );								// pause the music
DARKSDK void ResumeMusic			( int iID );								// resume playing the music

// property commands
DARKSDK void SetMusicVolume			( int iID, int iVolume );				// set music volume
DARKSDK void SetMusicSpeed			( int iID, int iSpeed );

// get information commands
DARKSDK int  GetMusicExist			( int iID );								// does music exist?
DARKSDK int  GetMusicPlaying		( int iID );								// is music playing?
DARKSDK int  GetMusicLooping		( int iID );								// is music looping?
DARKSDK int  GetMusicPaused			( int iID );								// is music paused?
DARKSDK int  GetMusicVolume			( int iID );
DARKSDK int  GetMusicSpeed			( int iID );

DARKSDK int  GetNumberOfCDTracks	( void );								// get number of cd tracks

#ifdef DARKSDK_COMPILE
		void dbLoadMusic			( LPSTR szFilename, int iID );
		void dbLoadCDMusic			( int iTrackNumber, int iID );
		void dbDeleteMusic			( int iID );

		void dbPlayMusic			( int iID );
		void dbLoopMusic			( int iID );
		void dbStopMusic			( int iID );
		void dbPauseMusic			( int iID );
		void dbResumeMusic			( int iID );

		void dbSetMusicVolume       ( int iID, int iVolume );
		void dbSetMusicSpeed		( int iID, int iSpeed );

		int  dbMusicExist			( int iID );
		int  dbMusicPlaying			( int iID );
		int  dbMusicLooping			( int iID );
		int  dbMusicPaused			( int iID );
		int  dbMusicVolume			( int iID );
		int  dbMusicSpeed			( int iID );

		int  dbGetNumberOfCDTracks  ( void );
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif _CMUSIC_H_