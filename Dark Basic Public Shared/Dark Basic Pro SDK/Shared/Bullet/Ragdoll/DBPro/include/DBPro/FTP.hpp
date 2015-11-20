/*
            File : DBPro/FTP.hpp
       Generated : 2011/09/02 20:14:35
   DBPro release : 7.7
*/

#ifndef INCLUDE_GUARD_DBPro_FTP_hpp
#define INCLUDE_GUARD_DBPro_FTP_hpp

#ifdef DBPRO__COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

#define VC_EXTRALEAN
#include <windows.h>
#include <D3dx9tex.h>


namespace DBPro
{
    void __cdecl FtpConnect(LPCSTR UrlString,LPCSTR UserString,LPCSTR PasswordString);
    void __cdecl FtpConnect(LPCSTR UrlString,LPCSTR UserString,LPCSTR PasswordString,int WindowMode);
    void __cdecl FtpDeleteFile(LPCSTR FtpFilenameString);
    void __cdecl FtpDisconnect();
    void __cdecl FtpDisconnect(int DialupdisconnectFlag);
    void __cdecl FtpFindFirst();
    void __cdecl FtpFindNext();
    void __cdecl FtpGetFile(LPCSTR FtpFilenameString,LPCSTR b);
    void __cdecl FtpGetFile(LPCSTR FtpFilenameString,LPCSTR LocalFilenameString,int GrabinbitsFlag);
    void __cdecl FtpProceed();
    void __cdecl FtpPutFile(LPCSTR LocalFilenameString);
    void __cdecl FtpSetDir(LPCSTR DirectoryString);
    void __cdecl FtpTerminate();
    LPSTR __cdecl GetFtpDir();
    LPSTR __cdecl GetFtpError();
    int __cdecl GetFtpFailure();
    LPSTR __cdecl GetFtpFileName();
    int __cdecl GetFtpFileSize();
    int __cdecl GetFtpFileType();
    int __cdecl GetFtpProgress();
    int __cdecl GetFtpStatus();
    void __cdecl HttpConnect(LPCSTR UrlString);
    void __cdecl HttpConnect(LPCSTR UrlString,int Port);
    void __cdecl HttpDisconnect();
    LPSTR __cdecl HttpRequestData(LPCSTR Verb,LPCSTR Object,LPCSTR StringToSend);
    LPSTR __cdecl HttpRequestData(LPCSTR Verb,LPCSTR Object,LPCSTR StringToSend,int AccessFlag);
}

#endif
