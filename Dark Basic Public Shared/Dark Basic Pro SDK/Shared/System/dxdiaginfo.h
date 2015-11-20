//----------------------------------------------------------------------------
// File: dxdiag.h
//
// Desc: 
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DXDIAG_H
#define DXDIAG_H

// Headers for dxdiagn.dll
#include <dxdiag.h>

// Headers for structs to hold info
#include "fileinfo.h"
#include "sysinfo.h"
#include "dispinfo.h"
#include "sndinfo.h"
#include "musinfo.h"
#include "inptinfo.h"
#include "netinfo.h"
#include "showinfo.h"

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------

// IRM 20120301 - Problems throughout the class - Removed everything that was not used
//                to ease debug.

class CDxDiagInfo
{
public:
    CDxDiagInfo();
    ~CDxDiagInfo();

    HRESULT Init( BOOL bAllowWHQLChecks );
    HRESULT QueryDxDiagViaDll();

public:
    HRESULT GetDisplayInfo( vector<DisplayInfo*>& vDisplayInfo );
    HRESULT GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, TCHAR* strValue, int nStrLen );
    VOID DestroyDisplayInfo( vector<DisplayInfo*>& vDisplayInfo );

    IDxDiagProvider*  m_pDxDiagProvider;
    IDxDiagContainer* m_pDxDiagRoot;
    BOOL              m_bCleanupCOM;

public:
    vector<DisplayInfo*>        m_vDisplayInfo;
};


#endif // DXDIAG_H
