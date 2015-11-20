//
// DARKBASIC FTP IMPLEMENTATION
//

#include "windows.h"
#include "ftp.h"

// GlobStruct for window detection
#include ".\..\core\globstruct.h"
extern GlobStruct* g_pGlob;

// External Data
extern HWND GlobalHwndCopy;

// Internal Data
HINTERNET hInet = NULL;
HINTERNET hInetConnect = NULL;
WIN32_FIND_DATA FileDataObject;
HINTERNET hInetFileObject = NULL;
HINTERNET hInetOpenFile = NULL;
HANDLE hLocalFile = NULL;
DWORD FileByteCount=0;
DWORD LastFileByteCount=0;
char gStoredlocalfile[256];

int FtpStatus = 0;
char FtpFileName[256];
int FtpFileType = 0;
int FtpFileSize = 0;
bool FtpUseProgress=false;
int FtpProgress = 0;
int LastFtpProgress = 0;
DWORD LastFtpProgressTime = 0;
int FtpProgressBuffered = 0;
int FtpProgressFileSize = 0;
char FtpStoreUrl[256];
char FtpStoreUsername[256];
char FtpStorePassword[256];
char FtpLastError[256];
DWORD FtpLastErrorNum;
DWORD FtpLastErrorLen;
int FtpCommandFailed=0;
bool UsedModemDialup=false;
int FtpUseWindows=0;

// Initialisation

void ReportError(void)
{
	if(FtpCommandFailed==0)
	{
		FtpLastErrorNum=0;
		FtpLastErrorLen=256;
		InternetGetLastResponseInfo(&FtpLastErrorNum, FtpLastError, &FtpLastErrorLen);
		FtpCommandFailed=1;
		FtpFileType=-1;
	}
}

void FTP_Reset(void)
{
	hInetConnect=NULL;
	hInetFileObject=NULL;
	hInetOpenFile=NULL;
	hLocalFile=NULL;
	FileByteCount=0;
	strcpy(FtpFileName,"");
	strcpy(gStoredlocalfile,"");
	FtpFileType=-1;
	FtpFileSize=0;
	FtpProgress=-1;
	LastFtpProgress=FtpProgress;
	LastFtpProgressTime=timeGetTime();
	FtpStatus=0;
	FtpCommandFailed=0;
	strcpy(FtpLastError,"");
	FtpUseWindows=0;
}

bool FTP_ConnectEx(char* url, char* username, char* password, int iUseWindows)
{
	bool bHiddenWindow=false;
	bool bResult=false;

	// Remember connection data
	strcpy(FtpStoreUrl, url);
	strcpy(FtpStoreUsername, username);
	strcpy(FtpStorePassword, password);

	// Ensure we are disconnected first
	if(hInetConnect==NULL)
	{
		// Connection flag
		FtpUseWindows = iUseWindows;
		FtpStatus=0;

		// Minimise Window on fresh connect
		DWORD nettype = INTERNET_CONNECTION_LAN;
		UsedModemDialup=false;
		if(hInet==NULL && !InternetGetConnectedState(&nettype,0))
		{
			// Only if FULL SCREEN EXCLUSIVE
			if ( g_pGlob )
			{
				if ( g_pGlob->dwAppDisplayModeUsing==3 )
				{
					if ( FtpUseWindows==1 )
					{
						ShowWindow(GlobalHwndCopy, SW_MINIMIZE);
						ShowCursor(TRUE);
					}
				}
			}
			bHiddenWindow=true;

			// leefix - 260604 - u54 - tries an autodial modem connection
			InternetAutodial ( INTERNET_AUTODIAL_FORCE_UNATTENDED, 0 );
			UsedModemDialup=true;
		}

		// Attempt to connect...
		DWORD dwResult=InternetAttemptConnect(0);
		if(dwResult!=ERROR_SUCCESS)
		{
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
					ReportError();
				}
				else
				{
//					DWORD timeout = 10000;
//					InternetSetOption(hInet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInet, INTERNET_OPTION_DATA_SEND_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInet, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &timeout, sizeof(DWORD));
				}
			}
			if(hInet)
			{
				// Internet Connect
				hInetConnect = InternetConnect(	hInet,
												url,
												INTERNET_DEFAULT_FTP_PORT,
												username,
												password,
												INTERNET_SERVICE_FTP,
												INTERNET_FLAG_PASSIVE,
												0);
				if(hInetConnect==NULL)
				{
					ReportError();
				}
				else
				{
//					DWORD timeout = 10000;
//					InternetSetOption(hInetConnect, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInetConnect, INTERNET_OPTION_DATA_SEND_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInetConnect, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(DWORD));
//					InternetSetOption(hInetConnect, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &timeout, sizeof(DWORD));
				}

				if(hInetConnect)
				{
					// Successfully connected to site
					FtpStatus=1;
					bResult=true;
				}
				else
				{
					// Release inet
					InternetCloseHandle(hInet);
					bResult=false;
					FtpStatus=0;
					hInet=NULL;
				}
			}
		}

		// Maximise Window regardless
		if ( g_pGlob )
		{
			if ( g_pGlob->dwAppDisplayModeUsing==3 )
			{
				if ( FtpUseWindows==1 )
				{
					ShowWindow(GlobalHwndCopy, SW_MAXIMIZE);
					if(bHiddenWindow)
					{
						ShowCursor(FALSE);
					}
				}
			}
		}
	}
	return bResult;
}

