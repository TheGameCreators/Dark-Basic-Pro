//----------------------------------------------------------------------------
// File: fileinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef FILEINFO_H
#define FILEINFO_H


struct FileNode
{
    TCHAR m_szName[MAX_PATH];
    TCHAR m_szPath[MAX_PATH];
    TCHAR m_szVersion[50];
    TCHAR m_szLanguageEnglish[100];
    TCHAR m_szLanguageLocalized[100]; 
    FILETIME m_FileTime;
    TCHAR m_szDatestampEnglish[30];
    TCHAR m_szDatestampLocalized[30]; 
    TCHAR m_szAttributes[50];
    LONG m_lNumBytes;
    BOOL m_bExists;
    BOOL m_bBeta;
    BOOL m_bDebug;
    BOOL m_bObsolete; 
    BOOL m_bProblem;

    DWORD m_nElementCount;
};

struct FileInfo
{
    vector<FileNode*> m_vDxComponentsFiles;
    TCHAR m_szDXFileNotesLocalized[3000];       // DirectX file notes (localized)
    TCHAR m_szDXFileNotesEnglish[3000];         // DirectX file notes (english)
    DWORD m_nElementCount;
};

#endif // FILEINFO_H