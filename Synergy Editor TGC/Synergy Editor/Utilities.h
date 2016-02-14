#pragma once

class Utilities
{
public:
	static const int O_WRONLY = 1;
	static const int O_RDONLY = 0;
	static const int O_RDWR = 2;

	static CString ExtractFilename(CString item);
	static CString RemoveModifiedFlag(CString title);
	static CString ExtractPath(CString item);
	static CString GetRelativePath(CString item);
	static CString RemoveExtension(CString item);
	static CString MixedCase(CString item);
	static bool isNumber(CString &item);
	static CString GetNumber(int num);
	static bool CheckPathExists(CString szPath);
	static bool CheckFileExists(CString szFile);
	static bool EnsurePathExists(CString path);
	static CTime GetFileWriteTime(CString szFile);
	static void GoToLine(CString file);
	static CString GetDBPLocation();
	static int GetProcessorCount();
	static CString GetTemporaryPath();
	static int GetWindowsMajor();
	static CString GetApplicationPath();
	static CString FormatCode(CString file);

	static bool CheckAccess(CString path, int accessType);

	static bool ShowBrowseWindow(CString &folder, CString text, CString defaultPath);
	static int CALLBACK BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData );
	static CString GetSpecialFolderLocation(int nFolder);

	// Checksum stuff
	static DWORD getChecksum() { return sum; }
	static void InitChecksum();
	static void AddToChecksum(const CString & s);
    
private:
	static void add(BYTE b);
	static WORD r;
    static WORD c1;
    static WORD c2;
    static DWORD sum;
};