bool FTP_Connect(char* url, char* username, char* password)
{
	return FTP_ConnectEx(url, username, password, 0);
}

bool ReattemptSameConnect(void)
{
	return FTP_Connect(FtpStoreUrl, FtpStoreUsername, FtpStorePassword);
}

bool FTP_Disconnect(int dialupdisconnect)
{
	// Suspend any download activity neatly
	FTP_TerminateDownload();

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

	// Hang-up any dialup connections
	if(dialupdisconnect==1 && UsedModemDialup==true)
	{
		if ( g_pGlob )
			if ( g_pGlob->dwAppDisplayModeUsing==3 )
				if ( FtpUseWindows==1 )
					ShowWindow(GlobalHwndCopy, SW_MINIMIZE);

		//leefix - 260604 - u54 - added support for FtpUseWindows

		InternetAutodialHangup(0);
		UsedModemDialup=false;
		Sleep(3000);

		if ( g_pGlob )
			if ( g_pGlob->dwAppDisplayModeUsing==3 )
				if ( FtpUseWindows==1 )
					ShowWindow(GlobalHwndCopy, SW_MAXIMIZE);
	}

	// Always clear pointers
	FTP_Reset();

	return true;
}

void FTP_CloseCompletely(void)
{
	// Close connection
	FTP_Disconnect(1);
}

void FtpIfNoConnectionTerminate(void)
{
	// Close connection if invalid (must be able to go to where I already am)
	if(!FtpSetCurrentDirectory(hInetConnect, "."))
		FTP_Disconnect(0);
}

// Handling

bool FTP_SetDir(char* ftpdir)
{
	// Switch to OBJECTS directory
	BOOL bResult = FtpSetCurrentDirectory(hInetConnect, ftpdir);
	if(bResult==TRUE)
		return true;
	else
	{
		ReportError();
		FtpIfNoConnectionTerminate();
		return false;
	}
}

void CopyFileDataOver(void)
{
	// File Type
	FtpFileType=0;
	DWORD Attribs = FileDataObject.dwFileAttributes;
	if(Attribs==0xFFFFFFFF)
		FtpFileType=-1;
	if(Attribs & FILE_ATTRIBUTE_DIRECTORY)
		FtpFileType=1;

	// File Name
	strcpy(FtpFileName, FileDataObject.cFileName);

	// File Size
	if(FileDataObject.nFileSizeHigh>0)
		FtpFileSize = 0; //very very big file!
	else
		FtpFileSize = FileDataObject.nFileSizeLow;
}

bool FTP_FindFirst(void)
{
	if(hInetFileObject)
	{
		InternetCloseHandle(hInetFileObject);
		hInetFileObject=NULL;
	}

	hInetFileObject = FtpFindFirstFile(	hInetConnect, NULL, &FileDataObject, INTERNET_FLAG_RESYNCHRONIZE, 0);
	if(hInetFileObject)
	{
		CopyFileDataOver();
		return true;
	}
	else
	{
		ReportError();
		FtpIfNoConnectionTerminate();
		return false;
	}
}

bool FTP_FindNext(void)
{
	if(hInetFileObject)
	{
		if(InternetFindNextFile(hInetFileObject, &FileDataObject)==TRUE)
		{
			CopyFileDataOver();
			return true;
		}
		else
		{
			InternetCloseHandle(hInetFileObject);
			hInetFileObject=NULL;
			FtpFileType=-1;
			FtpIfNoConnectionTerminate();
			return false;
		}
	}
	else
		return false;
}

