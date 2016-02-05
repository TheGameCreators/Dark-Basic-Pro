
////////////////////////////////////////////////////////////////////
// INFORMATION /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/*
	FILE BLOCKS COMMANDS
*/

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// DEFINES AND INCLUDES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#define DARKSDK	__declspec ( dllexport )

#include "stdafx.h"
#include "Zip\ZipArchive.h"
#include "core.h"
#include "fileblocksblowfish.h"

#pragma comment ( lib, ".\\ziparchive.lib" )

const long	MAX_VOLUME_SIZE_BYTES	= 100000000;	// maximum size of the zip
const int	ZIP_COMPRESS_LEVEL		= 9;			// maximum compression level

// file block structure
struct sFileBlock
{
	CZipArchive*	pZip;
	bool			bEncrypted;
	char			szName     [ 255 ];
	char			szZipName  [ 255 ];
	char			szPassword [ 255 ];
	int				iCompression;
	int				iFileCount;
	
	sFileBlock ( )
	{
		pZip = NULL;

		bEncrypted = false;
		
		memset ( szName,     0, sizeof ( szName     ) );
		memset ( szZipName,  0, sizeof ( szZipName  ) );
		memset ( szPassword, 0, sizeof ( szPassword ) );
		
		strcpy ( szPassword, "default" );

		iCompression = 9;

		iFileCount = 0;
	}
};

sFileBlock	g_FileBlocks    [ 255 ];
char	    g_TempDirectory [ 255 ];
char        g_szRestoreDir  [ 255 ];

#define MAX_ZIP	255

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

DARKSDK void CreateFileBlock					( int iID, DWORD dwFilename );
DARKSDK void DeleteFileBlock					( int iID );

DARKSDK void SetFileBlockKey					( int iID, DWORD dwKey );
DARKSDK void SetFileBlockCompression			( int iID, int iLevel );

DARKSDK void AddFileToBlock						( int iID, DWORD dwFile );
DARKSDK void AddObjectToBlock					( int iID, int iObject, DWORD dwFilename );
DARKSDK void AddImageToBlock					( int iID, int iImage, DWORD dwFilename );
DARKSDK void AddBitmapToBlock                   ( int iID, int iBitmap, DWORD dwFilename );

DARKSDK void SaveFileBlock						( int iID );
DARKSDK void OpenFileBlock						( DWORD dwFile, int iID );
DARKSDK void OpenFileBlock						( DWORD dwFile, int iID, DWORD dwKey );
DARKSDK void CloseFileBlock						( int iID );

DARKSDK void RemoveFileFromBlock				( int iID, DWORD dwFile );

DARKSDK void PerformCheckListForFileBlockData	( int iID );
DARKSDK void ExtractFileFromBlock               ( int iID, DWORD dwFilename, DWORD dwPath );

DARKSDK void LoadObjectFromBlock				( int iID, DWORD dwFile, int iObject );
DARKSDK void LoadImageFromBlock					( int iID, DWORD dwFile, int iImage );
DARKSDK void LoadBitmapFromBlock				( int iID, DWORD dwFile, int iBitmap );

//DARKSDK void LoadSoundFromBlock					( int iID, DWORD dwFile, int iSound );
//DARKSDK void LoadFileFromBlock					( int iID, DWORD dwFile, int iFile );
//DARKSDK void LoadMemblockFromBlock				( int iID, DWORD dwFile, int iMemblock );
//DARKSDK void LoadAnimationFromBlock				( int iID, DWORD dwFile, int iAnimation );

DARKSDK int GetFileBlockExists					( int iID );
DARKSDK int GetFileBlockSize					( int iID );
DARKSDK int GetFileBlockCount					( int iID );
DARKSDK int GetFileBlockDataExists				( int iID, DWORD dwFile );
DARKSDK int GetFileBlockKey						( int iID );
DARKSDK int GetFileBlockCompression				( int iID );

void SetupFileBlocks    ( void );
void DestroyFileBlocks  ( void );
void SetTempDirectory   ( void );
void RestoreDirectory   ( void );

void AddOrObtainResourceFromBlock ( int iID, int iIndex, DWORD dwFilename, int iType, int iMode );
void Encryption                   ( int iID, int iMode );

