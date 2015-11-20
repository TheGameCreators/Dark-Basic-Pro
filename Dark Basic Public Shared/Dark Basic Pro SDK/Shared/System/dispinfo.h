//----------------------------------------------------------------------------
// File: dispinfo.h
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DISPINFO_H
#define DISPINFO_H

struct DxDiag_DXVA_DeinterlaceCaps 
{
    TCHAR szD3DInputFormat[100];    
    TCHAR szD3DOutputFormat[100];    
    TCHAR szGuid[100];    
    TCHAR szCaps[100];    
    DWORD dwNumPreviousOutputFrames;
    DWORD dwNumForwardRefSamples;
    DWORD dwNumBackwardRefSamples;

    DWORD m_nElementCount;
};

struct DisplayInfo
{
    TCHAR m_szDeviceName[100];
    TCHAR m_szDescription[200];
    TCHAR m_szKeyDeviceID[200];
    TCHAR m_szKeyDeviceKey[200];
    TCHAR m_szManufacturer[200];
    TCHAR m_szChipType[100];
    TCHAR m_szDACType[100];
    TCHAR m_szRevision[100];
    TCHAR m_szDisplayMemoryLocalized[100];
    TCHAR m_szDisplayMemoryEnglish[100];
    TCHAR m_szDisplayModeLocalized[100];
    TCHAR m_szDisplayModeEnglish[100];

    DWORD m_dwWidth;
    DWORD m_dwHeight;
    DWORD m_dwBpp;
    DWORD m_dwRefreshRate;

    TCHAR m_szMonitorName[100];
    TCHAR m_szMonitorMaxRes[100];

    TCHAR m_szDriverName[100];
    TCHAR m_szDriverVersion[100];
    TCHAR m_szDriverAttributes[100];
    TCHAR m_szDriverLanguageEnglish[100];
    TCHAR m_szDriverLanguageLocalized[100];
    TCHAR m_szDriverDateEnglish[100];
    TCHAR m_szDriverDateLocalized[100];
    LONG  m_lDriverSize;
    TCHAR m_szMiniVdd[100];
    TCHAR m_szMiniVddDateLocalized[100];
    TCHAR m_szMiniVddDateEnglish[100];
    LONG  m_lMiniVddSize;
    TCHAR m_szVdd[100];

    BOOL m_bCanRenderWindow;
    BOOL m_bDriverBeta;
    BOOL m_bDriverDebug;
    BOOL m_bDriverSigned;
    BOOL m_bDriverSignedValid;
    DWORD m_dwDDIVersion;
    TCHAR m_szDDIVersionEnglish[100];
    TCHAR m_szDDIVersionLocalized[100];

    DWORD m_iAdapter;
    TCHAR m_szVendorId[50];
    TCHAR m_szDeviceId[50];
    TCHAR m_szSubSysId[50];
    TCHAR m_szRevisionId[50];
    DWORD m_dwWHQLLevel;
    TCHAR m_szDeviceIdentifier[100];
    TCHAR m_szDriverSignDate[50]; 

    BOOL m_bNoHardware;
    BOOL m_bDDAccelerationEnabled;
    BOOL m_b3DAccelerationExists;
    BOOL m_b3DAccelerationEnabled;
    BOOL m_bAGPEnabled;
    BOOL m_bAGPExists;
    BOOL m_bAGPExistenceValid; 

    TCHAR m_szDXVAModes[100];

    vector<DxDiag_DXVA_DeinterlaceCaps*> m_vDXVACaps;
    
    TCHAR m_szDDStatusLocalized[100]; 
    TCHAR m_szDDStatusEnglish[100]; 
    TCHAR m_szD3DStatusLocalized[100]; 
    TCHAR m_szD3DStatusEnglish[100]; 
    TCHAR m_szAGPStatusLocalized[100]; 
    TCHAR m_szAGPStatusEnglish[100]; 

    TCHAR m_szNotesLocalized[3000]; 
    TCHAR m_szNotesEnglish[3000]; 
    TCHAR m_szRegHelpText[3000];

    TCHAR m_szTestResultDDLocalized[3000];
    TCHAR m_szTestResultDDEnglish[3000];
    TCHAR m_szTestResultD3D7Localized[3000];
    TCHAR m_szTestResultD3D7English[3000];
    TCHAR m_szTestResultD3D8Localized[3000];
    TCHAR m_szTestResultD3D8English[3000];
    TCHAR m_szTestResultD3D9Localized[3000];
    TCHAR m_szTestResultD3D9English[3000];

    DWORD m_nElementCount;
};

#endif // DISPINFO_H
