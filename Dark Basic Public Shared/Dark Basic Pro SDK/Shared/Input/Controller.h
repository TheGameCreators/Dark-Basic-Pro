#pragma once

#include "cinputc.h"

class CController
{
public:
    CController();
    ~CController();

    void Release();
    void Acquire( LPDIRECTINPUTDEVICE8 lpNewDevice );
    void Poll( DIJOYSTATE2& ControlDeviceBuffer );

    void SetForceFeedback() { bHasForceFeedback = true; }
    bool HasForceFeedback() const { return bHasForceFeedback; }

    void SetName(char* sNewName);
    const char* Name() const { return sName; }

    // Effects
    bool DirectionEffect(short id, int magnitude, bool bStart);
    bool ShootEffect(int magnitude, int delay, bool bStart);
    bool ChainsawEffect(int magnitude, int delay, bool bStart);
    bool ImpactEffect(int magnitude, int delay, bool bStart);
    bool AngleEffect(int magnitude, int angle, int delay, bool bStart);
    bool WaterEffect(int magnitude, int delay, bool bStart);
    void StopEffect();
    void AutoCenter(bool AutoCenterOn);

    bool HasX, HasY, HasZ, HasRx, HasRy, HasRz;
    int  SliderCount;
    int  ButtonCount;
    int  PovCount;
    int  Deadzone;

    // Safer version of operator bool
    operator const void *() const { return lpDevice; }
private:
    LPDIRECTINPUTDEVICE8    lpDevice;
    bool                    bHasForceFeedback;
    char                    sName[256];

    enum E_ForceType
    {
        EFF_UP,
        EFF_DOWN,
        EFF_LEFT,
        EFF_RIGHT,
        EFF_ANGLE,
        EFF_CHAINSAW,
        EFF_SHOOT,
        EFF_IMPACT,
        EFF_NONE,
        EFF_WATER1,
        EFF_WATER2,
        EFF_MAX
    };

    LPDIRECTINPUTEFFECT     lpdiEffect[ EFF_MAX ];

    void ResetDetail();
};
