//
// FileReader.cpp: implementation of the CFileReader class.
//

// Includes
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)
#include "FileReader.h"
#include "direct.h"

// Externals
extern char gUnpackDirectory[_MAX_PATH];
extern DWORD gEncryptionKey;

CFileReader::CFileReader()
{
	m_pDataBlock = NULL;
	m_DataBlockSize = 0;
	m_RefTable = NULL;
}

CFileReader::~CFileReader()
{
	// Allow to delete virtual temp file (delete all files and temp file itself)
	char TempVirtFile[256];
	strcpy(TempVirtFile, gUnpackDirectory);
	strcat(TempVirtFile, "\\_virtual.dat");
	GoodDeleteFile(TempVirtFile);

	// Delete usages
	FreeEXEDataFromMemory();

	// U75 - 051209 - better way to delete reftable (nested deletes creates tack overflow with 10,000 items)
	if ( m_RefTable )
	{
		CFileTable* pTableEntry = m_RefTable;
		while ( pTableEntry )
		{
			CFileTable* pDeleteThis = pTableEntry;
			pTableEntry = pTableEntry->GetNext();
			delete pDeleteThis;
		}
		m_RefTable=NULL;
	}
}

bool CFileReader::MakePCKFromEXE(LPSTR ActualEXEFilename, LPSTR pPCKFilename)
{
	// U75 - 051209 - modified contents of this function (if PCK detected, do NOT read from EXE)
	// NOTE: This helps systems that want an untouched EXE (with no data appended to end of it)
	char pPossiblePCKFile[_MAX_PATH];
	strcpy(pPossiblePCKFile, ActualEXEFilename);
	DWORD dwLength = strlen(pPossiblePCKFile);
	pPossiblePCKFile[dwLength-4]=0;
	strcat(pPossiblePCKFile, ".pck");
	DWORD ValidityValue = 0;
	if ( FileExists ( pPossiblePCKFile )==true )
	{
		// This is an EXE + PCK (no EXE data)
		ValidityValue = 0;
		m_dwKindOfExecutable = 0; //  not installer!
	}
	else
	{
		// Load Data segment into memory
		HANDLE hfile = CreateFile(ActualEXEFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hfile==INVALID_HANDLE_VALUE)
			return false;

		// First, read whole file into memory
		DWORD bytesread=0;
		DWORD filesize = GetFileSize(hfile, NULL);	
		char* filedata = (char*)GlobalAlloc(GMEM_FIXED, filesize);
		ReadFile(hfile, filedata, filesize, &bytesread, NULL); 
		CloseHandle(hfile);

		// Read footer data for location of data segment of file
		ValidityValue = *(DWORD*)(filedata+filesize-12);
		if(ValidityValue==12345678)
		{
			// Encryptor Code If Any
			gEncryptionKey = *(DWORD*)(filedata+filesize-16);

			// Kind Of Executable
			m_dwKindOfExecutable = *(DWORD*)(filedata+filesize-8);

			// Obtain actual size of executable so we can skip it
			DWORD SizeOfEXECode = *(DWORD*)(filedata+filesize-4);

			// Adjust for reading just the data
			filesize-=SizeOfEXECode;
			char* filedataptr = filedata+SizeOfEXECode;

			// Create DataBlock Memory
			m_DataBlockSize = filesize;	
			m_pDataBlock = (char*)GlobalAlloc(GMEM_FIXED, m_DataBlockSize);
			CopyMemory(m_pDataBlock, filedataptr, m_DataBlockSize);

			// Create new PCK File from Data
			GoodDeleteFile(pPCKFilename);
			hfile = CreateFile(pPCKFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hfile!=INVALID_HANDLE_VALUE)
			{
				// Write Data to File (becomes the PCK File from the EXE File)
				DWORD byteswritten=0;
				WriteFile(hfile, m_pDataBlock, m_DataBlockSize, &byteswritten, NULL); 
				CloseHandle(hfile);
			}

			// Free EXEData Now
			FreeEXEDataFromMemory();
		}

		// Release filedata (data extracted)
		GlobalFree(filedata);
		filedata=NULL;
	}

	// Where EXE is not containing data, use regular EXE + PCK approach
	if ( ValidityValue!=12345678 )
	{
		// EXE is alone, set pck filename to exename + pck
		strcpy(pPCKFilename, pPossiblePCKFile);
	}

	// Complete
	return true;
}

