
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <windows.h> 
#include <windowsx.h>
#include "mmsystem.h"
#include <stdio.h>
#include <stdlib.h>
#include "cfilec.h"
#include "direct.h"
#include "time.h"
#include "io.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include "winioctl.h"
#include "shlobj.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

// 20091129 v75 - IRM - Provide automatically expanding buffer when reading strings
#include <vector>

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKMemblocks.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef LPSTR			( *RetVoidMakeMemblock )		( int, DWORD );
typedef void			( *RetVoidFreeMemblock )		( int );
typedef DWORD			( *RetVoidGetMemblockSize )		( int );
typedef void			( *RetVoidSetMemblockSize )		( int, DWORD);
typedef LPSTR			( *RetVoidGetMemblockPtr )		( int );

struct sHardDrive
{
	LARGE_INTEGER	liCylinderCount;
	DWORD			dwTracksPerCylinder; 
	DWORD			dwSectorsPerTrack; 
	DWORD			dwBytesPerSector; 

	ULONGLONG		ulTotalBytes;
	ULONGLONG		ulTotalMB;
	ULONGLONG		ulTotalGB;
};

#define MAX_HARD_DRIVE 24

DBPRO_GLOBAL sHardDrive					g_HardDrives      [ MAX_HARD_DRIVE ];
DBPRO_GLOBAL char						g_HardDiskLetters [ MAX_HARD_DRIVE ] [ 4 ] = 
																					{
																						"c:\\",	"d:\\",	"e:\\",	"f:\\",	"g:\\",	"h:\\",
																						"i:\\",	"j:\\",	"k:\\",	"l:\\",	"m:\\",	"n:\\",
																						"o:\\",	"p:\\",	"q:\\",	"r:\\",	"s:\\",	"t:\\",
																						"u:\\",	"v:\\",	"w:\\",	"x:\\",	"y:\\",	"z:\\"
																					};
DBPRO_GLOBAL int						g_iHardDriveCount = 0;
DBPRO_GLOBAL HANDLE						ghExecuteFileProcess			= NULL;
#define					MAX_FILES	64
DBPRO_GLOBAL HANDLE						File[MAX_FILES];
DBPRO_GLOBAL BOOL						FileEOF[MAX_FILES];
DBPRO_GLOBAL LPSTR						pVirtFileEncrypted[MAX_FILES];
DBPRO_GLOBAL char						filetext[_MAX_PATH];
DBPRO_GLOBAL struct _finddata_t			filedata;
DBPRO_GLOBAL long						hInternalFile					= NULL;
DBPRO_GLOBAL int						FileReturnValue					= -1;
DBPRO_GLOBAL bool						g_bCreateChecklistNow;
DBPRO_GLOBAL DWORD						g_dwMaxStringSizeInEnum;
DBPRO_GLOBAL char						m_pWorkString[1024];
DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;
DBPRO_GLOBAL PTR_FuncCreateStr			g_pCreateDeleteStringFunction	= NULL;

DBPRO_GLOBAL RetVoidMakeMemblock		g_MEM_Make						= NULL;
DBPRO_GLOBAL RetVoidFreeMemblock		g_MEM_Free						= NULL;
DBPRO_GLOBAL RetVoidGetMemblockSize		g_MEM_GetSize					= NULL;
DBPRO_GLOBAL RetVoidSetMemblockSize		g_MEM_SetSize					= NULL;
DBPRO_GLOBAL RetVoidGetMemblockPtr		g_MEM_GetPtr					= NULL;

// Memblock Array Strcutre
//#define					MEMBLOCKSIZE 258
//DWORD					gpMemblockSize[MEMBLOCKSIZE];
//char*					gpMemblock[MEMBLOCKSIZE];

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( void )
{
	ZeroMemory(File, sizeof(File));
	ZeroMemory(FileEOF, sizeof(FileEOF));
	ZeroMemory(filetext, sizeof(filetext));
	ZeroMemory(&filedata, sizeof(filedata));
	ZeroMemory(&pVirtFileEncrypted, sizeof(pVirtFileEncrypted));

	{
		// local variables
		int				iIndex          = 0;		// drive index
		HANDLE			hDevice         = NULL;		// handle to device
		DWORD			dwBytesReturned = 0;		// bytes returned from io control

		DISK_GEOMETRY	drive;						// drive structure
		char			szDrive [ 256 ];			// to store drive name

		// get a handle to the device
		hDevice = CreateFile ( "\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );

		// check handle is valid
		if ( hDevice == INVALID_HANDLE_VALUE )
			return;

		// loop round for all drives
		while ( hDevice != INVALID_HANDLE_VALUE )
		{
			// get information
			if ( !DeviceIoControl (
										hDevice,						// handle to device
										IOCTL_DISK_GET_DRIVE_GEOMETRY,	// control type
										NULL,							// no input data
										0,								// use 0 because we have no input data
										&drive,							// pointer to drive structure
										sizeof ( DISK_GEOMETRY ),		// size of data
										&dwBytesReturned,				// number of bytes returned
										( LPOVERLAPPED ) NULL			// ignored
								  ) )
										return;

			// calculate the disk size
			ULONGLONG TotalBytes = drive.Cylinders.QuadPart * ( ULONG ) drive.TracksPerCylinder * ( ULONG ) drive.SectorsPerTrack * ( ULONG ) drive.BytesPerSector;
			
			g_HardDrives [ g_iHardDriveCount ].liCylinderCount     = drive.Cylinders;
			g_HardDrives [ g_iHardDriveCount ].dwTracksPerCylinder = drive.TracksPerCylinder;
			g_HardDrives [ g_iHardDriveCount ].dwSectorsPerTrack   = drive.SectorsPerTrack;
			g_HardDrives [ g_iHardDriveCount ].dwBytesPerSector    = drive.BytesPerSector;
			g_HardDrives [ g_iHardDriveCount ].ulTotalBytes        = TotalBytes;
			g_HardDrives [ g_iHardDriveCount ].ulTotalMB           = TotalBytes / 1024 / 1024;
			g_HardDrives [ g_iHardDriveCount ].ulTotalGB           = TotalBytes / 1024 / 1024 / 1024;

			g_iHardDriveCount++;

			// close this handle
			CloseHandle ( hDevice );

			// set up string for next drive
			sprintf ( szDrive, "\\\\.\\PhysicalDrive%d", ++iIndex );

			// access the next drive
			hDevice = CreateFile ( szDrive, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
		}
	}

}

DARKSDK void Destructor ( void )
{
	if(hInternalFile)
	{
		_findclose(hInternalFile);
		hInternalFile=NULL;
	}
	for(DWORD f=0; f<MAX_FILES; f++)
	{
		if(File[f])
		{
			CloseHandle(File[f]);
			File[f]=NULL;
		}
		if(pVirtFileEncrypted[f])
		{
			delete pVirtFileEncrypted[f];
			pVirtFileEncrypted[f]=NULL;
		}
	}
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

	// Construct links to memblock access functions
	#ifndef DARKSDK_COMPILE
	if(g_pGlob->g_Memblocks)
	{
		g_MEM_Make = ( RetVoidMakeMemblock )		GetProcAddress ( g_pGlob->g_Memblocks, "?ExtMakeMemblock@@YAPADHK@Z" );
		g_MEM_Free = ( RetVoidFreeMemblock )		GetProcAddress ( g_pGlob->g_Memblocks, "?ExtFreeMemblock@@YAXH@Z" );
		g_MEM_GetSize = ( RetVoidGetMemblockSize )	GetProcAddress ( g_pGlob->g_Memblocks, "?ExtGetMemblockSize@@YAKH@Z" );
		g_MEM_SetSize = ( RetVoidSetMemblockSize )	GetProcAddress ( g_pGlob->g_Memblocks, "?ExtSetMemblockSize@@YAXHK@Z" );
		g_MEM_GetPtr = ( RetVoidGetMemblockPtr )	GetProcAddress ( g_pGlob->g_Memblocks, "?ExtGetMemblockPtr@@YAPADH@Z" );
	}
	#else
		g_MEM_Make		= dbExtMakeMemblock;
		g_MEM_Free		= dbExtFreeMemblock;
		g_MEM_GetSize	= dbExtGetMemblockSize;
		g_MEM_SetSize	= dbExtSetMemblockSize;
		g_MEM_GetPtr	= dbExtGetMemblockPtr;
	#endif
}

DARKSDK void FFindCloseFile(void)
{
	_findclose(hInternalFile);
	hInternalFile=NULL;
}

DARKSDK void FFindFirstFile(void)
{
	if(hInternalFile) FFindCloseFile();
	hInternalFile = _findfirst("*.*", &filedata);
	if(hInternalFile!=-1L)
	{
		// Success!
		FileReturnValue=0;
	}
	else
		RunTimeWarning(RUNTIMEERROR_CANNOTSCANCURRENTDIR);
}

DARKSDK int FGetFileReturnValue(void)
{
	return FileReturnValue;
}

DARKSDK void FFindNextFile(void)
{
	FileReturnValue = _findnext(hInternalFile, &filedata);
}

DARKSDK int FGetActualTypeValue(int flagvalue)
{
	if(flagvalue & _A_SUBDIR)
		return 1;
	else
		return 0;
}

DARKSDK BOOL DB_FileExist(char* Filename)
{
	// If no string, no file
	if(Filename==NULL)
		return FALSE;

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)Filename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Open BASIC Script
	HANDLE hfile = CreateFile(VirtualFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hfile);
	return TRUE;
}

DARKSDK DWORD DB_FileSize(char* Filename)
{
	DWORD size;

	// If no string, no file
	if(Filename==NULL)
		return 0;

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)Filename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Open BASIC Script
	HANDLE hfile = CreateFile(VirtualFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError();
		return 0;
	}

	// Obtain filesize
	size = GetFileSize(hfile, NULL);
	CloseHandle(hfile);

	return size;
}

