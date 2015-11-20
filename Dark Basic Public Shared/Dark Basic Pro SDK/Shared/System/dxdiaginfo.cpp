//----------------------------------------------------------------------------
// File: dxdiaginfo.cpp
//
// Desc: Sample app to read info from dxdiagn.dll
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define INITGUID
#define _CRT_SECURE_NO_DEPRECATE // Not recommended but need to support VS2003 and VS2005 for now
#include <windows.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 
#include <initguid.h>
#include <wchar.h>
#pragma warning( push, 3 )
#pragma warning(disable:4995 4786 4788)
#include <vector>
#pragma warning( pop )
using namespace std;
#include "dxdiaginfo.h"
//#include "resource.h"




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_BSTR_FREE(x)    if(x) { SysFreeString( x ); x = NULL; }
#define EXPAND(x)            x, sizeof(x)/sizeof(TCHAR)




//-----------------------------------------------------------------------------
// Name: CDxDiagInfo()
// Desc: Constuct class
//-----------------------------------------------------------------------------
CDxDiagInfo::CDxDiagInfo()
{
    m_pDxDiagProvider               = NULL;
    m_pDxDiagRoot                   = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CDxDiagInfo()
// Desc: Cleanup
//-----------------------------------------------------------------------------
CDxDiagInfo::~CDxDiagInfo()
{
    SAFE_RELEASE( m_pDxDiagRoot );
    SAFE_RELEASE( m_pDxDiagProvider );

    DestroyDisplayInfo( m_vDisplayInfo );

    if( m_bCleanupCOM )
        CoUninitialize();
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Connect to dxdiagn.dll and init it
//-----------------------------------------------------------------------------
HRESULT CDxDiagInfo::Init( BOOL bAllowWHQLChecks )
{
    HRESULT       hr;

    hr = CoInitialize( NULL );
    m_bCleanupCOM = SUCCEEDED(hr);

    hr = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           (LPVOID*) &m_pDxDiagProvider);
    if( FAILED(hr) )
        goto LCleanup;
    if( m_pDxDiagProvider == NULL )
    {
        hr = E_POINTER;
        goto LCleanup;
    }

    // Fill out a DXDIAG_INIT_PARAMS struct and pass it to IDxDiagContainer::Initialize
    // Passing in TRUE for bAllowWHQLChecks, allows dxdiag to check if drivers are 
    // digital signed as logo'd by WHQL which may connect via internet to update 
    // WHQL certificates.    
    DXDIAG_INIT_PARAMS dxDiagInitParam;
    ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );

    dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
    dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
    dxDiagInitParam.bAllowWHQLChecks        = bAllowWHQLChecks;
    dxDiagInitParam.pReserved               = NULL;

    hr = m_pDxDiagProvider->Initialize( &dxDiagInitParam );
    if( FAILED(hr) )
        goto LCleanup;

    hr = m_pDxDiagProvider->GetRootContainer( &m_pDxDiagRoot );
    if( FAILED(hr) )
        goto LCleanup;

LCleanup:
    return hr;
}




//-----------------------------------------------------------------------------
// Name: QueryDxDiagViaDll()
// Desc: Query dxdiagn.dll for all its information
//-----------------------------------------------------------------------------
HRESULT CDxDiagInfo::QueryDxDiagViaDll()
{
    if( NULL == m_pDxDiagProvider )
        return E_INVALIDARG;

	// IRM 20120301 - Only need this information once - it won't change second time through.
	if ( m_vDisplayInfo.empty() )
	    GetDisplayInfo( m_vDisplayInfo );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: GetDisplayInfo()
// Desc: Get the display info from the dll
//-----------------------------------------------------------------------------
#define Msg(x) MessageBox(0, x, __FUNCTION__, MB_OK)
HRESULT CDxDiagInfo::GetDisplayInfo( vector<DisplayInfo*>& vDisplayInfo )
{
    HRESULT           hr;
    WCHAR             wszContainer[256];
    IDxDiagContainer* pContainer      = NULL;
    IDxDiagContainer* pObject         = NULL;
    DWORD             nInstanceCount    = 0;
    DWORD             nItem             = 0;
    DWORD             nCurCount         = 0;

    // Get the IDxDiagContainer object called "DxDiag_DisplayDevices".
    // This call may take some time while dxdiag gathers the info.
    if( FAILED( hr = m_pDxDiagRoot->GetChildContainer( L"DxDiag_DisplayDevices", &pContainer ) ) )
        goto LCleanup;
    if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
        goto LCleanup;

	if (nInstanceCount > 0)
	{
		DWORD nItem = 0;

		DisplayInfo* pDisplayInfo = new DisplayInfo;
        if (pDisplayInfo == NULL)
            return E_OUTOFMEMORY;

        // Add pDisplayInfo to vDisplayInfo
        vDisplayInfo.push_back( pDisplayInfo );

        hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
        if( FAILED( hr ) )
            goto LCleanup;
        hr = pContainer->GetChildContainer( wszContainer, &pObject );
        if( FAILED( hr ) || pObject == NULL )
        {
            if( pObject == NULL )
                hr = E_FAIL;
            goto LCleanup;
        }

        if( FAILED( hr = GetStringValue( pObject, L"szDisplayMemoryLocalized", EXPAND(pDisplayInfo->m_szDisplayMemoryLocalized) ) ) )
            goto LCleanup;

#ifdef _DEBUG
        // debug check to make sure we got all the info from the object
        if( FAILED( hr = pObject->GetNumberOfProps( &pDisplayInfo->m_nElementCount ) ) )
            return hr;
        if( pDisplayInfo->m_nElementCount != nCurCount )
            OutputDebugString( TEXT("Not all elements in pDisplayInfo recorded") );
#endif

        SAFE_RELEASE( pObject );
    }

LCleanup:
    SAFE_RELEASE( pObject );
    SAFE_RELEASE( pContainer );
    return hr;
}




//-----------------------------------------------------------------------------
// Name: DestroyDisplayInfo()
// Desc: Cleanup the display info
//-----------------------------------------------------------------------------
VOID CDxDiagInfo::DestroyDisplayInfo( vector<DisplayInfo*>& vDisplayInfo )
{
    DisplayInfo* pDisplayInfo;
    vector<DisplayInfo*>::iterator iter; 
    for( iter = vDisplayInfo.begin(); iter != vDisplayInfo.end(); iter++ )
    {
        pDisplayInfo = *iter;

        DxDiag_DXVA_DeinterlaceCaps* pDXVANode;
        vector<DxDiag_DXVA_DeinterlaceCaps*>::iterator iterDXVA; 
        for( iterDXVA = pDisplayInfo->m_vDXVACaps.begin(); iterDXVA != pDisplayInfo->m_vDXVACaps.end(); iterDXVA++ )
        {
            pDXVANode = *iterDXVA;
            SAFE_DELETE( pDXVANode );
        }
        pDisplayInfo->m_vDXVACaps.clear();

        SAFE_DELETE( pDisplayInfo );
    }
    vDisplayInfo.clear();
}




//-----------------------------------------------------------------------------
// Name: GetStringValue()
// Desc: Get a string value from a IDxDiagContainer object
//-----------------------------------------------------------------------------
HRESULT CDxDiagInfo::GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, TCHAR* strValue, int nStrLen )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_BSTR )
        return E_INVALIDARG;
    
#ifdef _UNICODE
    wcsncpy( strValue, var.bstrVal, nStrLen-1 );
#else
    wcstombs( strValue, var.bstrVal, nStrLen );   
#endif
    strValue[nStrLen-1] = TEXT('\0');
    VariantClear( &var );

    return S_OK;
}
