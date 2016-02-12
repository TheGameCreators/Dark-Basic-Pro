//
// HTTP Secure Server Communucation
//

//
// Includes
//

#include "stdafx.h"
#include "ExternalText.h"
#include "wininet.h"

// Externals
extern LPSTR g_pFeedbackAreaString;
extern bool g_bExtraReporting;

//
// Internal Global Data
//

HINTERNET hInet = NULL;
HINTERNET hInetConnect = NULL;

//
// Internal Functions
//

void ReportError( void )
{
	// Generic error
	DWORD dwErr = GetLastError();
	if ( g_bExtraReporting )
	{
		if ( dwErr==ERROR_INTERNET_EXTENDED_ERROR )
		{
			char err[10240];
			DWORD dwErrMsg = 0;
			DWORD dwSize = 10240;
			InternetGetLastResponseInfo ( &dwErrMsg, err, &dwSize );
			strcat ( g_pFeedbackAreaString, err );
			strcat ( g_pFeedbackAreaString, "\r\n" );
		}
		else
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			if ( lpMsgBuf )
			{
				strcat ( g_pFeedbackAreaString, (LPCTSTR)lpMsgBuf );
				strcat ( g_pFeedbackAreaString, "\r\n" );
				LocalFree( lpMsgBuf );
			}
			else
			{
				char num[256];
				wsprintf(num,"Error Code:%d", dwErr );
				strcat ( g_pFeedbackAreaString, num );
				strcat ( g_pFeedbackAreaString, "\r\n" );
			}
		}
	}
}

//
// Base Functions
//

bool HTTP_Connect ( char* url )
{
	// Result var
	bool bResult=false;

	// Access type SECURE
	WORD wHTTPType = INTERNET_DEFAULT_HTTPS_PORT;

	// Ensure we are disconnected first
	if(hInetConnect==NULL)
	{
		// Attempt to connect...
		DWORD dwResult=InternetAttemptConnect(0);
		if(dwResult!=ERROR_SUCCESS)
		{
			// No connection
			if ( g_bExtraReporting )
			{
				strcat ( g_pFeedbackAreaString, g_pTGCWord[51] );
				strcat ( g_pFeedbackAreaString, "\r\n" );
			}
			ReportError();
		}
		else
		{
			// Create Internet-Open
			if(hInet==NULL)
			{
				hInet = InternetOpen(	"InternetConnection",
										INTERNET_OPEN_TYPE_PRECONFIG,
										NULL, NULL, 0);
				if(hInet==NULL)
				{
					if ( g_bExtraReporting )
					{
						strcat ( g_pFeedbackAreaString, g_pTGCWord[52] );
						strcat ( g_pFeedbackAreaString, "\r\n" );
					}
					ReportError();
				}
			}
			if(hInet)
			{
				// Internet Connect
				hInetConnect = InternetConnect(	hInet,
												url,
												wHTTPType,
												"",
												NULL,
												INTERNET_SERVICE_HTTP,
												0, 0);
				if(hInetConnect==NULL)
				{
					if ( g_bExtraReporting )
					{
						strcat ( g_pFeedbackAreaString, g_pTGCWord[53] );
						strcat ( g_pFeedbackAreaString, "\r\n" );
					}
					ReportError();
				}

				if(hInetConnect)
				{
					// Successfully connected to site
					bResult=true;
				}
				else
				{
					// Release inet
					InternetCloseHandle(hInet);
					hInet=NULL;
				}
			}
		}
	}

	// complete
	return bResult;
}

LPSTR HTTP_RequestData ( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize )
{
	// return var
	DWORD dwDataReturned = 0;
	LPSTR pDataReturned = NULL;

	// access type SECURE
	DWORD dwFlag = INTERNET_FLAG_SECURE;

	// prepare request
	HINTERNET hHttpRequest = HttpOpenRequest ( hInetConnect, pVerb, pObjectName, "HTTP/1.1", "The Agent", NULL, dwFlag, 0 );

	// send request with header
	BOOL bSendResult = HttpSendRequest ( hHttpRequest, pHeaderString, dwHeaderSize, pPostData, dwPostDataSize );
	if ( bSendResult )
	{
		// reading all data
		do
		{
			// data comes in chunks
			DWORD dwSize = 0;
			if (!InternetQueryDataAvailable(hHttpRequest,&dwSize,0,0))
			{
				// error getting chunk
				if ( g_bExtraReporting )
				{
					strcat ( g_pFeedbackAreaString, g_pTGCWord[54] );
					strcat ( g_pFeedbackAreaString, "\r\n" );
				}
				ReportError ();
				break;
			}
			else
			{
				// chunk ready - allocate a buffer
				LPSTR lpszData = new TCHAR[dwSize+1];

				// read the data from the HINTERNET handle.
				DWORD dwDownloaded = 0;
				if(!InternetReadFile(hHttpRequest,(LPVOID)lpszData,dwSize,&dwDownloaded))
				{
					// failed to read the data
					if ( g_bExtraReporting ) strcat ( g_pFeedbackAreaString, "InternetReadFile failed.\r\n" );
					delete[] lpszData;
					ReportError ();
					break;
				}
				else
				{
					// add a null terminator to the end of the data buffer.
					lpszData[dwDownloaded]='\0';

					// Check the size of the remaining data.  If it is zero, break.
					if (dwDownloaded == 0)
						break;
				}

				// Add part-data to overall data returned
				if ( pDataReturned==NULL )
				{
					// create a new one
					dwDataReturned = dwDownloaded;
					pDataReturned = new char [ dwDataReturned+1 ];
					memcpy ( pDataReturned, lpszData, dwDataReturned );
				}
				else
				{
					// recreate a bigger array and add datas to it
					DWORD dwNewDataSize = dwDataReturned + dwDownloaded;
					LPSTR pNewData = new char [ dwNewDataSize+1 ];
					memcpy ( pNewData, lpszData, dwDataReturned );
					memcpy ( pNewData+dwDataReturned, lpszData+dwDataReturned, dwDownloaded );

					// delete old and replace varvalues
					delete pDataReturned;
					pDataReturned = pNewData;
					dwDataReturned = dwNewDataSize;
				}

				// Add a null terminator to the end of the data
				pDataReturned[dwDataReturned]='\0';

				// free usages
				if ( lpszData )
				{
					delete[] lpszData;
					lpszData = NULL;
				}
			}
		}
		while(TRUE);
	}
	else
	{
		// failed result
		if ( g_bExtraReporting )
		{
			strcat ( g_pFeedbackAreaString, g_pTGCWord[55] );
			strcat ( g_pFeedbackAreaString, "\r\n" );
		}
		ReportError();
	}

	// return data in ptr
	return pDataReturned;
}

bool HTTP_Disconnect ( void )
{
	// Close Internet-Connect
	if(hInetConnect)
	{
		InternetCloseHandle(hInetConnect);
		hInetConnect=NULL;
	}

	// Close Internet-Open
	if(hInet)
	{
		InternetCloseHandle(hInet);
		hInet=NULL;
	}

	// complete
	return true;
}
