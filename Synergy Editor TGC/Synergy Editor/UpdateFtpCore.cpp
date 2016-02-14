#include "StdAfx.h"
#include "UpdateFtpCore.h"
#include "Utilities.h"
#include "Settings.h"
#include "UpdateGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UpdateTool *CUpdateFtpCore::m_Tool;
bool CUpdateFtpCore::cancel;

void CUpdateFtpCore::InitSession(UpdateTool *tool)
{
	m_Tool = tool;
	cancel = false;
}

int CUpdateFtpCore::CheckForUpdate()
{		
	CString updateVersion = GetFileContents(_T("update.txt"));
	if(updateVersion.GetLength() == 0)
	{
		return 2;
	}

	CString lineToken;
	int curLine = 0;

	CString resToken;
	int curPos = 0;
	int mode = 1;

	CString Name, Type, Version, Size, Help, File, GUID;
	UINT i;
	bool canAdd;

	lineToken = updateVersion.Tokenize(_T("\n"),curLine);
	while (lineToken != "")
	{
		mode = 1;
		curPos = 0;

		if(cancel)
		{
			break;
		}

		resToken = lineToken.Tokenize(_T("|"),curPos);
		while (resToken != "")
		{
		   switch(mode)
		   {
		   case 1:
				Name = resToken;
			   break;
			   case 2:
				Type = resToken;
			   break;
			   case 3:
				Version = resToken;
			   break;
			   case 4:
				Size = resToken;
			   break;
			   case 5:
				   if(resToken.Right(3) == _T("txt"))
				   {
						Help = GetFileContents(resToken);
				   }
				   else
				   {
						Help = resToken;
				   }
			   break;
			   case 6:
				File = resToken;
			   break;
			   case 7:
				GUID = resToken;
			   break;
		   }

		   resToken= lineToken.Tokenize(_T("|"),curPos);
		   mode++;
		};

		canAdd = true;
		for(i=0; i < m_Tool->m_GUIDs.size(); i++)
		{
			if(m_Tool->m_GUIDs.at(i) == GUID)
			{
				canAdd = false;
				break;
			}
		}
		if(canAdd && !cancel)
		{
			m_Tool->AddItem(Name, Type, Version, Size, Help, File, GUID);
		}
		lineToken = updateVersion.Tokenize(_T("\n"),curLine);
	};
		
	return cancel;
}

void CUpdateFtpCore::DownloadSelected(CString file)
{
	int size = file.GetLength() + 1;
	TCHAR *newfile = new TCHAR[size];
	wcscpy_s(newfile, size, file);
	AfxBeginThread(DownloadSelectedThread, newfile);
}