DARKSDK BOOL DB_FileWriteProtected(char* Filename)
{

	// If no string, no file
	if(Filename==NULL)
		return 0;

	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)Filename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	DWORD flags = GetFileAttributes(VirtualFilename);
	if(flags & FILE_ATTRIBUTE_READONLY)
		return TRUE;
	else
		return FALSE;
}

DARKSDK BOOL DB_DeleteFile(char* Filename)
{
	if(DB_FileExist(Filename))
	{
		DeleteFile(Filename);
		return TRUE;
	}
	else
		return FALSE;
}

DARKSDK bool rFindFileInSub(char* currentpath, char* searchfile, char* returnpath)
{
	struct _finddata_t filedata;

	// Set directory
	_chdir(currentpath);

	// Find first file in this directory
	long hFile = _findfirst("*.*", &filedata);
	if(hFile!=-1L)
	{
		// Skip '..'
		_findnext(hFile, &filedata);

		// Start directory scan
		while(_findnext(hFile, &filedata)==0)
		{
			// Is File or Directory
			if(filedata.attrib & _A_SUBDIR)
			{
				// Sub-Directory
				char gointodir[512];
				strcpy(gointodir, currentpath);
				strcat(gointodir, "\\");
				strcat(gointodir, filedata.name);
				if(strlen(gointodir)<=255)
				{
					if(rFindFileInSub(gointodir, searchfile, returnpath))
					{
						_findclose(hFile);
						return TRUE;
					}
				}
			}
			else
			{
				// File
				char checkfilenameA[256];
				strcpy(checkfilenameA, _strlwr(filedata.name));
				if(strcmp(checkfilenameA, searchfile)==0)
				{
					// Found File & Path!
					strcpy(returnpath, currentpath);
					strcat(returnpath, "\\");
					strcat(returnpath, searchfile);
					_findclose(hFile);
					return TRUE;
				}
			}
		}

		// Close Scan File 
		_findclose(hFile);
	}

	return FALSE;
}

DARKSDK BOOL DB_FindFileInSubPath(char* filename, char* returnpath)
{
	BOOL bResult=FALSE;

	// Get current directory
	char path[256];
	_getcwd(path, 256);

	// Make search filename lowercase
	char searchfile[256];
	strcpy(searchfile, filename);
	_strlwr(searchfile);

	// Search for filen
	if(rFindFileInSub(path, searchfile, returnpath))
		bResult=TRUE;

	// Restore current working directory
	_chdir(path);

	return bResult;
}