void CheckData ( int iID );



/*
	// export names for string table -

		CREATE FILE BLOCK%LS%?CreateFileBlock@@YAXHK@Z%id, filename
		DELETE FILE BLOCK%L%?DeleteFileBlock@@YAXH@Z%ID

		SET FILE BLOCK KEY%LS%?SetFileBlockKey@@YAXHK@Z%ID, Key
		SET FILE BLOCK COMPRESSION%LL%?SetFileBlockCompression@@YAXHH@Z%ID, Flag

		ADD FILE TO BLOCK%LS%?AddFileToBlock@@YAXHK@Z%ID, File
		ADD OBJECT TO BLOCK%LLS%?AddObjectToBlock@@YAXHHK@Z%id, object, filename
		ADD IMAGE TO BLOCK%LLS%?AddImageToBlock@@YAXHHK@Z%id, object, filename
		ADD BITMAP TO BLOCK%LLS%?AddBitmapToBlock@@YAXHHK@Z%id, bitmap, filename

		REMOVE FILE FROM BLOCK%LS%?RemoveFileFromBlock@@YAXHK@Z%ID, File

		SAVE FILE BLOCK%L%?SaveFileBlock@@YAXH@Z%id
		OPEN FILE BLOCK%SL%?OpenFileBlock@@YAXKH@Z%File, ID
		OPEN FILE BLOCK%SLS%?OpenFileBlock@@YAXKHK@Z%File, ID, Key
		
		PERFORM CHECKLIST FOR FILE BLOCK DATA%L%?PerformCheckListForFileBlockData@@YAXH@Z%ID
		EXTRACT FILE FROM BLOCK%LSS%?ExtractFileFromBlock@@YAXHKK@Z%id, filename, path

		LOAD OBJECT FROM BLOCK%LSL%?LoadObjectFromBlock@@YAXHKH@Z%ID, File, Object
		LOAD BITMAP FROM BLOCK%LSL%?LoadBitmapFromBlock@@YAXHKH@Z%ID, File, Bitmap
		LOAD IMAGE FROM BLOCK%LSL%?LoadImageFromBlock@@YAXHKH@Z%ID, File, Image
		LOAD SOUND FROM BLOCK%LSL%?LoadSoundFromBlock@@YAXHKH@Z%ID, File, Sound
		LOAD FILE FROM BLOCK%LSL%?LoadFileFromBlock@@YAXHKH@Z%ID, File, File ID
		LOAD MEMBLOCK FROM BLOCK%LSL%?LoadMemblockFromBlock@@YAXHKH@Z%ID, File, Memblock
		LOAD ANIMATION FROM BLOCK%LSL%?LoadAnimationFromBlock@@YAXHKH@Z%ID, File, Animation
		
		GET FILE BLOCK EXISTS[%LL%?GetFileBlockExists@@YAHH@Z%ID
		GET FILE BLOCK SIZE[%LL%?GetFileBlockSize@@YAHH@Z%ID
		GET FILE BLOCK COUNT[%LL%?GetFileBlockCount@@YAHH@Z%ID
		GET FILE BLOCK DATA EXISTS[%LLS%?GetFileBlockDataExists@@YAHHK@Z%ID, File
		GET FILE BLOCK KEY[%LL%?GetFileBlockKey@@YAHH@Z%ID
		GET FILE BLOCK COMPRESSION[%LL%?GetFileBlockCompression@@YAHH@Z%ID
*/

void CheckID ( int iID )
{
	if ( iID < 0 || iID > 255 )
	{
		Error ( 7 );
		return;
	}
}

void CheckData ( int iID )
{
	// ensure the zip file is valid
	if ( !g_FileBlocks [ iID ].pZip )
	{
		// display a runtime error if something is wrong
		Error ( 11 );
		return;
	}
}

void SetupFileBlocks ( void )
{
	// set up the temp directory for file extraction
	GetWindowsDirectory ( g_TempDirectory, 255 );
	strcat ( g_TempDirectory, "\\temp\\" );
}

