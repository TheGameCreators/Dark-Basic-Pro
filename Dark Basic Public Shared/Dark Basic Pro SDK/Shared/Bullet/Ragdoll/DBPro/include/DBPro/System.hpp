/*
            File : DBPro/System.hpp
       Generated : 2011/09/02 20:14:56
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_System_hpp
#define INCLUDE_GUARD_DBPro_System_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    float __cdecl ChecklistFvalueA(int Index);
    float __cdecl ChecklistFvalueB(int Index);
    float __cdecl ChecklistFvalueC(int Index);
    float __cdecl ChecklistFvalueD(int Index);
    int __cdecl ChecklistQuantity();
    LPSTR __cdecl ChecklistString(int a);
    int __cdecl ChecklistValueA(int a);
    int __cdecl ChecklistValueB(int a);
    int __cdecl ChecklistValueC(int a);
    int __cdecl ChecklistValueD(int a);
    void __cdecl DisableEscapekey();
    void __cdecl DisableSystemkeys();
    void __cdecl EmptyChecklist();
    void __cdecl EnableEscapekey();
    void __cdecl EnableSystemkeys();
    void __cdecl ExitPrompt(LPCSTR MessageString,LPCSTR CaptionString);
    int __cdecl SystemDmemAvailable();
    int __cdecl SystemSmemAvailable();
    int __cdecl SystemSmemAvailable(int Mode);
    int __cdecl SystemTmemAvailable();
}

#endif
