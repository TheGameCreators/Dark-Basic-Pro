/*
            File : DBPro/Text.hpp
       Generated : 2011/09/02 20:14:59
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_Text_hpp
#define INCLUDE_GUARD_DBPro_Text_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    int __cdecl Asc(LPCSTR String);
    LPSTR __cdecl Bin(int Value);
    void __cdecl CenterText(int X,int Y,LPCSTR String);
    LPSTR __cdecl Chr(int Value);
    int __cdecl CompareCase(LPCSTR StringA,LPCSTR StringB);
    int __cdecl FindFirstChar(LPCSTR Source,LPCSTR Char);
    int __cdecl FindLastChar(LPCSTR Source,LPCSTR Char);
    int __cdecl FindSubString(LPCSTR Source,LPCSTR String);
    LPSTR __cdecl FirstToken(LPCSTR Source,LPCSTR Delim);
    LPSTR __cdecl Hex(int Value);
    LPSTR __cdecl Left(LPCSTR String,int Value);
    int __cdecl Len(LPCSTR String);
    LPSTR __cdecl Lower(LPCSTR String);
    LPSTR __cdecl Mid(LPCSTR String,int Value);
    LPSTR __cdecl NextToken(LPCSTR Delim);
    void __cdecl PerformChecklistForFonts();
    void __cdecl Reverse(LPCSTR String);
    LPSTR __cdecl Right(LPCSTR String,int Value);
    void __cdecl SetTextFont(LPCSTR Fontname);
    void __cdecl SetTextFont(LPCSTR Fontname,int CharsetValue);
    void __cdecl SetTextOpaque();
    void __cdecl SetTextSize(int PointSize);
    void __cdecl SetTextToBold();
    void __cdecl SetTextToBolditalic();
    void __cdecl SetTextToItalic();
    void __cdecl SetTextToNormal();
    void __cdecl SetTextTransparent();
    LPSTR __cdecl Space(int NumberOfSpaces);
    LPSTR __cdecl Str(float Value);
    LPSTR __cdecl Str(float Value,int DecimalPlaces);
    LPSTR __cdecl Str(int Value);
    LPSTR __cdecl Str(double DoubleFloat);
    LPSTR __cdecl Str(LONGLONG DoubleInteger);
    int __cdecl TextBackgroundType();
    LPSTR __cdecl TextFont();
    int __cdecl TextHeight(LPCSTR String);
    int __cdecl TextSize();
    int __cdecl TextStyle();
    int __cdecl TextWidth(LPCSTR String);
    void __cdecl Text(int X,int Y,LPCSTR String);
    LPSTR __cdecl Upper(LPCSTR String);
    float __cdecl Val(LPCSTR String);
}

#endif
