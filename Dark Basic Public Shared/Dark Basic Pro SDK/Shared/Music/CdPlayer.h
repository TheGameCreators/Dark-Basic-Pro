#pragma once
#include "BasePlayer.h"

namespace DBP_Music
{
    class CdPlayer : public BasePlayer
    {
    public:
        CdPlayer(int Track);
        ~CdPlayer();

        static int TracksAvailable();
    private:
        virtual bool DoUpdate();
        virtual void DoPlay();
        virtual void DoStop();
        virtual void DoPause();
        virtual void DoResume();
        virtual void DoSetVolume( int Vol );
        virtual void DoSetSpeed( int Spd );

        int	iTrack;
        char* szTrackEnd;

        static CdPlayer* pCurrent;
    };
}
