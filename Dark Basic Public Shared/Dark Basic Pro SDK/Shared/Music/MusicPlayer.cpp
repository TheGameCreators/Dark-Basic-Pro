#include "MusicPlayer.h"
#include "CMusicC.h"
#include ".\..\error\cerror.h"

namespace DBP_Music
{
    MusicPlayer::MusicPlayer( LPCSTR szFilename ) :
        pGraphBuilder( 0 ),
        pMediaControl( 0 ),
        pMediaEvent( 0 ),
        pBasicAudio( 0 ),
        pMediaSeeking( 0 )
    {
        if (! InitialiseInterfaces( szFilename ))
        {
            ReleaseInterfaces();
	        RunTimeError(RUNTIMEERROR_MUSICLOADFAILED);
        }
        else
            DoSetVolume(100);
    }

    MusicPlayer::~MusicPlayer()
    {
        ReleaseInterfaces();
    }

    bool MusicPlayer::DoUpdate()
    {
	    LONG	evCode,
			    evParam1,
			    evParam2;

        bool Playing = true;

	    while ( SUCCEEDED ( pMediaEvent->GetEvent ( &evCode, &evParam1, &evParam2, 0 ) ) )
	    {
		    // run through events
		    pMediaEvent->FreeEventParams ( evCode, evParam1, evParam2 );

		    if ( evCode == EC_COMPLETE )
		    {
                DoStop();
                if ( bLoop )
                    DoPlay();
                else
                    Playing = false;
		    }
	    }

        return Playing;
    }

    void MusicPlayer::DoPlay()
    {
        pMediaControl->Run ( );
    }

    void MusicPlayer::DoStop()
    {
	    // reset to start
	    __int64 pos = 0;
	    pMediaSeeking->SetPositions ( &pos, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning );
	    pMediaControl->Stop ( );
    }

    void MusicPlayer::DoPause()
    {
        pMediaControl->Stop();
    }

    void MusicPlayer::DoResume()
    {
        pMediaControl->Run();
    }

    // TODO: Does not work for MIDI for some reason
    void MusicPlayer::DoSetVolume(int Vol)
    {
        long lVolume = -2500 - (iVolume * -25);
	    if(iVolume>0)
		    pBasicAudio->put_Volume ( lVolume );
	    else
		    pBasicAudio->put_Volume ( -10000 );
    }

    void MusicPlayer::DoSetSpeed(int Spd)
    {
	    pMediaControl->Pause();
	    pMediaSeeking->SetRate( (double)iSpeed/100.0 );
	    pMediaControl->Run();

    }


    bool MusicPlayer::InitialiseInterfaces( LPCSTR szFilename )
    {
        HRESULT hr = CoCreateInstance(	CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, 
					        IID_IGraphBuilder, (void **)&pGraphBuilder);
        if (SUCCEEDED(hr))
        {
            DWORD dwLength = strlen(szFilename)+1;
            wchar_t* pWideStr = new wchar_t[dwLength];
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFilename, -1, pWideStr, dwLength*2);

            hr = pGraphBuilder->RenderFile ( pWideStr, 0 );
            delete[] pWideStr;
        }

        if (SUCCEEDED(hr))
            hr = pGraphBuilder->QueryInterface ( IID_IMediaControl, ( void** ) &pMediaControl );
        
        if (SUCCEEDED(hr))
            hr = pGraphBuilder->QueryInterface ( IID_IMediaEventEx, ( void** ) &pMediaEvent );
        
        if (SUCCEEDED(hr))
            hr = pGraphBuilder->QueryInterface ( IID_IMediaSeeking, ( void** ) &pMediaSeeking );
        
        if (SUCCEEDED(hr))
            hr = pGraphBuilder->QueryInterface ( IID_IBasicAudio, ( void** ) &pBasicAudio );
        
        return SUCCEEDED(hr);
    }

    void MusicPlayer::ReleaseInterfaces()
    {
        SAFE_RELEASE ( pBasicAudio );
        SAFE_RELEASE ( pMediaSeeking );
        SAFE_RELEASE ( pMediaEvent );
        SAFE_RELEASE ( pMediaControl );
        SAFE_RELEASE ( pGraphBuilder );
    }
}
