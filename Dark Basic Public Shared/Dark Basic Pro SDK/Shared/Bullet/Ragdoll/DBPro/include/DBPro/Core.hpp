/*
            File : DBPro/Core.hpp
       Generated : 2011/09/02 20:14:33
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Core_hpp
#define INCLUDE_GUARD_DBPro_Core_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


// Include DBPro specific types, if not already included
#ifndef INCLUDE_GUARD_Support_DBProTypes_hpp
#include <Support/DBProTypes.hpp>
#endif



namespace DBPro
{
    float __cdecl Abs(float Value);
    float __cdecl Acos(float Value);
    LPVOID __cdecl AddToQueue(DWORD* ArrayName);
    LPVOID __cdecl AddToStack(DWORD* ArrayName);
    void __cdecl AlwaysActiveOff();
    void __cdecl AlwaysActiveOn();
    int __cdecl ArrayCount(DWORD* ArrayName);
    void __cdecl ArrayDeleteElement(DWORD* ArrayName);
    void __cdecl ArrayDeleteElement(DWORD* ArrayName,int Index);
    void __cdecl ArrayIndexToBottom(DWORD* ArrayName);
    void __cdecl ArrayIndexToQueue(DWORD* ArrayName);
    void __cdecl ArrayIndexToStack(DWORD* ArrayName);
    void __cdecl ArrayIndexToTop(DWORD* ArrayName);
    int __cdecl ArrayIndexValid(DWORD* ArrayName);
    LPVOID __cdecl ArrayInsertAtBottom(DWORD* ArrayName);
    LPVOID __cdecl ArrayInsertAtBottom(DWORD* ArrayName,int Index);
    LPVOID __cdecl ArrayInsertAtElement(DWORD* ArrayName,int Index);
    LPVOID __cdecl ArrayInsertAtTop(DWORD* ArrayName);
    LPVOID __cdecl ArrayInsertAtTop(DWORD* ArrayName,int Index);
    float __cdecl Asin(float Value);
    float __cdecl Atanfull(float DistanceX,float DistanceY);
    float __cdecl Atan(float Value);
    float __cdecl Ceil(float Value);
    LPSTR __cdecl Cl();
    void __cdecl Cls();
    float __cdecl Cos(float Value);
    float __cdecl Curveangle(float DestinationValue,float CurrentValue,float SpeedValue);
    float __cdecl Curvevalue(float DestinationValue,float CurrentValue,float SpeedValue);
    void __cdecl DrawSpritesFirst();
    void __cdecl DrawSpritesLast();
    void __cdecl DrawToBack();
    void __cdecl DrawToCamera();
    void __cdecl DrawToFront();
    void __cdecl DrawToScreen();
    void __cdecl EmptyArray(DWORD* ArrayName);
    float __cdecl Exp(float Value);
    void __cdecl Fastsync();
    void __cdecl Fastsync(int NonDisplayUpdateFlag);
    float __cdecl Floor(float Value);
    int __cdecl GetArrayType(DWORD* ArrayName);
    LPSTR __cdecl GetDate();
    LPSTR __cdecl GetDxVersion();
    LPSTR __cdecl GetTime();
    LPSTR __cdecl GetTypePattern(LPCSTR TypeName,DWORD TypeIndex);
    float __cdecl Hcos(float Value);
    float __cdecl Hsin(float Value);
    float __cdecl Htan(float Value);
    LPSTR __cdecl Inkey();
    int __cdecl Int(float Value);
    void __cdecl LoadArray(LPCSTR Filename,DWORD* ArrayName);
    float __cdecl Newxvalue(float CurrentXValue,float AngleValue,float StepValue);
    float __cdecl Newyvalue(float CurrentYValue,float AngleValue,float StepValue);
    float __cdecl Newzvalue(float CurrentZValue,float AngleValue,float StepValue);
    void __cdecl NextArrayIndex(DWORD* ArrayName);
    LONGLONG __cdecl Perffreq();
    LONGLONG __cdecl Perftimer();
    void __cdecl PreviousArrayIndex(DWORD* ArrayName);
    void __cdecl Randomize(int SeedValue);
    void __cdecl RemoveFromQueue(DWORD* ArrayName);
    void __cdecl RemoveFromStack(DWORD* ArrayName);
    int __cdecl Rnd(int RangeValue);
    void __cdecl SaveArray(LPCSTR Filename,DWORD* ArrayName);
    void __cdecl SetCursor(int X,int Y);
    float __cdecl Sin(float Value);
    void __cdecl Sleep(int Number);
    float __cdecl Sqrt(float Value);
    void __cdecl SuspendForKey();
    void __cdecl SuspendForMouse();
    void __cdecl SyncMask(DWORD IgnoreCameraMask);
    void __cdecl SyncOff();
    void __cdecl SyncOn();
    void __cdecl SyncRate(int Rate);
    void __cdecl SyncSleep(int FlagProcessorFriendly);
    void __cdecl Sync();
    void __cdecl Sync(int ProcessMessages);
    float __cdecl Tan(float Value);
    int __cdecl Timer();
    void __cdecl WaitKey();
    void __cdecl WaitMouse();
    void __cdecl Wait(int a);
    float __cdecl Wrapvalue(float AngleValue);
    LPVOID __cdecl AllocMem(int SizeInBytes);
    void __cdecl FreeMem(LPVOID MemoryAddress);
    DWORD __cdecl ProcessMessages();
    DWORD __cdecl ProcessMessagesOnly();
    DWORD __cdecl Quit();
    DWORD __cdecl CloseDisplay();
    void __cdecl InternalClearWindowsEntry();
    void __cdecl AddToRenderList(function_vv Function,int Priority);
    void __cdecl RemoveFromRenderList(function_vv Function);
}

#endif