void DestroyFileBlocks ( void )
{
	// destroy file blocks
	for ( int i = 0; i < MAX_ZIP; i++ )
	{
		// check the file block
		if ( g_FileBlocks [ i ].pZip )
		{
			char szOriginalName [ 256 ] = "";

			for ( int y = 0; y < (int)strlen ( g_FileBlocks [ i ].szName ); y++ )
			{
				if ( g_FileBlocks [ i ].szName [ y ] == '.' )
				{
					memcpy ( szOriginalName, g_FileBlocks [ i ].szName, sizeof ( char ) * y );
					break;
				}
			}

			// destroy it
			delete g_FileBlocks [ i ].pZip;
			g_FileBlocks [ i ].pZip = NULL;

			strcat ( szOriginalName, ".000" );
			DeleteFile ( szOriginalName );
		}
	}
}

void SetTempDirectory ( void )
{
	// set the temporary directory
	GetCurrentDirectory ( 255, g_szRestoreDir );
	SetCurrentDirectory ( g_TempDirectory );
}

void RestoreDirectory ( void )
{
	// restore the original directory
	SetCurrentDirectory ( g_szRestoreDir );
}

void GetZipName ( int iID, char* szFileName )
{
	for ( int i = strlen ( szFileName ), c = 0; i > 0; i--, c++ )
	{
		if ( szFileName [ i ] == '.' )
		{
			memcpy ( g_FileBlocks [ iID ].szZipName, szFileName, sizeof ( char ) * i );
			strcat ( g_FileBlocks [ iID ].szZipName, ".zip" );
		}
	}
}

void CreateFileBlock ( int iID, DWORD dwFilename )
{
	// create a new file block

	// check ID of file block
	CheckID ( iID );

	/*
	if ( g_FileBlocks [ iID ].pZip )
	{
		MessageBox ( NULL, "invalid zip", "error", MB_OK );

		Error ( 13 );
		return;
	}
	*/

	// safely delete any currently existing zip file
	SAFE_DELETE ( g_FileBlocks [ iID ].pZip );

	DeleteFile ( ( char* ) dwFilename );

	g_FileBlocks [ iID ].pZip = NULL;

	g_FileBlocks [ iID ].bEncrypted = false;
	
	memset ( g_FileBlocks [ iID ].szName,     0, sizeof ( g_FileBlocks [ iID ].szName     ) );
	memset ( g_FileBlocks [ iID ].szZipName,  0, sizeof ( g_FileBlocks [ iID ].szZipName  ) );
	memset ( g_FileBlocks [ iID ].szPassword, 0, sizeof ( g_FileBlocks [ iID ].szPassword ) );
	
	strcpy ( g_FileBlocks [ iID ].szPassword, "default" );

	g_FileBlocks [ iID ].iCompression = 9;

	g_FileBlocks [ iID ].iFileCount = 0;

	// create a new zip archive
	g_FileBlocks [ iID ].pZip = new CZipArchive;

	// check memory was allocated
	if ( !g_FileBlocks [ iID ].pZip )
	{
		// display error on failure
		Error ( 11 );
		return;
	}

	// attempt to open the zip file	
	// 280305 - do not create in span mode
	//g_FileBlocks [ iID ].pZip->Open ( ( char* ) dwFilename, CZipArchive::createSpan, MAX_VOLUME_SIZE_BYTES );
	g_FileBlocks [ iID ].pZip->Open ( ( char* ) dwFilename, CZipArchive::create, MAX_VOLUME_SIZE_BYTES );

	// store the name of the zip in the file block structure
	strcpy ( g_FileBlocks [ iID ].szName, ( char* ) dwFilename );
}

void AddObjectToBlock ( int iID, int iObject, DWORD dwFilename )
{
	// check ID of file block
	CheckID ( iID );

	// add an object to a block
	AddOrObtainResourceFromBlock ( iID, iObject, dwFilename, 0, 0 );
}

void AddImageToBlock ( int iID, int iImage, DWORD dwFilename )
{
	// check ID of file block
	CheckID ( iID );

	// add an image to the block
	AddOrObtainResourceFromBlock ( iID, iImage, dwFilename, 1, 0 );
}

void AddBitmapToBlock ( int iID, int iBitmap, DWORD dwFilename )
{
	// check ID of file block
	CheckID ( iID );

	// add an image to the block
	AddOrObtainResourceFromBlock ( iID, iBitmap, dwFilename, 2, 0 );
}

