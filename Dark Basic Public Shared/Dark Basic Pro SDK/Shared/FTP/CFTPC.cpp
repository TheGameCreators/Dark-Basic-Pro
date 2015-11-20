
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <windows.h> 
#include <windowsx.h>

#include "ftp.h"
#include "HTTPComm.h"
#include "cftpc.h"
#include "wininet.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#pragma comment ( lib, "wininet.lib" )

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DBPRO_GLOBAL GlobStruct*			g_pGlob							= NULL;
DBPRO_GLOBAL PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;
HWND								GlobalHwndCopy					= NULL;
DBPRO_GLOBAL char					m_pWorkString[_MAX_PATH];

// new HTTP data globals
char								g_pFeedbackAreaString [ 1024 ];

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( void )
{
}

DARKSDK void Destructor ( void )
{
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
	g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;
	GlobalHwndCopy = g_pGlob->hWnd;
}

DARKSDK LPSTR GetReturnStringFromWorkString(void)
{
	LPSTR pReturnString=NULL;
	if(m_pWorkString)
	{
		DWORD dwSize=strlen(m_pWorkString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, m_pWorkString);
	}
	return pReturnString;
}

DARKSDK void ConnectEx( DWORD dwString, DWORD dwString2, DWORD dwString3, int iUseWindow )
{
	if(!FTP_ConnectEx((char*)dwString, (char*)dwString2, (char*)dwString3, iUseWindow))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCONNECTIONFAILED);
}

DARKSDK void Connect( DWORD dwString, DWORD dwString2, DWORD dwString3 )
{
	if(!FTP_Connect((char*)dwString, (char*)dwString2, (char*)dwString3))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCONNECTIONFAILED);
}

DARKSDK void SetDir( DWORD dwString )
{
	if(!FTP_SetDir((char*)dwString))
		RunTimeSoftWarning(RUNTIMEERROR_FTPPATHCANNOTBEFOUND);
}

DARKSDK void FindFirst(void)
{
	FTP_FindFirst();
}

DARKSDK void FindNext(void)
{
	FTP_FindNext();
}

DARKSDK void PutFileCore( DWORD dwString )
{
	if(!FTP_PutFile((char*)dwString))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCANNOTPUTFILE);
}

DARKSDK void PutFile( DWORD dwString )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)dwString);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	PutFileCore( (DWORD)VirtualFilename );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void DeleteFile( DWORD dwString )
{
	if(!FTP_DeleteFile((char*)dwString))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCANNOTDELETEFILE);
}

DARKSDK void GetFile( DWORD dwString, DWORD dwString2 )
{
	if(!FTP_GetFile((char*)dwString, (char*)dwString2, 0, 0))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCANNOTGETFILE);
}

DARKSDK void GetFile( DWORD dwString, DWORD dwString2, int iFlag )
{
	if(!FTP_GetFile((char*)dwString, (char*)dwString2, iFlag, 1))
		RunTimeSoftWarning(RUNTIMEERROR_FTPCANNOTGETFILE);
}

DARKSDK void Disconnect(void)
{
	FTP_Disconnect(0);
}

DARKSDK void DisconnectEx( int iFlag )
{
	FTP_Disconnect(iFlag);
}

DARKSDK void Proceed(void)
{
	FTP_ControlDownload(0);
}

DARKSDK void Terminate(void)
{
	FTP_TerminateDownload();
}

DARKSDK int GetStatus(void)
{
	return FTP_GetStatus();
}

DARKSDK int GetFailure(void)
{
	return FTP_GetFailureState();
}

