//----------------------------------------------------------------------------
// File: musinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef MUSINFO_H
#define MUSINFO_H

struct MusicPort
{
    TCHAR m_szGuid[300];
    BOOL m_bSoftware;
    BOOL m_bKernelMode;
    BOOL m_bUsesDLS;
    BOOL m_bExternal;
    DWORD m_dwMaxAudioChannels;
    DWORD m_dwMaxChannelGroups;
    BOOL m_bDefaultPort;
    BOOL m_bOutputPort;
    TCHAR m_szDescription[300];

    DWORD m_nElementCount;
};

struct MusicInfo
{
    BOOL m_bDMusicInstalled;
    TCHAR m_szGMFilePath[MAX_PATH]; 
    TCHAR m_szGMFileVersion[100];
    BOOL m_bAccelerationEnabled;
    BOOL m_bAccelerationExists;
    
    TCHAR m_szNotesLocalized[3000]; 
    TCHAR m_szNotesEnglish[3000]; 
    TCHAR m_szRegHelpText[3000];
    TCHAR m_szTestResultLocalized[3000];
    TCHAR m_szTestResultEnglish[3000];

    vector<MusicPort*> m_vMusicPorts;
    DWORD m_nElementCount;
};

#endif // DISPINFO_H
