#include "StdAfx.h"
#include "HtmlViewCustom.h"
#include "Help.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CHtmlViewCustom, CHtmlView)

BEGIN_MESSAGE_MAP(CHtmlViewCustom, CHtmlView)
	//{{AFX_MSG_MAP(CMfcieDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CHtmlViewCustom::CHtmlViewCustom(void)
{
}

CHtmlViewCustom::~CHtmlViewCustom(void)
{
}

void CHtmlViewCustom::OpenALink( CString sLink )
{
	char szBrowser [ MAX_PATH ];
	bool bResult = GetDefaultBrowserPath( szBrowser, MAX_PATH );
	if ( !bResult ) strcpy_s( szBrowser, MAX_PATH, "iexplore.exe" );
	char pANSI[512];
	BOOL b = FALSE;
	WideCharToMultiByte ( CP_ACP, WC_NO_BEST_FIT_CHARS, sLink, -1, pANSI, 512, "_", &b);
	if ( sLink.GetLength() > 0 ) ShellExecuteA( NULL, "open", szBrowser, pANSI, NULL, SW_SHOWNORMAL );
}

bool CHtmlViewCustom::GetDefaultBrowserPath( char *szValue, DWORD dwSize )
{
	// if no value, exit
	if ( !szValue ) return false;

	// get users browser from registry
	HKEY hKey = 0;
	DWORD dwDataSize = dwSize;
	DWORD dwType = REG_SZ;
	try
	{
		// read registry key
		LSTATUS ls = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "SOFTWARE\\Clients\\StartMenuInternet", NULL, KEY_QUERY_VALUE, &hKey );
		if ( ls != ERROR_SUCCESS ) throw 0;
		ls = RegQueryValueExA( hKey, NULL, NULL, &dwType, (BYTE*)szValue, &dwDataSize );
		if ( ls != ERROR_SUCCESS ) throw 1;
		RegCloseKey( hKey );
	}
	catch (int)
	{
		// if exception, close registry key
		RegCloseKey( hKey );
		return false;
	}

	// now find the browser command name
	hKey = 0;
	dwDataSize = dwSize;
	dwType = REG_SZ;
	CString sPath = _T("SOFTWARE\\Clients\\StartMenuInternet\\");
	sPath += szValue;
	sPath += "\\shell\\open\\command";
	try
	{
		// read browser name from registry
		LSTATUS ls = RegOpenKeyEx( HKEY_LOCAL_MACHINE, sPath, NULL, KEY_QUERY_VALUE, &hKey );
		if ( ls != ERROR_SUCCESS ) throw 0;
		ls = RegQueryValueExA( hKey, NULL, NULL, &dwType, (BYTE*)szValue, &dwDataSize );
		if ( ls != ERROR_SUCCESS ) throw 1;
		RegCloseKey( hKey );
	}
	catch (int)
	{
		// close registry key
		RegCloseKey( hKey );
		return false;
	}

	// success
	return true;
}

// CHtmlView message handlers AFX_IDC_BROWSER
BEGIN_EVENTSINK_MAP(CHtmlViewCustom, CHtmlView)
	ON_EVENT(CHtmlViewCustom, AFX_IDW_PANE_FIRST, 273, CHtmlViewCustom::NewWindow3Explorer1, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

void CHtmlViewCustom::NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{
	// U75 - 291009 - intercept regular browser launch, and replace with users current browser
	// cancel the old browser form launching
	*Cancel = TRUE;

	// obtain the URL that was intended and launch with new users browser
	CString sURL = bstrUrl;
	OpenALink ( sURL );
}
