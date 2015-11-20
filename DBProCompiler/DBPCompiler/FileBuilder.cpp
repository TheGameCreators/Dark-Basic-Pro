//
// FileBuilder.cpp: implementation of the CFileBuilder class.
//

// Includes
#include "windows.h"
#include "resource.h"
#include "FileBuilder.h"
#include "Error.h"
#include "macros.h"
#include "wingdi.h"
#include "Encryptor.h"

// 'C' Includes
extern "C"
{
	#include "icons\dib.h"
	#include "icons\icons.h"
}

// External Class Pointer
extern CError* g_pErrorReport;

// External Data
extern char gUnpackDirectory[_MAX_PATH];
extern bool g_bLocalTempFolder;

// Implementations
CFileBuilder::CFileBuilder()
{
	// EXEMaker
	m_hfile = NULL;
	m_SizeOfEXECode = 0;

	// File Table
	m_dwFileTableSize=0;
	m_pFileTable=NULL;
	m_pFileTablePlacement=NULL;
}

CFileBuilder::~CFileBuilder()
{
	DeleteFileTable();
}

void CFileBuilder::DeleteFileTable(void)
{
	if(m_pFileTable)
	{
		for(DWORD f=0; f<m_dwFileTableSize; f++)
		{
			SAFE_DELETE(m_pFileTablePlacement[f]);
			m_pFileTablePlacement[f]=NULL;
			SAFE_DELETE(m_pFileTable[f]);
			m_pFileTable[f]=NULL;
		}
	}
	SAFE_DELETE(m_pFileTablePlacement);
	SAFE_DELETE(m_pFileTable);
}

bool CFileBuilder::NewFileTable(void)
{
	// Delete odl filetable
	DeleteFileTable();

	// New File Table
	m_dwFileTableSize=10;
	m_pFileTable = new LPSTR[m_dwFileTableSize];
	ZeroMemory(m_pFileTable, sizeof(LPSTR)*m_dwFileTableSize);
	m_pFileTablePlacement = new LPSTR[m_dwFileTableSize];
	ZeroMemory(m_pFileTablePlacement, sizeof(LPSTR)*m_dwFileTableSize);
	m_dwFileTableIndex=0;

	// Complete
	return true;
}

bool CFileBuilder::AddFile(LPSTR pFilename, LPSTR pPlacementFolder)
{
	// Increase dize of file table if too small
	if(m_dwFileTableIndex>m_dwFileTableSize-2)
	{
		// New Table Array
		DWORD dwSize = m_dwFileTableSize*2;
		LPSTR* pNewTable = new LPSTR[dwSize];
		ZeroMemory(pNewTable, dwSize*sizeof(LPSTR));
		LPSTR* pNewTablePlacement = new LPSTR[dwSize];
		ZeroMemory(pNewTablePlacement, dwSize*sizeof(LPSTR));

		// Copy what have so far
		memcpy(pNewTable, m_pFileTable, sizeof(LPSTR)*m_dwFileTableSize);
		memcpy(pNewTablePlacement, m_pFileTablePlacement, sizeof(LPSTR)*m_dwFileTableSize);

		// Delete old
		SAFE_DELETE(m_pFileTable);
		SAFE_DELETE(m_pFileTablePlacement);

		// New Ptrs
		m_dwFileTableSize=dwSize;
		m_pFileTable=pNewTable;
		m_pFileTablePlacement=pNewTablePlacement;
	}

	// Add filename to table
	DWORD dwFilenameLength = strlen(pFilename);
	m_pFileTable[m_dwFileTableIndex] = new char[dwFilenameLength+1];
	strcpy(m_pFileTable[m_dwFileTableIndex], pFilename);

	// Add placement path to table
	dwFilenameLength = strlen(pPlacementFolder);
	m_pFileTablePlacement[m_dwFileTableIndex] = new char[dwFilenameLength+1];
	strcpy(m_pFileTablePlacement[m_dwFileTableIndex], pPlacementFolder);

	// Advance count
	m_dwFileTableIndex++;

	// Complete
	return true;
}

bool CFileBuilder::AddWildcardFiles(LPSTR pMediaRoot, LPSTR pMediaWidlcardFile)
{
	// Filename contains a wildcard, so add multiple files

	// Store current dir
	char pStoreBeforeWildScan[_MAX_PATH];
	getcwd(pStoreBeforeWildScan, _MAX_PATH);

	// Source media folder
	char pSrcFolder[_MAX_PATH];
	strcpy(pSrcFolder, pMediaRoot);
	strcat(pSrcFolder, pMediaWidlcardFile);
	for(DWORD e=strlen(pSrcFolder); e>0; e--)
		if(pSrcFolder[e]=='\\' || pSrcFolder[e]=='/')
			{ pSrcFolder[e+1]=0; break; }

	// Destination media folder
	char pDestFolder[_MAX_PATH];
	strcpy(pDestFolder, pMediaWidlcardFile);
	for(DWORD d=strlen(pDestFolder); d>0; d--)
		if(pDestFolder[d]=='\\' || pDestFolder[d]=='/')
			{ pDestFolder[d+1]=0; break; }

	// Wildcard Filename Only
	char pWildcardOnly[_MAX_PATH];
	strcpy(pWildcardOnly, pMediaWidlcardFile+strlen(pDestFolder));

	// Jump to source of media files
	chdir(pSrcFolder);

	// Find specific file(s)
	_finddata_t filedata;
	long hFile = _findfirst(pWildcardOnly, &filedata);
	if(hFile!=-1L)
	{
		long endyet=0;
		while(endyet==0)
		{
			if( filedata.attrib & _A_SUBDIR )
			{
				// Nest Dir
				if(strcmp(filedata.name,".")!=NULL && strcmp(filedata.name, "..")!=NULL)
				{
					char pNestMediaDir[_MAX_PATH];
					strcpy(pNestMediaDir, pDestFolder);
					strcat(pNestMediaDir, filedata.name);
					strcat(pNestMediaDir, "\\");
					strcat(pNestMediaDir, pWildcardOnly);
					AddWildcardFiles(pMediaRoot, pNestMediaDir);
				}
			}
			else
			{
				// Do File
				char pMediaPath[_MAX_PATH];
				char pAbsPathToMedia[_MAX_PATH];
				strcpy(pMediaPath, "media\\");
				strcat(pMediaPath, pDestFolder);
				strcat(pMediaPath, filedata.name);
				strcpy(pAbsPathToMedia, pSrcFolder);
				strcat(pAbsPathToMedia, filedata.name);
				AddFile(pAbsPathToMedia, pMediaPath);
			}
			endyet = _findnext(hFile, &filedata);
		}
	}

	// Close file search
	if(hFile)
	{
		_findclose(hFile);
		hFile=NULL;
	}

	// Restore old directory
	chdir(pStoreBeforeWildScan);

	// Complete
	return true;
}