void AddOrObtainResourceFromBlock ( int iID, int iIndex, DWORD dwFilename, int iType, int iMode )
{
	// values for iType -
	//
	//	0 = object
	//	1 = image
	//  2 = bitmap
	//
	// values for iMode -
	//
	//	0 = add to block
	//	1 = load from block
	//

	// check ID of file block
	CheckID ( iID );

	// check the zip file
	CheckData ( iID );

	// type definitions
	typedef void  ( *SaveObjectPFN         ) ( DWORD dwFilename,  int iID );
	typedef void  ( *SaveImagePFN          ) ( char* pszFilename, int iID );
	typedef void  ( *LoadObjectPFN         ) ( DWORD dwFilename,  int iID );
	typedef void  ( *MemblockFromBitmapPFN ) ( int iMemblock, int iID );
	typedef char* ( *GetMemblockPtrPFN     ) ( int iMemblock );
	typedef int   ( *GetMemblockSizePFN    ) ( int iMemblock );

	typedef int   ( *GetBitmapWidthPFN    ) ( int );
	typedef bool  ( *GrabImagePFN         ) ( int, int, int, int, int, int );
	typedef void  ( *DeleteImagePFN       ) ( int );
	typedef bool  ( *GetImageExistPFN     ) ( int );
	
	// declare function pointers
	SaveObjectPFN pSaveObject = NULL;
	SaveImagePFN  pSaveImage  = NULL;
	LoadObjectPFN pLoadObject = NULL;
	LoadObjectPFN pLoadImage  = NULL;
	
	SaveImagePFN  pLoadBitmap = NULL;
	SaveImagePFN  pSaveBitmap = NULL;

	MemblockFromBitmapPFN pMemblockFromBitmap = NULL;
	GetMemblockPtrPFN	  pGetMemblockPtr     = NULL;
	GetMemblockSizePFN	  pGetMemblockSize    = NULL;

	GetBitmapWidthPFN	pGetBitmapWidth  = NULL;
	GetBitmapWidthPFN	pGetBitmapHeight = NULL;
	GrabImagePFN		pGrabImage		 = NULL;
	DeleteImagePFN		pDeleteImage	 = NULL;
	GetImageExistPFN	pGetImageExist   = NULL;


	// check media to add or obtain
	switch ( iType )
	{
		case 0:
		{
			// determine if DLL is valid
			if ( !g_pGlob->g_Basic3D )
				return;

			// set function pointers
			pSaveObject = ( SaveObjectPFN ) GetProcAddress ( g_pGlob->g_Basic3D, "?Save@@YAXKH@Z" );
			pLoadObject = ( LoadObjectPFN ) GetProcAddress ( g_pGlob->g_Basic3D, "?Load@@YAXKH@Z" );
		}
		break;

		case 1:
		{
			// check DLL
			if ( !g_pGlob->g_Image )
				return;

			// set function pointers
			pSaveImage = ( SaveImagePFN  ) GetProcAddress ( g_pGlob->g_Image, "?Save@@YA_NPADH@Z" );
			pLoadImage = ( LoadObjectPFN ) GetProcAddress ( g_pGlob->g_Image, "?Load@@YA_NPADH@Z" );
		}
		break;

		case 2:
		{
			// check DLL
			if ( !g_pGlob->g_Bitmap )
				return;

			if ( !g_pGlob->g_Image )
				return;

			// set function pointers
			pSaveBitmap = ( SaveImagePFN ) GetProcAddress ( g_pGlob->g_Bitmap, "?SaveBitmap@@YAXPADH@Z" );
			pLoadBitmap = ( SaveImagePFN ) GetProcAddress ( g_pGlob->g_Bitmap, "?LoadBitmapA@@YAXPADH@Z" );

			pSaveImage = ( SaveImagePFN  ) GetProcAddress ( g_pGlob->g_Image, "?Save@@YA_NPADH@Z" );
			pGrabImage       = ( GrabImagePFN       ) GetProcAddress ( g_pGlob->g_Image,  "?GrabImageEx@@YAXHHHHHH@Z" );
			pDeleteImage     = ( DeleteImagePFN     ) GetProcAddress ( g_pGlob->g_Image,  "?DeleteEx@@YAXH@Z" );
			pGetImageExist   = ( GetImageExistPFN   ) GetProcAddress ( g_pGlob->g_Image,  "?GetExistEx@@YAHH@Z" );
			pGetBitmapWidth  = ( GetBitmapWidthPFN  ) GetProcAddress ( g_pGlob->g_Bitmap, "?BitmapWidth@@YAHH@Z"     );
			pGetBitmapHeight = ( GetBitmapWidthPFN  ) GetProcAddress ( g_pGlob->g_Bitmap, "?BitmapHeight@@YAHH@Z"    );
		}
		break;
	}

	// set the temp directory
	SetTempDirectory ( );

	// mode 1 obtains the data - extract the file into the temporary folder
	if ( iMode == 1 )
	{
		SetFileAttributes(( char* ) dwFilename, FILE_ATTRIBUTE_NORMAL);
		DeleteFile ( ( char* ) dwFilename );

		ExtractFileFromBlock ( iID, dwFilename, ( DWORD ) g_TempDirectory );
	}

	
	/*
	char szFilename [ 256 ] = "";

	{
		char szTest [ 256 ] = "";
		char szTestA [ 256 ] = "";

		

		strcpy ( szFilename, ( char* ) dwFilename );

		strcpy ( szTest, szFilename );

		//szFileName.TrimLeft(_T("."));
		//szFileName.TrimLeft(_T("\\"));
		//szFileName.TrimLeft(_T("media"));

		int c = 0;

		for ( int i = strlen ( szTest ); i > 0; i--, c++ )
		{
			if ( szTest [ i ] == '\\' || szTest [ i ] == '/' )
			{
				memcpy ( szTestA, &szTest [ i + 1 ], sizeof ( char ) * c );
				break;
			}
		}

		//szFilename = szTestA;

		strcpy ( szFilename, szTestA );

		c = 0;
	}
*/

	// check the media option
	switch ( iType )
	{
		case 0:
			// save or load an object
			if ( pSaveObject && iMode == 0 ) pSaveObject ( dwFilename, iIndex );
			if ( pLoadObject && iMode == 1 ) pLoadObject ( dwFilename, iIndex );
		break;

		case 1:
			// save or load an image
			if ( pSaveImage && iMode == 0 ) pSaveImage ( ( char* ) dwFilename, iIndex );
			if ( pLoadImage && iMode == 1 ) pLoadImage ( dwFilename, iIndex );
			//if ( pLoadImage && iMode == 1 ) pLoadImage ( ( DWORD ) szFilename, iIndex );
		break;

		case 2:
		{
			// save or load a bitmap
			if ( iMode == 0 )
			{
				if ( pGrabImage && pDeleteImage && pGetImageExist && pGetBitmapWidth && pGetBitmapHeight )
				{
					int iImageID = 0;

					for ( int i = 1; i < 22000000; i++ )
					{
						if ( !pGetImageExist ( i ) )
						{
							iImageID = i;
							break;
						}
					}

					int w = pGetBitmapWidth  ( iIndex );
					int h = pGetBitmapHeight ( iIndex );

					pGrabImage ( iImageID, 0, 0, pGetBitmapWidth ( iIndex ), pGetBitmapHeight ( iIndex ), 1 );
					pSaveImage ( ( char* ) dwFilename, iImageID );

					pDeleteImage ( iImageID );
				}
			}

			if ( pLoadBitmap && iMode == 1 ) pLoadBitmap ( ( char* ) dwFilename, iIndex );
		}
		break;
	}


	// mode 0 adds the data to the zip
	if ( iMode == 0 )
	{
		// lee - 200306 - u6b4 - so can overwrite (overwrite any existing file like AddFileToBlock)
		int iIndex = g_FileBlocks [ iID ].pZip->FindFile ( ( char* ) dwFilename );
		g_FileBlocks [ iID ].pZip->DeleteFile ( iIndex );

		// add the resource
		g_FileBlocks [ iID ].pZip->AddNewFile ( ( char* ) dwFilename, g_FileBlocks [ iID ].iCompression );
	}
	{
		
		/////////////

		CFile f;

		//CFileStatus status;
		//f.Open ( ( char*)dwFilename, CFile::modeReadWrite, NULL );
		//f.GetStatus( ( char*) dwFilename, status );
		//status.m_attribute = 0;
		//f.SetStatus( (char*)dwFilename, status );
		//f.Close ( );
		/////////////

		SetFileAttributes(( char* ) dwFilename, FILE_ATTRIBUTE_NORMAL);
		//DeleteFile ( ( char* ) dwFilename );
	}

	// delete the temporary file
	DeleteFile ( ( char* ) dwFilename );
	//DeleteFile ( szFilename );

	// restore the previous directory
	RestoreDirectory ( );
}

