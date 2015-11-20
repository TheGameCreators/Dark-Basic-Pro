#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#include <windows.h>
#include <commctrl.h>
#include <dplay8.h>
#include <stdio.h>
#include <tchar.h>

// Global Shared Data Pointer (passed in from core)
GlobStruct*				g_pGlob							= NULL;
PTR_FuncCreateStr		g_pCreateDeleteStringFunction	= NULL;


// Used to call memblockDLL for memblock return ptr function
typedef int						( *MEMBLOCKS_GetMemblockExist   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockPtr   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockSize  ) ( int );
typedef void					( *MEMBLOCKS_MemblockFromMedia ) ( int, int );
typedef void					( *MEMBLOCKS_MediaFromMemblock ) ( int, int );
typedef void					( *MEMBLOCKS_MakeMemblock  ) ( int, int );
typedef void					( *MEMBLOCKS_DeleteMemblock ) ( int );
MEMBLOCKS_GetMemblockExist		g_Memblock_GetMemblockExist;
MEMBLOCKS_GetMemblockPtr		g_Memblock_GetMemblockPtr;
MEMBLOCKS_GetMemblockSize		g_Memblock_GetMemblockSize;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromImage;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromBitmap;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromSound;
MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromMesh;
MEMBLOCKS_MediaFromMemblock		g_Memblock_ImageFromMemblock;
MEMBLOCKS_MediaFromMemblock		g_Memblock_BitmapFromMemblock;
MEMBLOCKS_MediaFromMemblock		g_Memblock_SoundFromMemblock;
MEMBLOCKS_MediaFromMemblock		g_Memblock_MeshFromMemblock;
MEMBLOCKS_MakeMemblock			g_Memblock_MakeMemblock;
MEMBLOCKS_DeleteMemblock		g_Memblock_DeleteMemblock;

// Used to call mediaDLLs
typedef int						( *MEDIA_GetExist   ) ( int );
MEDIA_GetExist					g_Image_GetExist;
MEDIA_GetExist					g_Bitmap_GetExist;
MEDIA_GetExist					g_Sound_GetExist;
MEDIA_GetExist					g_Basic3D_GetExist;

// local checklist work vars
bool							g_bCreateChecklistNow			= false;
DWORD							g_dwMaxStringSizeInEnum			= 0;