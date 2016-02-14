#pragma once

#include "UpdateWindow.h"
#include <afxinet.h>

#define TRANSFER_SIZE 4096

class CUpdateFtpCore
{
public:
	static CString GetFileContents(CString filename);
	static int CheckForUpdate();
	static void InitSession(UpdateTool *tool);
	static void Cancel();
	static void DownloadSelected(CString file);
	static bool DownloadFile(CString remoteFile, CString localFile);
	static UINT DownloadSelectedThread(LPVOID pParam);	

private:
	static UpdateTool *m_Tool;
	static bool cancel;
};