DARKSDK BOOL DB_CanMakeFile(char* Filename)
{
	// Create Empty File
	HANDLE hfile = CreateFile(Filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfile);
		DeleteFile(Filename);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DARKSDK BOOL DB_MakeFile(char* Filename)
{
	// Create Empty File
	HANDLE hfile = CreateFile(Filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hfile);
	return TRUE;
}

DARKSDK BOOL DB_CopyFile(char* From, char* To)
{
	if(DB_FileExist(From))
	{
		if(!CopyFile(From, To, TRUE))
			return FALSE;
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENOTEXIST);

	return TRUE;
}

DARKSDK BOOL DB_MoveFile(char* From, char* To)
{
	if(DB_FileExist(From))
	{
		if(!MoveFile(From, To))
			return FALSE;
	}
	else
		RunTimeError(RUNTIMEERROR_FILEEXISTS);

	return TRUE;
}

DARKSDK BOOL DB_RenameFile(char* From, char* To)
{
	if(!MoveFile(From, To))
		return FALSE;

	return TRUE;
}

DARKSDK BOOL DB_PathExist(char* OriginalPathname)
{
	// Get File Data
	DWORD Attribs = GetFileAttributes(OriginalPathname);
	if(Attribs==0xFFFFFFFF)
		return FALSE;

	// If file a directory
	if(Attribs | FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;

	// Not a directory
	return FALSE;
}

DARKSDK BOOL DB_MakeDir(char* Dirname)
{
	// Create Empty Directory
	BOOL bResult = CreateDirectory(Dirname, NULL);
	return bResult;
}

DARKSDK BOOL DB_DeleteDir(char* Dirname)
{
	if(DB_PathExist(Dirname))
	{
		RemoveDirectory(Dirname);
		return TRUE;
	}
	else
		return FALSE;
}

DARKSDK void EmptyThisDirectoryFirst(char* Dirname)
{
	struct _finddata_t filedata;

	// Record old dir
	char olddir[256];
	getcwd(olddir, 256);

	// Set directory
	_chdir(Dirname);

	// Find first file in this directory
	int res;
	long hFile = _findfirst("*.*", &filedata);
	if(hFile!=-1L)
	{
		if(strcmp(filedata.name,".")==0) res = _findnext(hFile, &filedata);
		if(strcmp(filedata.name,"..")==0) res = _findnext(hFile, &filedata);
		while(res!=-1)
		{
			if(filedata.attrib & _A_SUBDIR)
			{
				EmptyThisDirectoryFirst(filedata.name);
				RemoveDirectory(filedata.name);
			}
			else
				DeleteFile(filedata.name);

			res = _findnext(hFile, &filedata);
		}
		_findclose(hFile);
	}

	// Restore old dir
	_chdir(olddir);
}

DARKSDK BOOL DB_DeleteDirRecursively(char* Dirname)
{
	if(DB_PathExist(Dirname))
	{
		EmptyThisDirectoryFirst(Dirname);
		BOOL bResult = RemoveDirectory(Dirname);
		return TRUE;
	}
	else
		return FALSE;
}

DARKSDK BOOL DB_ExecuteFile(HANDLE* phExecuteFileProcess, char* Operation, char* Filename, char* String, char* Path, bool bWaitForTermination )
{
	if(*phExecuteFileProcess)
	{
		CloseHandle(*phExecuteFileProcess);
		*phExecuteFileProcess=NULL;
	}
	if(bWaitForTermination==true)
	{
		SHELLEXECUTEINFO seinfo;
		ZeroMemory(&seinfo, sizeof(seinfo));
		seinfo.cbSize = sizeof(seinfo);
		seinfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		seinfo.hwnd = NULL;
		seinfo.lpVerb = "open";
		seinfo.lpFile = Filename;
		seinfo.lpParameters = String;
		seinfo.lpDirectory = Path;
		seinfo.nShow = SW_SHOWDEFAULT;
		if(ShellExecuteEx(&seinfo)==TRUE)
		{
			*phExecuteFileProcess=seinfo.hProcess;
			return TRUE;
		}
		else
		{
			*phExecuteFileProcess=NULL;
			return FALSE;
		}
	}
	else
	{
		HINSTANCE hinstance = ShellExecute(	NULL,//g_pGlob->hWnd,
											"open",
											Filename,
											String,
											Path,
											SW_SHOWDEFAULT);
		if((DWORD)hinstance<=32)
			return FALSE;
		else
			return TRUE;
	}
}

DARKSDK BOOL DB_ExecuteFileIndi ( DWORD* dwExecuteFileProcess, char* Operation, char* Filename, char* String, char* Path, int iPriorityOfProcess )
{
	// create process data
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWDEFAULT;

	// directory must be absolute
	LPSTR pDirectory = NULL;
	if ( strlen ( Path )>0 )
	{
		pDirectory = new char[_MAX_PATH];
		if ( Path[1]==':' )
		{
			// absolute
			strcpy ( pDirectory, Path );
		}
		else
		{
			// relative
			getcwd ( pDirectory, _MAX_PATH );
			strcat ( pDirectory, "\\" );
			strcat ( pDirectory, Path );
		}
	}

	// Concat Filename and Commandline String
	char pConcat[512];
	strcpy ( pConcat, Filename );
	strcat ( pConcat, " " );
	strcat ( pConcat, String );

	// Process priority
	DWORD dwPriority = NORMAL_PRIORITY_CLASS;
	switch ( iPriorityOfProcess )
	{
		case 1 : dwPriority = HIGH_PRIORITY_CLASS;	break;
	}

    // Start the process. 
    if( CreateProcess(	NULL,
						pConcat,	
						NULL, 
						NULL, 
						FALSE,
						dwPriority,
						NULL,
						pDirectory,
						&si, 
						&pi )			) 
    {
		SAFE_DELETE(pDirectory);
		CloseHandle ( pi.hThread );
		CloseHandle ( pi.hProcess );
		*dwExecuteFileProcess = pi.dwProcessId;
		return TRUE;
	}
	else
	{
		SAFE_DELETE(pDirectory);
		*dwExecuteFileProcess = 0;
		return FALSE;
	}
}

DARKSDK BOOL DB_OpenToReadCore(int FileIndex, char* Filename)
{
	// Create file READ
	File[FileIndex] = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(File[FileIndex]==INVALID_HANDLE_VALUE)
	{
		File[FileIndex]=NULL;
		FileEOF[FileIndex]=FALSE;
		return FALSE;
	}
	return TRUE;
}

DARKSDK BOOL DB_OpenToRead(int FileIndex, char* Filename)
{
	BOOL bRes = FALSE;
	if(pVirtFileEncrypted[FileIndex]==NULL)
	{
		// Uses actual or virtual file..
		char VirtualFilename[_MAX_PATH];
		strcpy(VirtualFilename, (LPSTR)Filename);
		g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

		CheckForWorkshopFile ( VirtualFilename );

		// Decrypt and use media
		g_pGlob->Decrypt( (DWORD)VirtualFilename );
		pVirtFileEncrypted[FileIndex] = new char[strlen(VirtualFilename)+1];
		strcpy(pVirtFileEncrypted[FileIndex], VirtualFilename);
		bRes = DB_OpenToReadCore( FileIndex, VirtualFilename );
	}
	return bRes;
}

DARKSDK BOOL DB_OpenToWrite(int FileIndex, char* Filename)
{
	// Create file WRITE
	File[FileIndex] = CreateFile(Filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(File[FileIndex]==INVALID_HANDLE_VALUE)
	{
		File[FileIndex]=NULL;
		FileEOF[FileIndex]=FALSE;
		return FALSE;
	}
	return TRUE;
}

DARKSDK BOOL DB_CloseFile(int FileIndex)
{
	if(File[FileIndex])
	{
		CloseHandle(File[FileIndex]);
		File[FileIndex]=NULL;
		FileEOF[FileIndex]=FALSE;

		// Re-encrypt
		if(pVirtFileEncrypted[FileIndex])
		{
			g_pGlob->Encrypt( (DWORD)pVirtFileEncrypted[FileIndex] );
			delete pVirtFileEncrypted[FileIndex];
			pVirtFileEncrypted[FileIndex]=NULL;
		}
	}
	return TRUE;
}


DARKSDK LPSTR GetReturnStringFromWorkString(char* WorkString = m_pWorkString)
{
    // 20091129 v75 - IRM - Optional parameter that defaults to the workstring
    //                      Can be overridden by providing your own buffer
	LPSTR pReturnString=NULL;
	if(WorkString)
	{
		DWORD dwSize=strlen(WorkString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, WorkString);
	}
	return pReturnString;
}

//
// Command Functions
//

DARKSDK void SetDir( DWORD pString )
{
	if(pString)
	{
		if(_chdir((LPSTR)pString)==-1)
		{
			char strstr[512];
			wsprintf(strstr,"Path:%s",(LPSTR)pString);
			MessageBox ( NULL, strstr, "Path Cannot Be Found", MB_OK );
			RunTimeError(RUNTIMEERROR_PATHCANNOTBEFOUND);
		}
	}
	else
		RunTimeError(RUNTIMEERROR_PATHCANNOTBEFOUND);
}

DARKSDK void Dir(void)
{
	// Show CWD..
	getcwd(filetext, _MAX_PATH);
	strcat(filetext, ":");
	g_pGlob->PrintStringFunction(filetext, true);

	// List Files..
	FFindFirstFile();
	while(FGetFileReturnValue()!=-1L)
	{
		if(filedata.attrib & _A_SUBDIR)	
			wsprintf(filetext, "<dir>%s", filedata.name);
		else
			wsprintf(filetext, "%s", filedata.name);
		
		g_pGlob->PrintStringFunction(filetext, true);
		FFindNextFile();
	}
	FFindCloseFile();
}

DARKSDK void DriveList(void)
{
	// List Drives..
	char storedrive[_MAX_PATH];
	getcwd(storedrive, _MAX_PATH);
	_strlwr(storedrive);
	for(int drive = 1; drive <= 26; drive++)
	{
		if(!_chdrive( drive ))
		{
			wsprintf(filetext, "%c:", drive + 'A' - 1);
			g_pGlob->PrintStringFunction(filetext, true);
		}
	}
	_chdrive( storedrive[0] - 'a' + 1 );
}

DARKSDK void ChecklistForFiles(void)
{
	// Checklist flags
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;
	g_pGlob->checklistexists=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		FFindFirstFile();
		g_pGlob->checklistqty=0;
		while(FGetFileReturnValue()!=-1L)
		{
			DWORD dwSize = strlen(filedata.name)+1;
			if(dwSize>g_dwMaxStringSizeInEnum) g_dwMaxStringSizeInEnum=dwSize;
			if(g_bCreateChecklistNow)
			{
				strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, filedata.name);
				g_pGlob->checklist[g_pGlob->checklistqty].valuea = FGetActualTypeValue(filedata.attrib);
			}
			g_pGlob->checklistqty++;
			FFindNextFile();
		}
		FFindCloseFile();
	}
}

DARKSDK void ChecklistForDrives(void)
{
	// mike - 250604
	char szList [ 26 ] [ 255 ];
	int  iCount = 0;

	memset ( szList, 0, sizeof ( szList ) );

	strcpy ( szList [ iCount++ ], "a:\\" );

	for ( int iCounter = 0; iCounter < MAX_HARD_DRIVE; iCounter++ )
	{
		if ( GetDriveType ( g_HardDiskLetters [ iCounter ] ) == DRIVE_FIXED )
			strcpy ( szList [ iCount++ ], g_HardDiskLetters [ iCounter ] );

		if ( GetDriveType ( g_HardDiskLetters [ iCounter ] ) == DRIVE_CDROM )
			strcpy ( szList [ iCount++ ], g_HardDiskLetters [ iCounter ] );
	}

	
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	g_pGlob->checklistexists=true;

	g_pGlob->checklistqty = iCount;

	for(int c=0; c<g_pGlob->checklistqty; c++)
		GlobExpandChecklist(c, 255);

	// mike - 020206 - addition for vs8
	//for( c=0; c<g_pGlob->checklistqty; c++)
	for( int c=0; c<g_pGlob->checklistqty; c++)
	{
		strcpy(g_pGlob->checklist[c].string, szList[c]);
	}

	int i = g_pGlob->checklistqty;

	

	

	
	/*
	char storedrive[_MAX_PATH];
	getcwd(storedrive, _MAX_PATH);
	_strlwr(storedrive);

	// Checklist flags
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	g_pGlob->checklistexists=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		g_pGlob->checklistqty=0;
		//for(int drive = 1; drive <= 26; drive++)
		for(int drive = 2; drive <= 26; drive++)
		{
			if(!_chdrive( drive ))
			{
				wsprintf(filetext, "%c:", drive + 'A' - 1);
				DWORD dwSize = strlen(filetext)+1;
				if(dwSize>g_dwMaxStringSizeInEnum) g_dwMaxStringSizeInEnum=dwSize;
				if(g_bCreateChecklistNow)
				{
					strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, filetext);
				}
				g_pGlob->checklistqty++;
			}
		}
	}

	_chdrive( storedrive[0] - 'a' + 1 );
	*/
	
}

DARKSDK void FindFirst(void)
{
	FFindFirstFile();
}

DARKSDK void FindNext(void)
{
	if(hInternalFile)
	{
		if(FileReturnValue==-1L)
		{
			RunTimeError(RUNTIMEERROR_NOMOREFILESINDIR);
			return;
		}

		FFindNextFile();
	}
}

DARKSDK int CanMakeFile( DWORD pFilename )
{
	// 031107 - used to determine if in LIMITED USER AREA (no write)
	if(DB_CanMakeFile((LPSTR)pFilename))
		return 1;
	else
		return 0;
}

DARKSDK void MakeFile( DWORD pFilename )
{
	if(!DB_MakeFile((LPSTR)pFilename))
		RunTimeWarning(RUNTIMEERROR_CANNOTMAKEFILE);
}

DARKSDK void DeleteFile( DWORD pFilename )
{
	// LEEADD - 190803 - Will set the file to NORMAL so can delete READONLY files too
	if ( SetFileAttributes ( (LPSTR)pFilename, FILE_ATTRIBUTE_NORMAL )==FALSE )
	{
		RunTimeSoftWarning(RUNTIMEERROR_CANNOTDELETEFILE);
		return;
	}

	// LEEFIX - 081102 - Added Code to return if file locked when deleting it..
	if(DB_FileExist((LPSTR)pFilename))
	{
		DeleteFile((LPSTR)pFilename);
		DWORD dwErr = GetLastError();
		if(dwErr==ERROR_SHARING_VIOLATION)
		{
			RunTimeWarning(RUNTIMEERROR_FILEISLOCKED);
		}
	}
	else
	{
		RunTimeSoftWarning(RUNTIMEERROR_CANNOTDELETEFILE);
	}
}

DARKSDK void CopyFileCore( DWORD pFromFilename, DWORD pFilename2 )
{
	if(!DB_CopyFile((LPSTR)pFromFilename, (LPSTR)pFilename2 ))
		RunTimeWarning(RUNTIMEERROR_FILEEXISTS);
}

DARKSDK void CopyFile( DWORD szFilename, DWORD pFilename2 )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	CopyFileCore ( (DWORD)VirtualFilename, pFilename2 );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void RenameFile( DWORD pFilename, DWORD pFilename2 )
{
	if(!DB_FileExist((LPSTR)pFilename2))
	{
		if(!DB_RenameFile((LPSTR)pFilename, (LPSTR)pFilename2 ))
			RunTimeWarning(RUNTIMEERROR_CANNOTRENAMEFILE);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILEEXISTS);
}

DARKSDK void MoveFile( DWORD pFilename, DWORD pFilename2 )
{
	if(!DB_FileExist((LPSTR)pFilename2))
	{
		if(!DB_RenameFile((LPSTR)pFilename, (LPSTR)pFilename2))
			RunTimeWarning(RUNTIMEERROR_CANNOTMOVEFILE);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILEEXISTS);
}

DARKSDK void WriteByteToFile( DWORD pFilename, int iPos, int iByte )
{
	char FilenameString[256];
	strcpy(FilenameString, (LPSTR)pFilename);
	if(DB_FileExist(FilenameString))
	{
		// Open file to be read
		HANDLE hreadfile = CreateFile(FilenameString, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hreadfile!=INVALID_HANDLE_VALUE)
		{
			// Read file into memory
			DWORD bytesread;
			int filebuffersize = GetFileSize(hreadfile, NULL);	
			char* filebuffer = (char*)GlobalAlloc(GMEM_FIXED, filebuffersize);
			ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
			CloseHandle(hreadfile);		

			// Modify byte
			int offset = iPos;
			if(offset>=0 && offset<filebuffersize)
				filebuffer[offset] = iByte;

			// Write back out again
			HANDLE hwritefile = CreateFile(FilenameString, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hreadfile!=INVALID_HANDLE_VALUE)
			{
				// Write mem to file
				DWORD byteswritten;
				WriteFile(hwritefile, filebuffer, filebuffersize, &byteswritten, NULL); 
				CloseHandle(hwritefile);		
			}

			// Discard memory used
			if(filebuffer)
			{
				GlobalFree(filebuffer);
				filebuffer=NULL;
			}
		}
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENOTEXIST);
}

DARKSDK int ReadByteFromFile( DWORD pFilename, int iPos )
{
	int iResult=0;
	char FilenameString[_MAX_PATH];
	strcpy(FilenameString, (LPSTR)pFilename);
	if(DB_FileExist(FilenameString))
	{
		// Open file to be read
		HANDLE hreadfile = CreateFile(FilenameString, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hreadfile!=INVALID_HANDLE_VALUE)
		{
			// Read file into memory
			DWORD bytesread;
			int filebuffersize = GetFileSize(hreadfile, NULL);	
			char* filebuffer = (char*)GlobalAlloc(GMEM_FIXED, filebuffersize);
			ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
			CloseHandle(hreadfile);		

			// Read byte
			int data = 0;
			int offset = iPos;
			if(offset>=0 && offset<filebuffersize) data = filebuffer[offset];
			iResult=data;

			// Discard memory used
			if(filebuffer)
			{
				GlobalFree(filebuffer);
				filebuffer=NULL;
			}
		}
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENOTEXIST);

	return iResult;
}

DARKSDK void MakeDir( DWORD pFilename )
{
	if(!DB_MakeDir((LPSTR)pFilename))
		RunTimeWarning(RUNTIMEERROR_CANNOTMAKEDIR);
}

DARKSDK void DeleteDir( DWORD pFilename )
{
	if(!DB_DeleteDirRecursively((LPSTR)pFilename))
		RunTimeWarning(RUNTIMEERROR_CANNOTDELETEDIR);
}

DARKSDK void DeleteDir( DWORD pFilename, int iFlag )
{
	if(!DB_DeleteDirRecursively((LPSTR)pFilename))
		RunTimeWarning(RUNTIMEERROR_CANNOTDELETEDIR);
}

DARKSDK void ExecuteFile( DWORD pFilename, DWORD pFilename2, DWORD pFilename3 )
{
	if(!DB_ExecuteFile(&ghExecuteFileProcess, "", (LPSTR)pFilename, (LPSTR)pFilename2, (LPSTR)pFilename3, false ))
		RunTimeWarning(RUNTIMEERROR_CANNOTEXECUTEFILE);
}

DARKSDK void ExecuteFileEx( DWORD pFilename, DWORD pFilename2, DWORD pFilename3, int iWaitForExeEndFlag )
{
	if(!DB_ExecuteFile(&ghExecuteFileProcess, "",(LPSTR)pFilename, (LPSTR)pFilename2, (LPSTR)pFilename3, true ))
		RunTimeWarning(RUNTIMEERROR_CANNOTEXECUTEFILE);

	// Wait Here Until Exe Terminates
	if ( iWaitForExeEndFlag==1 )
	{
		while(ghExecuteFileProcess)
		{
			DWORD dwStatus;
			if(GetExitCodeProcess(ghExecuteFileProcess, &dwStatus)==TRUE)
			{
				if(dwStatus!=STILL_ACTIVE)
				{
					// Closes process after it deactivates
					CloseHandle(ghExecuteFileProcess);
					ghExecuteFileProcess=NULL;
				}
			}
			if(g_pGlob->ProcessMessageFunction()==1)
			{
				// Closes process if main app terminates
				CloseHandle(ghExecuteFileProcess);
				ghExecuteFileProcess=NULL;
			}
		}
	}
}

DARKSDK DWORD ExecuteFileIndi( DWORD pFilename, DWORD pFilename2, DWORD pFilename3, int iPriorityOfProcess )
{
	// Create process and return handle
	DWORD dwIndiExecuteFileProcess = 0;
	if(!DB_ExecuteFileIndi ( &dwIndiExecuteFileProcess, "",(LPSTR)pFilename, (LPSTR)pFilename2, (LPSTR)pFilename3, iPriorityOfProcess ) )
		RunTimeWarning(RUNTIMEERROR_CANNOTEXECUTEFILE);

	// return handle for later monitoring
	return dwIndiExecuteFileProcess;
}

DARKSDK DWORD ExecuteFileIndi( DWORD pFilename, DWORD pFilename2, DWORD pFilename3 )
{
	return ExecuteFileIndi( pFilename, pFilename2, pFilename3, 0 );
}

DARKSDK BOOL CALLBACK EnumWindowsProcForTerminator( HWND hwnd, LPARAM lParam )
{
	// check process ID of window
	DWORD dwProcessID;
	GetWindowThreadProcessId ( hwnd, &dwProcessID );
	if ( dwProcessID==(DWORD)lParam )
	{
		// Post close message to any windows associated with process
		PostMessage ( hwnd, WM_CLOSE, 0, 0 );
	}
	return TRUE;
}

DARKSDK void StopExecutable ( DWORD dwIndiExecuteFileProcess )
{
	// check if exe in active and running
	if ( dwIndiExecuteFileProcess )
	{
		// Enumerate all windows and close any that are owned by the process ID
		EnumWindows ( EnumWindowsProcForTerminator, dwIndiExecuteFileProcess );
	}
}

DARKSDK int GetExecutableRunning ( DWORD dwIndiExecuteFileProcess )
{
	// running
	int iRunning=0;

	// check if exe in active and running
	if ( dwIndiExecuteFileProcess )
	{
		// get handle to process
		HANDLE hIndiExecuteFileProcess = OpenProcess ( PROCESS_QUERY_INFORMATION, TRUE, dwIndiExecuteFileProcess );

		DWORD dwStatus;
		if ( GetExitCodeProcess ( hIndiExecuteFileProcess, &dwStatus )==TRUE )
			if(dwStatus==STILL_ACTIVE)
				iRunning = 1;

		// close handle to process
		CloseHandle ( hIndiExecuteFileProcess );
	}

	// not running
	return iRunning;
}

//
// File Mapping Functions
//

DARKSDK void WriteFilemap ( DWORD pFilemapname, DWORD dwValue, DWORD pString, int iWriteType )
{
	// Open or create filemap
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_WRITE, TRUE, (LPSTR)pFilemapname);
	if ( hFileMap==NULL ) hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,1024,(LPSTR)pFilemapname);
	LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,1024);

	// Copy data to filemap
	if ( iWriteType==0 )
	{
		*((DWORD*)lpVoid+0) = dwValue;
	}
	if ( iWriteType==1 )
	{
		// Copy data to filemap
		DWORD dwStringSize = strlen((LPSTR)pString);
		*((DWORD*)lpVoid+1) = dwStringSize;
		strcpy((LPSTR)lpVoid+8, (LPSTR)pString);
	}

	// Release virtual file
	UnmapViewOfFile(lpVoid);