typedef int ( *COMPRESSFUNC ) ( DWORD*, int );
bool CFileBuilder::MakeEXE(LPSTR destEXEfilename, bool bEncryptionState, LPSTR pCompressDLL)
{
	// Must create EXE

	// Store Encryption Key
	m_bEncryptionState = bEncryptionState;

	// Calculate name of PCK File
	char destPCKfilename[_MAX_PATH];
	GetPCKFileFromEXEFile(destPCKfilename, destEXEfilename);

	// EXE-Alone (now a seperate PCK file)
	ConstructEXE(destEXEfilename);

	// Start PCK File Creation
	ConstructPCK(destPCKfilename);

	// Go Through Files in Table
	float pBit = 30.0f/m_dwFileTableIndex;
	for(DWORD f=0; f<m_dwFileTableIndex; f++)
	{
		AddFileToConstruct(m_pFileTable[f], m_pFileTablePlacement[f]);
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc((DWORD)(10+(pBit*f))));
	}

	// Complete PCK File Creation
	FinishPCK();

	// Progress Reporting Tool
	g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(45));

	// Make a CompressedPCK File if flagged
	if(pCompressDLL)
	{
		// Load PCK Data 
		HANDLE hreadfile = CreateFile(destPCKfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hreadfile==INVALID_HANDLE_VALUE)
		{
			// Report failure
			g_pErrorReport->AddErrorString("Failed to 'MakeEXE' : CompressDLL Create Read File Failed");
			return false;
		}

		// Read readout file into memory
		DWORD bytesread=0;
		DWORD filebuffersize = GetFileSize(hreadfile, NULL);	
		LPSTR filebuffer = (LPSTR)GlobalAlloc(GMEM_FIXED, filebuffersize);
		ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
		CloseHandle(hreadfile);

		// Dynamically load compress.dll and use it to compress
		HMODULE hModule = LoadLibrary(pCompressDLL);
		COMPRESSFUNC CompressBlock = ( COMPRESSFUNC ) GetProcAddress ( hModule, "compress_block" );

		// Compress PCK Data
		LPSTR pData=NULL;
		DWORD dwDataSize=0;
		int iReturnInt =  CompressBlock((DWORD*)filebuffer, (int)filebuffersize);
		if(iReturnInt==-1)
		{
			// Error while compressing!
			SAFE_FREE(filebuffer);
			g_pErrorReport->AddErrorString("Failed to 'MakeEXE' : Error while compressing!");
			return false;
		}
		if(iReturnInt==-2)
		{
			// Compressed data is larger than uncompressed data!
			SAFE_FREE(filebuffer);
			g_pErrorReport->AddErrorString("Failed to 'MakeEXE' : Compressed data is larger than uncompressed data!");
			return false;
		}

		// Get The actual data
		HANDLE hGlobAlloc = (HANDLE)iReturnInt;
		pData = (LPSTR)GlobalLock(hGlobAlloc);
		dwDataSize = GlobalSize(hGlobAlloc);

		// Free old data
		SAFE_FREE(filebuffer);

		// Create new PCK File (CompressDLL + PCKData)
		ConstructPCK(destPCKfilename);
		AddFileToConstruct(pCompressDLL, "compress.dll");
		AddDataToConstruct(pData, dwDataSize);
		FinishPCK();

		// Free compressed data too
		if(pData)
		{
			GlobalUnlock(pData);
			pData=NULL;
		}

		// Free compression DLL
		if(hModule)
		{
			FreeLibrary(hModule);
			hModule=NULL;
		}
	}

	// Complete
	return true;
}

bool CFileBuilder::ConstructEXE(LPSTR EXEfilename)
{
	// Create File and place EXE Runner Code
	DeleteFile(EXEfilename);
	m_hfile = CreateFile(EXEfilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hfile==INVALID_HANDLE_VALUE)
	{
		char err[256];
		wsprintf(err, "Could not create %s", EXEfilename);
		g_pErrorReport->AddErrorString(err);
		return false;
	}

	// Use FULL or DEMO Core for demo (no silly code this time)
	WORD wCoreCode = IDR_CORE;

	// lee - 050406 - u6rc6 - If LOCAL EXE required, use the EXELOCAL version
	if ( g_bLocalTempFolder ) wCoreCode = IDR_X1;

	// Get EXE Runner Code
	m_SizeOfEXECode = SizeofResource(NULL, FindResource(NULL, MAKEINTRESOURCE(wCoreCode), "X"));
	HGLOBAL hGlobal = LoadResource(NULL, FindResource(NULL, MAKEINTRESOURCE(wCoreCode), "X"));
	LPVOID lpResDataBuffer = LockResource(hGlobal);
	if(m_SizeOfEXECode<=0)
	{
		g_pErrorReport->AddErrorString("Failed to 'CFileBuilder::ConstructEXE::LockResource'");
		return false;
	}

	// Write EXE Code first to launch core executable
	DWORD byteswritten;
	WriteFile(m_hfile, lpResDataBuffer, m_SizeOfEXECode, &byteswritten, NULL); 

	// EXE is complete
	CloseHandle(m_hfile);
	return true;
}

bool CFileBuilder::AddDataToConstruct(LPSTR filebuffer, DWORD filebuffersize)
{
	// Add single datablock to construction
	if(m_hfile)
	{
		DWORD byteswritten=0;
		WriteFile(m_hfile, filebuffer, filebuffersize, &byteswritten, NULL); 
	}
	else
	{
		// Soft fail
		return false;
	}
	return true;
}