void SetFileBlockKey ( int iID, DWORD dwKey )
{
	// sets the key for file block encryption

	// check ID of file block
	CheckID ( iID );

	// check the file block
	CheckData ( iID );

	// set the password
	g_FileBlocks [ iID ].pZip->SetPassword ( ( char* ) dwKey );

	// store the password in the file block structure
	strcpy ( g_FileBlocks [ iID ].szPassword, ( char* ) dwKey );
}

void SetFileBlockCompression ( int iID, int iLevel )
{
	// set the file block compression

	// check ID of file block
	CheckID ( iID );

	// check the file block
	CheckData ( iID );

	// set the compression level
	g_FileBlocks [ iID ].iCompression = iLevel;
}

void AddFileToBlock ( int iID, DWORD dwFile )
{
	// adds a file to a file block

	// check ID of file block
	CheckID ( iID );
	
	// check the file block is ok
	CheckData ( iID );

	// 280305 - overwrite any existing file
	{
		int iIndex = g_FileBlocks [ iID ].pZip->FindFile ( ( char* ) dwFile );
		g_FileBlocks [ iID ].pZip->DeleteFile ( iIndex );
	}

	// add a file to the block
	g_FileBlocks [ iID ].pZip->AddNewFile ( ( char* ) dwFile, g_FileBlocks [ iID ].iCompression );
}

