//----------------------------------------------------------------------------
// File: netinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef NETINFO_H
#define NETINFO_H

struct NetSP
{
    TCHAR m_szNameLocalized[200];
    TCHAR m_szNameEnglish[200];
    TCHAR m_szGuid[100];
    TCHAR m_szFile[100];
    TCHAR m_szPath[MAX_PATH];
    TCHAR m_szVersionLocalized[50];
    TCHAR m_szVersionEnglish[50];
    BOOL m_bRegistryOK;
    BOOL m_bProblem;
    BOOL m_bFileMissing;
    BOOL m_bInstalled;
    DWORD m_dwDXVer;

    DWORD m_nElementCount;
};

struct NetAdapter
{
    TCHAR m_szAdapterName[200];
    TCHAR m_szSPNameEnglish[200];
    TCHAR m_szSPNameLocalized[200];
    TCHAR m_szGuid[100];
    DWORD m_dwFlags;

    DWORD m_nElementCount;
};

struct NetApp
{
    TCHAR m_szName[200];
    TCHAR m_szGuid[100];
    TCHAR m_szExeFile[100];
    TCHAR m_szExePath[MAX_PATH];
    TCHAR m_szExeVersionLocalized[50];
    TCHAR m_szExeVersionEnglish[50];
    TCHAR m_szLauncherFile[100];
    TCHAR m_szLauncherPath[MAX_PATH];
    TCHAR m_szLauncherVersionLocalized[50];
    TCHAR m_szLauncherVersionEnglish[50];
    BOOL m_bRegistryOK;
    BOOL m_bProblem;
    BOOL m_bFileMissing;
    DWORD m_dwDXVer;

    DWORD m_nElementCount;
};

struct NetVoiceCodec
{
    TCHAR m_szName[200];
    TCHAR m_szGuid[100];
    TCHAR m_szDescription[500];
    DWORD m_dwFlags;
    DWORD m_dwMaxBitsPerSecond;

    DWORD m_nElementCount;
};

struct NetInfo
{
    TCHAR m_szNetworkNotesLocalized[3000];       // DirectX file notes (localized)
    TCHAR m_szNetworkNotesEnglish[3000];         // DirectX file notes (english)
    TCHAR m_szRegHelpText[3000];
    TCHAR m_szTestResultLocalized[3000];
    TCHAR m_szTestResultEnglish[3000];
    TCHAR m_szVoiceWizardFullDuplexTestLocalized[200];
    TCHAR m_szVoiceWizardHalfDuplexTestLocalized[200];
    TCHAR m_szVoiceWizardMicTestLocalized[200];
    TCHAR m_szVoiceWizardFullDuplexTestEnglish[200];
    TCHAR m_szVoiceWizardHalfDuplexTestEnglish[200];
    TCHAR m_szVoiceWizardMicTestEnglish[200];

    vector<NetSP*> m_vNetSPs;
    vector<NetApp*> m_vNetApps;
    vector<NetVoiceCodec*> m_vNetVoiceCodecs;
    vector<NetAdapter*> m_vNetAdapters;
    DWORD m_nElementCount;
};

#endif // NETINFO_H