UINT CUpdateFtpCore::DownloadSelectedThread(LPVOID pParam)
{
	bool success = true;
	CString file((TCHAR*)pParam);
	delete [] pParam;

	TCHAR path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	CString exeName;
	CString directory = path;
	CString newLocation;
	
	CString resToken;
	int curPos = 0;
	resToken = file.Tokenize(_T(","),curPos);
	while (resToken != "")
	{
		newLocation = Settings::DBPLocation + resToken;

		exeName = Utilities::ExtractFilename(resToken);

		// Download the updated file
		//CString URL = exeName;
		// leefix - 011008 - location of file can be in a subfolder on FTP (better organised)
		CString URL = file;

		CString updateFileLocation = directory + exeName;

		// Delete temp file if it already exists
		if(Utilities::CheckFileExists(updateFileLocation))
		{
			::DeleteFile(updateFileLocation);
		}

		// Proceed with the update	
		bool bTransferSuccess = DownloadFile(URL, updateFileLocation);
		if (!bTransferSuccess)
		{
			success = false;
			break;
		}	

		if(cancel) // If cancelled during download then we may have a corrupt download, return
		{
			break;
		}

		// If cancelled here then we continue doing what we are doing
		if (!UpdateGlobal::Switch(newLocation, updateFileLocation, false))
		{
			success = false;
			break;
		}

		// Check if it is a sfx
		if(newLocation.Right(11) == _T("_update.exe") && !cancel)
		{
			STARTUPINFO         siStartupInfo;
			PROCESS_INFORMATION piProcessInfo;
			memset(&siStartupInfo, 0, sizeof(siStartupInfo));
			memset(&piProcessInfo, 0, sizeof(piProcessInfo));
			siStartupInfo.cb = sizeof(siStartupInfo);
			CreateProcess(newLocation, _T(" x -y"), 0, 0, FALSE, CREATE_DEFAULT_ERROR_MODE, 0, NULL, &siStartupInfo, &piProcessInfo);
			WaitForSingleObject(piProcessInfo.hProcess, INFINITE);
			::CloseHandle(piProcessInfo.hThread);
			::CloseHandle(piProcessInfo.hProcess);
			::DeleteFile(newLocation);
		}

		resToken= file.Tokenize(_T(","),curPos);
	};

	if(cancel) // Might have hit cancel during file copy, so cancel next operation
	{
		::SendMessage(m_Tool->GetSafeHwnd(), WM_USER + 3, 0, 0); // Fail
	}
	else if(success)
	{
		::SendMessage(m_Tool->GetSafeHwnd(), WM_USER + 1, 0, 0); // Next
	}
	else
	{
		::SendMessage(m_Tool->GetSafeHwnd(), WM_USER + 3, 1, 0); // Fail
	}

	return 0;
}