//	CloseHandle(hFileMap);
}

DARKSDK void WriteFilemapValue ( DWORD pFilemapname, DWORD dwValue )
{
	// Write value to filemap
	WriteFilemap ( pFilemapname, dwValue, NULL, 0 );
}

DARKSDK void WriteFilemapString ( DWORD pFilemapname, DWORD pString )
{
	// Write string to filemap
	if ( pString )
		if ( strlen((LPSTR)pString)<=1000 )
			WriteFilemap ( pFilemapname, 0, pString, 1 );
}

DARKSDK DWORD ReadFilemapValue ( DWORD pFilemapname )
{
	// Open or create filemap for reading
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_WRITE, TRUE, (LPSTR)pFilemapname);
	if ( hFileMap==NULL ) hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,1024,(LPSTR)pFilemapname);
	LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,1024);

	// Copy data from filemap
	DWORD dwValue = *((DWORD*)lpVoid+0);

	// Release virtual file
	UnmapViewOfFile(lpVoid);
//	CloseHandle(hFileMap);

	// return data
	return dwValue;
}

DARKSDK DWORD ReadFilemapString ( DWORD pDestStr, DWORD pFilemapname )
{
	// Open or create filemap for reading
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_READ, TRUE, (LPSTR)pFilemapname);
	if ( hFileMap==NULL ) hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,1024,(LPSTR)pFilemapname);
	LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,1024);

	// Copy data from filemap
	strcpy ( m_pWorkString, (LPSTR)lpVoid+8 );

	// Release virtual file
	UnmapViewOfFile(lpVoid);
