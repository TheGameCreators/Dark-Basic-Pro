#include "CdPlayer.h"
#include "CMusicC.h"

#include ".\..\core\globstruct.h"
#include ".\..\error\cerror.h"

#include <stdio.h>

namespace DBP_Music
{
    CdPlayer* CdPlayer::pCurrent = 0;

    CdPlayer::CdPlayer(int Track) :
        iTrack( Track ),
        szTrackEnd( new char[20] )
    {
	    // open the audio and set the time format
	    mciSendString ( "open cdaudio", 0, 0, 0 );
	    mciSendString ( "set cdaudio time format tmsf", 0, 0, 0 );

        strcpy(szTrackEnd, "00:00:00:00");
        DoSetVolume(100);
    }

    CdPlayer::~CdPlayer()
    {
        DoStop();
        delete[] szTrackEnd;
    }

    bool CdPlayer::DoUpdate()
    {
        // Get the current position in the track
	    char szReturn  [ 256 ];
        mciSendString ( "status cdaudio position", szReturn, sizeof(szReturn), 0 );

        // If the current position matches or exceeds the track length, then we're done
        if (strcmp( szReturn, szTrackEnd ) >= 0)
	    {
            if ( !bLoop)
            {
                DoStop();
                return false;
            }

		    DoPlay();
	    }

        return true;
    }

    void CdPlayer::DoPlay()
    {
        int iMaxTrack = CdPlayer::TracksAvailable();

        // Can only play 1 CD track at a time, so stop the currently playing track
        if (pCurrent)
            pCurrent->Stop();
        pCurrent = this;

        char	szCommand [ 256 ];
	    char	szReturn  [ 256 ];

        // End of track can always be detected using the track length so prepare that right now.
        sprintf( szCommand, "status cdaudio length track %d", iTrack );
        mciSendString ( szCommand, szReturn, sizeof( szReturn ), 0 );
        sprintf( szTrackEnd, "%02d:%s", iTrack, szReturn );

        // Start playing the track
	    if ( iTrack < iMaxTrack )
		    sprintf ( szCommand, "play cdaudio from %d to %d", iTrack, iTrack+1 );
	    else
		    sprintf ( szCommand, "play cdaudio from %d", iTrack );
	    mciSendString ( szCommand, 0, 0, 0 );
    }

    void CdPlayer::DoStop()
    {
        if (pCurrent == this)
            pCurrent = 0;
        mciSendString ( "stop cdaudio", 0, 0, 0 );
    }

    void CdPlayer::DoPause()
    {
        mciSendString ( "pause cdaudio", 0, 0, 0 );
    }

    void CdPlayer::DoResume()
    {
        mciSendString ( "resume cdaudio", 0, 0, 0 );
    }


    void CdPlayer::DoSetVolume(int Vol)
    {
	    DWORD m_nNumMixers = ::mixerGetNumDevs();
	    HMIXER m_hMixer = 0;
	    MIXERCAPS m_mxcaps;
	    ::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));
	    if (m_nNumMixers != 0)
	    {
		    if (::mixerOpen(&m_hMixer,
						    0,
						    reinterpret_cast<DWORD>(g_pGlob->hWnd),
						    0,
						    MIXER_OBJECTF_MIXER | CALLBACK_WINDOW)
			    != MMSYSERR_NOERROR)
			    return;

		    if (::mixerGetDevCaps(reinterpret_cast<UINT>(m_hMixer),
							      &m_mxcaps, sizeof(MIXERCAPS))
			    != MMSYSERR_NOERROR)
			    return;
	    }

	    // get dwLineID
	    DWORD m_dwVolumeControlID = 0;
	    MIXERLINE mxl;
	    mxl.cbStruct = sizeof(MIXERLINE);
	    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	    if (::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer),
						       &mxl,
						       MIXER_OBJECTF_HMIXER |
						       MIXER_GETLINEINFOF_COMPONENTTYPE)
		    != MMSYSERR_NOERROR)
		    return;

	    // get dwControlID
	    MIXERCONTROL mxc;
	    MIXERLINECONTROLS mxlc;
	    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	    mxlc.dwLineID = mxl.dwLineID;
	    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	    mxlc.cControls = 1;
	    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	    mxlc.pamxctrl = &mxc;
	    if (::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer),
							       &mxlc,
							       MIXER_OBJECTF_HMIXER |
							       MIXER_GETLINECONTROLSF_ONEBYTYPE)
		    != MMSYSERR_NOERROR)
		    return;

	    // store dwControlID
	    DWORD m_dwMinimum = mxc.Bounds.dwMinimum;
	    DWORD m_dwMaximum = mxc.Bounds.dwMaximum;
	    m_dwVolumeControlID = mxc.dwControlID;

	    // set volume
	    DWORD dwVal = (DWORD)(((m_dwMaximum - m_dwMinimum)/100.0f) * iVolume);
	    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	    MIXERCONTROLDETAILS mxcd;
	    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	    mxcd.dwControlID = m_dwVolumeControlID;
	    mxcd.cChannels = 1;
	    mxcd.cMultipleItems = 0;
	    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	    mxcd.paDetails = &mxcdVolume;
	    if (::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
								     &mxcd,
								     MIXER_OBJECTF_HMIXER |
								     MIXER_SETCONTROLDETAILSF_VALUE)
		    != MMSYSERR_NOERROR)
			    return;

	    if (m_hMixer != 0)
	    {
		    BOOL bSucc = (::mixerClose(m_hMixer) == MMSYSERR_NOERROR);
		    m_hMixer = 0;
	    }
    }

    void CdPlayer::DoSetSpeed(int Spd)
    {
        // Do nothing
    }

    int CdPlayer::TracksAvailable()
    {
	    char	szReturn  [ 256 ];

        mciSendString ( "status cdaudio number of tracks", szReturn, sizeof ( szReturn ), 0 );

	    return atoi ( szReturn );
    }
}