CString CUpdateFtpCore::GetFileContents(CString filename)
{
	// vars
	HINTERNET hInet, hInetConnect, hFtpFile;
	CString ret = _T("");

	// access FTP 
	try
	{
		// internet handle
		hInet = InternetOpen( _T("InternetConnection"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( !hInet ) throw 1;

		// connection handle
		hInetConnect = InternetConnect( hInet, _T("ftp.thegamecreators.com"), INTERNET_DEFAULT_FTP_PORT, _T("anonymous"), NULL, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0 );
		if ( !hInetConnect ) throw 2;

		// go to folder in FTP
		if ( !FtpSetCurrentDirectory( hInetConnect, _T("dbpro_updates") ) ) throw 21;

		// get file handle
		hFtpFile = FtpOpenFile( hInetConnect, _T("update.txt"), GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, 0 );
		if ( !hFtpFile )
		{
			TCHAR error [ 256 ];
			DWORD dwError, dwLen;
			InternetGetLastResponseInfo( &dwError, error, &dwLen );
			throw 3;
		}

		// get the file itself
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = FtpGetFileSize( hFtpFile, &dwSizeHigh );
		float fSize = (float) dwSizeLow;

		// download file vars
		bool bFinished = false;
		char pBuffer [ 20481 ];
		memset ( pBuffer, 0, 20481 );
		DWORD dwReceived = 0;
		DWORD dwTotalReceived = 0;
		
		// while not finished and not cancelled, download
		while ( !bFinished )
		{
			BOOL bResult = InternetReadFile( hFtpFile, pBuffer, 20480, &dwReceived );
			if ( !bResult )
			{
				TCHAR error [ 256 ];
				DWORD dwError, dwLen;
				InternetGetLastResponseInfo( &dwError, error, &dwLen );
				throw 4;
			}

			// when stopped receiving, finish
			if ( dwReceived == 0 ) bFinished = true;
			dwTotalReceived += dwReceived;

			// while receiving, write to local file from FTP file handle
			if ( dwReceived > 0 )
			{
				//fwrite( pBuffer, 1, dwReceived, pLocalFile );
				pBuffer [ dwReceived ] = 0;
				TCHAR* pTString = new TCHAR[dwReceived+1];
				MultiByteToWideChar ( CP_ACP, 0, ( const char* )pBuffer, -1, pTString, strlen ( ( const char* ) pBuffer ) + 1 );
				ret.Append(pTString);
				if(pTString) delete pTString;
			}
		}

		// end connection
		InternetCloseHandle( hFtpFile );
		InternetCloseHandle( hInetConnect );
		InternetCloseHandle( hInet );
	}
	catch(...)
	{
		if ( hFtpFile ) InternetCloseHandle( hFtpFile );
		if ( hInetConnect ) InternetCloseHandle( hInetConnect );
		if ( hInet ) InternetCloseHandle( hInet );
	}

	/*
	CInternetSession sess(_T("SynergyEditor/1.0"));
    CFtpConnection* pConnect = NULL;
	CInternetFile* read = NULL;

	CString ret = _T("");

	BYTE pBufT[4096];
	BYTE *pBuf = NULL;

    try
    {
		pConnect = sess.GetFtpConnection(_T("ftp.thegamecreators.com"));
		pConnect->SetCurrentDirectory(_T("dbpro_updates"));

		read = pConnect->OpenFile(filename, GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, 1);
		ULONGLONG SIZE_READ = read->GetLength();
		pBuf = new BYTE[(size_t)SIZE_READ];
		memset(pBuf, NULL, (size_t)SIZE_READ);
		ULONGLONG total = 0;
		do
		{
			memset(pBufT, NULL, 4096);
			UINT amount = read->Read(pBufT, 4096);
			if(amount > (size_t)SIZE_READ)
			{
				amount = (UINT)SIZE_READ;
			}
			if(amount > 0)
			{
				for(UINT i=0; i < amount; i++)
				{
					pBuf[i+total] = pBufT[i];
				}
				total += amount;
			}
			else
			{
				read->Close();
				break;
			}
		}
		while (true);
		for(int j=0; j<total; j++)
		{
			ret.AppendChar((TCHAR)pBuf[j]);
		}		
    }
    catch (CInternetException* pEx)
    {
        TCHAR sz[1024];
        pEx->GetErrorMessage(sz, 1024);
        printf_s("ERROR!  %s\n", sz);
		// U70 - added to inform user what is really going on (firewall block says 'up to date')
		MessageBox ( NULL, sz, _T("Internet Connection Error"), MB_OK ); 
        pEx->Delete();
     }

	// If the buffer is used delete it's contents
	if(pBuf != NULL)
	{
		delete[] pBuf;
	}

	// If the file is open close it
	if(read != NULL)
	{
		read->Close();
        delete read;
	}

    // if the connection is open, close it
    if (pConnect != NULL) 
    {
        pConnect->Close();
        delete pConnect;
    }

	sess.Close();
	*/

	return ret;
}

// Download a file to a specified location
bool CUpdateFtpCore::DownloadFile(CString remoteFile, CString localFile)
{	
	// vars
	HINTERNET hInet, hInetConnect, hFtpFile;
	HANDLE	hOutFile;
	DWORD	dwNumWritten;
	bool	res = true;

	hOutFile = CreateFile(localFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE) return false;

	// access FTP 
	try
	{
		// internet handle
		hInet = InternetOpen( _T("InternetConnection"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( !hInet ) throw 1;

		// connection handle
		hInetConnect = InternetConnect( hInet, _T("ftp.thegamecreators.com"), INTERNET_DEFAULT_FTP_PORT, _T("anonymous"), NULL, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0 );
		if ( !hInetConnect ) throw 2;

		// go to folder in FTP
		if ( !FtpSetCurrentDirectory( hInetConnect, _T("dbpro_updates") ) ) throw 21;

		// leeadd - 011008 - navigate through folders of remoteFile on FTP
		CString remoteEXE = Utilities::ExtractFilename ( remoteFile );
		CString remotePath = Utilities::ExtractPath ( remoteFile );
		while ( remotePath.Find(_T("\\"))!=-1 )
		{
			int iFindFolderPos = remotePath.Find(_T("\\"));
			CString oneFOLDER = remotePath.Left(iFindFolderPos);
			FtpSetCurrentDirectory( hInetConnect, oneFOLDER );
			remotePath = remotePath.Right(remotePath.GetLength()-(iFindFolderPos+1));
		}

		// get file handle
		hFtpFile = FtpOpenFile( hInetConnect, remoteEXE, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0 );
		if ( !hFtpFile )
		{
			TCHAR error [ 256 ];
			DWORD dwError, dwLen;
			InternetGetLastResponseInfo( &dwError, error, &dwLen );
			throw 3;
		}

		// get the file itself
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = FtpGetFileSize( hFtpFile, &dwSizeHigh );
		float fSize = (float) dwSizeLow;

		// download file vars
		bool bFinished = false;
		char pBuffer [ 20481 ];
		memset ( pBuffer, 0, 20481 );
		DWORD dwReceived = 0;
		DWORD dwTotalReceived = 0;
		
		// while not finished and not cancelled, download
		while ( !bFinished && !cancel )
		{
			BOOL bResult = InternetReadFile( hFtpFile, pBuffer, 20480, &dwReceived );
			if ( !bResult )
			{
				TCHAR error [ 256 ];
				DWORD dwError, dwLen;
				InternetGetLastResponseInfo( &dwError, error, &dwLen );
				throw 4;
			}

			// when stopped receiving, finish
			if ( dwReceived == 0 ) bFinished = true;
			dwTotalReceived += dwReceived;

			// while receiving, write to local file from FTP file handle
			if ( dwReceived > 0 )
			{
				WriteFile(hOutFile, pBuffer, dwReceived, &dwNumWritten, NULL); 
				if (dwReceived != dwNumWritten)
				{
					res = false;
					break;
				}
				::SendMessage(m_Tool->GetSafeHwnd(), WM_USER + 2, dwReceived, 0); // Update GUI
			}
			else
			{
				break;
			}
		}

		// finish off local file
		if ( hOutFile ) { CloseHandle( hOutFile ); hOutFile=NULL; }

		// end connection
		InternetCloseHandle( hFtpFile );
		InternetCloseHandle( hInetConnect );
		InternetCloseHandle( hInet );
	}
	catch(...)
	{
		if ( hOutFile ) CloseHandle( hOutFile );
		if ( hFtpFile ) InternetCloseHandle( hFtpFile );
		if ( hInetConnect ) InternetCloseHandle( hInetConnect );
		if ( hInet ) InternetCloseHandle( hInet );
	}

	/*
	CInternetSession	sess(_T("SynergyEditor/1.0"));
    CFtpConnection*		pConnect = NULL;
	CInternetFile*		read = NULL;

	DWORD dwWriteSize, dwNumWritten;

    try
    {
        pConnect = sess.GetFtpConnection(_T("ftp.thegamecreators.com"));
		pConnect->SetCurrentDirectory(_T("dbpro_updates"));

		read = pConnect->OpenFile(remoteFile, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);

		do
		{			
			dwReadSizeOut = read->Read(pBuf, TRANSFER_SIZE);
			dwWriteSize = dwReadSizeOut;

			if (dwReadSizeOut > 0)
			{
				::SendMessage(m_Tool->GetSafeHwnd(), WM_USER + 2, dwReadSizeOut, 0); // Update GUI
				WriteFile(hOutFile, pBuf, dwWriteSize, &dwNumWritten, NULL); 
				// File write error
				if (dwWriteSize != dwNumWritten)
				{
					res = false;
					break;
				}
			}
			else
			{
				break;
			}
		}
		while(!cancel);
    }
    catch (CInternetException* pEx)
    {
        TCHAR sz[1024];
        pEx->GetErrorMessage(sz, 1024);
        printf_s("ERROR!  %s\n", sz);
        pEx->Delete();
     }

	// If the file is open close it
	if(read != NULL)
	{
		read->Close();
        delete read;
	}

	CloseHandle(hOutFile);	

    // if the connection is open, close it
    if (pConnect != NULL) 
    {
        pConnect->Close();
        delete pConnect;
    }

	sess.Close();
	*/

	return res;
}

void CUpdateFtpCore::Cancel()
{
	cancel = true;	
}