void RemoveFileFromBlock ( int iID, DWORD dwFile )
{
	// 280305 - remove a file from a file block

	// check ID of file block
	CheckID ( iID );
	
	// check the file block is ok
	CheckData ( iID );

	int iIndex = g_FileBlocks [ iID ].pZip->FindFile ( ( char* ) dwFile );
	g_FileBlocks [ iID ].pZip->DeleteFile ( iIndex );
}

void SaveFileBlock ( int iID )
{
	// saves the file block

	// check ID of file block
	CheckID ( iID );

	// check the file block
	CheckData ( iID );

	// close the file block
	g_FileBlocks [ iID ].pZip->Close ( );

	GetZipName ( iID, g_FileBlocks [ iID ].szName );

	MoveFile ( g_FileBlocks [ iID ].szZipName, g_FileBlocks [ iID ].szName );
}

void CloseFileBlock	( int iID )
{
	// check ID of file block
	CheckID ( iID );

	// close the file block
	g_FileBlocks [ iID ].pZip->Close ( );

	// safely delete any currently existing zip file
	SAFE_DELETE ( g_FileBlocks [ iID ].pZip );
}

void Encryption ( int iID, int iMode )
{
	// encrypt a file

	// check ID of file block
	CheckID ( iID );

	if ( g_FileBlocks [ iID ].bEncrypted && iMode == 0 )
		return;

	if ( iMode == 0 )
		g_FileBlocks [ iID ].bEncrypted = true;

	if ( iMode == 1 )
		g_FileBlocks [ iID ].bEncrypted = false;

	const size_t	bufferSize = 1024;
	size_t			readRet    = 0;
	int				encRet;

	char	szNewFile  [ 255 ];
	char	readBuffer [ bufferSize ];
	char	outBuffer  [ bufferSize ];
	char	_password  [ 100 ];

	strncpy ( _password, g_FileBlocks [ iID ].szPassword, 100 );
	
	BlowFishEnc encryption ( _password );

	strcpy ( szNewFile, g_FileBlocks [ iID ].szName );
	strcat ( szNewFile, ".fb" );

	FILE* readFile  = fopen ( g_FileBlocks [ iID ].szName, "rb" );
	FILE* writeFile = fopen ( szNewFile, "wb" );

	while ( !feof ( readFile ) )
	{
		readRet = fread ( readBuffer, sizeof ( char ), bufferSize, readFile );

		if ( iMode == 0 )
			encRet = encryption.encryptStream ( readBuffer, ( DWORD ) readRet, outBuffer );

		if ( iMode == 1 )
		{
			encRet = encryption.decryptStream(readBuffer, (DWORD)readRet, outBuffer);
			if feof(readFile)
			{
				int pos = 0;
				while ((pos < 8) && ((outBuffer + encRet - pos)[0] == 0)) pos++;
				if (pos) encRet -= (pos - 1);
			}
		}

		fwrite ( outBuffer, sizeof ( char ), encRet, writeFile );
	}

	fflush ( writeFile );

	fclose ( writeFile );
	fclose ( readFile );

	ZeroMemory ( outBuffer,  bufferSize );
	ZeroMemory ( readBuffer, bufferSize );

	DeleteFile ( g_FileBlocks [ iID ].szName );
	CopyFile   ( szNewFile, g_FileBlocks [ iID ].szName, false );
	DeleteFile ( szNewFile );
}

