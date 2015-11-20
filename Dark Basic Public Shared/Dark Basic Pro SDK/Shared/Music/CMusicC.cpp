#include ".\..\core\globstruct.h"
#include ".\..\error\cerror.h"

#include "CMusicC.h"
#include "CMusicManagerC.h"

#include "BasePlayer.h"

#include "CdPlayer.h"
#include "MusicPlayer.h"
#include "MidiPlayer.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msacm32.lib")

/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////


// Global Shared Data Pointer (passed in from core)
DBPRO_GLOBAL GlobStruct*		g_pGlob = NULL;

namespace
{
    CMusicManager				m_MusicManager;
    DBP_Music::BasePlayer*		m_ptr;
}

DARKSDK void Constructor ( HWND hWnd )
{
}

DARKSDK void Destructor ( void )
{
	// shut down the manager
	m_MusicManager.ShutDown ( );
}

DARKSDK bool UpdatePtr ( int iID )
{
	// update internal data

	m_ptr = m_MusicManager.GetData ( iID );

	if ( m_ptr == NULL )
		return false;

	return true;
}

DARKSDK void Update ( void )
{
    // Update all music
    m_MusicManager.Update();
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
}

DARKSDK void RefreshD3D ( int iMode )
{
}

DARKSDK void LoadMusicCore ( LPSTR szFilename, int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICALREADYEXISTS);
		return;
	}

    DBP_Music::BasePlayer* m_Data = 0;

    bool IsMidi = false;
    LPCSTR Extension = strrchr(szFilename, '.');
    if (Extension)
    {
        if (strnicmp(Extension+1, "mid", 3) == 0)
            IsMidi = true;
        if (! IsMidi && strnicmp(Extension+1, "rmi", 3) == 0)
            IsMidi = true;
    }

    if (IsMidi)
        m_Data = new DBP_Music::MidiPlayer( szFilename );
    else
        m_Data = new DBP_Music::MusicPlayer( szFilename );

	m_MusicManager.Add ( m_Data, iID );
}

DARKSDK void LoadMusic ( LPSTR szFilename, int iID )
{
    if (!szFilename || !szFilename[0])
    {
	    RunTimeError(RUNTIMEERROR_MUSICLOADFAILED);
        return;
    }

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile (VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadMusicCore ( VirtualFilename, iID );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void LoadCDMusic ( int iTrackNumber, int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICALREADYEXISTS);
		return;
	}
	if ( iTrackNumber < 1 || iTrackNumber > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICTRACKILLEGAL);
		return;
	}

	// setup a CD track
    DBP_Music::BasePlayer* m_Data = new DBP_Music::CdPlayer( iTrackNumber );

	// add to the list
	m_MusicManager.Add ( m_Data, iID );
}

DARKSDK void PlayMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

    m_ptr->Play();
}

DARKSDK int GetNumberOfCDTracks ( void )
{
    return DBP_Music::CdPlayer::TracksAvailable();
}

DARKSDK void LoopMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

    m_ptr->Loop();
}

DARKSDK void StopMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

    m_ptr->Stop();
}

DARKSDK void PauseMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

    m_ptr->Pause();
}

DARKSDK void ResumeMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

    m_ptr->Resume();
}

DARKSDK void DeleteMusic ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

	m_MusicManager.Delete ( iID );
}

DARKSDK int GetMusicExist ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
		return 0;
	
	return 1;
}

DARKSDK int GetMusicPlaying ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return 0;
	}

    return m_ptr->IsPlaying() ? 1 : 0;
}

DARKSDK int GetMusicLooping ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return 0;
	}

    return m_ptr->IsLooping() ? 1 : 0;
}

DARKSDK int GetMusicPaused ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return 0;
	}

    return m_ptr->IsPaused() ? 1 : 0;
}

DARKSDK int GetMusicVolume ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return 0;
	}
	return m_ptr->Volume();
}

DARKSDK int GetMusicSpeed ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return 0;
	}
	return m_ptr->Speed();
}

DARKSDK void SetMusicVolume ( int iID, int iVolume )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}

	// Ensure volume within acceptable limits
    if (iVolume < 0) iVolume = 0;
    if (iVolume > 100) iVolume = 100;

    m_ptr->SetVolume( iVolume );
}

DARKSDK void SetMusicSpeed ( int iID, int iSpeed )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_MUSICNUMBERILLEGAL);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_MUSICNOTEXIST);
		return;
	}
	if ( iSpeed < 0 )
	{
		RunTimeError(RUNTIMEERROR_MUSICSPEEDILLEGAL);
		return;
	}

    m_ptr->SetSpeed( iSpeed );
}


//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorMusic ( HWND hWnd )
{
	Constructor ( hWnd );
}

void DestructorMusic ( void )
{
	Destructor ( );
}

void SetErrorHandlerMusic	( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler (pErrorHandlerPtr  );
}

void PassCoreDataMusic ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DMusic ( int iMode )
{
	RefreshD3D ( iMode );
}

void UpdateMusic ( void )
{
	Update ( );
}

void dbLoadMusic ( LPSTR szFilename, int iID )
{
	LoadMusic ( szFilename, iID );
}

void dbLoadCDMusic ( int iTrackNumber, int iID )
{
	LoadCDMusic ( iTrackNumber, iID );
}

void dbDeleteMusic ( int iID )
{
	DeleteMusic ( iID );
}

void dbPlayMusic ( int iID )
{
	PlayMusic ( iID );
}
 
void dbLoopMusic ( int iID )
{
	LoopMusic ( iID );
}
 
void dbStopMusic ( int iID )
{
	StopMusic ( iID );
}
 
void dbPauseMusic ( int iID )
{
	PauseMusic ( iID );
}
 
void dbResumeMusic ( int iID )
{
	ResumeMusic ( iID );
}

void dbSetMusicVolume ( int iID, int iVolume )
{
	SetMusicVolume ( iID, iVolume );
}
	
void dbSetMusicSpeed ( int iID, int iSpeed )
{
	SetMusicSpeed ( iID, iSpeed );
}

int dbMusicExist ( int iID )
{
	return GetMusicExist ( iID );
}
	
int dbMusicPlaying ( int iID )
{
	return GetMusicPlaying ( iID );
}
	
int dbMusicLooping ( int iID )
{
	return GetMusicLooping ( iID );
}
	
int dbMusicPaused ( int iID )
{
	return GetMusicPaused ( iID );
}
	
int dbMusicVolume ( int iID )
{
	return GetMusicVolume ( iID );
}

int dbMusicSpeed ( int iID )
{
	return GetMusicSpeed ( iID );
}

int dbGetNumberOfCDTracks ( void )
{
	return GetNumberOfCDTracks ( );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////