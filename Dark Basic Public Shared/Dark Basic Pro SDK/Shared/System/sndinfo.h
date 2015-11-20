//----------------------------------------------------------------------------
// File: sndinfo.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef SNDINFO_H
#define SNDINFO_H

struct SoundInfo
{
    DWORD m_dwDevnode;
    TCHAR m_szGuidDeviceID[100];
    TCHAR m_szHardwareID[200];
    TCHAR m_szRegKey[200];
    TCHAR m_szManufacturerID[100];
    TCHAR m_szProductID[100];
    TCHAR m_szDescription[200];
    TCHAR m_szDriverName[200];
    TCHAR m_szDriverPath[MAX_PATH+1];
    TCHAR m_szDriverVersion[100];
    TCHAR m_szDriverLanguageEnglish[100];
    TCHAR m_szDriverLanguageLocalized[100];
    TCHAR m_szDriverAttributes[100];
    TCHAR m_szDriverDateEnglish[60];
    TCHAR m_szDriverDateLocalized[60];
    TCHAR m_szOtherDrivers[200];
    TCHAR m_szProvider[200];
    TCHAR m_szType[100]; // Emulated / vxd / wdm
    LONG m_lNumBytes;
    BOOL m_bDriverBeta;
    BOOL m_bDriverDebug;
    BOOL m_bDriverSigned;
    BOOL m_bDriverSignedValid;
    LONG m_lAccelerationLevel;
    DWORD m_dwSpeakerConfig;

    BOOL m_bDefaultSoundPlayback;
    BOOL m_bDefaultVoicePlayback;
    BOOL m_bVoiceManager;
    BOOL m_bEAX20Listener;
    BOOL m_bEAX20Source;
    BOOL m_bI3DL2Listener;
    BOOL m_bI3DL2Source;
    BOOL m_bZoomFX;

    DWORD m_dwFlags;  
    DWORD m_dwMinSecondarySampleRate; 
    DWORD m_dwMaxSecondarySampleRate; 
    DWORD m_dwPrimaryBuffers; 
    DWORD m_dwMaxHwMixingAllBuffers; 
    DWORD m_dwMaxHwMixingStaticBuffers; 
    DWORD m_dwMaxHwMixingStreamingBuffers; 
    DWORD m_dwFreeHwMixingAllBuffers; 
    DWORD m_dwFreeHwMixingStaticBuffers; 
    DWORD m_dwFreeHwMixingStreamingBuffers; 
    DWORD m_dwMaxHw3DAllBuffers; 
    DWORD m_dwMaxHw3DStaticBuffers; 
    DWORD m_dwMaxHw3DStreamingBuffers; 
    DWORD m_dwFreeHw3DAllBuffers; 
    DWORD m_dwFreeHw3DStaticBuffers; 
    DWORD m_dwFreeHw3DStreamingBuffers; 
    DWORD m_dwTotalHwMemBytes; 
    DWORD m_dwFreeHwMemBytes; 
    DWORD m_dwMaxContigFreeHwMemBytes; 
    DWORD m_dwUnlockTransferRateHwBuffers; 
    DWORD m_dwPlayCpuOverheadSwBuffers; 

    TCHAR m_szNotesLocalized[3000]; 
    TCHAR m_szNotesEnglish[3000]; 
    TCHAR m_szRegHelpText[3000];
    TCHAR m_szTestResultLocalized[3000];
    TCHAR m_szTestResultEnglish[3000];

    DWORD m_nElementCount;
};

struct SoundCaptureInfo
{
    TCHAR m_szDescription[200];
    TCHAR m_szGuidDeviceID[100];
    TCHAR m_szDriverName[200];
    TCHAR m_szDriverPath[MAX_PATH+1];
    TCHAR m_szDriverVersion[100];
    TCHAR m_szDriverLanguageEnglish[100];
    TCHAR m_szDriverLanguageLocalized[100];
    TCHAR m_szDriverAttributes[100];
    TCHAR m_szDriverDateEnglish[60];
    TCHAR m_szDriverDateLocalized[60];
    LONG m_lNumBytes;
    BOOL m_bDriverBeta;
    BOOL m_bDriverDebug;

    BOOL  m_bDefaultSoundRecording;
    BOOL  m_bDefaultVoiceRecording;

    DWORD m_dwFlags;  
    DWORD m_dwFormats;  

    DWORD m_nElementCount;
};

#endif // SNDINFO_H