void ExtractFileFromBlock ( int iID, DWORD dwFilename, DWORD dwPath )
{
	// extracts a file from the block
	
	// check ID of file block
	CheckID ( iID );

	// check the file block data
	CheckData ( iID );

	// get the index of the file
	int iIndex = g_FileBlocks [ iID ].pZip->FindFile ( ( char* ) dwFilename );

	// attempt to extract the file
	if ( !g_FileBlocks [ iID ].pZip->ExtractFile ( iIndex, ( char* ) dwPath ) )
	{
		// display error on failure - silent failure
		return;
	}
}

void OpenFileBlock ( DWORD dwFile, int iID )
{
	// open a file block

	// check ID of file block
	CheckID ( iID );

	// set the key to default
	char* szKey = "default";

	// open the block
	OpenFileBlock ( dwFile, iID, ( DWORD ) szKey );
}

void OpenFileBlock ( DWORD dwFile, int iID, DWORD dwKey )
{
	// open a file block from disk

	// check ID of file block
	CheckID ( iID );

	// safely delete any existing file block
	SAFE_DELETE ( g_FileBlocks [ iID ].pZip );

	// store name and password in file block structure
	strcpy ( g_FileBlocks [ iID ].szName,     ( char* ) dwFile );
	strcpy ( g_FileBlocks [ iID ].szPassword, ( char* ) dwKey  );

	// MoveFile ( g_FileBlocks [ iID ].szZipName, g_FileBlocks [ iID ].szName );
	
	// create a new zip archive
	g_FileBlocks [ iID ].pZip = new CZipArchive;

	// check file is okay
	if ( !g_FileBlocks [ iID ].pZip )
	{
		// error on failure
		Error ( 11 );
		return;
	}

	// lee - 290306 - u6rc3 - reject if not ZIP ext, as regular files crash pZip interface
	LPSTR lpFilename = ( char* ) dwFile;
	if ( strnicmp ( lpFilename + strlen(lpFilename) - 4, ".zip", 4 )!=NULL )
	{
		Error ( 13 );
		return;
	}

	// open the zip and set the password
	if ( !g_FileBlocks [ iID ].pZip->Open ( ( char* ) dwFile, CZipArchive::open, MAX_VOLUME_SIZE_BYTES ) )
	{
//		MessageBox ( NULL, "failed to open zip", "error", MB_OK );
		Error ( 13 );
		return;
	}

	g_FileBlocks [ iID ].pZip->SetPassword ( ( char* ) dwKey );

	g_FileBlocks [ iID ].iFileCount = g_FileBlocks [ iID ].pZip->GetNoEntries ( );
}

void DeleteFileBlock ( int iID )
{
	// delete the file block

	// check ID of file block
	CheckID ( iID );

	// check the data
	CheckData ( iID );

	// clear the file block
	SAFE_DELETE ( g_FileBlocks [ iID ].pZip );
}

