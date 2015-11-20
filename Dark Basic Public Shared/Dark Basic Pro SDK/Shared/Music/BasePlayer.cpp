#include "BasePlayer.h"

namespace DBP_Music
{
    void BasePlayer::Update()
    {
        if (eState == ePlaying)
        {
            bool Playing = DoUpdate();

            if (Playing == false)
                eState = eStopped;
        }
    }

    void BasePlayer::Play()
    {
        if (eState == ePaused)
            DoStop();

        if (eState != ePlaying)
            DoPlay();

        eState = ePlaying;
        bLoop = false;
    }

    void BasePlayer::Loop()
    {
        Play();
        bLoop = true;
    }

    void BasePlayer::Stop()
    {
        DoStop();

        eState = eStopped;
    }

    void BasePlayer::Pause()
    {
        if (eState == ePlaying)
        {
            DoPause();
            eState = ePaused;
        }
    }

    void BasePlayer::Resume()
    {
        if (eState == ePaused)
        {
            DoResume();

            eState = ePlaying;
        }
    }

    void BasePlayer::SetVolume(int Vol)
    {
        DoSetVolume(Vol);
        iVolume = Vol;
    }

    void BasePlayer::SetSpeed(int Spd)
    {
        DoSetSpeed(Spd);
        iSpeed = Spd;
    }
}