void CFileReader::FreeEXEDataFromMemory(void)
{
	// Free data as well
	if(m_pDataBlock)
	{
		GlobalFree(m_pDataBlock);
		m_pDataBlock=NULL;
	}
}

typedef int ( *UNCOMPRESSFUNC ) ( DWORD*, int );
bool CFileReader::CreateVirtualFileTable(LPSTR pPCKFilename)
{
	// Load Data segment into memory
	HANDLE hfile = CreateFile(pPCKFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return false;

	// Create DataBlock Memory
	DWORD bytesread=0;
	m_DataBlockSize = GetFileSize(hfile, NULL);	
	m_pDataBlock = (char*)GlobalAlloc(GMEM_FIXED, m_DataBlockSize);
	ReadFile(hfile, m_pDataBlock, m_DataBlockSize, &bytesread, NULL); 
	CloseHandle(hfile);

	// Record old folder
	char pOldDir[_MAX_PATH];
	getcwd(pOldDir, _MAX_PATH);

	// Switch to unpack folder
	chdir(gUnpackDirectory);

	// First file flag
	bool bFirstFile=true;
	bool bCompressedMediaFound=false;

	// Create table for quick reference
	char filename[256];
	char* DataPtr = m_pDataBlock;
	while(DataPtr<m_pDataBlock+m_DataBlockSize-4)
	{
		// Read Filename Ref First
		DWORD FilenameSize = *(DWORD*)(DataPtr);
		if(FilenameSize==0) break;

		DataPtr+=4;
		for(unsigned int n=0; n<FilenameSize; n++)
			filename[n] = *(DataPtr++);
		filename[n] = 0;

		// Record Pointer to The Data
		DWORD DataSize = *(DWORD*)(DataPtr);
		DataPtr+=4;
		char* pPtr = DataPtr;
		DataPtr+=DataSize;

		// Add to Table
		CFileTable* pRef = new CFileTable;
		_strlwr(filename);
		pRef->SetFilename(filename);
		pRef->SetDataPtr(pPtr);
		pRef->SetDataSize(DataSize);
		if(m_RefTable==NULL)
			m_RefTable = pRef;
		else
			m_RefTable->Add(pRef);

		if(m_dwKindOfExecutable==1)
		{
			// Installer does not run executable, so no need for virtual files
			if(bFirstFile==true)
			{
				// If compression DLL, the rest of PCK is another PCK filecompressed
				if(stricmp(filename, "compress.dll")==NULL)
				{
					// Cannot Install Compressed Media
					bCompressedMediaFound=true;
				}
			}
		}
		else
		{
			// Executable PCK unpacked files can go different places
			CreateFileFromData(filename, pPtr, DataSize);
			pPtr+=DataSize;
		}

		// If first file is compression.dll, create uncompressed PCK and use that,
		if(bFirstFile==true)
		{
			// If compression DLL, the rest of PCK is another PCK filecompressed
			if(stricmp(filename, "compress.dll")==NULL)
			{
				// Uncompress rest of PCK Data
				HMODULE hModule = LoadLibrary(filename);
				UNCOMPRESSFUNC UncompressBlock = ( UNCOMPRESSFUNC ) GetProcAddress ( hModule, "decompress_block" );

				// Compress PCK Data
				LPSTR pData=NULL;
				DWORD dwDataSize=0;
				DWORD dwRestOfSize = m_DataBlockSize - DataSize;
				int iReturnInt =  UncompressBlock((DWORD*)pPtr, (int)dwRestOfSize);
				if(iReturnInt==-1)
				{
					// Error while uncompressing!
					if(m_pDataBlock)
					{
						GlobalFree(m_pDataBlock);
						m_pDataBlock=NULL;
					}
					break;
				}

				// Get The actual data
				HANDLE hGlobAlloc = (HANDLE)iReturnInt;
				pData = (LPSTR)GlobalLock(hGlobAlloc);
				dwDataSize = GlobalSize(hGlobAlloc);

				// Clear ref table (dont need compress.dll)
				if(m_RefTable)
				{
					delete m_RefTable;
					m_RefTable=NULL;
				}

				// New uncompressed data is used for new PCK extraction
				if(m_pDataBlock) GlobalFree(m_pDataBlock);
				m_DataBlockSize = dwDataSize;	
				m_pDataBlock = (LPSTR)GlobalAlloc(GMEM_FIXED, m_DataBlockSize);
				memcpy(m_pDataBlock, pData, m_DataBlockSize);
				DataPtr = m_pDataBlock;

				// Free uncompressed data
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
		}

		// Only allow this to be true for first file
		bFirstFile=false;
	}

	// Switch back to previous folder
	chdir(pOldDir);

	// Error message
	if(bCompressedMediaFound)
	{
		MessageBox(NULL, "Cannot Install Compressed Files", "Error", MB_OK);
		return false;
	}

	// Successfully completed
	return true;
}

bool CFileReader::FileExists(LPSTR pFilename)
{
	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Close File
		CloseHandle(hFile);
		hFile=NULL;
		return true;
	}
	// soft fail
	return false;
}

bool CFileReader::PathExists(LPSTR pPathname)
{
	char oldone[_MAX_PATH];
	getcwd(oldone, 256);
	if(chdir(pPathname)==0)
	{
		chdir(oldone);
		return true;
	}
	return false;
}

bool CFileReader::CreateInstallerFolder(LPSTR ActualEXEFilename)
{
	// Store CWD
	char pOldDir[_MAX_PATH];
	getcwd(pOldDir, _MAX_PATH);

	// Determine temp name for installer folder
	char pInstallerFolder[_MAX_PATH];
	strcpy(pInstallerFolder, ActualEXEFilename);
	DWORD dwLength = strlen(pInstallerFolder);
	pInstallerFolder[dwLength-4]=0;

	// Determine if foldername unique, then create it
	unsigned char cChar=65;
	while(PathExists(pInstallerFolder) || FileExists(pInstallerFolder))
	{
		if(cChar>=255) return false;
		pInstallerFolder[dwLength-4]=cChar;
		pInstallerFolder[dwLength-3]=0;
		cChar++;
	}
	mkdir(pInstallerFolder);

	// Create Media Files in Installer Folder
	CFileTable* pRef = m_RefTable;
	while(pRef)
	{
		// Executable PCK unpacked files can go different places
		LPSTR filename = pRef->GetFilename();
		if(strnicmp(filename, "media\\", 6)==NULL)
		{
			// Actual relative installer path to use 
			char pNameWithoutMediaFolder[_MAX_PATH];
			strcpy(pNameWithoutMediaFolder, filename);
			strrev(pNameWithoutMediaFolder);
			DWORD dwLength = strlen(pNameWithoutMediaFolder)-6;
			pNameWithoutMediaFolder[dwLength]=0;
			strrev(pNameWithoutMediaFolder);

			// Actual relative installer path to use 
			char pRelInstallerPath[_MAX_PATH];
			strcpy(pRelInstallerPath, pInstallerFolder);
			strcat(pRelInstallerPath, "\\");
			strcat(pRelInstallerPath, pNameWithoutMediaFolder);

			// Create the file in the current directory (installer option)
			CreateFileFromData(pRelInstallerPath, pRef->GetDataPtr(), pRef->GetDataSize());
		}

		// Next file in virtual table
		pRef = pRef->GetNext();
	}

	// Restore Folder
	chdir(pOldDir);

	// Load Data segment into memory
	HANDLE hfile = CreateFile(ActualEXEFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return false;

	// First load the entire EXEData
	DWORD bytesread=0;
	m_DataBlockSize = GetFileSize(hfile, NULL);	
	m_pDataBlock = (char*)GlobalAlloc(GMEM_FIXED, m_DataBlockSize);
	ReadFile(hfile, m_pDataBlock, m_DataBlockSize, &bytesread, NULL); 
	CloseHandle(hfile);

	// Obtain actual size of executable so we can skip it
	DWORD SizeOfEXECode = *(DWORD*)((m_pDataBlock+m_DataBlockSize)-4);

	// Change the 'kindof' state to 'normal' runnable executable
	*(DWORD*)((m_pDataBlock+m_DataBlockSize)-8)=0;

	// Adjust for reading just the data
	char filename[_MAX_PATH];
	LPSTR DataPtr = m_pDataBlock+SizeOfEXECode;
	while(DataPtr<m_pDataBlock+m_DataBlockSize-4)
	{
		// Get first byte of this entry
		LPSTR pFirstByte = DataPtr;

		// Get filename of item in table
		DWORD FilenameSize = *(DWORD*)(DataPtr);
		if(FilenameSize==0) break;
		DataPtr+=4;
		for(unsigned int n=0; n<FilenameSize; n++)
			filename[n] = *(DataPtr++);
		filename[n] = 0;

		// Get Ptr to first byte in data for item in table
		DWORD DataSize = *(DWORD*)(DataPtr);
		DataPtr+=4;
		char* pPtr = DataPtr;
		DataPtr+=DataSize;

		// If find a media item, remove it from datablock now
		if(strnicmp(filename, "media\\", 6)==NULL)
		{
			// Shuffle data to squash out media item from table
			DWORD dwBytesIn = DataPtr-m_pDataBlock;
			DWORD dwRemainingBytes = m_DataBlockSize - dwBytesIn;
			memcpy(pFirstByte, DataPtr, dwRemainingBytes);
			DWORD dwBytesEaten = (DataPtr-pFirstByte);

			// Adjust pointer to compensate
			m_DataBlockSize-=dwBytesEaten;
			DataPtr-=dwBytesEaten;
		}
	}

	// Create a new executable inside the install folder
	char pRelNewEXEPath[_MAX_PATH];
	strcpy(pRelNewEXEPath, pInstallerFolder);
	strcat(pRelNewEXEPath, "\\");
	strcat(pRelNewEXEPath, ActualEXEFilename);
	CreateFileFromData(pRelNewEXEPath, m_pDataBlock, m_DataBlockSize);

	// Free memoru used
	if(m_pDataBlock)
	{
		GlobalFree(m_pDataBlock);
		m_pDataBlock=NULL;
	}

	// Complete
	return true;
}

bool CFileReader::CreateFileFromData(LPSTR UseFilename, char* dataptr, DWORD datasize)
{
	// Ensure folder path exists for extraction
	char pPathOnly[_MAX_PATH];
	strcpy(pPathOnly, UseFilename);
	StripPathOnly(pPathOnly);

	// Trace Path and recreate folders
	RecreatePathFolders(pPathOnly);

	// Open File
	GoodDeleteFile(UseFilename);
	HANDLE hfile = CreateFile(UseFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return false;

	// Write BLOCK file from data mem
	DWORD byteswritten;
	WriteFile(hfile, dataptr, datasize, &byteswritten, NULL); 
	CloseHandle(hfile);
	return true;
}

void CFileReader::StripPathOnly(LPSTR pFilename)
{
	// Find filepart
	int iFilePartStart=-1;
	DWORD dwLength = strlen(pFilename);
	for(DWORD n=dwLength-1; n>0; n--)
	{
		// Find start position of filename part
		if(pFilename[n]=='/') pFilename[n]='\\';
		if(pFilename[n]=='\\' && iFilePartStart==-1)
		{
			iFilePartStart=n;
		}
	}

	// Cut off filename part
	if(iFilePartStart==-1)
	{
		// Cut all - filename only - no path!
		pFilename[0]=0;
	}
	else
	{
		// Cut off filename part
		pFilename[iFilePartStart+1]=0;
	}
}

void CFileReader::StripFileOnly(LPSTR pFilename)
{
	// Find filepart
	int iFilePartStart=-1;
	DWORD dwLength = strlen(pFilename);
	for(DWORD n=dwLength-1; n>0; n--)
	{
		// Find start position of filename part
		if(pFilename[n]=='/') pFilename[n]='\\';
		if(pFilename[n]=='\\' && iFilePartStart==-1)
		{
			iFilePartStart=n;
		}
	}

	// Cut off filename part
	if(iFilePartStart!=-1)
	{
		iFilePartStart++;
		DWORD dwNameLength = (dwLength - iFilePartStart)+1;
		memcpy(pFilename, pFilename+iFilePartStart, dwNameLength);
	}
}

void CFileReader::RecreatePathFolders(LPSTR pPath)
{
	// Record old folder
	char pOldDir[_MAX_PATH];
	getcwd(pOldDir, _MAX_PATH);

	// Find filepart
	int iFolderStart=0;
	LPSTR pStartPtr=pPath;
	DWORD dwLength = strlen(pPath);
	for(DWORD n=0; n<dwLength; n++)
	{
		// Find start position of filename part
		if(pPath[n]=='\\')
		{
			// Check this folder exists, else we create it
			DWORD dwThisLength = n-iFolderStart;
			if(dwThisLength>0)
			{
				// Get Folder Name
				LPSTR pFolderName = new char[dwThisLength+1];
				memcpy(pFolderName, pStartPtr, dwThisLength);
				pFolderName[dwThisLength]=0;

				// Check Folder, else Create Folder
				if(chdir(pFolderName)==-1)
				{
					mkdir(pFolderName);
					chdir(pFolderName);
				}

				// Free folder name string
				if(pFolderName)
				{
					delete pFolderName;
					pFolderName=NULL;
				}
			}

			// Ptr for next folder
			pStartPtr=pPath+n+1;
			iFolderStart=n+1;
		}
	}

	// Restore directory
	chdir(pOldDir);
}

void CFileReader::GoodDeleteFile(LPSTR filename)
{
	// Only deletes files created by this executable
	if(DeleteFile(filename)==FALSE)
	{
		// Skip DLLs' unknwon to be naugty
		bool bSkip = false;
		if ( stricmp ( filename, "blitzterrain.dll" )==NULL ) bSkip = true;
		if ( bSkip==false )
		{
			// Might be filemapped still, so release filemap
			WIN32_FIND_DATA filedata;
			HANDLE handle = FindFirstFile ( filename, &filedata );
			if(handle!=INVALID_HANDLE_VALUE)
			{
				try
				{
					CloseHandle(handle);
				}
				catch(...)
				{
					// ignore this exception and no not delete
					return;
				}
			}
			DeleteFile(filename);
		}
	}
}

bool CFileReader::RemoveVirtualFileTable(void)
{
	// Record old folder
	char pOldDir[_MAX_PATH];
	getcwd(pOldDir, _MAX_PATH);

	// Switch to unpack folder
	chdir(gUnpackDirectory);

	// Delete all files referenced by virtual table
	DWORD dwCountFiles=0;
	CFileTable* pRef = m_RefTable;
	while(pRef)
	{
		LPSTR filename = pRef->GetFilename();
		GoodDeleteFile(filename);
		pRef = pRef->GetNext();
		dwCountFiles++;
	}

	// Delete compress.dll if present
	GoodDeleteFile("compress.dll");

	// Delete debug.txt if present
	GoodDeleteFile("debug.txt");

	// Delete all folders if all files where removed
	DWORD dwTryFolderDelete = dwCountFiles+1;

	// U75 - 051209 - cap the nest count (as 10,000 files makes it look like it's frozen)
	if ( dwTryFolderDelete > 20 ) dwTryFolderDelete = 20;

	while(dwTryFolderDelete>0)
	{
		pRef = m_RefTable;
		while(pRef)
		{
			// Table item filename string
			LPSTR filename = pRef->GetFilename();

			// Attempt to delete directory
			char pPathOnly[_MAX_PATH];
			strcpy(pPathOnly, filename);
			StripPathOnly(pPathOnly);
			if(strlen(pPathOnly)>0)
			{
				RemoveDirectory(pPathOnly);
			}

			// Next file
			pRef = pRef->GetNext();
		}

		// Due to irregular folder nests (need to keep going in case main folder is first in table thus could not be removed until all other folders had gone)
		dwTryFolderDelete--;
	}

	// Switch back to previous folder
	chdir(pOldDir);

	// Successfully completed
	return true;
}


// FileTable Implementations

CFileTable::CFileTable()
{
	m_pNext=NULL;
	m_pPrev=NULL;

	strcpy(m_filename, "");
	m_dataptr=NULL;
	m_datasize=0;
}

CFileTable::~CFileTable()
{
	/* U75 - 051209 - stack overflow if 10,000 nests (handled at filetable list delete stage)
	if(m_pNext)
	{
		delete m_pNext;
		m_pNext=NULL;
	}
	*/
}

void CFileTable::Add(CFileTable* pNewItem)
{
	// Different way to add file to file table list
	CFileTable* pTableEntry = this;
	while ( pTableEntry->GetNext() )
		pTableEntry = pTableEntry->GetNext();

	// when no more next entries, add as last item
	pTableEntry->SetNext(pNewItem);
	pNewItem->SetPrev(pTableEntry);

	/* U75 - 051209 - 10,000 media attachments causes stack overflow!!
	if(GetNext()==NULL)
	{
		SetPrev(this);
		this->SetNext(pNewItem);
	}
	else
		GetNext()->Add(pNewItem);
	*/
}