bool CFileBuilder::AddFileToConstruct(LPSTR FilenameString, LPSTR pPlacement)
{
	DWORD bytesread;
	DWORD byteswritten;
	DWORD FilenameLength = strlen(FilenameString) + 1;

	// Add single file to construction
	if(m_hfile)
	{
		// Open file to be added
		HANDLE hreadfile = CreateFile(FilenameString, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hreadfile==INVALID_HANDLE_VALUE)
		{
			char err[256];
			wsprintf(err, "Cannot find %s", FilenameString);
			g_pErrorReport->AddErrorString(err);
			return false;
		}

		// Read readout file into memory
		DWORD filebuffersize = GetFileSize(hreadfile, NULL);	
		LPSTR filebuffer = (char*)GlobalAlloc(GMEM_FIXED, filebuffersize);
		ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
		CloseHandle(hreadfile);

		// Header: [FILENAME LENGTH] and [FILENAME STRING]
		DWORD dwPlacementFilename = strlen(pPlacement);
		WriteFile(m_hfile, &dwPlacementFilename, 4, &byteswritten, NULL); 
		WriteFile(m_hfile, pPlacement, dwPlacementFilename, &byteswritten, NULL); 

		// Encrypt File (only if PCK is to be added to EXE (if exe alone key will be blanked))
		// And Only Encrypt Media Files (not internal root files)
		if(strnicmp(pPlacement, "media\\", 6)==NULL)
		{
			DWORD dwVal = 0;
			if(m_bEncryptionState==true) dwVal = 12321;
			CEncryptor pEncryptor(dwVal);
			pEncryptor.EncryptFileData(filebuffer, filebuffersize, true);
		}

		// Header: [FILEDATA LENGTH] and [FILEDATA]
		WriteFile(m_hfile, &filebuffersize, 4, &byteswritten, NULL); 
		WriteFile(m_hfile, filebuffer, filebuffersize, &byteswritten, NULL); 

		// Clear mem usage
		if(filebuffer)
		{
			GlobalFree(filebuffer);
			filebuffer=NULL;
		}
	}
	else
	{
		// Soft fail
		return false;
	}

	return true;
}

bool CFileBuilder::ConstructPCK(LPSTR PCKfilename)
{
	// Create File
	DeleteFile(PCKfilename);
	m_hfile = CreateFile(PCKfilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hfile==INVALID_HANDLE_VALUE)
	{
		g_pErrorReport->AddErrorString("Failed to 'CFileBuilder::ConstructPCK'");
		return false;
	}

	// Leave file Open..for PCK files
	return true;
}

bool CFileBuilder::FinishPCK(void)
{
	// Close File
	CloseHandle(m_hfile);
	m_hfile = NULL;
	return true;
}

struct newBITMAPINFO
{
    BITMAPINFOHEADER	 bmiHeader; 
    RGBQUAD		       bmiColors[256]; 
}; 

bool CFileBuilder::ReplaceVersionInfoBlockInEXE(LPSTR pFilenameEXE, LPSTR pVersioBlock, DWORD dwOffsetToFirstEntry, DWORD dwVersionBlockSize)
{
	// Simply scans the EXE and locates the Version Block, and directly replaces it
	LPSTR pEXEData = NULL;
	DWORD dwSizeOfEXECode = 0;	
	HANDLE hreadfile = CreateFile(pFilenameEXE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// Read EXE into memory
		DWORD bytesread=0;
		dwSizeOfEXECode = GetFileSize(hreadfile, NULL);	
		pEXEData = new char[dwSizeOfEXECode];
		ReadFile(hreadfile, pEXEData, dwSizeOfEXECode, &bytesread, NULL); 
		CloseHandle(hreadfile);

		// Modify this data
		LPSTR pPtr = pEXEData;
		LPSTR pPtrEnd = pPtr + dwSizeOfEXECode;
		while (pPtr<pPtrEnd)
		{
			// find a match with the first X bytes
			if ( pPtr+dwOffsetToFirstEntry<pPtrEnd )
			{
				// check byteblock
				bool bOkaySoFar=true;
				LPSTR pCheckByte=pPtr;
				for ( DWORD n=0; n<dwOffsetToFirstEntry; n++ )
				{
					if ( *pCheckByte != pVersioBlock[n] )
					{
						bOkaySoFar=false;
						break;
					}
					pCheckByte++;
				}

				// if it matches versionblock
				if ( bOkaySoFar==true )
				{
					// replace whole version block
					memcpy ( pPtr, pVersioBlock, dwVersionBlockSize );
					pPtr=pPtrEnd+1;
					break;
				}
			}

			// next byte
			pPtr++;
		}

		// Write EXE back out
		HANDLE hwritefile = CreateFile(pFilenameEXE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hwritefile!=INVALID_HANDLE_VALUE)
		{
			DWORD byteswritten=0;
			WriteFile(hwritefile, pEXEData, dwSizeOfEXECode, &byteswritten, NULL); 
			CloseHandle(hwritefile);
		}

		// Free usages
		SAFE_DELETE(pEXEData);
	}

	// complete
	return true;
}

