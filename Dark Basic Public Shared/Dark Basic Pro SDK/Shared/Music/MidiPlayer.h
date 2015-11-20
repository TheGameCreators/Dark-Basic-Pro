#pragma once
#include <wtypes.h>

// Set to 0 to play midi via DirectShow
// Set to 1 to play midi via MCI
#define _MIDI_PLAYER_TYPE_      0

#if _MIDI_PLAYER_TYPE_ == 0

#include "MusicPlayer.h"

namespace DBP_Music
{
    class MidiPlayer : public MusicPlayer
    {
    public:
        MidiPlayer( LPCSTR szFilename) : MusicPlayer(szFilename) {}

    private:
        virtual void DoSetVolume( int Vol );
    };
}

#else

#include "BasePlayer.h"

namespace DBP_Music
{
    class MidiPlayer : public BasePlayer
    {
    public:
        MidiPlayer( LPCSTR szFilename );
        ~MidiPlayer();

    private:
        virtual bool DoUpdate();
        virtual void DoPlay();
        virtual void DoStop();
        virtual void DoPause();
        virtual void DoResume();
        virtual void DoSetVolume( int Vol );
        virtual void DoSetSpeed( int Spd ) { }

        LPSTR szAlias;
    };
}

#endif
