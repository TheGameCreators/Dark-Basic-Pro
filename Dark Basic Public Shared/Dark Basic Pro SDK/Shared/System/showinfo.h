//----------------------------------------------------------------------------
// File: showinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef SHOWINFO_H
#define SHOWINFO_H

struct ShowFilterInfo
{
    TCHAR   m_szName[1024];             // friendly name
    TCHAR   m_szVersion[32];            // version
    TCHAR   m_ClsidFilter[300];         // guid
    TCHAR   m_szFileName[MAX_PATH];     // file name
    TCHAR   m_szFileVersion[32];        // file version
    TCHAR   m_szCatName[1024];          // category name
    TCHAR   m_ClsidCat[300];            // category guid
    DWORD   m_dwInputs;                 // number input pins
    DWORD   m_dwOutputs;                // number output pins
    DWORD   m_dwMerit;                  // merit - in hex

    DWORD m_nElementCount;
};

struct ShowInfo
{
    vector<ShowFilterInfo*> m_vShowFilters;
};

#endif // SHOWINFO_H