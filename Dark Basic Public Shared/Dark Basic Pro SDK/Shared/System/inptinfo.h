//----------------------------------------------------------------------------
// File: inptinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef INPUTINFO_H
#define INPUTINFO_H

struct InputRelatedDeviceInfo
{
    DWORD     m_dwVendorID;
    DWORD     m_dwProductID;
    TCHAR     m_szDescription[MAX_PATH];
    TCHAR     m_szLocation[MAX_PATH];
    TCHAR     m_szMatchingDeviceId[MAX_PATH];
    TCHAR     m_szUpperFilters[MAX_PATH];
    TCHAR     m_szService[MAX_PATH];
    TCHAR     m_szLowerFilters[MAX_PATH];
    TCHAR     m_szOEMData[MAX_PATH];
    TCHAR     m_szFlags1[MAX_PATH];
    TCHAR     m_szFlags2[MAX_PATH];

    vector<FileNode*> m_vDriverList;
    vector<InputRelatedDeviceInfo*> m_vChildren;
    DWORD m_nElementCount;
};

struct InputDeviceInfo
{
    TCHAR m_szInstanceName[MAX_PATH];
    BOOL  m_bAttached;
    DWORD m_dwJoystickID;  
    DWORD m_dwVendorID;
    DWORD m_dwProductID;
    DWORD m_dwDevType;
    TCHAR m_szFFDriverName[MAX_PATH];
    TCHAR m_szFFDriverDateEnglish[MAX_PATH];
    TCHAR m_szFFDriverVersion[MAX_PATH];
    LONG  m_lFFDriverSize;

    DWORD m_nElementCount;
};

struct InputInfo
{
    BOOL  m_bPollFlags;
    TCHAR m_szInputNotesLocalized[3000];       // DirectX file notes (localized)
    TCHAR m_szInputNotesEnglish[3000];         // DirectX file notes (english)
    TCHAR m_szRegHelpText[3000];

    vector<InputRelatedDeviceInfo*> m_vGamePortDevices;
    vector<InputRelatedDeviceInfo*> m_vUsbRoot;
    vector<InputRelatedDeviceInfo*> m_vPS2Devices;
    vector<InputDeviceInfo*>        m_vDirectInputDevices;

    DWORD m_nElementCount;
};


#endif // INPUTINFO_H