DARKSDK DWORD GetError( DWORD pDestStr )
{
	FTP_GetError(m_pWorkString);
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetDir( DWORD pDestStr )
{
	FTP_GetDir(m_pWorkString);
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetFileName( DWORD pDestStr )
{
	FTP_GetFileName(m_pWorkString);
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int GetFileType(void)
{
	return FTP_GetFileType();
}

DARKSDK int GetFileSize(void)
{
	return FTP_GetFileSize();
}

DARKSDK int GetProgress(void)
{
	return FTP_GetProgress();
}

// HTTP Implementations

DARKSDK void HTTPConnect ( DWORD dwUrl )
{
	char* lpUrl = (char*)dwUrl;
	if (lpUrl && *lpUrl)
		HTTP_Connect ( lpUrl );
}

DARKSDK void HTTPConnect ( DWORD dwUrl, DWORD port )
{
	char* lpUrl = (char*)dwUrl;
	if (lpUrl && *lpUrl)
		HTTP_Connect ( lpUrl, port );
}

DARKSDK void HTTPConnect ( DWORD dwUrl, DWORD port, int secure )
{
	char* lpUrl = (char*)dwUrl;
	if (lpUrl && *lpUrl)
		HTTP_Connect ( lpUrl, port, secure );
}

DARKSDK DWORD HTTPRequestData ( DWORD pDestStr, DWORD dwVerb, DWORD dwObjectName, DWORD dwPostData, DWORD dwAccessFlag )
{
	char* lpVerb = (char*)dwVerb;
	char* lpObjectName = (char*)dwObjectName;
	char* lpPostData = (char*)dwPostData;

	// 20120418 IanM - Ensure that the verb is set
	if (!lpVerb || !*lpVerb)
	{
		if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
		return NULL;
	}

	// 20120418 IanM - Don't care about these so much, just as long as they are non-NULL
	if (!lpObjectName)
		lpObjectName = "";
	if (!lpPostData)
		lpPostData = "";

	// default HTTP comm strings
	DWORD dwPostDataSize = strlen ( lpPostData );
	LPSTR pHeader = new char[256];
	wsprintf ( pHeader, "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n", dwPostDataSize );
	DWORD dwHeaderSize = -1L;

	// send data to get data
	LPSTR pReturnData = HTTP_RequestData ( lpVerb, lpObjectName, pHeader, dwHeaderSize, lpPostData, dwPostDataSize, dwAccessFlag );

	// delete old string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

	// make new string
	LPSTR pReturnString = NULL;
	DWORD dwSize = strlen(g_pFeedbackAreaString);
	if ( pReturnData ) dwSize = strlen(pReturnData);
	g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
	if ( pReturnData ) 
		strcpy ( pReturnString, pReturnData );
	else
		strcpy ( pReturnString, g_pFeedbackAreaString );

	// delete temp return data
	SAFE_DELETE(pReturnData);
	SAFE_DELETE(pHeader);

	// return new string
	return (DWORD)pReturnString;
}

DARKSDK DWORD HTTPRequestData ( DWORD pDestStr, DWORD dwVerb, DWORD dwObjectName, DWORD dwPostData )
{
	// 20120416 IanM - Cleared default security type
	return HTTPRequestData ( pDestStr, dwVerb, dwObjectName, dwPostData, 0 );
}

DARKSDK void HTTPDisconnect ( void )
{
	// disconnect when data exchange complete
	if ( HTTP_Disconnect ( ) )
		return;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorFTP ( void )
{
	Constructor ( );
}

void DestructorFTP ( void )
{
	Destructor ( );
}

void SetErrorHandlerFTP ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataFTP ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void dbFTPConnect ( char* szString, char* szString2, char* szString3, int iUseWindow )
{
	ConnectEx ( ( DWORD ) szString, ( DWORD ) szString2, ( DWORD ) szString3, iUseWindow );
}

void dbFTPConnect ( char* szString, char* szString2, char* szString3 )
{
	Connect ( ( DWORD ) szString, ( DWORD ) szString2, ( DWORD ) szString3 );
}

void dbFTPDisconnect ( void )
{
	Disconnect ( );
}

void dbFTPDisconnect ( int iFlag )
{
	DisconnectEx ( iFlag );
}

void dbFTPSetDir ( char* szString )
{
	SetDir ( ( DWORD ) szString );
}

void dbFTPPutFile ( char* szString )
{
	PutFile ( ( DWORD ) szString );
}

void dbFTPGetFile ( char* szString, char* szString2 )
{
	GetFile ( ( DWORD ) szString, ( DWORD ) szString2 );
}

void dbFTPGetFile ( char* szString, char* szString2, int iFlag )
{
	GetFile ( ( DWORD ) szString, ( DWORD ) szString2, iFlag );
}

void dbFTPDeleteFile ( char* szString )
{
	DeleteFile ( ( DWORD ) szString );
}

void dbFTPFindFirst ( void )
{
	FindFirst ( );
}

void dbFTPFindNext ( void )
{
	FindNext ( );
}

void dbFTPTerminate ( void )
{
	Terminate ( );
}

void dbFTPProceed ( void )
{
	Proceed ( );
}

char* dbGetFTPError ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetError ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbGetFTPDir ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetDir ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbGetFTPFileName ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetFileName ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int	dbGetFTPFileType ( void )
{
	return GetFileType ( );
}

int	dbGetFTPFileSize ( void )
{
	return GetFileSize ( );
}

int	dbGetFTPProgress ( void )
{
	return GetProgress ( );
}

int	dbGetFTPStatus ( void )
{
	return GetStatus ( );
}

int	dbGetFTPFailure ( void )
{
	return GetFailure ( );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////