bool FTP_GetFile(char* ftpfile, char* localfile, DWORD optionalfilesize, DWORD useprogress)
{
	if(optionalfilesize==0)
	{
		if(FtpGetFile(hInetConnect, ftpfile, localfile, FALSE, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0))// | INTERNET_FLAG_RESYNCHRONIZE, 0))
			return true;
		else
		{
			ReportError();
			return false;
		}
	}
	else
	{
		// leefix - 260604 - u54 - obtain file size, then close again
		FTP_FindFirst();
		while ( 1 )
		{
			if ( stricmp ( FtpFileName, ftpfile )==NULL )
			{
				optionalfilesize = FtpFileSize;
				break;
			}
			if ( FTP_FindNext()==false )
				break;
		}
		if ( hInetFileObject )
		{
			InternetCloseHandle(hInetFileObject);
			hInetFileObject=NULL;
			FtpFileType=-1;
			FtpIfNoConnectionTerminate();
		}

		// begin by opening file
		hInetOpenFile = FtpOpenFile(	hInetConnect,
										ftpfile,
										GENERIC_READ,
										FTP_TRANSFER_TYPE_BINARY |
										INTERNET_FLAG_RESYNCHRONIZE, 0);

		if(hInetOpenFile)
		{
			// Create File for local writing
			hLocalFile = CreateFile(	localfile,
										GENERIC_WRITE,
										FILE_SHARE_WRITE,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

			strcpy(gStoredlocalfile,localfile);
			FileByteCount=0;

			if(hLocalFile!=INVALID_HANDLE_VALUE)
			{
				// Progress Resets
				if(useprogress==1)
				{
					FtpUseProgress=true;
					FtpProgress=0;
					LastFtpProgress=FtpProgress;
					LastFtpProgressTime=timeGetTime();
					FtpProgressBuffered=0;
					FtpProgressFileSize=optionalfilesize;
				}
				else
				{
					FtpUseProgress=false;
					FtpProgress=1;
					LastFtpProgress=FtpProgress;
					LastFtpProgressTime=timeGetTime();
					FTP_ControlDownload(optionalfilesize);
					FTP_ControlDownload(0);
				}
				return true;
			}
			else
				return false;
		}
		else
		{
			ReportError();
			FtpIfNoConnectionTerminate();
			return false;
		}
	}
}

void FTP_DownloadFailure(void)
{
	if(hLocalFile)
	{
		CloseHandle(hLocalFile);
		hLocalFile=NULL;
		FileByteCount=0;
		DeleteFile(gStoredlocalfile);
		strcpy(gStoredlocalfile,"");
	}
	if(hInetOpenFile)
	{
		InternetCloseHandle(hInetOpenFile);
		hInetOpenFile=NULL;
		FtpProgress=-1;
	}
}

void FTP_ControlDownload(int grabsize)
{
	// Default grab size
	if(grabsize==0) grabsize=3*1024;
	
	DWORD bytesread = 0;
	char* buffer = new char[grabsize];
	int grab=grabsize/1024;
	DWORD buff=0;
	bool ok=true;
	DWORD totalbytes=0;
	while(ok==true && grab>0)
	{
		if(!InternetReadFile(	hInetOpenFile,
								buffer+buff,
								1024,
								&bytesread))
		{
			ok=false;
		}
		grab--;
		buff+=bytesread;
		totalbytes+=bytesread;
		if(bytesread<1024) break;
	}
	if(ok==true)
	{
		// Copy data from buffer to file
		if(totalbytes>0)
		{
			if(hLocalFile)
			{
				DWORD byteswritten;
				if(!WriteFile(hLocalFile, buffer, totalbytes, &byteswritten, NULL))
				{
					// Failure to download
					FTP_DownloadFailure();
					goto donethiscycle;
				}
				FileByteCount+=byteswritten;
			}
			FtpProgressBuffered+=totalbytes;
		}
		if(totalbytes==0)
		{
			// Download Complete
			if(hInetOpenFile)
			{
				InternetCloseHandle(hInetOpenFile);
				hInetOpenFile=NULL;
				FtpProgress=-1;
			}
			if(hLocalFile)
			{
				CloseHandle(hLocalFile);
				hLocalFile=NULL;
				FileByteCount=0;
			}
		}
		else
		{
			// Download in Progress
			if(FtpProgressFileSize>0)
				FtpProgress = (int)((100.0/FtpProgressFileSize)*FtpProgressBuffered);
			else
				FtpProgress = 1;
		}
	}
	else
	{
		FtpIfNoConnectionTerminate();
		goto donethiscycle;
	}

	// Monitor and ensure don't stay on same progress..
	if(FtpProgress>=0)
	{
		if(FileByteCount==LastFileByteCount)
		{
			if((timeGetTime()-LastFtpProgressTime)>10000)
			{
				// No change in over 10 seconds
				// Failure to download
				FTP_DownloadFailure();
				FtpIfNoConnectionTerminate();
				goto donethiscycle;
			}
		}
		else
		{
			// Mark change in progress time
			LastFileByteCount=FileByteCount;
			LastFtpProgress=FtpProgress;
			LastFtpProgressTime=timeGetTime();
		}
	}

	donethiscycle:
	delete buffer;
	buffer=0;
}

void FTP_TerminateDownload(void)
{
	// Failure to download
	FTP_DownloadFailure();
}

bool FTP_PutFile(char* localfile)
{
	if(FtpPutFile(hInetConnect, localfile, localfile, 0, 0))
		return true;
	else
	{
		ReportError();
		return false;
	}
}

bool FTP_DeleteFile(char* ftpfile)
{
	if(FtpDeleteFile(hInetConnect, ftpfile))
		return true;
	else
		return false;
}

// Expressions

int FTP_GetStatus(void)
{
	return FtpStatus;
}

int FTP_GetFailureState(void)
{
	int State=FtpCommandFailed;
	FtpCommandFailed=0;
	return State;
}

void FTP_GetError(char* errorstring)
{
	strcpy(errorstring, FtpLastError);
}

bool FTP_GetDir(char* ftpdir)
{
	return false;
}

int FTP_GetFileType(void)
{
	return FtpFileType;
}

bool FTP_GetFileName(char* ftpfile)
{
	strcpy(ftpfile, FtpFileName);
	return false;
}

int FTP_GetFileSize(void)
{
	return FtpFileSize;
}

int FTP_GetProgress(void)
{
	return FtpProgress;
}