/* U59 FileBuilder Code
bool CFileBuilder::ChangeEXE(LPSTR pFilenameEXE, LPSTR pPathToPluginFolderForBuilder)
{
	// File 0-9 is version string info
	LPSTR pVerComments = m_pFileTable[0];
	LPSTR pVerCompany = m_pFileTable[1];
	LPSTR pVerFileDesc = m_pFileTable[2];
	LPSTR pVerFileNumber = m_pFileTable[3];
	LPSTR pVerInternal = m_pFileTable[4];
	LPSTR pVerCopyright = m_pFileTable[5];
	LPSTR pVerTrademark = m_pFileTable[6];
	LPSTR pVerFilename = m_pFileTable[7];
	LPSTR pVerProduct = m_pFileTable[8];
	LPSTR pVerProductNumber = m_pFileTable[9];

	// Files are 32x32 and 16x16 Icons
	LPSTR pLargeIcon = m_pFileTable[10];
	LPSTR pSmallIcon = m_pFileTable[11];
	LPSTR pLarge256Icon = m_pFileTable[12];

	// Absolute Path for Modulename
	char ModuleName[256];
	if(pFilenameEXE[1]==':')
	{
		// Filename is absolute
		strcpy(ModuleName, pFilenameEXE);
	}
	else
	{
		// File is relative
		getcwd(ModuleName, 256);
		strcat(ModuleName, "\\");
		strcat(ModuleName, pFilenameEXE);
	}

	// Change VERSION INFORMATION
	if(pVerComments)
	{
		// Access Resource from EXE
		HMODULE hEXE = LoadLibraryEx(ModuleName, NULL, LOAD_LIBRARY_AS_DATAFILE);
		HRSRC hRes=FindResource(hEXE, (LPCTSTR)1, RT_VERSION);
		DWORD dwDataSize = SizeofResource(hEXE, hRes);
		HGLOBAL hGlobal = LoadResource(hEXE, hRes);
		LPVOID lpResReal = LockResource(hGlobal);

		// Get Version Data in UNICODE
		LPSTR pVersonData = new char[dwDataSize];
		memcpy(pVersonData, (LPSTR)lpResReal, dwDataSize);

		// Construct WideCharacter
		int iIndex=0;
		LPSTR pPtr = pVersonData;
		LPSTR pVersonDataEnd=pVersonData+dwDataSize;
		DWORD dwOffsetToFirstEntry=0;
		while(pPtr<pVersonDataEnd)
		{
			// Get Src
			DWORD dwLength = 0;
			LPSTR pCharStr = NULL;
			if(iIndex==0) { dwLength=32; pCharStr=pVerComments; }
			if(iIndex==1) { dwLength=32; pCharStr=pVerCompany; }
			if(iIndex==2) { dwLength=32; pCharStr=pVerFileDesc; }
			if(iIndex==3) { dwLength=10; pCharStr=pVerFileNumber; }
			if(iIndex==4) { dwLength=32; pCharStr=pVerInternal; }
			if(iIndex==5) { dwLength=32; pCharStr=pVerCopyright; }
			if(iIndex==6) { dwLength=32; pCharStr=pVerTrademark; }
			if(iIndex==7) { dwLength=32; pCharStr=pVerFilename; }
			if(iIndex==8) { dwLength=32; pCharStr=pVerProduct; }
			if(iIndex==9) { dwLength=10; pCharStr=pVerProductNumber; }

			// Build match string
			char pMatchStr[100];
			int c=0;
			for(DWORD b=0; b<dwLength; b++)
			{
				int d=iIndex+1;
				if(iIndex==9) d=0;
				pMatchStr[c++]=48+d;
				pMatchStr[c++]=0;
			}
			pMatchStr[c++]=0;

			// Is there a match?
			bool bMatch=true;
			for(DWORD e=0; e<dwLength*2; e++)
			{
				if(pPtr+e>=pVersonDataEnd)
				{
					bMatch=false;
					break;
				}
				if(pPtr[e]!=pMatchStr[e])
					bMatch=false;
			}
				
			// Write direct to version data
			if(bMatch==true)
			{
				if(iIndex==0) dwOffsetToFirstEntry=pPtr-pVersonData;
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pCharStr, -1, (LPWSTR)pPtr, dwLength*2);
				iIndex++;
			}


			// Next char in version data
			pPtr++;
		}

		// Finished with EXE Access
		FreeLibrary(hEXE);

		// Works for all Operating Systems - replace VersionBlock
		ReplaceVersionInfoBlockInEXE ( ModuleName, pVersonData, dwOffsetToFirstEntry, dwDataSize );

		// Free usages
		SAFE_DELETE(pVersonData);
	}

	// Progress Reporting Tool
	g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(70));

	// Change ICONS (Small, Large and Large256)
	for(short icon=1; icon<=0; icon++)
	{
		// Get Icon Rsource Size From Existing Resource
		HMODULE hEXE = LoadLibrary(ModuleName);
		LPCTSTR pResName = 0;

		// Determined by icon order in EXE
		if(icon==1) pResName=(LPCTSTR)2;
		if(icon==2) pResName=(LPCTSTR)3;
		if(icon==3) pResName=(LPCTSTR)1;

		HRSRC hRes=FindResource(hEXE, pResName, RT_ICON);
		DWORD dwDataSize = SizeofResource(hEXE, hRes);
		HGLOBAL hGlobal = LoadResource(hEXE, hRes);
		LPVOID lpResReal = LockResource(hGlobal);

		// Copy Icon Image Only
		LPSTR pIconMem = new char[dwDataSize];
		memcpy(pIconMem, (LPSTR)lpResReal, dwDataSize);

		// Finished with EXE-Reading
		FreeLibrary(hEXE);

		// Get Filename
		ICONINFO IconInfo;
		HBITMAP hCol = NULL;
		HBITMAP hMask = NULL;
		HANDLE hImage = NULL;
		LPSTR pFilename = NULL;
		if(icon==1) pFilename = pLargeIcon;
		if(icon==2) pFilename = pSmallIcon;
		if(icon==3) pFilename = pLarge256Icon;
		DWORD dwLength = strlen(pFilename);
		if(strnicmp(pFilename+dwLength-4, ".bmp", 4)==NULL)
		{
			// Make an ICO file from BMP
			char pWorkIcon[_MAX_PATH];
			strcpy(pWorkIcon, pPathToPluginFolderForBuilder);
			MakeICOFromBMP(pFilename, pWorkIcon);
			strcat(pWorkIcon, "workicon.ico");
			if(icon==1) hImage = LoadImage( NULL, pWorkIcon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if(icon==2) hImage = LoadImage( NULL, pWorkIcon, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if(icon==3) hImage = LoadImage( NULL, pWorkIcon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		}
		else
		{
			// Load Icon Image File
			if(icon==1) hImage = LoadImage( NULL, pLargeIcon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if(icon==2) hImage = LoadImage( NULL, pSmallIcon, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
			if(icon==3) hImage = LoadImage( NULL, pLarge256Icon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		}
		GetIconInfo((HICON)hImage, &IconInfo);
		hCol = IconInfo.hbmColor;
		hMask = IconInfo.hbmMask;

		// Create bitmap info header
		newBITMAPINFO BitmapInfo;
		ZeroMemory(&BitmapInfo, sizeof(newBITMAPINFO));
		BitmapInfo.bmiHeader.biSize=sizeof(newBITMAPINFO);
		if(icon==1 || icon==3)
		{
			BitmapInfo.bmiHeader.biWidth=32;
			BitmapInfo.bmiHeader.biHeight=32;
		}
		else
		{
			BitmapInfo.bmiHeader.biWidth=16;
			BitmapInfo.bmiHeader.biHeight=16;
		}
		BitmapInfo.bmiHeader.biPlanes=1;
		if(icon==3)
			BitmapInfo.bmiHeader.biBitCount=8;
		else
			BitmapInfo.bmiHeader.biBitCount=4;

		float fPerByte=(BitmapInfo.bmiHeader.biBitCount/8.0f);
		DWORD dwBitsSize=(DWORD)((BitmapInfo.bmiHeader.biWidth*BitmapInfo.bmiHeader.biHeight)*fPerByte);

		HDC hdc = CreateCompatibleDC(NULL);
		LPSTR pColArray = new char[dwBitsSize];
		LPSTR pMaskArray = new char[dwBitsSize];
		if(icon==1) GetDIBits(hdc, hCol, 0, 32, pColArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		if(icon==2) GetDIBits(hdc, hCol, 0, 16, pColArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		if(icon==3) GetDIBits(hdc, hCol, 0, 32, pColArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		BitmapInfo.bmiHeader.biBitCount=1;
		if(hMask)
		{
			if(icon==1) GetDIBits(hdc, hMask, 0, 32, pMaskArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
			if(icon==2) GetDIBits(hdc, hMask, 0, 16, pMaskArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
			if(icon==3) GetDIBits(hdc, hMask, 0, 32, pMaskArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		}
		else
		{
			// Default Mask of 1's
			memset(pMaskArray, 1, dwBitsSize);
		}

		// Fill Icon Image area with load icon image
		DWORD ColOffset=104;
		if(icon==3) ColOffset=1064;
		memcpy(pIconMem+ColOffset, pColArray, dwBitsSize);
		DWORD MaskOffset=ColOffset+dwBitsSize;
		if(icon==1) memcpy(pIconMem+MaskOffset, pMaskArray, dwBitsSize/4);
		if(icon==2) memcpy(pIconMem+MaskOffset, pMaskArray, dwBitsSize/2);
		if(icon==3) memcpy(pIconMem+MaskOffset, pMaskArray, dwBitsSize/8);

		// Open EXE for editing
		HANDLE hUpdateRes = BeginUpdateResource( ModuleName, FALSE);

		// Add Large Icon to Executable
		UpdateResource(	hUpdateRes,
						RT_ICON,
						pResName,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
						pIconMem, dwDataSize);

		// Free Icon GDI refs
		DeleteObject(hCol);
		DeleteObject(hMask);
		DestroyIcon((HICON)hImage);

		// Free usages
		SAFE_DELETE(pMaskArray);
		SAFE_DELETE(pColArray);
		SAFE_DELETE(pIconMem);
		DeleteDC(hdc);

		// Close EXE
		EndUpdateResource(hUpdateRes, FALSE);

		// Progress Reporting Tool
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(80+((icon-1)*3)));
	}

	// Complete
	return true;
}
*/

