/*
            File : DBPro/Input.hpp
       Generated : 2011/09/02 20:14:39
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Input_hpp
#define INCLUDE_GUARD_DBPro_Input_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl ChangeMouse(int CursorNumber);
    void __cdecl ChangeMouse(int CursorNumber,int TouchFriendly);
    void __cdecl ClearEntryBuffer();
    int __cdecl ControlDeviceIndex();
    LPSTR __cdecl ControlDeviceName();
    int __cdecl ControlDeviceX();
    int __cdecl ControlDeviceY();
    int __cdecl ControlDeviceZ();
    int __cdecl Controlkey();
    int __cdecl Downkey();
    LPSTR __cdecl Entry();
    LPSTR __cdecl Entry(int AutoBackspaceMode);
    int __cdecl Escapekey();
    void __cdecl ForceAngle(int MagnitudeValue,int AngleValue,int DelayValue);
    void __cdecl ForceAutoCenterOff();
    void __cdecl ForceAutoCenterOn();
    void __cdecl ForceChainsaw(int MagnitudeValue,int DelayValue);
    void __cdecl ForceDown(int MagnitudeValue);
    void __cdecl ForceImpact(int MagnitudeValue,int DelayValue);
    void __cdecl ForceLeft(int MagnitudeValue);
    void __cdecl ForceNoEffect();
    void __cdecl ForceRight(int MagnitudeValue);
    void __cdecl ForceShoot(int MagnitudeValue,int DelayValue);
    void __cdecl ForceUp(int MagnitudeValue);
    void __cdecl ForceWaterEffect(int MagnitudeValue,int DelayValue);
    LPSTR __cdecl GetClipboard();
    int __cdecl GetKeyState(int VirtualKey);
    LPSTR __cdecl GetRegistry(LPCSTR FolderName,LPCSTR KeyName);
    LPSTR __cdecl GetRegistry(LPCSTR FolderName,LPCSTR KeyName,int CurrentUserMode);
    void __cdecl HideMouse();
    int __cdecl JoystickDown();
    int __cdecl JoystickFireA();
    int __cdecl JoystickFireB();
    int __cdecl JoystickFireC();
    int __cdecl JoystickFireD();
    int __cdecl JoystickFireX(int ButtonNumber);
    int __cdecl JoystickHatAngle(int HatNumber);
    int __cdecl JoystickInformation(int InformationMode);
    int __cdecl JoystickLeft();
    int __cdecl JoystickRight();
    int __cdecl JoystickSliderA();
    int __cdecl JoystickSliderB();
    int __cdecl JoystickSliderC();
    int __cdecl JoystickSliderD();
    int __cdecl JoystickTwistX();
    int __cdecl JoystickTwistY();
    int __cdecl JoystickTwistZ();
    int __cdecl JoystickUp();
    int __cdecl JoystickX();
    int __cdecl JoystickY();
    int __cdecl JoystickZ();
    int __cdecl Keystate(int Scancode);
    int __cdecl Leftkey();
    int __cdecl Mouseclick();
    int __cdecl Mousemovex();
    int __cdecl Mousemovey();
    int __cdecl Mousemovez();
    int __cdecl Mousex();
    int __cdecl Mousey();
    int __cdecl Mousez();
    void __cdecl PerformChecklistForControlDevices();
    void __cdecl PositionMouse(int X,int Y);
    int __cdecl Returnkey();
    int __cdecl Rightkey();
    int __cdecl Scancode();
    void __cdecl SetControlDeviceIndex(int ZerobasedIndexOfDevice);
    void __cdecl SetControlDevice(LPCSTR DeviceName$);
    void __cdecl SetControlDevice(LPCSTR DeviceName$,int DeviceIndex);
    void __cdecl SetJoystickDeadzone(int PercentageDeadzone);
    void __cdecl SetKeyStateToggle(int VirtualKey,int ToggleFlag);
    void __cdecl SetRegistryHkey(int CurrentUserMode);
    int __cdecl Shiftkey();
    void __cdecl ShowMouse();
    int __cdecl Spacekey();
    int __cdecl Upkey();
    void __cdecl WriteStringToRegistry(LPCSTR FolderName,LPCSTR KeyName,LPCSTR String);
    void __cdecl WriteStringToRegistry(LPCSTR FolderName,LPCSTR KeyName,LPCSTR String,int CurrentUserMode);
    void __cdecl WriteToClipboard(LPCSTR String);
    void __cdecl WriteToRegistry(LPCSTR FolderName,LPCSTR KeyName,int Value);
}

#endif