//	CloseHandle(hFileMap);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();

	// return data
	return (DWORD)pReturnString;
}

//
// Sequential File Access Functions
//

DARKSDK void OpenToRead( int f, DWORD pFilename )
{

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]==NULL)
		{
			if(DB_FileExist((LPSTR)pFilename))
			{
				if(!DB_OpenToRead(f, (LPSTR)pFilename))
					RunTimeSoftWarning(RUNTIMEERROR_CANNOTOPENFILEFORREADING);
			}
			else
				RunTimeError(RUNTIMEERROR_FILENOTEXIST);
		}
		else
			RunTimeError(RUNTIMEERROR_FILEALREADYOPEN);
	}
	else
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void OpenToWrite( int f, DWORD pFilename )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]==NULL)
		{
			if(!DB_FileExist((LPSTR)pFilename))
			{
				if(!DB_OpenToWrite(f, (LPSTR)pFilename))
					RunTimeSoftWarning(RUNTIMEERROR_CANNOTOPENFILEFORWRITING);
			}
			else
				RunTimeWarning(RUNTIMEERROR_FILEEXISTS);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILEALREADYOPEN);
	}
	else
	{
		// mike - 011005 - use run time error for this
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
	}
}

DARKSDK void CloseFile( int f )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
			DB_CloseFile(f);
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK int ReadByte( int f )
{
	int iResult=0;
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Read from file
			unsigned char data;
			if(ReadFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			if(bytes==0) FileEOF[f]=TRUE;

			iResult = data;
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return iResult;
}

DARKSDK int ReadWord( int f )
{
	int iResult=0;
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Read from file
			WORD data;
			if(ReadFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			if(bytes==0) FileEOF[f]=TRUE;

			iResult = data;
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return iResult;
}

DARKSDK int ReadLong( int f )
{
	int iResult=0;
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Read from file
			DWORD data;
			if(ReadFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			if(bytes==0) FileEOF[f]=TRUE;

			iResult = data;
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return iResult;
}

DARKSDK DWORD ReadFloat( int f )
{
	float fResult=0.0f;
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Read from file
			float data;
			if(ReadFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			if(bytes==0) FileEOF[f]=TRUE;

			fResult = data;
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return *(DWORD*)&fResult;
}

DARKSDK DWORD ReadString( int f, DWORD pDestStr )
{
    /*
        20091129 v75 - IRM - http://forum.thegamecreators.com/?m=forum_view&t=81894&b=15
        Use of fixed size buffers (1024 bytes for m_pWorkString and 2048 for internal
        buffer in this function gave two chances for buffer overruns to crash the program.
        
        Replaced the use of these buffers with a single buffer implemented using an
        std::vector, and changed the routine GetReturnStringFromWorkString to allow an
        optional buffer to be provided (defaults to m_pWorkString).
    */

	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);

    LPSTR pReturnString=0;

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			unsigned char c=0;
            DWORD bytes;
            std::vector<char> WorkString;

			bool eof=false;
			do
			{
				if(ReadFile(File[f], &c, 1, &bytes, NULL)==0)
				{
					RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
					goto fileerror;
				}
				if(bytes==0)
                {
                    FileEOF[f]=TRUE;
					eof=true;
                }
				else if(c>=32 || c==9)
                {
                    WorkString.push_back(c);
                }
			} while((c>=32 || c==9) && !eof);

            WorkString.push_back(0);

			if(c==13)
			{
				if(ReadFile(File[f], &c, 1, &bytes, NULL)==0)
				{
					RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
					goto fileerror;
				}
				if(bytes==0) FileEOF[f]=TRUE;
			}

	        // Create and return string
	        pReturnString=GetReturnStringFromWorkString( &WorkString[0] );
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

fileerror:

    return (DWORD)pReturnString;
}

DARKSDK void ReadFileBlockCore(char* FilenameString, int f )
{
	// Get Size of fileblock
	DWORD bytes;
	DWORD nSize;
	ReadFile(File[f], &nSize, 4, &bytes, NULL);

	// Create mem for it
	char* pBuffer = (char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, nSize);
	if(pBuffer)
	{
		// Read it in
		ReadFile(File[f], pBuffer, nSize, &bytes, NULL);
		if(bytes==0) FileEOF[f]=TRUE;

		// Write it to file
		DWORD byteswritten;
		HANDLE hwritefile = CreateFile(FilenameString, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hwritefile!=INVALID_HANDLE_VALUE)
		{
			WriteFile(hwritefile, pBuffer, nSize, &byteswritten, NULL);
			CloseHandle(hwritefile);		
		}

		// Free buffer
		GlobalFree(pBuffer);
		pBuffer=NULL;
	}
}

DARKSDK void MakePathToThisFolder(char* thepathiwant)
{
	// get directory desired
	char file[_MAX_PATH];
	strcpy(file, thepathiwant);

	// Get path from filename (upto 8 nests)
	char filepath[8][256];
	int filepathindex=0;
	for(unsigned int n=0; n<8; n++)
		strcpy(filepath[n],"");

	// mike - 020206 - addition for vs8
	//for(n=0; n<strlen(file); n++)
	for(unsigned int n=0; n<strlen(file); n++)
	{
		if(file[n]=='\\')
		{
			// mike - 020206 - addition for vs8
			unsigned int o = 0;

			// Get folder name
			char folder[256];
			
			//for(unsigned int o=0; o<n; o++)
			for(o=0; o<n; o++)
				folder[o]=file[o];
			folder[o]=0;

			// Copy and store it
			strcpy(filepath[filepathindex], folder);
			if(filepathindex<7) filepathindex++;

			// Truncate and continue
			unsigned int q=0;
			for(unsigned int p=n+1; p<=strlen(file); p++)
				file[q++]=file[p];
			file[q]=0;
			n=0;
		}
	}

	// Store current directory
	char olddir[256];
	_getcwd(olddir, 256);

	// If filename has a path, and it doesnt exist, make it
	for(int m=0; m<filepathindex; m++)
	{
		if(strcmp(filepath[m],"")!=0)
		{
			if(chdir(filepath[m])==-1)
			{
				mkdir(filepath[m]);
				chdir(filepath[m]);
			}
		}
	}

	// Restore directory
	chdir(olddir);
}

DARKSDK void ReadFileBlock( int f, DWORD pFilename )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			char* FilenameString = (LPSTR)pFilename;

			// Store current directory
			char olddir[256];
			getcwd(olddir, 256);

			// If directory doesn't exist, create one
			char DirString[256];
			strcpy(DirString, FilenameString);

			// mike - 020206 - addition for vs8
			int n = 0;
			//for(int n=strlen(DirString)-1; n>0 ; n--)
			for(n=strlen(DirString)-1; n>0 ; n--)
				if(DirString[n]=='\\') break;
			DirString[n+1]=0;
			MakePathToThisFolder(DirString);

			// Create file (nicely into dir if created earlier)
			ReadFileBlockCore(FilenameString, f);

			// Restore directory
			chdir(olddir);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void SkipBytes( int f, int iSkipValue )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			DWORD nSize = (int)iSkipValue;
			char* pBuffer = (char*)GlobalAlloc(GMEM_FIXED, nSize);
			if(pBuffer)
			{
				// Read from file skippable bytes
				if(ReadFile(File[f], pBuffer, nSize, &bytes, NULL)==0)
					RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
				if(bytes<nSize) FileEOF[f]=TRUE;
				GlobalFree(pBuffer);
				pBuffer=NULL;
			}
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void ReadDirBlock( int f, DWORD pFilename )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Find Directory to write
			char* DirString = (LPSTR)pFilename;

			// Store current directory
			char olddir[256];
			getcwd(olddir, 256);

			// If directory doesn't exist, create one
			char pNewDir[_MAX_PATH];
			strcpy(pNewDir, DirString);
			DWORD dwLength=strlen(pNewDir);
			if(pNewDir[dwLength-1]!='\\') { pNewDir[dwLength]='\\'; pNewDir[dwLength+1]=0; } 
			MakePathToThisFolder(pNewDir);
			chdir(pNewDir);

			// Read number of files in dirblock
			DWORD bytes;
			DWORD NumberOfFiles=0;
			if(ReadFile(File[f], &NumberOfFiles, sizeof(NumberOfFiles), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
			if(bytes==0) FileEOF[f]=TRUE;

			// Load all files in
			for(unsigned int n=0; n<NumberOfFiles; n++)
			{
				// Read size of filename
				int stringlength=0;
				if(ReadFile(File[f], &stringlength, sizeof(stringlength), &bytes, NULL)==0)
					RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
				if(bytes==0) FileEOF[f]=TRUE;
				if(stringlength>0)
				{
					// Read filename
					char* FilenameString = (char*)GlobalAlloc(GMEM_FIXED, stringlength);
					if(FilenameString)
					{
						if(ReadFile(File[f], FilenameString, stringlength, &bytes, NULL)==0)
							RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);

						// Get path from filename (upto 8 nests)
						MakePathToThisFolder(FilenameString);

						// Read fileblock
						ReadFileBlockCore(FilenameString, f);

						// Release string
						GlobalFree(FilenameString);
						FilenameString=NULL;
					}
					else
					{
						RunTimeWarning(RUNTIMEERROR_CANNOTREADFROMFILE);
					}
				}
			}

			// Restore directory
			chdir(olddir);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteByte( int f, int iValue )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Write to file
			unsigned char data = (unsigned char)iValue;
			if(WriteFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteWord( int f, int iValue )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Write to file
			WORD data = (WORD)iValue;
			if(WriteFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteLong( int f, int iValue )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Write to file
			DWORD data = (DWORD)iValue;
			if(WriteFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteFloat( int f, float fValue )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Write to file
			float data = fValue;
			if(WriteFile(File[f], &data, sizeof(data), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteString( int f, DWORD pString )
{
    /*
        20091129 v75 - IRM - http://forum.thegamecreators.com/?m=forum_view&t=108603&b=15
        Copy of the input string to an internal buffer of fixed size (2k) caused buffer
        overruns into the stack and program crashes.

        Elimitated the buffer by writing from the original string (if set).
    */

    char carriage[3];
	carriage[0]=13;
	carriage[1]=10;
	carriage[2]=0;

	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
            // 20091129 v75 - IRM - Only write the string if it has been set
            if (pString)
            {
                LPSTR string = (char*)pString;
                DWORD stringlength=strlen(string);

                // 20091129 v75 - IRM - Only write the string if >0 bytes
                if (stringlength)
                {
                    // 20091129 v75 - IRM - Write directly from the input data, not using a secondary buffer
    			    if(WriteFile(File[f], string, stringlength, &bytes, NULL)==0)
	    			    RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
                }
            }

			// Write Carriage-Return to file
			if(WriteFile(File[f], carriage, 2, &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteFileBlockCore( char* FilenameString, int f, int mode )
{
	DWORD bytes;
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Read fileblock from file
			DWORD bytesread;
			HANDLE hreadfile = CreateFile(FilenameString, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hreadfile!=INVALID_HANDLE_VALUE)
			{
				// Get Size of fileblock
				DWORD nSize = GetFileSize(hreadfile, NULL);

				// Create mem for it
				char* pBuffer = (char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, nSize);
				if(pBuffer)
				{
					// Read the data into buffer
					ReadFile(hreadfile, pBuffer, nSize, &bytesread, NULL);

					// Close handle to file
					CloseHandle(hreadfile);		
					hreadfile=NULL;

					// Write it out to my file
					if(mode==1)
					{
						// Don't write size, cannot read back!
					}
					else
						WriteFile(File[f], &nSize, 4, &bytes, NULL);

					WriteFile(File[f], pBuffer, nSize, &bytes, NULL);

					// Free buffer
					GlobalFree(pBuffer);
					pBuffer=NULL;
				}
			}
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteFileBlock( int f, DWORD pFilename )
{
	char* FilenameString = (LPSTR)pFilename;
	WriteFileBlockCore(FilenameString, f, 0);
}

DARKSDK void WriteFileBlock( int f, DWORD pFilename, int iFlag )
{
	char* FilenameString = (LPSTR)pFilename;
	WriteFileBlockCore(FilenameString, f, 1);
}

DARKSDK void WriteDirContents(int f, char* newdir, bool bMode, DWORD* pCount, char* relativedir)
{
	// Remember current dir
	char olddir[256];
	getcwd(olddir,256);

	// Switch to new dir
	chdir(newdir);

	// Go through dir and write out all files
	int res=-1;
	struct _finddata_t filedata;
	long hLocalFile = _findfirst("*.*", &filedata);
	if(strcmp(filedata.name,".")==0) res=_findnext(hLocalFile, &filedata);
	if(strcmp(filedata.name,"..")==0) res=_findnext(hLocalFile, &filedata);
	while(res!=-1L)
	{
		if(FGetActualTypeValue(filedata.attrib)==1)
		{
			char thisrelativedir[256];
			strcpy(thisrelativedir, relativedir);
			strcat(thisrelativedir, filedata.name);
			strcat(thisrelativedir, "\\");
			WriteDirContents(f, filedata.name, bMode, pCount, thisrelativedir);
		}
		else
		{
			if(bMode)
			{
				// Get size of filename
				DWORD bytes;
				char string[256];
				strcpy(string, relativedir);
				strcat(string, filedata.name);
				DWORD stringlength=strlen(string)+1;

				// Write size of filename first
				if(WriteFile(File[f], &stringlength, sizeof(stringlength), &bytes, NULL)==0)
					RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);

				// Write filename string second
				if(WriteFile(File[f], string, stringlength, &bytes, NULL)==0)
					RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);

				// Write actual fileblock
				WriteFileBlockCore(filedata.name, f, 0);
			}
			else
			{
				int inc = *(pCount);
				*(pCount)=inc+1;
			}
		}
		res=_findnext(hLocalFile, &filedata);
	}
	_findclose(hLocalFile);

	// Restore old dir
	chdir(olddir);
}

DARKSDK void WriteDirBlock( int f, DWORD pFilename )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			// Find Directory to write
			char* DirString = (LPSTR)pFilename;

			// Count files in dirblock
			DWORD Count=0;
			char RelativeDir[256];
			strcpy(RelativeDir,"");
			WriteDirContents(f, DirString, false, &Count, RelativeDir);

			// Write Header to DirBlock
			DWORD bytes;
			if(WriteFile(File[f], &Count, sizeof(Count), &bytes, NULL)==0)
				RunTimeWarning(RUNTIMEERROR_CANNOTWRITETOFILE);

			// Write all files in dir
			strcpy(RelativeDir,"");
			WriteDirContents(f, DirString, true, &Count, RelativeDir);
		}
		else
			RunTimeWarning(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void ReadMemblock( int f, int mbi )
{
	// mike - 011005 - quit if invalid pointer
	if ( !g_MEM_Make )
		return;

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			if(mbi>=1 && mbi<=255)
			{
				// Get Size of MEMBLOCK
				DWORD bytes, size;
				if(ReadFile(File[f], &size, 4, &bytes, NULL)!=0)
				{
					// Create memblock memory
					LPSTR pMem = g_MEM_Make ( mbi, size );
					if(pMem)
					{
						// Gey Data of MEMBLOCK from FILE
						if(ReadFile(File[f], pMem, size, &bytes, NULL)==0)
							RunTimeError(RUNTIMEERROR_CANNOTREADFROMFILE);
					}
					else
						RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
				}
				else
					RunTimeError(RUNTIMEERROR_CANNOTREADFROMFILE);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
		}
		else
			RunTimeError(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void MakeMemblockFromFile( int mbi, int f )
{
	// mike - 011005 - quit if invalid pointer
	if ( !g_MEM_Make )
		return;

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			if(mbi>=1 && mbi<=255)
			{
				// Get file size
				DWORD dwSize = GetFileSize(File[f], NULL);
				if(dwSize>0)
				{
					// Get Size of MEMBLOCK
					DWORD bytes;

					// Create memblock memory
					LPSTR pMem = g_MEM_Make ( mbi, dwSize );
					if(pMem)
					{
						// Gey Data of MEMBLOCK from FILE
						if(ReadFile(File[f], pMem, dwSize, &bytes, NULL)==0)
							RunTimeError(RUNTIMEERROR_CANNOTREADFROMFILE);
					}
					else
						RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
				}
				else
					RunTimeError(RUNTIMEERROR_CANNOTREADFROMFILE);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
		}
		else
			RunTimeError(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void MakeFileFromMemblock( int f, int mbi )
{
	// mike - 011005 - quit if invalid pointer
	if ( !g_MEM_GetPtr || !g_MEM_GetSize )
		return;

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			if(mbi>=1 && mbi<=255)
			{
				LPSTR pPtr = g_MEM_GetPtr ( mbi );
				if(pPtr)
				{
					// Write it to file (difference from WRIITE MEMBLOCK is that no size DWORD is written = pure file)
					DWORD bytes;
					DWORD dwSize = g_MEM_GetSize ( mbi );

					// Write MEMBLOCK to FILE
					if(WriteFile(File[f], pPtr, dwSize, &bytes, NULL)==0)
						RunTimeError(RUNTIMEERROR_CANNOTWRITETOFILE);
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
		}
		else
			RunTimeError(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
}

DARKSDK void WriteMemblock( int f, int mbi )
{
	// mike - 011005 - quit if invalid pointer
	if ( !g_MEM_GetPtr || !g_MEM_GetSize )
		return;

	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
		{
			if(mbi>=1 && mbi<=255)
			{
				LPSTR pPtr = g_MEM_GetPtr ( mbi );
				if(pPtr)
				{
					// Write MEMBLOCK Size to FILE
					DWORD dwSize = g_MEM_GetSize ( mbi );
					DWORD bytes;
					if(WriteFile(File[f], &dwSize, 4, &bytes, NULL)!=0)
					{
						// Write MEMBLOCK to FILE
						if(WriteFile(File[f], pPtr, dwSize, &bytes, NULL)==0)
							RunTimeError(RUNTIMEERROR_CANNOTWRITETOFILE);
					}
					else
						RunTimeError(RUNTIMEERROR_CANNOTWRITETOFILE);
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
		}
		else
			RunTimeError(RUNTIMEERROR_FILENOTOPEN);
	}
	else
		RunTimeError(RUNTIMEERROR_FILENUMBERINVALID);
}

//		
// Command Expression Functions
//

DARKSDK DWORD GetDir( DWORD pDestStr )
{
	// Create and return string
	getcwd(m_pWorkString, 1024);
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetFileName( DWORD pDestStr )
{
	if(hInternalFile)
		strcpy(m_pWorkString, filedata.name);
	else
		strcpy(m_pWorkString, "");

	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int GetFileType( void )
{
	if(FGetFileReturnValue()==-1L || hInternalFile==NULL)
		return -1;
	else
		return FGetActualTypeValue(filedata.attrib);
}

DARKSDK DWORD GetFileDate( DWORD pDestStr )
{
	if(hInternalFile)
		wsprintf(m_pWorkString, "%.24s", ctime( &( filedata.time_write)));
	else
		strcpy(m_pWorkString, "");

	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD GetFileCreation( DWORD pDestStr )
{
	if(hInternalFile)
		wsprintf(m_pWorkString, "%.24s", ctime( &( filedata.time_create)));
	else
		strcpy(m_pWorkString, "");

	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int FileExist( DWORD pFilename )
{
	if(DB_FileExist((LPSTR)pFilename))
		return 1;
	else
		return 0;
}

DARKSDK int FileSize( DWORD pFilename )
{
	return DB_FileSize((LPSTR)pFilename);
}

DARKSDK int PathExist( DWORD pFilename )
{
	if(DB_PathExist((LPSTR)pFilename))
		return 1;
	else
		return 0;
}

DARKSDK int FileOpen( int f )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(File[f]!=NULL)
			return 1;
		else
			return 0;
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return 0;
}

DARKSDK int FileEnd( int f )
{
	if(f>=1 && f<=MAX_FILES)
	{
		if(FileEOF[f]==TRUE)
			return 1;
		else
			return 0;
	}
	else
		RunTimeWarning(RUNTIMEERROR_FILENUMBERINVALID);

	return 0;
}

DARKSDK DWORD Appname( DWORD pDestStr )
{
	// Create and return string
	GetModuleFileName(g_pGlob->hInstance, m_pWorkString, 1024);
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Windir( DWORD pDestStr )
{
	// Create and return string
	GetWindowsDirectory(m_pWorkString, 1024);	
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Mydocdir( DWORD pDestStr )
{
	// lee - 040407 - return the My Documents folder in full
	SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, m_pWorkString );
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorFile ( void )
{
	Constructor ( );
}

void DestructorFile  ( void )
{
	Destructor ( );
}

void SetErrorHandlerFile ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataFile( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void dbSetDir ( char* pString )
{
	SetDir ( ( DWORD ) pString );
}

void dbDir ( void )
{
	Dir ( );
}

void dbDriveList ( void )
{
	DriveList ( );
}

void dbPerformCheckListForFiles ( void )
{
	ChecklistForFiles ( );
}

void dbPerformCheckListForDrives ( void )
{
	ChecklistForDrives ( );
}

void dbFindFirst ( void )
{
	FindFirst ( );
}

void dbFindNext ( void )
{
	FindNext ( );
}

void dbMakeFile ( char* pFilename )
{
	MakeFile ( ( DWORD ) pFilename );
}

void dbDeleteFile ( char* pFilename )
{
	DeleteFile ( ( DWORD ) pFilename );
}

void dbCopyFile ( char* pFilename, char* pFilename2 )
{
	CopyFile ( ( DWORD ) pFilename, ( DWORD ) pFilename2 );
}

void dbRenameFile ( char* pFilename, char* pFilename2 )
{
	RenameFile ( ( DWORD ) pFilename, ( DWORD ) pFilename2 );
}

void dbMoveFile ( char* pFilename, char* pFilename2 )
{
	MoveFile ( ( DWORD ) pFilename, ( DWORD ) pFilename2 );
}

void dbWriteByteToFile ( char* pFilename, int iPos, int iByte )
{
	WriteByteToFile ( ( DWORD ) pFilename, iPos, iByte );
}

int dbReadByteFromFile ( char* pFilename, int iPos )
{
	return ReadByteFromFile ( ( DWORD ) pFilename, iPos );
}

void dbMakeDirectory ( char* pFilename )
{
	MakeDir ( ( DWORD ) pFilename );
}

void dbDeleteDirectory ( char* pFilename )
{
	DeleteDir ( ( DWORD ) pFilename );
}

void dbDeleteDirectory ( char* pFilename, int iFlag )
{
	//DeleteDirEx ( pFilename, iFlag );
}

void dbExecuteFile ( char* pFilename, char* pFilename2, char* pFilename3 )
{
	ExecuteFile ( ( DWORD ) pFilename, ( DWORD ) pFilename2, ( DWORD ) pFilename3 );
}

void dbExecuteFile ( char* pFilename, char* pFilename2, char* pFilename3, int iFlag )
{
	ExecuteFileEx ( ( DWORD ) pFilename, ( DWORD ) pFilename2, ( DWORD ) pFilename3, iFlag );
}

DWORD dbExecuteExecutable ( char* pFilename, char* pFilename2, char* pFilename3 )
{
	return ExecuteFileIndi ( ( DWORD ) pFilename, ( DWORD ) pFilename2, ( DWORD ) pFilename3 );
}

DWORD dbExecuteExecutable ( char* pFilename, char* pFilename2, char* pFilename3, int iPriority )
{
	return ExecuteFileIndi ( ( DWORD ) pFilename, ( DWORD ) pFilename2, ( DWORD ) pFilename3, iPriority );
}

void dbStopExecutable ( DWORD hIndiExecuteFileProcess )
{
	StopExecutable ( hIndiExecuteFileProcess );
}

void dbWriteFilemapValue ( char* pFilemapname, DWORD dwValue )
{
	WriteFilemapValue ( ( DWORD ) pFilemapname, dwValue );
}

void dbWriteFilemapString ( char* pFilemapname, char* pString )
{
	WriteFilemapString ( ( DWORD ) pFilemapname, ( DWORD ) pString );
}

DWORD dbReadFilemapValue ( char* pFilemapname )
{
	return ReadFilemapValue ( ( DWORD ) pFilemapname );
}

char* dbReadFilemapString ( char* pFilemapname )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = ReadFilemapString ( NULL, ( DWORD ) pFilemapname );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

DWORD dbReadFilemapString ( DWORD pDestStr, DWORD pFilemapname )
{
	return ReadFilemapString ( ( DWORD ) pDestStr, ( DWORD ) pFilemapname );
}

void dbOpenToRead ( int f, char* pFilename )
{
	OpenToRead ( f, ( DWORD ) pFilename );
}

void dbOpenToWrite ( int f, char* pFilename )
{
	OpenToWrite ( f, ( DWORD ) pFilename );
}

void dbCloseFile ( int f )
{
	CloseFile ( f );
}

int dbReadByte ( int f )
{
	return ReadByte ( f );
}

int dbReadWord ( int f )
{
	return ReadWord ( f );
}

int dbReadFile ( int f )
{
	return ReadLong ( f );
}

float dbReadFloat ( int f )
{
	DWORD dwReturn = ReadFloat ( f );
	
	return *( float* ) &dwReturn;
}

char* dbReadString ( int f )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = ReadString ( f, NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

void dbReadFileBlock ( int f, char* pFilename )
{
	ReadFileBlock ( f, ( DWORD ) pFilename );
}

void dbSkipBytes ( int f, int iSkipValue )
{
	SkipBytes ( f, iSkipValue );
}

void dbReadDirBlock ( int f, char* pFilename )
{
	ReadDirBlock ( f, ( DWORD ) pFilename );
}

void dbWriteByte ( int f, int iValue )
{
	WriteByte ( f, iValue );
}

void dbWriteWord ( int f, int iValue )
{
	WriteWord ( f, iValue );
}

void dbWriteLong ( int f, int iValue )
{
	WriteLong ( f, iValue );
}

void dbWriteFloat ( int f, float fValue )
{
	WriteFloat ( f, fValue );
}

void dbWriteString ( int f, char* pString )
{
	WriteString ( f, ( DWORD ) pString );
}

void dbWriteFileBlock ( int f, char* pFilename )
{
	WriteFileBlock ( f, ( DWORD ) pFilename );
}

void dbWriteFileBlockEx ( int f, char* pFilename, int iFlag )
{
	//WriteFileBlockEx ( f, pFilename, iFlag );
}

void dbWriteDirBlock ( int f, char* pFilename )
{
	WriteDirBlock ( f, ( DWORD ) pFilename );
}

void dbReadMemblock ( int f, int mbi )
{
	ReadMemblock ( f, mbi );
}

void dbMakeMemblockFromFile	( int mbi, int f )
{
	MakeMemblockFromFile ( mbi, f );
}

void dbWriteMemblock ( int f, int mbi )
{
	WriteMemblock ( f, mbi );
}

void dbMakeFileFromMemblock	( int f, int mbi )
{
	MakeFileFromMemblock ( f, mbi );
}

char* dbGetDir ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetDir ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbGetFileName ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetFileName ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbGetFileType ( void )
{
	return GetFileType ( );
}

char* dbGetFileDate ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetFileDate ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbGetFileCreation ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = GetFileCreation ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbFileExist ( char* pFilename )
{
	return FileExist ( ( DWORD ) pFilename );
}

int dbFileSize ( char* pFilename )
{
	return FileSize ( ( DWORD ) pFilename );
}

int dbPathExist ( char* pFilename )
{
	return PathExist ( ( DWORD ) pFilename );
}

int dbFileOpen ( int f )
{
	return FileOpen ( f );
}

int dbFileEnd ( int f )
{
	return FileEnd ( f );
}

char* dbAppname ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Appname ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbWindir ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Windir ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbExecutableRunning ( DWORD hIndiExecuteFileProcess )
{
	return GetExecutableRunning ( hIndiExecuteFileProcess );
}

//
// lee - 300706 - GDK fixes
//
unsigned char dbReadByte ( int f, unsigned char* pByte )
{
	*pByte = dbReadByte ( f );
	return *pByte;
}
WORD dbReadWord	( int f, WORD* pWord )
{
	*pWord = dbReadWord ( f );
	return *pWord;
}
int	dbReadFile ( int f, int* pInteger )
{
	*pInteger = dbReadFile ( f );
	return *pInteger;
}
float dbReadFloat ( int f, float* pFloat )
{
	*pFloat = dbReadFloat ( f );
	return *pFloat;
}
char* dbReadString ( int f, char* pString )
{
	pString = dbReadString ( f );
	return pString;
}
int	dbReadLong ( int f, int* pLong )
{
	*pLong = ReadLong ( f );
	return *pLong;
}
void dbCD ( char* pPath )
{
	SetDir ( (DWORD)pPath );
}
void dbWriteFile ( int f, int iValue )
{
	WriteLong ( f, iValue );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////