bool CFileBuilder::ChangeEXE(LPSTR pFilenameEXE, LPSTR pPathToPluginFolderForBuilder)
{
	// File 0-9 is version string info
	LPSTR pVerComments = m_pFileTable[0];
	LPSTR pVerCompany = m_pFileTable[1];
	LPSTR pVerFileDesc = m_pFileTable[2];
	LPSTR pVerFileNumber = m_pFileTable[3];
	LPSTR pVerInternal = m_pFileTable[4];
	LPSTR pVerCopyright = m_pFileTable[5];
	LPSTR pVerTrademark = m_pFileTable[6];
	LPSTR pVerFilename = m_pFileTable[7];
	LPSTR pVerProduct = m_pFileTable[8];
	LPSTR pVerProductNumber = m_pFileTable[9];

	// Files are 32x32 and 16x16 Icons
	LPSTR pLargeIcon = m_pFileTable[10];
	LPSTR pSmallIcon = m_pFileTable[11];
	LPSTR pLarge256Icon = m_pFileTable[12];

	// Absolute Path for Modulename
	char ModuleName[256];
	if(pFilenameEXE[1]==':')
	{
		// Filename is absolute
		strcpy(ModuleName, pFilenameEXE);
	}
	else
	{
		// File is relative
		getcwd(ModuleName, 256);
		strcat(ModuleName, "\\");
		strcat(ModuleName, pFilenameEXE);
	}

	// Change VERSION INFORMATION
	if(pVerComments)
	{
		// Access Resource from EXE
		HMODULE hEXE = LoadLibraryEx(ModuleName, NULL, LOAD_LIBRARY_AS_DATAFILE);
		HRSRC hRes=FindResource(hEXE, (LPCTSTR)1, RT_VERSION);
		DWORD dwDataSize = SizeofResource(hEXE, hRes);
		HGLOBAL hGlobal = LoadResource(hEXE, hRes);
		LPVOID lpResReal = LockResource(hGlobal);

		// Get Version Data in UNICODE
		LPSTR pVersonData = new char[dwDataSize];
		memcpy(pVersonData, (LPSTR)lpResReal, dwDataSize);

		// Construct WideCharacter
		int iIndex=0;
		LPSTR pPtr = pVersonData;
		LPSTR pVersonDataEnd=pVersonData+dwDataSize;
		DWORD dwOffsetToFirstEntry=0;
		while(pPtr<pVersonDataEnd)
		{
			// Get Src
			DWORD dwLength = 0;
			LPSTR pCharStr = NULL;
			if(iIndex==0) { dwLength=32; pCharStr=pVerComments; }
			if(iIndex==1) { dwLength=32; pCharStr=pVerCompany; }
			if(iIndex==2) { dwLength=32; pCharStr=pVerFileDesc; }
			if(iIndex==3) { dwLength=10; pCharStr=pVerFileNumber; }
			if(iIndex==4) { dwLength=32; pCharStr=pVerInternal; }
			if(iIndex==5) { dwLength=32; pCharStr=pVerCopyright; }
			if(iIndex==6) { dwLength=32; pCharStr=pVerTrademark; }
			if(iIndex==7) { dwLength=32; pCharStr=pVerFilename; }
			if(iIndex==8) { dwLength=32; pCharStr=pVerProduct; }
			if(iIndex==9) { dwLength=10; pCharStr=pVerProductNumber; }

			// Build match string
			char pMatchStr[100];
			int c=0;
			for(DWORD b=0; b<dwLength; b++)
			{
				int d=iIndex+1;
				if(iIndex==9) d=0;
				pMatchStr[c++]=48+d;
				pMatchStr[c++]=0;
			}
			pMatchStr[c++]=0;

			// Is there a match?
			bool bMatch=true;
			for(DWORD e=0; e<dwLength*2; e++)
			{
				if(pPtr+e>=pVersonDataEnd)
				{
					bMatch=false;
					break;
				}
				if(pPtr[e]!=pMatchStr[e])
					bMatch=false;
			}
				
			// Write direct to version data
			if(bMatch==true)
			{
				if(iIndex==0) dwOffsetToFirstEntry=pPtr-pVersonData;
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pCharStr, -1, (LPWSTR)pPtr, dwLength*2);
				iIndex++;
			}


			// Next char in version data
			pPtr++;
		}

		// Finished with EXE Access
		FreeLibrary(hEXE);

		// Works for all Operating Systems - replace VersionBlock
		ReplaceVersionInfoBlockInEXE ( ModuleName, pVersonData, dwOffsetToFirstEntry, dwDataSize );

		// Free usages
		SAFE_DELETE(pVersonData);
	}

	// Progress Reporting Tool
	g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(70));

	// Change ICONS (256 32x32 and 16x16)
	for(short icon=1; icon<=2; icon++)
	{
		// Get Icon Rsource Size From Existing Resource
		HMODULE hEXE = LoadLibrary(ModuleName);
		LPCTSTR pResName = 0;

		// Determined by icon order in EXE
		if(icon==1) pResName=(LPCTSTR)1;
		if(icon==2) pResName=(LPCTSTR)2;

		// Access resource
		HRSRC hRes=FindResource(hEXE, pResName, RT_ICON);
		DWORD dwDataSize = SizeofResource(hEXE, hRes);
		HGLOBAL hGlobal = LoadResource(hEXE, hRes);
		LPVOID lpResReal = LockResource(hGlobal);

		// lee - 200206 - u60 - copy bitmap header into resource ONLY if resource is valid
		newBITMAPINFO BitmapInfo;
		if ( sizeof(BitmapInfo.bmiHeader)+sizeof(BitmapInfo.bmiColors) > dwDataSize )
		{
			// resource is a Win98 small return
			// free and next icon
			FreeLibrary(hEXE);
			continue;
		}

		// Copy Icon Image Only
		LPSTR pIconMem = new char[dwDataSize];
		memcpy(pIconMem, (LPSTR)lpResReal, dwDataSize);

		// Finished with EXE-Reading
		FreeLibrary(hEXE);

		// Get Filename
		ICONINFO IconInfo;
		HBITMAP hCol = NULL;
		HBITMAP hMask = NULL;
		HANDLE hImage = NULL;
		LPSTR pFilename = NULL;
		if(icon==1) pFilename = pLargeIcon;
		if(icon==2) pFilename = pSmallIcon;
		DWORD dwLength = strlen(pFilename);
		if(strnicmp(pFilename+dwLength-4, ".bmp", 4)==NULL)
		{
			// Make an ICO file from BMP
			char pWorkIcon[_MAX_PATH];
			strcpy(pWorkIcon, pPathToPluginFolderForBuilder);
			MakeICOFromBMP(pFilename, pWorkIcon);
			strcat(pWorkIcon, "workicon.ico");
			if(icon==1) hImage = LoadImage( NULL, pWorkIcon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if(icon==2) hImage = LoadImage( NULL, pWorkIcon, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		}
		else
		{
			// Load Icon Image File
			if(icon==1) hImage = LoadImage( NULL, pLargeIcon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
			if(icon==2) hImage = LoadImage( NULL, pSmallIcon, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		}
		GetIconInfo((HICON)hImage, &IconInfo);
		hCol = IconInfo.hbmColor;
		hMask = IconInfo.hbmMask;

		// Create bitmap info header
		ZeroMemory(&BitmapInfo, sizeof(newBITMAPINFO));
		BitmapInfo.bmiHeader.biSize=sizeof(newBITMAPINFO);
		if(icon==1)
		{
			BitmapInfo.bmiHeader.biWidth=32;
			BitmapInfo.bmiHeader.biHeight=32;
		}
		else
		{
			BitmapInfo.bmiHeader.biWidth=16;
			BitmapInfo.bmiHeader.biHeight=16;
		}
		BitmapInfo.bmiHeader.biPlanes=1;
		BitmapInfo.bmiHeader.biBitCount=8;

		float fPerByte=(BitmapInfo.bmiHeader.biBitCount/8.0f);
		DWORD dwBitsSize=(DWORD)((BitmapInfo.bmiHeader.biWidth*BitmapInfo.bmiHeader.biHeight)*fPerByte);

		HDC hdc = CreateCompatibleDC(NULL);
		LPSTR pColArray = new char[dwBitsSize];
		LPSTR pMaskArray = new char[dwBitsSize];
		if(icon==1) GetDIBits(hdc, hCol, 0, 32, pColArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		if(icon==2) GetDIBits(hdc, hCol, 0, 16, pColArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);

		// perform copy to retain palette of icon in new icon memory block
		memcpy(pIconMem+sizeof(BitmapInfo.bmiHeader), ((char*)&BitmapInfo)+sizeof(BitmapInfo.bmiHeader), sizeof(BitmapInfo.bmiColors) );
		BitmapInfo.bmiHeader.biBitCount=1;
		if(hMask)
		{
			if(icon==1) GetDIBits(hdc, hMask, 0, 32, pMaskArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
			if(icon==2) GetDIBits(hdc, hMask, 0, 16, pMaskArray, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
		}
		else
		{
			// Default Mask of 1's
			memset(pMaskArray, 1, dwBitsSize);
		}

		// Fill Icon Image area with load icon image
		DWORD ColOffset=sizeof(BitmapInfo);
		memcpy(pIconMem+ColOffset, pColArray, dwBitsSize);
		DWORD MaskOffset=ColOffset+dwBitsSize;
		if(icon==1) memcpy(pIconMem+MaskOffset, pMaskArray, dwBitsSize/8);

		// lee - 220306 -u6b4 - this seemed to fix it
		if(icon==2) memcpy(pIconMem+MaskOffset, pMaskArray, (dwBitsSize/8)*2);

		// Open EXE for editing (lee - 060406 - u6rc6 - win98/me cannot do this natively)
		HANDLE hUpdateRes = BeginUpdateResource( ModuleName, FALSE);
		if ( hUpdateRes )
		{
			// Add Icon to Executable
			UpdateResource(	hUpdateRes,
							RT_ICON,
							pResName,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
							pIconMem, dwDataSize);

			// Close EXE
			EndUpdateResource(hUpdateRes, FALSE);
		}

		// Free Icon GDI refs
		DeleteObject(hCol);
		DeleteObject(hMask);
		DestroyIcon((HICON)hImage);

		// Free usages
		SAFE_DELETE(pMaskArray);
		SAFE_DELETE(pColArray);
		SAFE_DELETE(pIconMem);
		DeleteDC(hdc);

		// Progress Reporting Tool
		g_pErrorReport->ProgressReport("Linker now at line ",g_pErrorReport->GetPerc(80+((icon-1)*3)));
	}

	// Complete
	return true;
}

bool CFileBuilder::MakeICOFromBMP(LPSTR pBMPFilename, LPSTR pDestICOPath)
{
	// Calc strings
	char pRawIcon[_MAX_PATH];
	char pWorkIcon[_MAX_PATH];
	strcpy(pWorkIcon, pDestICOPath);
	strcat(pWorkIcon, "workicon.ico");
	strcpy(pRawIcon, pDestICOPath);
	strcat(pRawIcon, "rawicon.ico");

	// Load raw icon
	LPICONRESOURCE hIconRes = ReadIconFromICOFile( pRawIcon );
	if(hIconRes)
	{
		// Loop through and copy bitmap to each icon
		for( DWORD i = 0; i < hIconRes->nNumImages; i++ )
		{
			// Load bitmap image
			BOOL bStretchToFit = TRUE;
			if(IconImageFromBMPFile( pBMPFilename, &hIconRes->IconImages[i], bStretchToFit ))
			{
				// Successfully replaced in this sub-icon with BMP
			}

			// Create a MASK using top/left corner of image (as transparency)
			POINT pt = { 0, 0 };
			MakeNewANDMaskBasedOnPoint(&hIconRes->IconImages[i], pt);
		}

		// Write out raw+image as work icon
		if(WriteIconToICOFile( hIconRes, pWorkIcon ))
		{
			// Successfully created icon file
		}
	}

	// Complete
	return true;
}

bool CFileBuilder::SaveIconCursorFileFromInfo(	LPSTR pszFullFileName,
									int iWidth, int iHeight, int iColors,
									int iHotspotX, int iHotspotY,
									LPSTR pImg, DWORD dwImgSize)
{
	// Open the file for writing.
	DWORD byteswritten;
	DeleteFile(pszFullFileName);
	HANDLE hfile = CreateFile(pszFullFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return false;

	// Cursor CUR File
	ICOCURSORHDR CurHeader;
	CurHeader.iReserved=(WORD)0;
	CurHeader.iResourceType=(WORD)2;
	CurHeader.iResourceCount=(WORD)1;

	// Write the header to disk.
	WriteFile(hfile, (LPSTR)&CurHeader, sizeof(ICOCURSORHDR), &byteswritten, NULL); 

	// Write all the descriptors.
	DWORD iBitsOffset;
	ICOCURSORDESC IcoCurDesc;
	iBitsOffset = sizeof(ICOCURSORHDR) + (1 * sizeof(ICOCURSORDESC));
	IcoCurDesc.iWidth = (BYTE)iWidth;
	IcoCurDesc.iHeight = (BYTE)iHeight;
	IcoCurDesc.iColorCount = (BYTE)0;
	IcoCurDesc.iUnused = 0;
	IcoCurDesc.iHotspotX = (WORD)iHotspotX;
	IcoCurDesc.iHotspotY = (WORD)iHotspotY;
	IcoCurDesc.DIBSize = sizeof(BITMAPINFOHEADER) + (2*sizeof(DWORD)) + (dwImgSize*2);
	IcoCurDesc.DIBOffset = iBitsOffset;
	WriteFile(hfile, (LPSTR)&IcoCurDesc, sizeof(ICOCURSORDESC), &byteswritten, NULL); 

	// Write Bitmap Info Header
	BITMAPINFOHEADER pBMHeader;
	memset(&pBMHeader, 0, sizeof(BITMAPINFOHEADER));
	pBMHeader.biSize=sizeof(BITMAPINFOHEADER); 
    pBMHeader.biWidth=iWidth; 
    pBMHeader.biHeight=iHeight * 2; 
    pBMHeader.biPlanes=1; 
    pBMHeader.biBitCount=1;
	WriteFile(hfile, (LPSTR)&pBMHeader, sizeof(BITMAPINFOHEADER), &byteswritten, NULL); 
 
	// RGB Colours
	DWORD dwRGB[2];
	dwRGB[0]=0;
	dwRGB[1]=0x00FFFFFF;
	WriteFile(hfile, (LPSTR)&dwRGB, sizeof(dwRGB), &byteswritten, NULL); 

	// Write XOR Data (and AND data)
	WriteFile(hfile, pImg, dwImgSize*2, &byteswritten, NULL); 

	// Close file
	CloseHandle(hfile);

	// Success
	return TRUE;
}

bool CFileBuilder::MakeCURFromBMP(LPSTR pBMPFilename, LPSTR pDestCURFilename)
{
	// Calc strings
	char pWorkCursor[_MAX_PATH];
	strcpy(pWorkCursor, pDestCURFilename);

	// Load Bitmap
	LPBITMAPINFO pDIB = (LPBITMAPINFO)ReadBMPFile ( pBMPFilename );
	if ( pDIB==NULL)
		return false;

	int iWidth = pDIB->bmiHeader.biWidth;
	int iHeight = pDIB->bmiHeader.biHeight;
	int iBPPColors = pDIB->bmiHeader.biBitCount;
	
	// Create a new bitmap, which includes a mask section
	LPBITMAPINFO pBigDIB = (LPBITMAPINFO)ConvertDIBFormat(pDIB, iWidth, iHeight*2, iBPPColors, TRUE);

	// Copy image into XOR part of big image
	LPBYTE pOrigBits = (LPBYTE)FindDIBBits( (LPSTR)pDIB );
	LPBYTE pDestBits = (LPBYTE)FindDIBBits( (LPSTR)pBigDIB );
	DWORD dwDestSize = pBigDIB->bmiHeader.biHeight * BytesPerLine(&(pBigDIB->bmiHeader)) / 2;
	memset(pDestBits, 0xFF, dwDestSize*2);
	memcpy(pDestBits, pOrigBits, dwDestSize);

	// Convert to monocrome (experiment with 256 colour after lunch)
	LPBITMAPINFO pNewDIB = (LPBITMAPINFO)ConvertDIBFormat ( pBigDIB, 32, 64, 1, TRUE );
 	DWORD dwImgSize = pNewDIB->bmiHeader.biHeight * BytesPerLine(&(pNewDIB->bmiHeader)) / 2;
	LPSTR pImg = FindDIBBits( (LPSTR)pNewDIB );

	// Create a MASK using top/left corner of image (as transparency)
	POINT pt = { 0, 0 };
	ICONIMAGE IconImage;
	IconImage.lpbi = pNewDIB;
	IconImage.lpXOR = (LPBYTE)pImg;
	IconImage.lpAND = (LPBYTE)pImg + dwImgSize;
	MakeNewANDMaskBasedOnPoint(&IconImage, pt);

	// rest of cursor data
	int iHotspotX = 0;
	int iHotSpotY = 0;
	int iColors = 2;//2 or 16

	// Save CUR file using bitmap bits
	if(SaveIconCursorFileFromInfo(	pWorkCursor, 32, 32, iColors, iHotspotX, iHotSpotY, pImg, dwImgSize ))
	{
		// Successfully created CUR file
	}

	// Complete
	return true;
}

bool CFileBuilder::AddPCKToEXE(LPSTR EXEfilename, DWORD KindOfExecutable)
{
	// Get pre-build EXE (with modified icon/etc data)
	LPSTR pEXEData = NULL;
	DWORD dwSizeOfEXECode = 0;	
	HANDLE hreadfile = CreateFile(EXEfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// Read pre-build EXE into memory
		DWORD bytesread=0;
		dwSizeOfEXECode = GetFileSize(hreadfile, NULL);	
		pEXEData = new char[dwSizeOfEXECode];
		ReadFile(hreadfile, pEXEData, dwSizeOfEXECode, &bytesread, NULL); 
		CloseHandle(hreadfile);
	}
	else
	{
		g_pErrorReport->AddErrorString("Failed to 'AddPCKToEXE::ReadEXEBit'");
		return false;
	}

	// Construct PCK File
	char destPCKfilename[_MAX_PATH];
	GetPCKFileFromEXEFile(destPCKfilename, EXEfilename);

	// Get pre-build PCK file
	LPSTR pPCKData = NULL;
	DWORD dwSizeOfPCKData = 0;	
	hreadfile = CreateFile(destPCKfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// Read pre-build EXE into memory
		DWORD bytesread=0;
		dwSizeOfPCKData = GetFileSize(hreadfile, NULL);	
		pPCKData = new char[dwSizeOfPCKData];
		ReadFile(hreadfile, pPCKData, dwSizeOfPCKData, &bytesread, NULL); 
		CloseHandle(hreadfile);
	}
	else
	{
		g_pErrorReport->AddErrorString("Failed to 'AddPCKToEXE::ReadEXEBit'");
		SAFE_DELETE(pEXEData);
		return false;
	}

	// Create File and place EXE Runner Code
	DeleteFile(EXEfilename);
	m_hfile = CreateFile(EXEfilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hfile==INVALID_HANDLE_VALUE)
	{
		g_pErrorReport->AddErrorString("Failed to 'CFileBuilder::AddPCKToEXE'");
		return false;
	}

	// Get Encruption Key so EXE can decrypt later on
	DWORD dwVal = 0;
	if(m_bEncryptionState==true) dwVal = 12321;
	CEncryptor pEncryptor(dwVal);
	DWORD dwUniqueKeyForEncryption=pEncryptor.GetUniqueKey();

	// Write EXE Code first to launch core executable
	DWORD byteswritten;
	WriteFile(m_hfile, pEXEData, dwSizeOfEXECode, &byteswritten, NULL); 

	// Write PCK File Packet Here
	WriteFile(m_hfile, pPCKData, dwSizeOfPCKData, &byteswritten, NULL); 

	// First DWORD of file-block is filename length header, so zero is quit
	DWORD FilenameLength=0;
	WriteFile(m_hfile, &FilenameLength, 4, &byteswritten, NULL); 

	// Last DWORD is length of footer for the readers backtrack referencer
	DWORD ValidityCode=12345678;
	WriteFile(m_hfile, &dwUniqueKeyForEncryption, 4, &byteswritten, NULL); 
	WriteFile(m_hfile, &ValidityCode, 4, &byteswritten, NULL); 
	WriteFile(m_hfile, &KindOfExecutable, 4, &byteswritten, NULL); 
	WriteFile(m_hfile, &dwSizeOfEXECode, 4, &byteswritten, NULL); 

	// Close File
	CloseHandle(m_hfile);
	m_hfile = NULL;

	// Free usages
	SAFE_DELETE(pEXEData);
	SAFE_DELETE(pPCKData);

	// Delete PCK File now redundant
	DeleteFile(destPCKfilename);

	// Complete
	return true;
}

void CFileBuilder::GetPCKFileFromEXEFile(LPSTR destPCKfilename, LPSTR destEXEfilename)
{
	strcpy(destPCKfilename, destEXEfilename);
	DWORD dwLength = strlen(destPCKfilename)-4;
	destPCKfilename[dwLength]=0;
	strcat(destPCKfilename, ".pck");
}

bool CFileBuilder::ReplaceDataBlockInEXE ( LPSTR pFilenameEXE, LPSTR pPattern, LPSTR pDataBlock, DWORD dwBlockSize )
{
	// Simply scans the EXE and locates the pattern in the data, and replaces it
	LPSTR pEXEData = NULL;
	DWORD dwSizeOfEXECode = 0;	
	HANDLE hreadfile = CreateFile(pFilenameEXE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// Read EXE into memory
		DWORD bytesread=0;
		dwSizeOfEXECode = GetFileSize(hreadfile, NULL);	
		pEXEData = new char[dwSizeOfEXECode];
		ReadFile(hreadfile, pEXEData, dwSizeOfEXECode, &bytesread, NULL); 
		CloseHandle(hreadfile);

		// Modify this data
		LPSTR pPtr = pEXEData;
		LPSTR pPtrEnd = pPtr + dwSizeOfEXECode;
		while (pPtr<pPtrEnd)
		{
			// find a match with the pattern
			if ( pPtr<pPtrEnd )
			{
				// check byteblock
				bool bOkaySoFar=true;
				LPSTR pCheckByte=pPtr;
				for ( DWORD n=0; n<dwBlockSize; n++ )
				{
					if ( *pCheckByte != pPattern[n] )
					{
						bOkaySoFar=false;
						break;
					}
					pCheckByte++;
				}

				// if it matches perfectly
				if ( bOkaySoFar==true )
				{
					// replace whole block
					memcpy ( pPtr, pDataBlock, dwBlockSize );
					pPtr=pPtrEnd+1;
					break;
				}
			}

			// next byte if still not found
			pPtr++;
		}

		// Write EXE back out
		HANDLE hwritefile = CreateFile(pFilenameEXE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hwritefile!=INVALID_HANDLE_VALUE)
		{
			DWORD byteswritten=0;
			WriteFile(hwritefile, pEXEData, dwSizeOfEXECode, &byteswritten, NULL); 
			CloseHandle(hwritefile);
		}

		// Free usages
		SAFE_DELETE(pEXEData);
	}

	// complete
	return true;
}
