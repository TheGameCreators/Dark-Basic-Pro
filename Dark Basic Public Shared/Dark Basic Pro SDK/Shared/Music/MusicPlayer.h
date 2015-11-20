#pragma once
#include "BasePlayer.h"

#include <direct.h>
#include <dshow.h>

namespace DBP_Music
{
    class MusicPlayer : public BasePlayer
    {
    public:
        MusicPlayer( LPCSTR szFilename );
        ~MusicPlayer();

    private:
        virtual bool DoUpdate();
        virtual void DoPlay();
        virtual void DoStop();
        virtual void DoPause();
        virtual void DoResume();
        virtual void DoSetVolume( int Vol );
        virtual void DoSetSpeed( int Spd );

        bool InitialiseInterfaces( LPCSTR szFilename );
        void ReleaseInterfaces();

	    IGraphBuilder*				pGraphBuilder;
	    IMediaControl*				pMediaControl;
	    IMediaEventEx*				pMediaEvent;
	    IBasicAudio*				pBasicAudio;
	    IMediaSeeking*				pMediaSeeking;
    };
}
