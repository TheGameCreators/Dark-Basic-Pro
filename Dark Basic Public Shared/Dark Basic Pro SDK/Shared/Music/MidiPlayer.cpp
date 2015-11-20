#include "MidiPlayer.h"
#include "CMusicC.h"
#include ".\..\core\globstruct.h"
#include ".\..\error\cerror.h"

#include <stdio.h>

namespace DBP_Music
{
    // Use mixer to adjust midi volume whether using DirectShow
    // or MCI - this may change in future.
    void MidiPlayer::DoSetVolume( int Vol )
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

    #if _MIDI_PLAYER_TYPE_ != 0

    MidiPlayer::MidiPlayer( LPCSTR szFilename )
    {
        // Generate a unique name for this midi file
        szAlias = new char[ 9 ];
        sprintf(szAlias, "%08x", this);

        char* szCommand = new char[ strlen(szFilename) + 100 ];
        sprintf(szCommand, "open \"%s\" type sequencer alias %s", szFilename, szAlias);

        int Status = mciSendString ( szCommand, NULL, NULL, NULL );
        delete[] szCommand;

        if (Status)
	        RunTimeError(RUNTIMEERROR_MUSICLOADFAILED);
    }

    MidiPlayer::~MidiPlayer()
    {
        DoStop();

        char szCommand[256];
        sprintf ( szCommand, "close %s", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );

        delete[] szAlias;
    }

    bool MidiPlayer::DoUpdate()
    {
        char szCommand [ 256 ];
	    char szReturn  [ 256 ];

        sprintf ( szCommand, "status %s mode", szAlias );
        mciSendString ( szCommand, szReturn, sizeof(szReturn), 0 );

        if (strcmp(szReturn, "stopped") == 0)
        {
            sprintf ( szCommand, "seek %s to start", szAlias );
            mciSendString ( szCommand, 0, 0, 0 );

            if ( !bLoop)
                return false;

		    DoPlay();
        }
        return true;
    }

    void MidiPlayer::DoPlay()
    {
        char szCommand[256];
        sprintf ( szCommand, "play %s", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );
    }

    void MidiPlayer::DoStop()
    {
        char szCommand[256];
        sprintf ( szCommand, "stop %s", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );
        sprintf ( szCommand, "seek %s to start", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );
    }

    void MidiPlayer::DoPause()
    {
        char szCommand[256];
        sprintf ( szCommand, "pause %s", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );
    }

    void MidiPlayer::DoResume()
    {
        char szCommand[256];
        sprintf ( szCommand, "play %s", szAlias );
        mciSendString ( szCommand, 0, 0, 0 );
    }

    #endif
}