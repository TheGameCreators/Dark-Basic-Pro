//
// HTTP Secure Server Communucation
//

//
// Includes
//

#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"
#include "wininet.h"

// Internal Flags
bool g_bExtraReporting = true;
DWORD g_dwSecureFlag = 0;

// Externals
extern char g_pFeedbackAreaString [ 1024 ];
extern HINTERNET hInet;
extern HINTERNET hInetConnect;

//
// Internal Functions
//

void ReportHTTPError( void )
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
			strcat ( g_pFeedbackAreaString, " " );
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
				strcat ( g_pFeedbackAreaString, " " );
				LocalFree( lpMsgBuf );
			}
			else
			{
				char num[256];
				wsprintf(num,"Error Code:%d", dwErr );
				strcat ( g_pFeedbackAreaString, num );
				strcat ( g_pFeedbackAreaString, " " );
			}
		}
	}
}

//
// Base Functions
//

// 20120418 IanM - Extended to include a 'secure' setting.
bool HTTP_Connect ( char* url, DWORD dwPort, int secure )
{
	if (!secure)
		g_dwSecureFlag = 0;
	else
		g_dwSecureFlag = INTERNET_FLAG_SECURE;

	// Result var
	bool bResult=false;
	strcpy ( g_pFeedbackAreaString, "" );

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
				strcat ( g_pFeedbackAreaString, "No connection" );
				strcat ( g_pFeedbackAreaString, " " );
			}
			ReportHTTPError();
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
						strcat ( g_pFeedbackAreaString, "Internet Open Function Failed" );
						strcat ( g_pFeedbackAreaString, " " );
					}
					ReportHTTPError();
				}
			}
			if(hInet)
			{
				// Internet Connect
				// 20120418 IanM - Force the port number into the valid range.
				hInetConnect = InternetConnect(	hInet,
												url,
												(WORD)(dwPort & 0xffff),
												"",
												NULL,
												INTERNET_SERVICE_HTTP,
												0, 0);
				if(hInetConnect==NULL)
				{
					if ( g_bExtraReporting )
					{
						strcat ( g_pFeedbackAreaString, "Internet Connection failed." );
						strcat ( g_pFeedbackAreaString, " " );
					}
					ReportHTTPError();
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

bool HTTP_Connect ( char* url, DWORD dwPort )
{
	// 20120418 IanM - If the port has bit 16 set (making it an illegal port number),
	//                 then use that to DISABLE the secure connection.
	int secure = 1;
	if (dwPort & 0x10000)
	{
		secure = 0;
		dwPort = dwPort & 0xffff;
	}

	return HTTP_Connect ( url, dwPort, secure );
}

bool HTTP_Connect ( char* url )
{
	// default access type is SECURE (port 443)
	// 20120418 IanM - Pass extra new 'secure' flag
	return HTTP_Connect ( url, INTERNET_DEFAULT_HTTPS_PORT, 1 );
}

LPSTR HTTP_RequestData ( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize, DWORD dwFlag )
{
	// return var
	DWORD dwDataReturned = 0;
	LPSTR pDataReturned = NULL;
	strcpy ( g_pFeedbackAreaString, "" );

	// prepare request
	// 20120418 IanM - Take into account the secure setting when submitting the new request
	HINTERNET hHttpRequest = HttpOpenRequest ( hInetConnect, pVerb, pObjectName, "HTTP/1.1", "The Agent", NULL, dwFlag | g_dwSecureFlag, 0 );

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
					strcat ( g_pFeedbackAreaString, "Internet Query Data failed." );
					strcat ( g_pFeedbackAreaString, " " );
				}
				ReportHTTPError ();
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
					ReportHTTPError ();
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
			strcat ( g_pFeedbackAreaString, "Send Request failed." );
			strcat ( g_pFeedbackAreaString, " " );
		}
		ReportHTTPError();
	}

	// return data in ptr
	return pDataReturned;
}

LPSTR HTTP_RequestData ( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize )
{
	// 20120418 IanM - Remove secure access flag - that will be set if necessary later.
	return HTTP_RequestData ( pVerb, pObjectName, pHeaderString, dwHeaderSize, pPostData, dwPostDataSize, 0 );
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
