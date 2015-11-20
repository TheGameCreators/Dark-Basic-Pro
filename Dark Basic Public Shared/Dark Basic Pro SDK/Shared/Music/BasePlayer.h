#ifndef _BASEPLAYER_H_
#define _BASEPLAYER_H_

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)

namespace DBP_Music
{
    class BasePlayer
    {
    public:
        enum ePlayState
        {
            eStopped,
            ePlaying,
            ePaused,
        };

        BasePlayer() :
            eState( eStopped ),
            bLoop( false ),
            iVolume( 100 ),
            iSpeed( 100 )
            {}
        virtual ~BasePlayer() { }

    public:
        void Update();
        void Play();
        void Loop();
        void Stop();
        void Pause();
        void Resume();
        void SetVolume(int Vol);
        void SetSpeed(int Spd);

        bool IsPlaying() const { return eState == ePlaying; }
        bool IsPaused()  const { return eState == ePaused; }
        bool IsLooping() const { return bLoop; }
        int  Volume()    const { return iVolume; }
        int  Speed()     const { return iSpeed; }

    private:
        virtual bool DoUpdate()             = 0;
        virtual void DoPlay()               = 0;
        virtual void DoStop()               = 0;
        virtual void DoPause()              = 0;
        virtual void DoResume()             = 0;
        virtual void DoSetVolume( int Vol ) = 0;
        virtual void DoSetSpeed( int Spd )  = 0;

    protected:
	    // shared properties
        ePlayState                  eState;
	    bool						bLoop;				// should music loop?

	    int							iVolume;
	    int							iSpeed;
    };
}

#endif _BASEPLAYER_H_
