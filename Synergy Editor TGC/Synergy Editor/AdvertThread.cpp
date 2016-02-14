// Includes
#include "stdafx.h"
#include "AdvertThread.h"
#include "Synergy Editor.h"
#include "Utilities.h"
#include "Settings.h"
#include "direct.h"
#include "io.h"
#include "httpcomm.h"

// Defines
#define _CRT_SECURE_NO_WARNINGS

// Globals
bool			g_bUsingAdvertising				= false;
bool			g_bUsingAdvertisingLoaded		= false;
LONGLONG		g_lUsingAdvertisingTimeStamp	= 0;

// this code is called by the thread (independent of the main code)
unsigned CAdvertThread::Run( )
{
	// are we free or full version user
	g_bUsingAdvertising = IsFreeVersion();

	// end thread
	return 0;
}
CAdvertThread::CAdvertThread()
{
}
CAdvertThread::~CAdvertThread()
{
}
bool CAdvertThread::IsCertificateFree ( LPSTR pCertificate )
{
	// connection details (default is free)
	LPSTR pServer = "www.darkbasic.com";
	LPSTR pPage = "tgco/isfree.php";

	// init connection
	HTTP_Connect ( pServer );

	// prepare data to post
	LPSTR pPostData = new char [ 256 ];
	strcpy ( pPostData, "c=12345678901234567890123456789012" );
	strcpy ( pPostData, "c=" );
	strcat ( pPostData, pCertificate );
	DWORD dwPostDataSize = strlen (pPostData);

	// prepare header
	LPSTR pHeader = new char[256];
	sprintf ( pHeader, "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n", dwPostDataSize );
	DWORD dwHeaderSize = -1L;

	// issue request and get data back
	LPSTR pData = HTTP_RequestData ( "POST", pPage, pHeader, dwHeaderSize, pPostData, dwPostDataSize );

	// free usages
	if ( pHeader )
	{
		delete pHeader;
		pHeader=NULL;
	}
	if ( pPostData )
	{
		delete pPostData;
		pPostData=NULL;
	}

	// free connection
	HTTP_Disconnect();

	// calculate result
	bool bIsFree = true;
	if ( pData )
	{
		if ( strcmp ( pData, "0" )==NULL ) bIsFree = false;
		delete pData;
	}

	// return if free version
	return bIsFree;
}
bool CAdvertThread::IsFreeVersion ( void )
{
	// scan certificates folder for DBP certificate, then send it to server
	// to see if we are using a free or paid-up version
	bool bFree = true;

	// find certificate folder and switch to it
	char pOldDir [ _MAX_PATH ];
	_getcwd ( pOldDir, _MAX_PATH );
	CString pCertFolder = Settings::DBPLocation + L"Compiler\\certificates\\";
	SetCurrentDirectory ( pCertFolder );

	// find DBP certificate
	_finddata_t filedata;
	long hInternalFile = _findfirst("*.*", &filedata);
	if(hInternalFile!=-1L)
	{
		int FileReturnValue = 0;
		bool bContainsDBPString = false;
		while ( FileReturnValue==0 && bFree==true )
		{
			if( !(filedata.attrib & _A_SUBDIR) )
			{
				char pFilename[256];
				strcpy ( pFilename, filedata.name );
				if ( pFilename )
				{
					if ( strlen(pFilename)==32 )
					{
						// found certificate file, read it to check if DBP certificate
						HANDLE hFile = NULL;
						DWORD dwBytesRead = 0;
						hFile = CreateFileA ( pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
						if ( hFile != INVALID_HANDLE_VALUE )
						{
							DWORD dwFileSize = GetFileSize ( hFile, NULL );
							LPSTR pFileData = new char[dwFileSize];
							if ( pFileData )
							{
								ReadFile ( hFile, pFileData, dwFileSize, &dwBytesRead, NULL );
								for ( DWORD n=0; n<dwFileSize; n++ )
								{
									if ( strnicmp ( pFileData + n, "Dark Basic Professional", 22 )==NULL )
									{
										// if it terminates
										LPSTR pEndChar = pFileData + n + 23;
										if ( *(pEndChar)==0 || *(pEndChar)==10 || *(pEndChar)==13 )
										{
											bContainsDBPString=true;
											break;
										}
									}
								}
								delete pFileData;
								if ( bContainsDBPString==true )
								{
									// found DBP certificate, is it a paid-up version?
									if ( IsCertificateFree ( pFilename )==false )
									{
										// not free, so remove advertising
										bFree = false;
									}
								}
							}
							CloseHandle ( hFile );
							hFile = NULL;
						}
					}
				}
			}
			FileReturnValue = _findnext(hInternalFile, &filedata);
		}
		_findclose(hInternalFile);
		hInternalFile=NULL;
	}

	// restore folder
	_chdir ( pOldDir );

	// return result
	return bFree;
}