void PerformCheckListForFileBlockData ( int iID )
{
	// fill the checklist with a list of files contained within
	// the file block

	// check ID of file block
	CheckID ( iID );

	// ensure the file block is valid
	CheckData ( iID );

	if ( !g_pGlob )
	{
		Error ( 1 );	
		return;
	}

	// set checklist properties
	g_pGlob->checklistexists     = true;											// set exist flag on
	g_pGlob->checklisthasstrings = true;											// set strings on
	g_pGlob->checklisthasvalues  = true;											// set strings on
	g_pGlob->checklistqty        = g_FileBlocks [ iID ].pZip->GetNoEntries ( );	// set the number of entries in the checklist

	// go through each item in the checklist
	for ( int i = 0; i < g_pGlob->checklistqty; i++ )
	{
		// expand the checklist so we can add a string
		GlobExpandChecklist ( i, 255 );

		// local variables
		CZipFileHeader	info;	// zip file header
		CString			file;	// string name of the file

		// get the file information
		g_FileBlocks [ iID ].pZip->GetFileInfo ( info, ( WORD ) i );

		// get the file name from the file information structure
		file = info.GetFileName ( );

		// copy the file name into the checklist
		strcpy ( g_pGlob->checklist [ i ].string, file );
	}
}

void LoadObjectFromBlock ( int iID, DWORD dwFile, int iObject )
{
	// check ID of file block
	CheckID ( iID );

	// load an object from a block
	AddOrObtainResourceFromBlock ( iID, iObject, dwFile, 0, 1 );
}

void LoadBitmapFromBlock ( int iID, DWORD dwFile, int iBitmap )
{
	// check ID of file block
	CheckID ( iID );

	// load an image from the block
	AddOrObtainResourceFromBlock ( iID, iBitmap, dwFile, 2, 1 );
}

void LoadImageFromBlock	( int iID, DWORD dwFile, int iImage )
{
	// check ID of file block
	CheckID ( iID );

	// load an image from the block
	AddOrObtainResourceFromBlock ( iID, iImage, dwFile, 1, 1 );
}

void LoadSoundFromBlock	( int iID, DWORD dwFile, int iSound )
{

}

void LoadFileFromBlock ( int iID, DWORD dwFile, int iFile )
{

}

void LoadMemblockFromBlock ( int iID, DWORD dwFile, int iMemblock )
{

}

void LoadAnimationFromBlock	( int iID, DWORD dwFile, int iAnimation )
{

}

int GetFileBlockExists ( int iID )
{
	// return true if the file block exists

	// check ID of file block
	CheckID ( iID );

	if ( g_FileBlocks [ iID ].pZip )
		return 1;

	return 0;
}

int GetFileBlockSize ( int iID )
{
	// get the size of the file block

	// check ID of file block
	CheckID ( iID );

	// check the data
	CheckData ( iID );

	// save the file block
	SaveFileBlock ( iID );

	// get size
	DWORD dwSize;

	HANDLE hfile = CreateFile ( g_FileBlocks [ iID ].szName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hfile == INVALID_HANDLE_VALUE )
		return -1;
	
	dwSize = GetFileSize ( hfile, NULL );
	CloseHandle ( hfile );

	// return the size
	return ( int ) dwSize;
}

int GetFileBlockCount ( int iID )
{
	// check ID of file block
	CheckID ( iID );

	// ensure the file block is valid
	CheckData ( iID );

	return g_FileBlocks [ iID ].pZip->GetNoEntries ( );
}

int GetFileBlockDataExists ( int iID, DWORD dwFile )
{
	// check ID of file block
	CheckID   ( iID );
	CheckData ( iID );
	int iIndex = g_FileBlocks [ iID ].pZip->FindFile ( ( char* ) dwFile );

	// lee - 240306 - u6b4 - true if not negative
	if ( iIndex>=0 )
		return 1;

	// not found
	return 0;
}

int GetFileBlockKey	( int iID )
{
	// check ID of file block
	CheckID   ( iID );
	CheckData ( iID );

	return 1;
}

int GetFileBlockCompression ( int iID )
{
	// check ID of file block
	CheckID ( iID );

	CheckData ( iID );

	return g_FileBlocks [ iID ].iCompression;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
