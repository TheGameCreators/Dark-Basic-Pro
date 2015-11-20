
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define INITGUID
#endif

#include <windows.h> 
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include ".\..\core\DBDLLArray.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBitmap.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSound.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
#endif

#include "CMemblocks.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// leefix - 131108 - DarkGDK does not want another global instance
#ifdef DARKSDK_COMPILE
	#define DBPRO_GLOBAL static
#endif

// Memblock Array Strcutre
#define					MEMBLOCKSIZE 258
DBPRO_GLOBAL DWORD					gpMemblockSize[MEMBLOCKSIZE];
DBPRO_GLOBAL LPSTR					gpMemblock[MEMBLOCKSIZE];

// Global Shared Data Pointer (passed in from core)
DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;
DBPRO_GLOBAL PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;

// Internal Work Variables
DBPRO_GLOBAL HWND					GlobalHwndCopy					= NULL;
DBPRO_GLOBAL char					m_pWorkString[_MAX_PATH];

// Declarations for memblock access functions
typedef void 				( *RetVoidGetBitmapDataPFN )   ( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
typedef void 				( *RetVoidSetBitmapDataPFN )   ( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize );
typedef void 				( *RetVoidGetSoundDataPFN )   ( int iID, DWORD* dwBitsPerSecond, DWORD* Frequency, DWORD* Duration, LPSTR* pData, DWORD* dwDataSize, bool bLockData, WAVEFORMATEX* wave );
typedef void 				( *RetVoidSetSoundDataPFN )   ( int iID, DWORD dwBitsPerSecond, DWORD Frequency, DWORD Duration, LPSTR pData, DWORD dwDataSize,  WAVEFORMATEX wave );
typedef void 				( *RetVoidGetMeshDataPFN )   ( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
typedef void 				( *RetVoidSetMeshDataPFN )   ( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax );

DBPRO_GLOBAL RetVoidGetBitmapDataPFN		g_MEM_GetBitmapData			= NULL;
DBPRO_GLOBAL RetVoidSetBitmapDataPFN		g_MEM_SetBitmapData 		= NULL;
DBPRO_GLOBAL RetVoidGetBitmapDataPFN		g_MEM_GetImageData			= NULL;
DBPRO_GLOBAL RetVoidSetBitmapDataPFN		g_MEM_SetImageData 			= NULL;
DBPRO_GLOBAL RetVoidGetSoundDataPFN		g_MEM_GetSoundData			= NULL;
DBPRO_GLOBAL RetVoidSetSoundDataPFN		g_MEM_SetSoundData 			= NULL;
DBPRO_GLOBAL RetVoidGetMeshDataPFN		g_MEM_GetMeshData			= NULL;
DBPRO_GLOBAL RetVoidSetMeshDataPFN		g_MEM_SetMeshData 			= NULL;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//
// Internal Functions
//

DARKSDK void Constructor ( void )
{
	// Clear Memblock Structures
	for(int m=0; m<MEMBLOCKSIZE; m++)
	{
		gpMemblockSize[m]=0;
		gpMemblock[m]=NULL;
	}
}

DARKSDK void Destructor ( void )
{
	// Ensure all memblocks are released
	for(int m=0; m<MEMBLOCKSIZE; m++)
	{
		if(gpMemblock[m])
		{
			//GlobalFree(gpMemblock[m]);
			delete gpMemblock[m];
			gpMemblockSize[m]=0;
			gpMemblock[m]=NULL;
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

	// Copy hWnd from global
	GlobalHwndCopy = g_pGlob->hWnd;

	// Construct links to memblock access functions
	#ifndef DARKSDK_COMPILE
		if(g_pGlob->g_Bitmap)
		{
			g_MEM_GetBitmapData = ( RetVoidGetBitmapDataPFN ) GetProcAddress ( g_pGlob->g_Bitmap, "?GetBitmapData@@YAXHPAK00PAPAD0_N@Z" );
			g_MEM_SetBitmapData = ( RetVoidSetBitmapDataPFN ) GetProcAddress ( g_pGlob->g_Bitmap, "?SetBitmapData@@YAXHKKKPADK@Z" );
		}
		if(g_pGlob->g_Image)
		{
			g_MEM_GetImageData = ( RetVoidGetBitmapDataPFN ) GetProcAddress ( g_pGlob->g_Image, "?GetImageData@@YAXHPAK00PAPAD0_N@Z" );
			g_MEM_SetImageData = ( RetVoidSetBitmapDataPFN ) GetProcAddress ( g_pGlob->g_Image, "?SetImageData@@YAXHKKKPADK@Z" );
		}
		if(g_pGlob->g_Sound)
		{
			// mike - 300305 - updated function pointer
			g_MEM_GetSoundData = ( RetVoidGetSoundDataPFN ) GetProcAddress ( g_pGlob->g_Sound, "?GetSoundData@@YAXHPAK00PAPAD0_NPAUtWAVEFORMATEX@@@Z" );
			g_MEM_SetSoundData = ( RetVoidSetSoundDataPFN ) GetProcAddress ( g_pGlob->g_Sound, "?SetSoundData@@YAXHKKKPADKUtWAVEFORMATEX@@@Z" );
		}
		if(g_pGlob->g_Basic3D)
		{
			g_MEM_GetMeshData = ( RetVoidGetMeshDataPFN ) GetProcAddress ( g_pGlob->g_Basic3D, "?GetMeshData@@YAXHPAK00PAPAD0_N@Z" );
			g_MEM_SetMeshData = ( RetVoidSetMeshDataPFN ) GetProcAddress ( g_pGlob->g_Basic3D, "?SetMeshData@@YAXHKKPADK@Z" );
		}
	#else
		g_MEM_GetBitmapData = dbGetBitmapData;
		g_MEM_SetBitmapData = dbSetBitmapData;
		
		g_MEM_GetImageData = dbGetImageData;
		g_MEM_SetImageData = dbSetImageData;
		
		g_MEM_GetSoundData = dbGetSoundData;
		g_MEM_SetSoundData = dbSetSoundData;
		
		g_MEM_GetMeshData = dbGetMeshData;
		g_MEM_SetMeshData = dbSetMeshData;
	#endif
	
}

DARKSDK void RefreshD3D ( int iMode )
{
}

DARKSDK void ReturnMemblockPtrs( LPVOID* pSizeArray, LPVOID* pDataArray )
{
	// Pass memblock arrays to other DLLs if requested
	*pSizeArray = gpMemblockSize;
	*pDataArray = gpMemblock;
}

DBPRO_GLOBAL LPSTR GetReturnStringFromWorkString(void)
{
	LPSTR pReturnString=NULL;
	if(m_pWorkString)
	{
		DWORD dwSize=strlen(m_pWorkString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, m_pWorkString);
	}
	return pReturnString;
}

DARKSDK BOOL DB_MemblockFromBitmap(int mbi, int bitmapindex)
{
	BOOL bResult=TRUE;
	LPSTR pData=NULL;
	DWORD dwDataSize=0;
	DWORD dwWidth=0, dwHeight=0, dwDepth=0;

	// Get bitmap data
	if(g_MEM_GetBitmapData) g_MEM_GetBitmapData ( bitmapindex, &dwWidth, &dwHeight, &dwDepth, &pData, &dwDataSize, true );
	if(bitmapindex==0 || pData)
	{
		// lee - 220206 - u60 - Free any old memblock
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblock[mbi]=NULL;
		}

		// Make memblock from bitmap data
		gpMemblockSize[mbi] = (sizeof(DWORD)*3) + dwDataSize;
		//gpMemblock[mbi]=(char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
		gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
		memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );
		if(gpMemblock[mbi])
		{
			// Bitmap Header (DWORD x3)
			*((DWORD*)gpMemblock[mbi]+0)=dwWidth;
			*((DWORD*)gpMemblock[mbi]+1)=dwHeight;
			*((DWORD*)gpMemblock[mbi]+2)=dwDepth;

			// Bitmap Data
			char* pMem = gpMemblock[mbi]+12;
			memcpy(pMem, pData, dwDataSize);
		}
		else
		{
			RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			bResult=FALSE;
		}

		// Free bitmap data
		if(g_MEM_GetBitmapData) g_MEM_GetBitmapData ( 0, 0, 0, 0, &pData, 0, false );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_BitmapFromMemblock(int mbi, int bitmapindex)
{
	BOOL bResult=TRUE;
	if(gpMemblock[mbi])
	{
		// MEMBLOCK Bitmap Header (DWORD x3)
		DWORD dwWidth = *((DWORD*)gpMemblock[mbi]+0);
		DWORD dwHeight = *((DWORD*)gpMemblock[mbi]+1);
		DWORD dwDepth = *((DWORD*)gpMemblock[mbi]+2);

		// MEMBLOCK Bitmap Data
		char* pData = gpMemblock[mbi]+12;
		DWORD bytesperpixel=(dwDepth/8);
		DWORD dwDataSize = dwWidth * dwHeight * bytesperpixel;

		// Recreate bitmap from memblock data
		if(g_MEM_SetBitmapData) g_MEM_SetBitmapData ( bitmapindex, dwWidth, dwHeight, dwDepth, pData, dwDataSize );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_MemblockFromImage(int mbi, int imageindex)
{
	BOOL bResult=TRUE;
	LPSTR pData=NULL;
	DWORD dwDataSize=0;
	DWORD dwWidth=0, dwHeight=0, dwDepth=0;

	// Get bitmap data
	if(g_MEM_GetImageData) g_MEM_GetImageData ( imageindex, &dwWidth, &dwHeight, &dwDepth, &pData, &dwDataSize, true );
	if(pData)
	{
		// lee - 220206 - u60 - Free any old memblock
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblock[mbi]=NULL;
		}

		// Make memblock from image data
		gpMemblockSize[mbi] = (sizeof(DWORD)*3) + dwDataSize;
		//gpMemblock[mbi]=(char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
		gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
		memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );
		if(gpMemblock[mbi])
		{
			// Image Header (DWORD x3)
			*((DWORD*)gpMemblock[mbi]+0)=dwWidth;
			*((DWORD*)gpMemblock[mbi]+1)=dwHeight;
			*((DWORD*)gpMemblock[mbi]+2)=dwDepth;

			// Image Data
			char* pMem = gpMemblock[mbi]+12;
			memcpy(pMem, pData, dwDataSize);
		}
		else
		{
			RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			bResult=FALSE;
		}

		// Free bitmap data
		if(g_MEM_GetImageData) g_MEM_GetImageData ( 0, 0, 0, 0, &pData, 0, false );
	}
	else
	{
		// leefix = 250604 - u54 - special size of one means data there but cannot lock (protected such as render surface)
		if ( dwDataSize==1 )
			RunTimeError(RUNTIMEERROR_IMAGELOCKED);
		else
			RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);

		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_ImageFromMemblock(int mbi, int imageindex)
{
	BOOL bResult=TRUE;
	if(gpMemblock[mbi])
	{
		// MEMBLOCK Image Header (DWORD x3)
		DWORD dwWidth = *((DWORD*)gpMemblock[mbi]+0);
		DWORD dwHeight = *((DWORD*)gpMemblock[mbi]+1);
		DWORD dwDepth = *((DWORD*)gpMemblock[mbi]+2);

		// MEMBLOCK Image Data
		char* pData = gpMemblock[mbi]+12;
		DWORD bytesperpixel=(dwDepth/8);
		DWORD dwDataSize = dwWidth * dwHeight * bytesperpixel;

		// Recreate image from memblock data
		if(g_MEM_SetImageData) g_MEM_SetImageData ( imageindex, dwWidth, dwHeight, dwDepth, pData, dwDataSize );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_MemblockFromSound(int mbi, int soundindex)
{
	BOOL bResult=TRUE;
	LPSTR pData=NULL;
	DWORD dwDataSize=0;
	DWORD dwBitsPerSecond=0;
	DWORD Frequency=0;
	DWORD Duration=0;

	// mike - 300305 - need more info for waveformat
	WAVEFORMATEX wfx;

	// mike - 300305 - pass in wave structure
	if(g_MEM_GetSoundData) g_MEM_GetSoundData ( soundindex, &dwBitsPerSecond, &Frequency, &Duration, &pData, &dwDataSize, true, &wfx );
	if(soundindex==0 || pData)
	{
		// lee - 220206 - u60 - Free any old memblock
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblock[mbi]=NULL;
		}

		// mike - 300305 - updated sound layout
		gpMemblockSize[mbi] = (sizeof(DWORD)*7) + dwDataSize;
		//gpMemblock[mbi]=(char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
		gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
		memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );

		if(gpMemblock[mbi])
		{
			// Sound Header (DWORD x3)
			//*((DWORD*)gpMemblock[mbi]+0)=dwBitsPerSecond;
			//*((DWORD*)gpMemblock[mbi]+1)=Frequency;
			//*((DWORD*)gpMemblock[mbi]+2)=Duration;

			// mike - 300305 - updated sound layout
			*((DWORD*)gpMemblock[mbi]+0)=wfx.wFormatTag;
			*((DWORD*)gpMemblock[mbi]+1)=wfx.nChannels;
			*((DWORD*)gpMemblock[mbi]+2)=wfx.nSamplesPerSec;
			*((DWORD*)gpMemblock[mbi]+3)=wfx.nAvgBytesPerSec;
			*((DWORD*)gpMemblock[mbi]+4)=wfx.nBlockAlign;
			*((DWORD*)gpMemblock[mbi]+5)=wfx.wBitsPerSample;
			*((DWORD*)gpMemblock[mbi]+6)=wfx.cbSize;

			
			// Sound Data
			//char* pMem = gpMemblock[mbi]+12;

			char* pMem = gpMemblock[mbi]+28;
			memcpy(pMem, pData, dwDataSize);
		}
		else
		{
			RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			bResult=FALSE;
		}

		// Free sound data
		if(g_MEM_GetSoundData) g_MEM_GetSoundData ( 0, 0, 0, 0, &pData, 0, false, 0 );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_SoundFromMemblock(int mbi, int soundindex)
{
	BOOL bResult=TRUE;
	if(gpMemblock[mbi])
	{
		// mike - 300305 - need more info for waveformat
		WAVEFORMATEX wfx;
		wfx.wFormatTag = (WORD)*((DWORD*)gpMemblock[mbi]+0);
		wfx.nChannels = (WORD)*((DWORD*)gpMemblock[mbi]+1);
		// leefix - 121008 - u70 - WAVEFORMAT uses DWORDs here
		//wfx.nSamplesPerSec = (WORD)*((DWORD*)gpMemblock[mbi]+2);
		//wfx.nAvgBytesPerSec = (WORD)*((DWORD*)gpMemblock[mbi]+3);
		wfx.nSamplesPerSec = *((DWORD*)gpMemblock[mbi]+2);
		wfx.nAvgBytesPerSec = *((DWORD*)gpMemblock[mbi]+3);
		wfx.nBlockAlign = (WORD)*((DWORD*)gpMemblock[mbi]+4);
		wfx.wBitsPerSample = (WORD)*((DWORD*)gpMemblock[mbi]+5);
		wfx.cbSize = (WORD)*((DWORD*)gpMemblock[mbi]+6);
		
		// MEMBLOCK sound Header (DWORD x3)
		DWORD dwBitsPerSecond = *((DWORD*)gpMemblock[mbi]+0);
		DWORD Frequency = *((DWORD*)gpMemblock[mbi]+1);
		DWORD Duration = *((DWORD*)gpMemblock[mbi]+2);

		// MEMBLOCK sound Data
		//char* pData = gpMemblock[mbi]+12;
		//DWORD dwDataSize=gpMemblockSize[mbi]-12;

		char* pData = gpMemblock[mbi]+28;
		DWORD dwDataSize=gpMemblockSize[mbi]-28;

		// Recreate sound from memblock data
		if(g_MEM_SetSoundData) g_MEM_SetSoundData ( soundindex, dwBitsPerSecond, Frequency, Duration, pData, dwDataSize, wfx );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_MemblockFromMesh(int mbi, int meshid)
{
	BOOL bResult=TRUE;
	LPSTR pData=NULL;
	DWORD dwDataSize=0;
	DWORD dwFVF=0, dwFVFSize=0, dwVertMax=0;

	// Get Mesh data
	if(g_MEM_GetMeshData) g_MEM_GetMeshData ( meshid, &dwFVF, &dwFVFSize, &dwVertMax, &pData, &dwDataSize, true );
	if(meshid==0 || pData)
	{
		// lee - 220206 - u60 - Free any old memblock
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblock[mbi]=NULL;
		}

		// Make memblock from Mesh data
		gpMemblockSize[mbi] = (sizeof(DWORD)*3) + dwDataSize;
		//gpMemblock[mbi] = (char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
		gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
		memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );
		if(gpMemblock[mbi])
		{
			// Mesh Header (DWORD x3)
			*((DWORD*)gpMemblock[mbi]+0)=dwFVF;
			*((DWORD*)gpMemblock[mbi]+1)=dwFVFSize;
			*((DWORD*)gpMemblock[mbi]+2)=dwVertMax;

			// Mesh Data
			char* pMem = gpMemblock[mbi]+12;
			memcpy(pMem, pData, dwDataSize);
		}
		else
		{
			RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			bResult=FALSE;
		}

		// Free mesh data
		if(g_MEM_GetMeshData) g_MEM_GetMeshData ( 0, 0, 0, 0, &pData, 0, false );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_B3DMESHNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

DARKSDK BOOL DB_MeshFromMemblock(int mbi, int meshid)
{
	BOOL bResult=TRUE;
	if(gpMemblock[mbi])
	{
		// MEMBLOCK Mesh Header (DWORD x3)
		DWORD dwFVF = *((DWORD*)gpMemblock[mbi]+0);
		DWORD dwFVFSize = *((DWORD*)gpMemblock[mbi]+1);
		DWORD dwVertMax = *((DWORD*)gpMemblock[mbi]+2);

		// MEMBLOCK Mesh Data
		char* pData = gpMemblock[mbi]+12;
		DWORD dwDataSize = dwVertMax * dwFVFSize;

		// Recreate bitmap from memblock data
		if(g_MEM_SetMeshData) g_MEM_SetMeshData ( meshid, dwFVF, dwFVFSize, pData, dwVertMax );
	}
	else
	{
		RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
		bResult=FALSE;
	}
	return bResult;
}

//
// Command Functions
//

DARKSDK void MakeMemblock( int mbi, int size )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi]==NULL)
		{
			if(size>=1)
			{
				gpMemblockSize[mbi]=size;
				//gpMemblock[mbi]=(char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
				gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
				memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );
				if(!gpMemblock[mbi])
					RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKSIZEINVALID);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKALREADYEXISTS);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void DeleteMemblock( int mbi )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblock[mbi]=NULL;
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK int MemblockExist( int mbi )
{
	int res=0;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi]) res=1;
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return res;
}

DARKSDK DWORD GetMemblockPtr( int mbi )
{
	char* ptr=NULL;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
			ptr=gpMemblock[mbi];
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return (DWORD)(char*)ptr;
}

DARKSDK int GetMemblockSize( int mbi )
{
	int size=0;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
			size=gpMemblockSize[mbi];
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return size;
}

DARKSDK void CopyMemblock( int mbiF, int mbiT, int posF, int posT, int size )
{
	if(mbiF>=1 && mbiF<=255)
	{
		if(gpMemblock[mbiF])
		{
			if(mbiT>=1 && mbiT<=255)
			{
				if(gpMemblock[mbiT])
				{
					if(posF>=0 && posF<(int)gpMemblockSize[mbiF])
					{
						if(posT>=0 && posT<(int)gpMemblockSize[mbiT])
						{
							if(posF+size<=(int)gpMemblockSize[mbiF]
							&& posT+size<=(int)gpMemblockSize[mbiT])
							{
								// Copy!
								memcpy(gpMemblock[mbiT]+posT, gpMemblock[mbiF]+posF, size);
							}
							else
								RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
						}
						else
							RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
					}
					else
						RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void WriteMemblockByte( int mbi, int pos, int data )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi])
			{
				if(data>=0 && data<=255)
				{
					// Write byte
					*(gpMemblock[mbi]+pos)=(unsigned char)data;
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTABYTE);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK int ReadMemblockByte( int mbi, int pos )
{
	unsigned char data=0;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi])
			{
				// Read byte
				data = *(gpMemblock[mbi]+pos);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return (int)data;
}

DARKSDK void WriteMemblockWord( int mbi, int pos, int data )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-1)
			{
				if(data>=0 && data<=65535)
				{
					// Write WORD
					*(WORD*)((gpMemblock[mbi]+pos))=(unsigned short)data;
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTAWORD);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK int ReadMemblockWordLLL( int mbi, int pos )
{
	unsigned short data=0;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-1)
			{
				// Read WORD
				data = *(unsigned short*)((gpMemblock[mbi]+pos));
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return (int)data;
}

DARKSDK void WriteMemblockDWord( int mbi, int pos, DWORD data )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-3)
			{
// LEEFIX - 171102 - Lee, DWORDs a re a little bigger :)
//				if(data>=0 && data<=2147483647)
//				{
					// Write DWORD
					*(DWORD*)((gpMemblock[mbi]+pos))=data;
//				}
//				else
//					RunTimeError(RUNTIMEERROR_MEMBLOCKNOTADWORD);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK DWORD ReadMemblockDWord( int mbi, int pos )
{
	DWORD data=0;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-3)
			{
				// Read DWORD
				data = *(DWORD*)((gpMemblock[mbi]+pos));
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return (int)data;
}

DARKSDK void WriteMemblockFloat( int mbi, int pos, float data )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-3)
			{
				// Write FLOAT
				*(DWORD*)((gpMemblock[mbi]+pos))=*(DWORD*)&data;
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK DWORD ReadMemblockFloat( int mbi, int pos )
{
	float data=0.0f;
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			if(pos>=0 && pos<(int)gpMemblockSize[mbi]-3)
			{
				// Read FLOAT
				data = *(float*)(DWORD*)((gpMemblock[mbi]+pos));
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKOUTSIDERANGE);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);

	return *(DWORD*)&data;
}

DARKSDK void CreateMemblockFromBitmap( int mbi, int bitmapindex )
{
	if(mbi>=1 && mbi<=257)
	{
		if(bitmapindex>=0 && bitmapindex<MAXIMUMVALUE)
		{
			DB_MemblockFromBitmap(mbi, bitmapindex);
		}
		else
			RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void CreateBitmapFromMemblock( int bitmapindex, int mbi )
{
	if(bitmapindex>=0 && bitmapindex<MAXIMUMVALUE)
	{
		if(mbi>=1 && mbi<=257)
		{
			DB_BitmapFromMemblock(mbi, bitmapindex);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
}

DARKSDK void CreateMemblockFromMesh( int mbi, int meshid )
{
	if(mbi>=1 && mbi<=257)
	{
		if(meshid>0 && meshid<MAXIMUMVALUE)
		{
			DB_MemblockFromMesh(mbi, meshid);
		}
		else
			RunTimeError(RUNTIMEERROR_B3DMESHNUMBERILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void CreateMeshFromMemblock( int meshid, int mbi )
{
	if(meshid>0 && meshid<MAXIMUMVALUE)
	{
		if(mbi>=1 && mbi<=257)
		{
			DB_MeshFromMemblock(mbi, meshid);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_B3DMESHNUMBERILLEGAL);
}

DARKSDK void ChangeMeshFromMemblock( int meshid, int mbi )
{
	if(meshid>0 && meshid<MAXIMUMVALUE)
	{
		if(mbi>=1 && mbi<=257)
		{
			DB_MeshFromMemblock(mbi, meshid);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_B3DMESHNUMBERILLEGAL);
}

DARKSDK void CreateMemblockFromSound( int mbi, int soundindex )
{
	if(mbi>=1 && mbi<=257)
	{
		if(soundindex>=1 && soundindex<MAXIMUMVALUE)
		{
			DB_MemblockFromSound(mbi, soundindex);
		}
		else
			RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void CreateSoundFromMemblock( int soundindex, int mbi )
{
	if(soundindex>=1 && soundindex<MAXIMUMVALUE)
	{
		if(mbi>=1 && mbi<=257)
		{
			DB_SoundFromMemblock(mbi, soundindex);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
}

DARKSDK void CreateMemblockFromImage( int mbi, int imageindex )
{
	if(mbi>=1 && mbi<=257)
	{
		if(imageindex>=1 && imageindex<=MAXIMUMVALUE)
		{
			DB_MemblockFromImage(mbi, imageindex);
 		}
		else
			RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);	
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void CreateImageFromMemblock( int imageindex, int mbi )
{
	if(imageindex>=1 && imageindex<=MAXIMUMVALUE)
	{
		if(mbi>=1 && mbi<=257)
		{
			if(gpMemblock[mbi])
			{
				DB_ImageFromMemblock(mbi, imageindex);
			}
			else
				RunTimeError(RUNTIMEERROR_MEMBLOCKNOTEXIST);
		}
		else
			RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
	}
	else
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);	
}

DARKSDK void CreateMemblockFromArray( int mbi, DWORD dwAllocation )
{
	if(mbi>=1 && mbi<=257)
	{
		// If no array, leave now
		if ( dwAllocation )
		{
			// leeadd - 140104 - create a memblock from entire contents of an array

			// obtain array data ptr and size
			LPSTR pData = NULL;
			DWORD dwDataSizeInBytes = 0;
			DWORD dwSizeOfTable = *((DWORD*)dwAllocation-4);
			if(dwSizeOfTable>0)
			{
				// Get Array Information
				LPSTR pArrayPtr = ((LPSTR)dwAllocation)-HEADERSIZEINBYTES;
				DWORD* pHeader	= (DWORD*)(pArrayPtr);
				DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
				DWORD dwSizeOfArray = pHeader[10];
				DWORD dwSizeOfOneDataItem = pHeader[11];
				DWORD dwTypeValueOfOneDataItem = pHeader[12];
				DWORD dwInternalIndex = pHeader[13];
				DWORD dwRefSizeInBytes = dwSizeOfArray * 4;
				DWORD dwFlagSizeInBytes = dwSizeOfArray * 1;
				dwDataSizeInBytes = dwSizeOfArray * dwSizeOfOneDataItem;
				DWORD* pRef = (DWORD*)(pArrayPtr+dwHeaderSizeInBytes);
				LPSTR pFlag = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes);
				pData = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes+dwFlagSizeInBytes);

				// create memblock
				gpMemblockSize[mbi] = dwDataSizeInBytes;
				//gpMemblock[mbi]=(char*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, gpMemblockSize[mbi]);
				gpMemblock[mbi]=(char*)new char[gpMemblockSize[mbi]];
				memset ( gpMemblock[mbi], 0, gpMemblockSize[mbi] );
				if(gpMemblock[mbi])
				{
					// copy data to memblock
					char* pMem = gpMemblock[mbi];
					memcpy(pMem, pData, dwDataSizeInBytes);
				}
				else
					RunTimeError(RUNTIMEERROR_MEMBLOCKCREATIONFAILED);
			}
			else
				RunTimeError(RUNTIMEERROR_ARRAYEMPTY);	
 		}
		else
			RunTimeError(RUNTIMEERROR_INVALIDARRAY);	
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

DARKSDK void CreateArrayFromMemblock( DWORD dwAllocation, int mbi )
{
	if(mbi>=1 && mbi<=257)
	{
		// If no array, leave now
		if ( dwAllocation )
		{
			// obtain array data ptr and size
			LPSTR pData = NULL;
			DWORD dwDataSizeInBytes = 0;
			DWORD dwSizeOfTable = *((DWORD*)dwAllocation-4);
			if(dwSizeOfTable>0)
			{
				// Get Array Information
				LPSTR pArrayPtr = ((LPSTR)dwAllocation)-HEADERSIZEINBYTES;
				DWORD* pHeader	= (DWORD*)(pArrayPtr);
				DWORD dwHeaderSizeInBytes = HEADERSIZEINBYTES;
				DWORD dwSizeOfArray = pHeader[10];
				DWORD dwSizeOfOneDataItem = pHeader[11];
				DWORD dwTypeValueOfOneDataItem = pHeader[12];
				DWORD dwInternalIndex = pHeader[13];
				DWORD dwRefSizeInBytes = dwSizeOfArray * 4;
				DWORD dwFlagSizeInBytes = dwSizeOfArray * 1;
				dwDataSizeInBytes = dwSizeOfArray * dwSizeOfOneDataItem;
				DWORD* pRef = (DWORD*)(pArrayPtr+dwHeaderSizeInBytes);
				LPSTR pFlag = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes);
				pData = (LPSTR)(pArrayPtr+dwHeaderSizeInBytes+dwRefSizeInBytes+dwFlagSizeInBytes);

				// memblock to array
				if(gpMemblock[mbi])
				{
					// copy memblock data to array data
					char* pMem = gpMemblock[mbi];
					memcpy(pData, pMem, dwDataSizeInBytes);
				}
			}
			else
				RunTimeError(RUNTIMEERROR_ARRAYEMPTY);	
 		}
		else
			RunTimeError(RUNTIMEERROR_INVALIDARRAY);	
	}
	else
		RunTimeError(RUNTIMEERROR_MEMBLOCKRANGEILLEGAL);
}

// Memblock External Access

DARKSDK LPSTR ExtMakeMemblock ( int mbi, DWORD size )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblockSize[mbi]=0;
			gpMemblock[mbi]=NULL;
		}
	}
	MakeMemblock ( mbi, size );
	LPSTR pMem=gpMemblock[mbi];
	return pMem;
}

DARKSDK void  ExtFreeMemblock ( int mbi )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			//GlobalFree(gpMemblock[mbi]);
			delete gpMemblock[mbi];
			gpMemblockSize[mbi]=0;
			gpMemblock[mbi]=NULL;
		}
	}
}

DARKSDK DWORD ExtGetMemblockSize ( int mbi )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			return gpMemblockSize[mbi];
		}
	}
	return 0;
}

DARKSDK void  ExtSetMemblockSize ( int mbi, DWORD size )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			gpMemblockSize[mbi]=size;
		}
	}
}

DARKSDK LPSTR ExtGetMemblockPtr ( int mbi )
{
	if(mbi>=1 && mbi<=257)
	{
		if(gpMemblock[mbi])
		{
			return gpMemblock[mbi];
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorMemblocks ( void )
{
	Constructor ( );
}

void DestructorMemblocks ( void )
{
	Destructor ( );
}

void SetErrorHandlerMemblocks ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataMemblocks( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DMemblocks ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbReturnMemblockPtrs ( LPVOID* pSizeArray, LPVOID* pDataArray )
{
	ReturnMemblockPtrs ( pSizeArray, pDataArray );
}

LPSTR dbExtMakeMemblock ( int mbi, DWORD size )
{
	return ExtMakeMemblock ( mbi, size );
}

void dbExtFreeMemblock ( int mbi )
{
	ExtFreeMemblock ( mbi );
}

DWORD dbExtGetMemblockSize ( int mbi )
{
	return ExtGetMemblockSize ( mbi );
}

void dbExtSetMemblockSize ( int mbi, DWORD size )
{
	ExtSetMemblockSize ( mbi, size );
}

LPSTR dbExtGetMemblockPtr ( int mbi )
{
	return ExtGetMemblockPtr ( mbi );
}

void dbMakeMemblock ( int mbi, int size )
{
	MakeMemblock ( mbi, size );
}

void dbDeleteMemblock ( int mbi )
{
	DeleteMemblock ( mbi );
}

int dbMemblockExist ( int mbi )
{
	return MemblockExist ( mbi );
}

DWORD dbGetMemblockPtr ( int mbi )
{
	return GetMemblockPtr ( mbi );
}

int dbGetMemblockSize ( int mbi )
{
	return GetMemblockSize ( mbi );
}

void dbCopyMemblock ( int mbiF, int mbiT, int posF, int posT, int size )
{
	CopyMemblock ( mbiF, mbiT, posF, posT,size );
}

void dbWriteMemblockByte ( int mbi, int pos, int data )
{
	WriteMemblockByte ( mbi, pos, data );
}

int dbMemblockByte ( int mbi, int pos )
{
	return ReadMemblockByte ( mbi, pos );
}

void dbWriteMemblockWord ( int mbi, int pos, int data )
{
	WriteMemblockWord ( mbi, pos, data );
}

int dbMemblockWord ( int mbi, int pos )
{
	return ReadMemblockWordLLL ( mbi, pos );
}

void dbWriteMemblockDWORD ( int mbi, int pos, DWORD data )
{
	WriteMemblockDWord ( mbi, pos, data );
}

DWORD dbMemblockDWORD ( int mbi, int pos )
{
	return ReadMemblockDWord ( mbi, pos );
}

void dbWriteMemblockFloat ( int mbi, int pos, float data )
{
	WriteMemblockFloat ( mbi, pos, data );
}

float dbMemblockFloat ( int mbi, int pos )
{
	DWORD dwReturn = ReadMemblockFloat ( mbi, pos );
	
	return *( float* ) &dwReturn;
}

void dbMakeMemblockFromBitmap ( int mbi, int bitmapindex )
{
	CreateMemblockFromBitmap ( mbi, bitmapindex );
}

void dbMakeBitmapFromMemblock ( int bitmapindex, int mbi )
{
	CreateBitmapFromMemblock ( bitmapindex, mbi );
}

void dbMakeMemblockFromMesh ( int mbi, int meshid )
{
	CreateMemblockFromMesh ( mbi, meshid );
}

void dbMakeMeshFromMemblock ( int meshid, int mbi )
{
	CreateMeshFromMemblock ( meshid, mbi );
}

void dbChangeMeshFromMemblock ( int meshid, int mbi )
{
	ChangeMeshFromMemblock ( meshid, mbi );
}

void dbMakeMemblockFromSound ( int mbi, int soundindex )
{
	CreateMemblockFromSound ( mbi, soundindex );
}

void dbMakeSoundFromMemblock ( int soundindex, int mbi )
{
	CreateSoundFromMemblock ( soundindex, mbi );
}

void dbMakeMemblockFromImage ( int mbi, int imageid )
{
	CreateMemblockFromImage ( mbi, imageid );
}

void dbMakeImageFromMemblock ( int imageid, int mbi )
{
	CreateImageFromMemblock ( imageid, mbi );
}

void dbMakeMemblockFromArray ( int mbi, DWORD arrayptr )
{
	CreateMemblockFromArray	( mbi, arrayptr );
}

void dbMakeArrayFromMemblock ( DWORD arrayptr, int mbi )
{
	CreateArrayFromMemblock ( arrayptr, mbi );
}

// lee - 300706 - GDK fixes
void dbWriteMemblockByte ( int mbi, int pos, unsigned char data ) { dbWriteMemblockByte ( mbi, pos, (int)data ); }
void dbWriteMemblockWord ( int mbi, int pos, WORD data ) { dbWriteMemblockWord ( mbi, pos, (int)data ); }
void dbWriteMemblockDword ( int mbi, int pos, DWORD data ) { dbWriteMemblockDWORD ( mbi, pos, data ); }
DWORD dbMemblockDword ( int mbi, int pos ) { return dbMemblockDWORD ( mbi, pos ); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////