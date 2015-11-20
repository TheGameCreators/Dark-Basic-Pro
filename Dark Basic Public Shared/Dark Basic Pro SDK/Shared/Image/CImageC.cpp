
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#define _HAS_ITERATOR_DEBUGGING 0

#include "cimagec.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include <stdio.h>
#include <direct.h>
#include <vector>
#include <map>
#include ".\..\Core\SteamCheckForWorkshop.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKSprites.h"
#endif

DBPRO_GLOBAL GlobStruct* g_pGlob = NULL;

namespace
{
    typedef std::map<int, tagData*>		ImageList_t;
    typedef ImageList_t::iterator		ImagePtr;

    // Image Block Globals

    bool								g_bImageBlockActive = false;
    LPSTR								g_iImageBlockFilename = NULL;
    LPSTR								g_iImageBlockRootPath = NULL;
    char								g_pImageBlockExcludePath[512];
    int									g_iImageBlockMode = -1;
    DWORD								g_dwImageBlockSize = 0;
    LPSTR								g_pImageBlockPtr = NULL;
    std::vector<LPSTR>					g_ImageBlockListFile;
    std::vector<DWORD>					g_ImageBlockListOffset;
    std::vector<DWORD>					g_ImageBlockListSize;

    typedef IDirect3DDevice9*			( *GFX_GetDirect3DDevicePFN ) ( void );
	typedef int							( *MEMBLOCKS_GetMemblockExist   ) ( int );
	typedef DWORD						( *MEMBLOCKS_GetMemblockPtr   ) ( int );
	typedef DWORD						( *MEMBLOCKS_GetMemblockSize  ) ( int );

    typedef void  						( *SPRITE_RetVoidParamInt3PFN ) ( int, int, int, float, float, int );
    typedef void  						( *SPRITE_RetVoidParamVoid ) ( void );
    typedef void  						( *BASIC3D_ClearObjectsOfTextureRef ) ( LPDIRECT3DTEXTURE9 );

    ImageList_t							m_List;
    LPDIRECT3D9							m_pDX			= NULL;				// pointer to D3D device
    LPDIRECT3DDEVICE9					m_pD3D			= NULL;				// pointer to D3D device
    int									m_iWidth		= 0;				// width of current texture
    int									m_iHeight		= 0;				// height of current texture
    int									m_iMipMapNum	= 9;				// default number of mipmaps
    int									m_iMemory		= 0;				// default memory pool
    bool								m_bSharing		= true;				// sharing flag
    bool								m_bMipMap		= true;				// mipmap on / off
    D3DCOLOR							m_Color         = D3DCOLOR_ARGB ( 255, 0, 0, 0 );							// default transparent color
    tagData*							m_ptr = NULL;
    int									m_CurrentId = 0;
    D3DFORMAT							g_DefaultD3Dformat;

    GFX_GetDirect3DDevicePFN			g_GFX_GetDirect3DDevice;	// get pointer to D3D device
	MEMBLOCKS_GetMemblockExist			g_Memblock_GetMemblockExist;
	MEMBLOCKS_GetMemblockPtr			g_Memblock_GetMemblockPtr;
	MEMBLOCKS_GetMemblockSize			g_Memblock_GetMemblockSize;

    SPRITE_RetVoidParamInt3PFN			g_Sprite_PasteImage;
    SPRITE_RetVoidParamVoid				g_Update_All_Sprites;
    BASIC3D_ClearObjectsOfTextureRef	g_Basic3D_ClearObjectsOfTextureRef;
	DWORD								g_dwMipMapGenMode;

    bool RemoveImage( int iID )
    {
        // Clear the cached value if the image being deleted is the current cached image.
        if (m_CurrentId == iID)
        {
            m_CurrentId = 0;
            m_ptr = NULL;
        }

        // Locate the image, and if found, release all of it's resources.
        ImagePtr pImage = m_List.find( iID );
        if (pImage != m_List.end())
        {
            SAFE_RELEASE( pImage->second->lpTexture );
            SAFE_DELETE( pImage->second->lpName );
            delete pImage->second;

            m_List.erase(pImage);

            return true;
        }

        return false;
    }

    bool UpdatePtrImage ( int iID )
    {
        // If the image required is not already cached, refresh the cached value
        if (!m_ptr || iID != m_CurrentId)
        {
            m_CurrentId = iID;

            ImagePtr p = m_List.find( iID );
            if (p == m_List.end())
            {
                m_ptr = NULL;
            }
            else
            {
                m_ptr = p->second;
            }
        }

        return m_ptr != NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void ConstructorD3D ( HINSTANCE hSetup )
{
	// setup the image library

	// check that the passed parameter does exist this should contain a reference to 
	// the setup library which in turn allows us to access Direct3D if it doesn't 
	// exist attempt to load in the DLL manually

	#ifndef DARKSDK_COMPILE
	if ( !hSetup )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
		if ( !hSetup ) Error ( "Cannot load setup library for image library" );
	}
	#endif

	// setup function pointers
	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	#endif
	
	m_pD3D                  = g_GFX_GetDirect3DDevice ( );
	m_pD3D->GetDirect3D(&m_pDX);

	g_Sprite_PasteImage		= NULL;	//filled later
	g_Update_All_Sprites	= NULL;
	g_Basic3D_ClearObjectsOfTextureRef = NULL;
	m_iMipMapNum			= 9;
}

DARKSDK void Constructor ( HINSTANCE hSetup )
{
	ConstructorD3D ( hSetup );

}

DARKSDK void DestructorD3D ( void )
{
    m_CurrentId = 0;
    m_ptr = NULL;

    for (ImagePtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
    {
        // Release the texture and texture name
        tagData* ptr = pCheck->second;
        SAFE_RELEASE( ptr->lpTexture );
        SAFE_DELETE( ptr->lpName );

        // Release the rest of the image storage
        delete ptr;

        // NOTE: Not removing from m_List at this point:
        // 1 - it makes moving to the next item harder
        // 2 - it's less efficient - we'll clear the entire list at the end
    }

    // Now clear the list
    m_List.clear();

	// Free reference to D3D we took
	SAFE_RELEASE(m_pDX);
}

DARKSDK void Destructor ( void )
{
	DestructorD3D();
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassSpriteInstance ( HINSTANCE hSprite )
{
	#ifndef DARKSDK_COMPILE
		g_Sprite_PasteImage = ( SPRITE_RetVoidParamInt3PFN ) GetProcAddress ( hSprite, "?PasteImageEx@@YAXHHHMMH@Z" );
		g_Update_All_Sprites = ( SPRITE_RetVoidParamVoid ) GetProcAddress ( hSprite, "?UpdateAllSprites@@YAXXZ" );
	#else
		g_Sprite_PasteImage = dbPasteImageEx;
		g_Update_All_Sprites = UpdateAllSpritesSprites;
	#endif


	// get reference to sprite functions (so we can use polypasting, etc)
	//g_Sprite_PasteImage = ( SPRITE_RetVoidParamInt3PFN ) GetProcAddress ( hSprite, "?PasteImageEx@@YAXHHHMMH@Z" );
	//g_Update_All_Sprites = ( SPRITE_RetVoidParamVoid ) GetProcAddress ( hSprite, "?UpdateAllSprites@@YAXXZ" );
}

DARKSDK int GetBitDepthFromFormat(D3DFORMAT Format)
{
	switch(Format)
	{
		case D3DFMT_R8G8B8 :		return 24;	break;
		case D3DFMT_A8R8G8B8 :		return 32;	break;
		case D3DFMT_X8R8G8B8 :		return 32;	break;
		case D3DFMT_R5G6B5 :		return 16;	break;
		case D3DFMT_X1R5G5B5 :		return 16;	break;
		case D3DFMT_A1R5G5B5 :		return 16;	break;
		case D3DFMT_A4R4G4B4 :		return 16;	break;
		case D3DFMT_A8	:			return 8;	break;
		case D3DFMT_R3G3B2 :		return 8;	break;
		case D3DFMT_A8R3G3B2 :		return 16;	break;
		case D3DFMT_X4R4G4B4 :		return 16;	break;
		case D3DFMT_A2B10G10R10 :	return 32;	break;
		case D3DFMT_G16R16 :		return 32;	break;
		case D3DFMT_A8P8 :			return 8;	break;
		case D3DFMT_P8 :			return 8;	break;
		case D3DFMT_L8 :			return 8;	break;
		case D3DFMT_A8L8 :			return 16;	break;
		case D3DFMT_A4L4 :			return 8;	break;
	}
	return 0;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;

	// and basic3d functions
	g_Basic3D_ClearObjectsOfTextureRef = ( BASIC3D_ClearObjectsOfTextureRef ) GetProcAddress ( g_pGlob->g_Basic3D, "?ClearObjectsOfTextureRef@@YAXPAUIDirect3DTexture9@@@Z" );

	// get memblock functions
	if ( g_pGlob->g_Memblocks )
	{
		g_Memblock_GetMemblockExist = ( MEMBLOCKS_GetMemblockExist ) GetProcAddress ( g_pGlob->g_Memblocks, "?MemblockExist@@YAHH@Z" );
		g_Memblock_GetMemblockPtr = ( MEMBLOCKS_GetMemblockPtr ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockPtr@@YAKH@Z" );
		g_Memblock_GetMemblockSize = ( MEMBLOCKS_GetMemblockSize ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockSize@@YAHH@Z" );
	}

	// Get default d3dformat from backbuffer
	D3DSURFACE_DESC backdesc;
	LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
	if(pBackBuffer)
	{
		HRESULT hRes = pBackBuffer->GetDesc(&backdesc);
		DWORD dwDepth=GetBitDepthFromFormat(backdesc.Format);
		if(dwDepth==16) g_DefaultD3Dformat = D3DFMT_A1R5G5B5;
		if(dwDepth==32) g_DefaultD3Dformat = D3DFMT_A8R8G8B8;
	}
	else
	{
		g_DefaultD3Dformat = D3DFMT_A8R8G8B8;
	}

	// Ensure textureformat is valid, else choose next valid..
	HRESULT hRes = m_pDX->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												backdesc.Format,
												0, D3DRTYPE_TEXTURE,
												g_DefaultD3Dformat);
	if ( FAILED( hRes ) )
	{
		// Need another texture format with an alpha
		for(DWORD t=0; t<12; t++)
		{
			switch(t)
			{
				case 0  : g_DefaultD3Dformat = D3DFMT_A8R8G8B8;		break;
				case 1  : g_DefaultD3Dformat = D3DFMT_X8R8G8B8;		break;
				case 2  : g_DefaultD3Dformat = D3DFMT_A1R5G5B5;		break;
				case 3  : g_DefaultD3Dformat = D3DFMT_A2B10G10R10;	break;
				case 4  : g_DefaultD3Dformat = D3DFMT_A4R4G4B4;		break;
				case 5  : g_DefaultD3Dformat = D3DFMT_A8R3G3B2;		break;
				case 6  : g_DefaultD3Dformat = D3DFMT_R8G8B8;		break;
				case 7  : g_DefaultD3Dformat = D3DFMT_R5G6B5;		break;
				case 8  : g_DefaultD3Dformat = D3DFMT_X1R5G5B5;		break;
				case 9  : g_DefaultD3Dformat = D3DFMT_R3G3B2;		break;
				case 10 : g_DefaultD3Dformat = D3DFMT_X4R4G4B4;		break;
				case 11 : g_DefaultD3Dformat = D3DFMT_G16R16;		break;
			}
			HRESULT hRes = m_pDX->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
														D3DDEVTYPE_HAL,
														backdesc.Format,
														0, D3DRTYPE_TEXTURE,
														g_DefaultD3Dformat);
			if ( SUCCEEDED( hRes ) )
			{
				// Found a texture we can use
				return;
			}
		}
	}
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		DestructorD3D();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		ConstructorD3D ( g_pGlob->g_GFX );
		PassCoreData ( g_pGlob );
		PassSpriteInstance ( g_pGlob->g_Sprites );
	}
}

DARKSDK int GetPowerSquareOfSize( int Size )
{
	if ( Size <= 1 ) Size = 1;
	else if ( Size <= 2 ) Size = 2;
	else if ( Size <= 4 ) Size = 4;
	else if ( Size <= 8 ) Size = 8;
	else if ( Size <= 16 ) Size = 16;
	else if ( Size <= 32 ) Size = 32;
	else if ( Size <= 64 ) Size = 64;
	else if ( Size <= 128 ) Size = 128;
	else if ( Size <= 256 ) Size = 256;
	else if ( Size <= 512 ) Size = 512;
	else if ( Size <= 1024 ) Size = 1024;
	else if ( Size <= 2048 ) Size = 2048;
	else if ( Size <= 4096 ) Size = 4096;
	return Size;
}

void GetFileInMemory ( LPSTR szFilename, LPVOID* ppFileInMemoryData, DWORD* pdwFileInMemorySize, LPSTR pFinalRelPathAndFileRef )
{
	*pdwFileInMemorySize = 0;
	*ppFileInMemoryData = NULL;
	if ( g_bImageBlockActive )
	{
		// final storage string of path and file resolver (makes the filename and path uniform for imageblock retrieval)
		char pFinalRelPathAndFile[512];

		// store old directory
		char pOldDir [ 512 ];
		_getcwd ( pOldDir, 512 );

		// get combined path only
		char pPath[1024];
		char pFile[1024];
		strcpy ( pPath, pOldDir );
		strcat ( pPath, "\\" );
		strcat ( pPath, szFilename );
		strcat ( pFile, "" );
		for ( int n=strlen(pPath); n>0; n-- )
		{
			if ( pPath[n]=='\\' || pPath[n]=='/' )
			{
				// split file and path
				strcpy ( pFile, pPath + n + 1 );
				pPath [ n + 1 ] = 0;
				break;
			}
		}

		// Combine current working folder and filename to get a resolved path (removes ..\..\ stuff)
		/* does not work if folders removed in final exe
		_chdir(pPath);
		char pResolvedDir[512];
		strcpy ( pResolvedDir, pPath );
		_getcwd ( pResolvedDir, 512 );
		*/
		char pResolvedDir[512];
		strcpy ( pResolvedDir, pPath );

		// Remove the part which represents the root location of the Image Block (g_iImageBlockRootPath)
		if ( strlen ( pResolvedDir ) <= strlen(g_iImageBlockRootPath) )
			strcpy ( pFinalRelPathAndFile, "" );
		else
			strcpy ( pFinalRelPathAndFile, pResolvedDir + strlen(g_iImageBlockRootPath) );

		// Ensure a \ is added
		if ( strlen ( pFinalRelPathAndFile ) > 0 )
		{
			if ( pFinalRelPathAndFile [ strlen(pFinalRelPathAndFile)-1 ]!='\\' )
			{
				// add folder divide at end of path string
				int iLen = strlen(pFinalRelPathAndFile);
				pFinalRelPathAndFile [ iLen+0 ] = '\\';
				pFinalRelPathAndFile [ iLen+1 ] = 0;
			}
		}

		// Add the filename back in
		strcat ( pFinalRelPathAndFile, pFile );

		// Restore folder
		_chdir ( pOldDir );

		// Retrieve file in memory
		if ( g_iImageBlockMode==1 )
		{
			*ppFileInMemoryData = RetrieveFromImageBlock ( pFinalRelPathAndFile, pdwFileInMemorySize );
		}

		// copy final rel path and file
		strcpy ( pFinalRelPathAndFileRef, pFinalRelPathAndFile );
	}
	else
	{
		strcpy ( pFinalRelPathAndFileRef, "" );
	}
}

DARKSDK LPDIRECT3DTEXTURE9 GetTextureCore ( char* szFilename, int iOneToOnePixels, int iFullTexturePlateMode, int iDivideTextureSize )
{
	// V109 BETA3 - 220408 - new feature IMAGEBLOCK
	DWORD dwFileInMemorySize = 0;
	LPVOID pFileInMemoryData = NULL;
	if ( g_bImageBlockActive )
	{
		// final storage string of path and file resolver (makes the filename and path uniform for imageblock retrieval)
		// and work out true file and path, then look for it in imageblock
		char pFinalRelPathAndFile[512];
		GetFileInMemory ( szFilename, &pFileInMemoryData, &dwFileInMemorySize, pFinalRelPathAndFile );

		// Add relative path and file to image block
		if ( g_iImageBlockMode==0 )
		{
			_chdir ( g_iImageBlockRootPath );
			AddToImageBlock ( pFinalRelPathAndFile );
		}
	}

	// loads a texture and returns a pointer to it
	// make sure that the device is valid
	if ( !m_pD3D )
		return NULL;

	// variable declarations
	LPDIRECT3DTEXTURE9	lpTexture = NULL;	// set texture to null

	// get file image info
	HRESULT hRes = 0;
	D3DXIMAGE_INFO info;
	if ( g_iImageBlockMode==1 && pFileInMemoryData )
	{
		hRes = D3DXGetImageInfoFromFileInMemory( pFileInMemoryData, dwFileInMemorySize, &info );
		// 20120526 IanM - Failed to get file from memory, try a disk-file
		if (FAILED(hRes))
			hRes = D3DXGetImageInfoFromFile( szFilename, &info );
	}
	else
		hRes = D3DXGetImageInfoFromFile( szFilename, &info );

	// 20120526 IanM - If failed to get image information, then assume can't be any image there either.
	if (FAILED(hRes))
		return NULL;

	// if texture size needs diviing, do so now
	if ( iDivideTextureSize>0 )
	{
		if ( iDivideTextureSize==16384 )
		{
			// notextureloadmode
			info.Width = 1;
			info.Height = 1;
		}
		else
		{
			// divide by specified value (reduce texture consumption)
			info.Width /= iDivideTextureSize;
			info.Height /= iDivideTextureSize;
		}
	}

	// leeadd - 290104 - if mode is CUBE(2) or VOLUME(3), direct cube loader
	if ( iFullTexturePlateMode==2 || iFullTexturePlateMode==3 || iDivideTextureSize==16384 )
	{
		if ( iDivideTextureSize==16384 )
		{
			// support for quick-fake-texture-load (apply texture to universe without loading it)
			LPDIRECT3DTEXTURE9 pFakeTex = NULL;
			hRes = D3DXCreateTexture ( m_pD3D,
									   info.Width,
									   info.Height,
									   1,//one mipmap only for one-to-one pixels
									   0,
									   g_DefaultD3Dformat,
									   D3DPOOL_MANAGED,
									   &pFakeTex );
			lpTexture = pFakeTex;
		}
		else
		{
			if ( iFullTexturePlateMode==2 ) 
			{
				// support for cube textures when specify texture flag of two (2)
				LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
				HRESULT hRes = 0;
				if ( g_iImageBlockMode==1 && pFileInMemoryData )
					hRes = D3DXCreateCubeTextureFromFileInMemoryEx( m_pD3D, pFileInMemoryData, dwFileInMemorySize, info.Width, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubeTex );
				else
					hRes = D3DXCreateCubeTextureFromFileEx( m_pD3D, szFilename, info.Width, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
					D3DPOOL_DEFAULT,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
					D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubeTex );

				if ( SUCCEEDED( hRes ) ) 
				{
					// cube loaded fine
					lpTexture = (LPDIRECT3DTEXTURE9)pCubeTex;
				}
				else
					return NULL;
			}
			if ( iFullTexturePlateMode==3 ) 
			{
				// support for volume textures when specify 3
				LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
				HRESULT hRes = 0;
				if ( g_iImageBlockMode==1 && pFileInMemoryData )
					hRes = D3DXCreateVolumeTextureFromFileInMemoryEx( m_pD3D, pFileInMemoryData, dwFileInMemorySize,
								info.Width, info.Height, info.Depth, 1, 0, D3DFMT_UNKNOWN,
								D3DPOOL_DEFAULT,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM! 
								D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
								&pVolumeTex );
				else
					hRes = D3DXCreateVolumeTextureFromFileEx( m_pD3D, szFilename, 
								info.Width, info.Height, info.Depth, 1, 0, D3DFMT_UNKNOWN,
								D3DPOOL_DEFAULT,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
								D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
								&pVolumeTex );

				if ( SUCCEEDED( hRes ) ) 
				{
					// volume loaded fine
					lpTexture = (LPDIRECT3DTEXTURE9)pVolumeTex;
				}
				else
					return NULL;
			}
		}
	}
	else
	{
		// leeadd - 120314 - texture flag can control if image is GPU only or MANAGED (SYS memory copy)
		D3DPOOL dwPoolType = D3DPOOL_DEFAULT;
		if ( iFullTexturePlateMode==10 )
		{
			dwPoolType = D3DPOOL_MANAGED;
		}

		// leefix - 150503 - perfect texture is one-to-one pixels and no mipmaps or alpha
		if ( iOneToOnePixels==1 )
		{
			// Keep Alpha from file
			DWORD dwUseAlphaCode = 0;
			if(_strnicmp( szFilename + (strlen(szFilename)-4), ".dds", 4)==NULL
			|| _strnicmp( szFilename + (strlen(szFilename)-4), ".png", 4)==NULL
			|| _strnicmp( szFilename + (strlen(szFilename)-4), ".tga", 4)==NULL)
				dwUseAlphaCode = 0;
			else
				dwUseAlphaCode = m_Color;

			// leeadd - 300305 - format selection (replaced info.Format or g_DefaultD3Dformat in createtexture as we need SET IMAGE COLORKEY to retain alpha!)
			D3DFORMAT newImageFormat = info.Format;
			if ( dwUseAlphaCode!=0 ) newImageFormat = g_DefaultD3Dformat;

			// create a new texture/image
			hRes = D3DXCreateTexture ( m_pD3D,
									   info.Width,
									   info.Height,
									   1,//one mipmap only for one-to-one pixels
									   0,
									   newImageFormat,
									   dwPoolType,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
									   &lpTexture );

			// 010205 - default mem can run out
			if ( lpTexture==NULL )
				return NULL;

			// adjust to actual size if texture smaller
			D3DSURFACE_DESC desc;
			lpTexture->GetLevelDesc(0,&desc);
			if(desc.Width<info.Width) info.Width=desc.Width;
			if(desc.Height<info.Height) info.Height=desc.Height;
			if( SUCCEEDED ( hRes ))
			{
				LPDIRECT3DSURFACE9 pSurface=NULL;
				hRes = lpTexture->GetSurfaceLevel(0, &pSurface);
				if( SUCCEEDED ( hRes ))
				{
					// load surface data into it
					RECT destrc = { 0, 0, info.Width, info.Height };
					if ( g_iImageBlockMode==1 && pFileInMemoryData )
						hRes = D3DXLoadSurfaceFromFileInMemory( pSurface, NULL, &destrc, pFileInMemoryData, dwFileInMemorySize, NULL, D3DX_FILTER_POINT, dwUseAlphaCode, &info );
					else
						hRes = D3DXLoadSurfaceFromFile( pSurface, NULL, &destrc, szFilename, NULL, D3DX_FILTER_POINT, dwUseAlphaCode, &info );
					pSurface->Release();
				}
			}
            // If any of the previous steps failed, release the target texture
            // to signal a failure to load.
            if ( FAILED ( hRes ) )
                SAFE_RELEASE( lpTexture );
		}
		else
		{
			// DDS or Other File Format (leefix - 220303 - added TGA to keep alpha load)
			if(_strnicmp( szFilename + (strlen(szFilename)-4), ".dds", 4)==NULL
			|| _strnicmp( szFilename + (strlen(szFilename)-4), ".png", 4)==NULL
			|| _strnicmp( szFilename + (strlen(szFilename)-4), ".tga", 4)==NULL)
			{
				// lee - 180406 - u6rc10 - not unknown, use file format
				D3DFORMAT newImageFormat = info.Format;
					
				// if DDS, load directly with original mipmap data intact
				if ( g_iImageBlockMode==1 && pFileInMemoryData )
					hRes = D3DXCreateTextureFromFileInMemoryEx(	m_pD3D,	pFileInMemoryData, dwFileInMemorySize, info.Width,info.Height,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,
					dwPoolType,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
					D3DX_DEFAULT,D3DX_DEFAULT,0,&info,NULL,&lpTexture );
				else
					hRes = D3DXCreateTextureFromFileEx(	m_pD3D,
													szFilename,
													info.Width,
													info.Height,
													g_dwMipMapGenMode,//D3DX_FROM_FILE,//D3DX_DEFAULT, // 031014 - take mipmap as stored in file, DO NOT generate a whole chain!!
													0,
													newImageFormat,// 180406 - u6rc10 - D3DFMT_UNKNOWN,//g_DefaultD3Dformat,
													dwPoolType,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
													D3DX_DEFAULT,
													D3DX_DEFAULT,
													0, //m_Color, LEEFIX - DDS/TGA has its own Alphamap!
													&info,
													NULL,
													&lpTexture );

				// 010205 - default mem can run out
				if ( lpTexture==NULL )
					return NULL;

				// adjust to actual size if texture smaller
				D3DSURFACE_DESC desc;
				lpTexture->GetLevelDesc(0,&desc);
				if(desc.Width<info.Width) info.Width=desc.Width;
				if(desc.Height<info.Height) info.Height=desc.Height;
			}
			else
			{
				// create a new texture/image
				hRes = D3DXCreateTexture ( 
										  m_pD3D,
										  info.Width,
										  info.Height,
										  D3DX_DEFAULT,
										  0,
										  g_DefaultD3Dformat,
										  dwPoolType,//D3DPOOL_MANAGED, lee - 010314 - preserve SYS MEM!
										  &lpTexture	       );

				// 010205 - default mem can run out
				if ( lpTexture==NULL )
					return NULL;

				// adjust to actual size if texture smaller
				D3DSURFACE_DESC desc;
				lpTexture->GetLevelDesc(0,&desc);
				if(desc.Width<info.Width) info.Width=desc.Width;
				if(desc.Height<info.Height) info.Height=desc.Height;
				if( SUCCEEDED ( hRes ))
				{
					// get surface of texture (as many mipmap levels as it has)
					for ( DWORD level=0; level<lpTexture->GetLevelCount(); level++ )
					{
						LPDIRECT3DSURFACE9 pSurface=NULL;
						hRes = lpTexture->GetSurfaceLevel(level, &pSurface);
						if( SUCCEEDED ( hRes ))
						{
							// load surface data into it
							// leefix-260603-ditheris aweful!
							// leefix-220703-changed again so level 0 is clean mip and rest are dithered for good distance textures
							if ( g_iImageBlockMode==1 && pFileInMemoryData )
							{
								if ( level==0 )
									hRes = D3DXLoadSurfaceFromFileInMemory( pSurface, NULL, NULL, pFileInMemoryData, dwFileInMemorySize, NULL, D3DX_FILTER_POINT, m_Color, &info );
								else
									hRes = D3DXLoadSurfaceFromFileInMemory( pSurface, NULL, NULL, pFileInMemoryData, dwFileInMemorySize, NULL, D3DX_DEFAULT, m_Color, &info );
							}
							else
							{
								if ( level==0 )
									hRes = D3DXLoadSurfaceFromFile( pSurface, NULL, NULL, szFilename, NULL, D3DX_FILTER_POINT, m_Color, &info );
								else
									hRes = D3DXLoadSurfaceFromFile( pSurface, NULL, NULL, szFilename, NULL, D3DX_DEFAULT, m_Color, &info );
							}

							pSurface->Release();
						}
						else
							break;
					}
				}
			}
		}
	}

	// check the texture loaded in ok
	if ( !lpTexture )
		Error ( "Failed to load texture for image library" );

	// needed image info
	m_iWidth  = info.Width;		// file width
	m_iHeight = info.Height;	// file height

	// finally return the pointer
	return lpTexture;
}

DARKSDK void SetImageAutoMipMap ( int iGenerateMipMaps )
{
	if ( iGenerateMipMaps==1 )
		g_dwMipMapGenMode = D3DX_DEFAULT;
	else
		g_dwMipMapGenMode = D3DX_FROM_FILE;
}

DARKSDK bool LoadFullTex ( char* szFilename, LPDIRECT3DTEXTURE9* pImage, D3DXIMAGE_INFO* info, int iFullTexturePlateMode, int iDivideTextureSize )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media
	bool bRes = false;
	g_pGlob->Decrypt( (DWORD)VirtualFilename );

	// load the media
	*pImage = GetTextureCore ( VirtualFilename, 0, iFullTexturePlateMode, iDivideTextureSize );
	if ( *pImage ) bRes = true;

	// get media info
	HRESULT hRes = 0;
	if ( g_bImageBlockActive && g_iImageBlockMode==1 )
	{
		DWORD dwFileInMemorySize = 0;
		LPVOID pFileInMemoryData = NULL;
		char pFinalRelPathAndFile[512];
		GetFileInMemory ( VirtualFilename, &pFileInMemoryData, &dwFileInMemorySize, pFinalRelPathAndFile );
		hRes = D3DXGetImageInfoFromFileInMemory( pFileInMemoryData, dwFileInMemorySize, info );
	}

	// U74 BETA9 - 040709 - get info from physical file if not in image block
	if ( hRes==0 || hRes!=D3D_OK ) hRes = D3DXGetImageInfoFromFile( VirtualFilename, info );

	// re-encrypt if applicable
	g_pGlob->Encrypt( (DWORD)VirtualFilename );

	// success or no
	return bRes;
}

DARKSDK bool Load ( char* szFilename, LPDIRECT3DTEXTURE9* pImage, D3DXIMAGE_INFO* info )
{
	// default is full texture plate mode zero = simple surface
	return LoadFullTex ( szFilename, pImage, info, 0, 0 );
}

DARKSDK bool FindInternalImage ( char* szFilename, int* pImageID )
{
	if ( szFilename && szFilename[0] )
	{
    	int iFindFilenameLength = strlen(szFilename);

		ImagePtr pCheck = m_List.begin();
		while ( pCheck != m_List.end() && pCheck->first < 0)
		{
            // get a pointer to the actual data
		    tagData* ptr = pCheck->second;
            if ( ptr && ptr->szShortFilename && ptr->lpTexture )
		    {
			    if ( _stricmp(ptr->szShortFilename, szFilename) == 0)
			    {
				    *pImageID = pCheck->first;
				    return true;
			    }
            }
            ++pCheck;
		}
	}

    // Static, so not reset between function calls - almost guaranteed to find the
    // next free image that way.
	static int iTry=-1;

    // Check to see if the image id is in use
    ImagePtr pCheck = m_List.find(iTry);
    while (pCheck != m_List.end())
    {
        // Is in use, check for the next number, but make sure that underflow is dealt with correctly
        // ie, iTry MUST stay negative. Note that this is not too likely to happen anyway.
        --iTry;
        if (iTry > 0)
            iTry = -1;

        pCheck = m_List.find(iTry);
    }

	// this image can be used = new slot
	*pImageID = iTry;

	// not found existing image..
	return false;
}

DARKSDK void ClearAnyLightMapInternalTextures ( void )
{
	ImagePtr pCheck = m_List.begin();
	while ( pCheck != m_List.end() && pCheck->first < 0)
	{
        // get a pointer to the actual data
		tagData* ptr = pCheck->second;
		int iFoundID = 0;
        if ( ptr && ptr->szShortFilename && ptr->lpTexture )
		{
			if ( ptr->szShortFilename!=NULL )
			{
				if ( strlen(ptr->szShortFilename)>11 )
				{
					for ( int n=0; n<strlen(ptr->szShortFilename)-11; n++ )
						if ( strnicmp ( ptr->szShortFilename + n, "lightmaps\\", 10 )==NULL )
							iFoundID = pCheck->first;
				}
			}
        }
		if ( iFoundID!=0 ) 
		{
			RemoveImage( iFoundID );
			pCheck = m_List.begin();
		}
		else
		{
	        ++pCheck;
		}
	}
}

// This load is NOT the main DBPro image loader - it is used here though (Load(x,x,x,x))
DARKSDK int LoadInternal ( char* szFilename, int iDivideTextureSize )
{
	// does image already exist?
	int iImageID = 0;
	if ( !FindInternalImage ( szFilename, &iImageID ) )
	{
		// copy of filename to attempt to load
		char VirtualFilename[MAX_PATH];
		strcpy ( VirtualFilename, szFilename );

		CheckForWorkshopFile ( VirtualFilename );

		// no, use standard loader
		g_pGlob->Decrypt( (DWORD)VirtualFilename );
		if ( LoadRetainName ( szFilename, (LPSTR)VirtualFilename, iImageID, 0, true, iDivideTextureSize ) )
		{
			// new image returned
		}
		else
		{
			// load failed
			iImageID=0;
		}
		g_pGlob->Encrypt( (DWORD)VirtualFilename );
	}
	return iImageID;
}

DARKSDK int LoadInternal ( char* szFilename )
{
	return LoadInternal ( szFilename, 0 );
}

DARKSDK LPDIRECT3DTEXTURE9 GetTexture ( char* szFilename, int iOneToOnePixels )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LPDIRECT3DTEXTURE9 Res = GetTextureCore ( VirtualFilename, iOneToOnePixels, 0, 0 );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
	return Res;
}

DARKSDK LPDIRECT3DTEXTURE9 GetPointer ( int iID )
{
	// update internal data
	if ( !UpdatePtrImage ( iID ) )
		return NULL;

	if ( m_ptr->lpTexture )
		return m_ptr->lpTexture;
	else
		return NULL;
}

DARKSDK int GetHeight ( int iID )
{
	// get the height of an image

	// update internal data
	if ( !UpdatePtrImage ( iID ) )
		return -1;

	// return the height
	return m_ptr->iHeight;
}

DARKSDK int GetWidth ( int iID )
{
	// get the width of an image

	// update internal data
	if ( !UpdatePtrImage ( iID ) )
		return -1;

	// return the width
	return m_ptr->iWidth;
}

DARKSDK float GetUMax ( int iID )
{
	// get the height of an image

	// update internal data
	if ( !UpdatePtrImage ( iID ) )
		return 1.0f;

	// return the fTexUMax
	return m_ptr->fTexUMax;
}

DARKSDK float GetVMax ( int iID )
{
	// get the width of an image

	// update internal data
	if ( !UpdatePtrImage ( iID ) )
		return 1.0f;

	// return the fTexVMax
	return m_ptr->fTexVMax;
}

DARKSDK void SetSharing ( bool bMode )
{
	// sets up the sharing mode, this helps to conserve
	// memory as it won't load in the same image more
	// than once

	// save mode
	m_bSharing = bMode;
}

DARKSDK void SetMemory ( int iMode )
{
	// sets the memory mode for the images

	// iMode possible values
	// 0 - default
	// 1 - managed ( must use this to lock textures )
	// 2 - system

	// check that the mode is valid
	if ( iMode < 0 || iMode > 2 )
		Error ( "Invalid memory mode specified" );

	// save the mode
	m_iMemory = iMode;
}

DARKSDK void Lock ( int iID )
{
	// lock the specified image, this allows you to perform
	// actions like plotting pixels etc

	HRESULT hr;

	if ( !UpdatePtrImage ( iID ) )
		return;

	if ( !m_ptr->bLocked )
	{
		memset ( &m_ptr->d3dlr, 0, sizeof ( D3DLOCKED_RECT ) );

		if ( FAILED ( hr = m_ptr->lpTexture->LockRect ( 0, &m_ptr->d3dlr, 0, 0 ) ) )
			Error ( "Failed to lock image for image library" );

		m_ptr->bLocked = true;
	}
	else
		Error ( "Failed to lock image for image library as it's already locked" );
}

DARKSDK void Unlock ( int iID )
{
	if ( !UpdatePtrImage ( iID ) )
		return;

	m_ptr->lpTexture->UnlockRect ( NULL );
	m_ptr->bLocked = false;
}

DARKSDK void Write ( int iID, int iX, int iY, int iA, int iR, int iG, int iB )
{
	if ( !UpdatePtrImage ( iID ) )
		return;

	if ( !m_ptr->bLocked )
		Error ( "Unable to modify texture data for image library as it isn't locked" );
	
	DWORD* pPix = ( DWORD* ) m_ptr->d3dlr.pBits;
	
	pPix [ ( ( m_ptr->d3dlr.Pitch >> 4 ) * iX ) + iY ] = D3DCOLOR_ARGB ( iA, iR, iG, iB );
}

DARKSDK void Get ( int iID, int iX, int iY, int* piR, int* piG, int* piB )
{
	// get a pixel at x, y

	// check pointers are valid
	if ( !piR || !piG || !piB )
		return;

	// update the internal data
	if ( !UpdatePtrImage ( iID ) )
		return;

	// check the image is locked
	if ( !m_ptr->bLocked )
		Error ( "Unable to get texture data for image library as it isn't locked" );

	// get a pointer to the data
	DWORD* pPix = ( DWORD* ) m_ptr->d3dlr.pBits;
	DWORD  pGet;

	// get offset in file
	pGet = pPix [ ( ( m_ptr->d3dlr.Pitch >> 4 ) * iX ) + iY ];

	// break value down
	DWORD dwAlpha = pGet >> 24;						// get alpha
	DWORD dwRed   = ((( pGet ) >> 16 ) & 0xff );	// get red
	DWORD dwGreen = ((( pGet ) >>  8 ) & 0xff );	// get green
	DWORD dwBlue  = ((( pGet ) )       & 0xff );	// get blue

	// save values
	*piR = dwRed;
	*piG = dwGreen;
	*piB = dwBlue;
}

DARKSDK LPDIRECT3DTEXTURE9 MakeFormat ( int iID, int iWidth, int iHeight, D3DFORMAT d3dformat, DWORD dwUsage )
{
	// make a new image
	// mike : can specify iID of -1 which means image is added to list (internal use only)

	// variable declarations
	tagData*	test = NULL;		// pointer to data structure
	HRESULT		hr;					// used for error checking
	
	// create a new block of memory
	test = new tagData;
	memset(test, 0, sizeof(tagData));

	// check the memory was created
	if ( test == NULL )
		return NULL;

	// clear out the memory
	memset ( test, 0, sizeof ( tagData ) );

	D3DPOOL dwPool = D3DPOOL_MANAGED;
	if ( dwUsage!=0 ) dwPool=D3DPOOL_DEFAULT;

	// create a new texture
	hr = D3DXCreateTexture ( 
							  m_pD3D,
							  iWidth,
							  iHeight,
							  1,
							  dwUsage,
							  d3dformat,
							  dwPool,
							  &test->lpTexture
					       );

	if ( FAILED ( hr ) )
	{
		Error ( "Failed to create new image" );
		return NULL;
	}

	// setup properties
	test->iHeight = iHeight;		// store the width
	test->iWidth  = iWidth;		// store the height
	test->iDepth  = GetBitDepthFromFormat ( d3dformat );
	test->bLocked = false;			// set locked to false

	// get actual dimensions of texture/image
	D3DSURFACE_DESC imageddsd;
	test->lpTexture->GetLevelDesc(0, &imageddsd);
//	test->fTexUMax=(float)((double)test->iWidth/(double)imageddsd.Width);
//	test->fTexVMax=(float)((double)test->iHeight/(double)imageddsd.Height);
	test->fTexUMax=(float)test->iWidth/(float)imageddsd.Width;
	test->fTexVMax=(float)test->iHeight/(float)imageddsd.Height;

	// Ensure smalltextres are handled
	if(test->fTexUMax>1.0f) test->fTexUMax=1.0f;
	if(test->fTexVMax>1.0f) test->fTexVMax=1.0f;

	// add to the list
    m_List.insert( std::make_pair(iID, test) );

	// ensure sprites all updated
	g_Update_All_Sprites();

	return test->lpTexture;
}

DARKSDK LPDIRECT3DTEXTURE9 Make ( int iID, int iWidth, int iHeight )
{
	return MakeFormat ( iID, iWidth, iHeight, g_DefaultD3Dformat, 0 );
}

DARKSDK LPDIRECT3DTEXTURE9 MakeUsage ( int iID, int iWidth, int iHeight, DWORD dwUsage )
{
	return MakeFormat ( iID, iWidth, iHeight, g_DefaultD3Dformat, dwUsage );
}

DARKSDK LPDIRECT3DTEXTURE9 MakeJustFormat ( int iID, int iWidth, int iHeight, D3DFORMAT Format )
{
	return MakeFormat ( iID, iWidth, iHeight, Format, 0 );
}

DARKSDK LPDIRECT3DTEXTURE9 MakeRenderTarget ( int iID, int iWidth, int iHeight, D3DFORMAT Format )
{
	return MakeFormat ( iID, iWidth, iHeight, Format, D3DUSAGE_RENDERTARGET );
}

DARKSDK void SetCubeFace ( int iID, LPDIRECT3DCUBETEXTURE9 pCubeMap, int iFace )
{
	// get ptr to image
	if ( !UpdatePtrImage ( iID ) )
		return;

	// set cube reference details
	m_ptr->pCubeMapRef = pCubeMap;
	m_ptr->iCubeMapFace = iFace;
}

DARKSDK void GetCubeFace ( int iID, LPDIRECT3DCUBETEXTURE9* ppCubeMap, int* piFace )
{
	// get ptr to image
	if ( !UpdatePtrImage ( iID ) )
		return;

	// return cube reference details
	if ( ppCubeMap ) *ppCubeMap = m_ptr->pCubeMapRef;
	if ( piFace ) *piFace = m_ptr->iCubeMapFace;
}

DARKSDK void SetMipmapMode ( bool bMode )
{
	// switch mip mapping on or off

	if ( !bMode )
	{
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	}
	else
	{
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
}

DARKSDK void SetMipmapType ( int iType )
{
	// set the type of mip mapping used

	// iType possible values
	// 0 - none
	// 1 - point
	// 2 - linear ( default )

	if ( !m_pD3D )
		return;

	switch ( iType )
	{
		case 0:
		{
			// use no mip mapping
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );//D3DTEXF_NONE ); // may not exist on HW driver any more!
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );//D3DTEXF_NONE );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );//D3DTEXF_NONE );
		}
		break;

		case 1:
		{
			// set up point mip mapping
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		}
		break;

		case 2:
		{
			// set up linear mip mapping
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	
		}
		break;
	}
}
	
DARKSDK void SetMipmapBias ( float fBias )
{
	// set the bias for the mip mapping, this allows you
	// to set the distance at which point the mip mapping
	// is brought into action

	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPMAPLODBIAS, *( ( LPDWORD ) ( &fBias ) ) );
}

DARKSDK void SetMipmapNum ( int iNum )
{
	// set the number of mip maps used, remember that if
	// you increase this value it will take up a lot more
	// video memory - turn this down if you don't need it
	m_iMipMapNum = iNum;
}

DARKSDK void SetTranslucency ( int iID, int iPercent )
{
	// set the translucency for an image

	HRESULT hr;

	if ( !UpdatePtrImage ( iID ) )
		return;

	// now lock the surface
	if ( FAILED ( hr = m_ptr->lpTexture->LockRect ( 0, &m_ptr->d3dlr, 0, 0 ) ) )
		Error ( "Failed to lock texture in translucency for image library" );
	{
		// get a pointer to surface data
		DWORD* pPix = ( DWORD* ) m_ptr->d3dlr.pBits;
		DWORD  pGet;

		for ( DWORD y = 0; y < (DWORD)m_ptr->iHeight; y++ )
		{
			for ( DWORD x = 0; x < (DWORD)m_ptr->iWidth; x++ )
			{
				pGet = *pPix;

				DWORD dwAlpha = pGet >> 24;
				DWORD dwRed   = ((( pGet ) >> 16 ) & 0xff );
				DWORD dwGreen = ((( pGet ) >>  8 ) & 0xff );
				DWORD dwBlue  = ((( pGet ) )       & 0xff );
				
				if ( *pPix != 0 )
					*pPix++ = D3DCOLOR_ARGB ( iPercent, dwRed, dwGreen, dwBlue );
				else
					*pPix++;
			}
		}
	}

	m_ptr->lpTexture->UnlockRect ( NULL );
}

DARKSDK bool GetExist ( int iID )
{
	// returns true if the image exists
	if ( !UpdatePtrImage ( iID ) )
		return false;

	// return true
	return true;
}

// This load is the MAIN IMAGE LOADER
DARKSDK bool LoadRetainName ( char* szRealName, char* szFilename, int iID, int iTextureFlag, bool bIgnoreNegLimit, int iDivideTextureSize )
{
	// iTextureFlag (0-default/1-sectionofplate/2-cube)
	// iDivideTextureSize (0-leave size,0>divide by)
	if ( bIgnoreNegLimit==false )
	{
		if( iID<1 || iID>MAXIMUMVALUE )
		{
			RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
			return false;
		}
	}

    RemoveImage( iID );

	// loads in image into the bank
	tagData*	test = NULL;		// pointer to data structure
	int			iLen1;				// used for checking length of strings
	int			iLen2;				// used for checking length of strings
	char		szTest [ 256 ];		// text buffer
	bool		bFound = false;		// flag which checks if we need to load the texture

	// create a new block of memory
	test = new tagData;

	// 20120526 IanM - Fixed error with clearing allocated memory
	// check the memory was created
	if ( test == NULL )
		return false;

	// clear out the memory
	memset(test, 0, sizeof(tagData));

	// clear out text buffers
	memset ( test->szLongFilename,  0, sizeof ( char ) * 256 );		// clear out long filename
	memset ( test->szShortFilename, 0, sizeof ( char ) * 256 );		// clear out short filename
	memset ( szTest,                0, sizeof ( char ) * 256 );		// clear out test buffer

	// sort which name to use
	LPSTR pNameForInternalList = szFilename;
	if ( strlen(szRealName)>1 ) pNameForInternalList = szRealName;

	// get length of filename and copy to shortfilename
	iLen1 = lstrlen ( pNameForInternalList );											// get length
	memcpy ( test->szShortFilename, pNameForInternalList, sizeof ( char ) * iLen1 );	// copy to short filename

	// sort out full filename
	GetCurrentDirectory ( 256, szTest );	// get the apps full directory e.g. "c:\test\"
	iLen1 = lstrlen ( szTest );				// get the length of the full directory
	iLen2 = lstrlen ( pNameForInternalList );			// get the length of the passed filename
	
	// copy memory
	memcpy ( test->szLongFilename,         szTest,         sizeof ( char ) * iLen1 );	// copy the full dir to the long filename

	// U74 BETA9 - 030709 - fix longfilename if full dir has no \ and filename does not start with
	// memcpy ( test->szLongFilename + iLen1, szFilename + 1, sizeof ( char ) * iLen2 );	// append the filename onto the long file name
	if ( pNameForInternalList[0]!='\\' )
	{
		if ( szTest[iLen1]!='\\' )
		{
			test->szLongFilename[iLen1]='\\';
			memcpy ( test->szLongFilename + iLen1 + 1, pNameForInternalList, sizeof ( char ) * iLen2 );
		}
		else
			memcpy ( test->szLongFilename + iLen1, pNameForInternalList, sizeof ( char ) * iLen2 );
	}
	else
	{
		// legacy support
		memcpy ( test->szLongFilename + iLen1, pNameForInternalList + 1, sizeof ( char ) * iLen2 );
	}

	// The default is a setting of zero (0)
	if( iTextureFlag==1 )
	{
		// loads image into a section of the texture plate
		if ( m_bSharing && !bFound )
			test->lpTexture = GetTexture ( szFilename, 1 );	// load the perfect texture
		else
			test->lpTexture = GetTexture ( szFilename, 1 );	// load the perfect texture

		// load failed
		if ( test->lpTexture==NULL )
		{
			SAFE_DELETE(test);
			return false;
		}

		// Set image settings
		test->iHeight = m_iHeight;					// store the width
		test->iWidth  = m_iWidth;					// store the height

		// get actual dimensions of texture/image
		D3DSURFACE_DESC imageddsd;
		test->lpTexture->GetLevelDesc(0, &imageddsd);
		test->fTexUMax=(float)test->iWidth/(float)imageddsd.Width;
		test->fTexVMax=(float)test->iHeight/(float)imageddsd.Height;

		// Ensure smalltextres are handled
		if(test->fTexUMax>1.0f) test->fTexUMax=1.0f;
		if(test->fTexVMax>1.0f) test->fTexVMax=1.0f;
	}
	else
	{
		// Load Image Into Whole Texture Plate
		D3DXIMAGE_INFO info; // image information
		LoadFullTex ( szFilename, &test->lpTexture, &info, iTextureFlag, iDivideTextureSize );	// loads into whole texture

		// load failed
		if ( test->lpTexture==NULL )
		{
			SAFE_DELETE(test);
			return false;
		}

		// get file image info
		test->iHeight = info.Height;
		test->iWidth  = info.Width;

		// Entire texture used
		test->fTexUMax=1.0f;
		test->fTexVMax=1.0f;
	}

	// Get depth of texture
	D3DSURFACE_DESC desc;
	test->lpTexture->GetLevelDesc(0, &desc);
	test->iDepth  = GetBitDepthFromFormat(desc.Format);

	// load failed
	if ( test->lpTexture==NULL )
	{
		SAFE_DELETE(test);
		return false;
	}

	// fill out rest of structure
	test->bLocked = false;

	// add to the list
    m_List.insert( std::make_pair(iID, test) );

	// ensure sprites all updated
	// V109 BETA3 - only need to update image/textures, not internal textures (negatives)
	if ( iID>0 ) g_Update_All_Sprites();

	return true;
}

DARKSDK bool Load ( char* szFilename, int iID, int iTextureFlag, bool bIgnoreNegLimit, int iDivideTextureSize )
{
	return LoadRetainName ( "", szFilename, iID, iTextureFlag, bIgnoreNegLimit, iDivideTextureSize );
}

DARKSDK bool Load ( char* szFilename, int iID )
{
	return Load ( szFilename, iID, 1, false, 0 );
}

DARKSDK bool Save ( char* szFilename, int iID, int iCompressionMode )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return false;
	}
	if ( !UpdatePtrImage ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);
		return false;
	}
	if ( szFilename==NULL )
	{
		RunTimeError(RUNTIMEERROR_FILENOTEXIST);
		return false;
	}

	// determine format from extension
	D3DXIMAGE_FILEFORMAT DestFormat = D3DXIFF_BMP;
	LPSTR szFilenameExt = szFilename + strlen(szFilename)-4;
	if ( _strnicmp ( szFilenameExt, ".bmp", 4 )==NULL ) DestFormat=D3DXIFF_BMP ;
	if ( _strnicmp ( szFilenameExt, ".dds", 4 )==NULL ) DestFormat=D3DXIFF_DDS ;
	if ( _strnicmp ( szFilenameExt, ".dib", 4 )==NULL ) DestFormat=D3DXIFF_DIB ;
	if ( _strnicmp ( szFilenameExt, ".jpg", 4 )==NULL ) DestFormat=D3DXIFF_JPG ;
	if ( _strnicmp ( szFilenameExt, ".png", 4 )==NULL ) DestFormat=D3DXIFF_PNG ;

	// determine region of surface to save
	LPDIRECT3DSURFACE9 pTexSurface = NULL;
	m_ptr->lpTexture->GetSurfaceLevel(0, &pTexSurface);
	if ( pTexSurface )
	{
		// determine size of surface
		D3DSURFACE_DESC srcddsd;
		HRESULT hRes = pTexSurface->GetDesc(&srcddsd);
		RECT rc = { 0, 0, (int)((float)srcddsd.Width*m_ptr->fTexUMax), (int)((float)srcddsd.Height*m_ptr->fTexVMax) };

		// If DDS, use automatic compression
		LPDIRECT3DTEXTURE9 pTextureDDS = NULL;
		LPDIRECT3DSURFACE9 pDDSSurface = NULL;
		if ( DestFormat==D3DXIFF_DDS )
		{
			// and only if source NOT already compressed
			if ( srcddsd.Format==D3DFMT_DXT1 || srcddsd.Format==D3DFMT_DXT2 || srcddsd.Format==D3DFMT_DXT3 || srcddsd.Format==D3DFMT_DXT4 || srcddsd.Format==D3DFMT_DXT5 )
			{
				// source already compressed
			}
			else
			{
				// create our DDS MASTER texture (the final one to be used)
				D3DFORMAT dwD3DSurfaceFormat = D3DFMT_A8R8G8B8;
				switch ( iCompressionMode )
				{
					case 1 : dwD3DSurfaceFormat = D3DFMT_DXT1; break;
					case 2 : dwD3DSurfaceFormat = D3DFMT_DXT2; break;
					case 3 : dwD3DSurfaceFormat = D3DFMT_DXT3; break;
					case 4 : dwD3DSurfaceFormat = D3DFMT_DXT4; break;
					case 5 : dwD3DSurfaceFormat = D3DFMT_DXT5; break;
				}
				m_pD3D->CreateTexture ( srcddsd.Width, srcddsd.Height, 1, 0, dwD3DSurfaceFormat, D3DPOOL_MANAGED, &pTextureDDS, NULL );
				if ( pTextureDDS )
				{
					// copy texture to DDS compressed texture
					pTextureDDS->GetSurfaceLevel ( 0, &pDDSSurface );
					if ( pDDSSurface )
					{
						D3DSURFACE_DESC dstddsd;
						HRESULT hRes = pDDSSurface->GetDesc(&dstddsd);
						if ( pTexSurface )
						{
							hRes = D3DXLoadSurfaceFromSurface ( pDDSSurface, NULL, NULL, pTexSurface, NULL, NULL, D3DX_FILTER_NONE, 0 );
							if ( FAILED ( hRes ) )
							{
								char pStrClue[512];
								wsprintf ( pStrClue, "D3DXLoadSurfaceFromSurface failure" );
								RunTimeError(RUNTIMEERROR_IMAGEERROR,pStrClue);
								SAFE_RELEASE(pDDSSurface);
								SAFE_RELEASE(pTextureDDS);
								SAFE_RELEASE(pTexSurface);
								return false;
							}
						}
					}

					// Copy over DDS to regular surface
					SAFE_RELEASE ( pTexSurface );
					pTexSurface = pDDSSurface;
					pDDSSurface = NULL;
				}
			}
		}

		// save surface of image to file
		hRes = D3DXSaveSurfaceToFile( szFilename, DestFormat, pTexSurface, NULL, &rc );
		if ( FAILED ( hRes ) )
		{
			char pStrClue[512];
			wsprintf ( pStrClue, "tex:%d filename:%s region:%d %d %d %d", (int)pTexSurface, szFilename, rc.left, rc.top, rc.right, rc.bottom );
			RunTimeError(RUNTIMEERROR_IMAGEERROR,pStrClue);
			SAFE_RELEASE(pTexSurface);
			return false;
		}

		SAFE_RELEASE(pDDSSurface);
		SAFE_RELEASE(pTextureDDS);
	}

	// free surface
	SAFE_RELEASE(pTexSurface);

	// success
	return true;
}

DARKSDK bool Save ( char* szFilename, int iID )
{
	// default behaviour
	return Save ( szFilename, iID, 0 );
}

DARKSDK bool GrabImage ( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlagForGrab )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return false;
	}
	if(iX1>=iX2 || iY1>=iY2)
	{
		RunTimeError(RUNTIMEERROR_IMAGEAREAILLEGAL);
		return false;
	}

	// Size of grab
	LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
	if(pBackBuffer)
	{
		// get format of backbuffer
		D3DSURFACE_DESC ddsd;
		HRESULT hRes = pBackBuffer->GetDesc(&ddsd);
		D3DFORMAT d3dformat = ddsd.Format;
		if(iX2>(int)ddsd.Width || iY2>(int)ddsd.Height)
		{
			RunTimeError(RUNTIMEERROR_IMAGEAREAILLEGAL);
			return false;
		}
	}

	// Image size
	int iImageWidth=iX2-iX1;
	int iImageHeight=iY2-iY1;

	// Get current render target surface
	if(pBackBuffer)
	{
		// get format of backbuffer
		D3DSURFACE_DESC ddsd;
		HRESULT hRes = pBackBuffer->GetDesc(&ddsd);
		D3DFORMAT d3dformat = ddsd.Format;

		// check if image already exists of same size and type
		if ( UpdatePtrImage ( iID ) )
		{
			// check against existing
			if(m_ptr->iWidth != iImageWidth || m_ptr->iHeight != iImageHeight)
			{
				// existing image and new image are different sizes - so delete any existing image
                RemoveImage( iID );
			}
		}

		// create temp image texture to copy backbuffer to (same format at first)
		LPDIRECT3DTEXTURE9 pTempTexture=NULL;
		hRes = D3DXCreateTexture (m_pD3D,
								  iImageWidth,
								  iImageHeight,
								  D3DX_DEFAULT,
								  0,
								  d3dformat,
								  D3DPOOL_MANAGED,
								  &pTempTexture	       );

		if ( pTempTexture )
		{
			// lock surface
			D3DLOCKED_RECT d3dlr;
			hRes = pTempTexture->LockRect ( 0, &d3dlr, 0, 0 );
			if ( SUCCEEDED(hRes) )
			{
				// get size of single pixel (16bit, 24bit or 32bit)
				DWORD dwPixelSize = GetBitDepthFromFormat(d3dformat)/8;

				// copy from backbuffer lock to texture lock
				RECT rc = { iX1, iY1, iX2, iY2 };
				D3DLOCKED_RECT backlock;
				pBackBuffer->LockRect(&backlock, &rc, D3DLOCK_READONLY);
				LPSTR pPtr = (LPSTR)backlock.pBits;
				if ( pPtr==NULL )
				{
					MessageBox ( NULL, "Tried to read a surface which has no read permissions!", "System Memory Bitmap Error", MB_OK );
					RunTimeError(RUNTIMEERROR_IMAGEERROR);
					SAFE_RELEASE(pTempTexture);
					return false;
				}

				// straight copy or stretch copy
				bool bStretchCopy=false;
				D3DSURFACE_DESC imageddsd;
				pTempTexture->GetLevelDesc(0, &imageddsd);
				if(imageddsd.Width<(DWORD)iImageWidth) bStretchCopy=true;
				if(imageddsd.Height<(DWORD)iImageHeight) bStretchCopy=true;
				if(bStretchCopy==true)
				{
					DWORD dwClipWidth = iImageWidth;
					DWORD dwClipHeight = iImageHeight;
					if(imageddsd.Width<dwClipWidth) dwClipWidth=imageddsd.Width;
					if(imageddsd.Height<dwClipHeight) dwClipHeight=imageddsd.Height;
					float fXBit = (float)iImageWidth/(float)dwClipWidth;
					float fYBit = (float)iImageHeight/(float)dwClipHeight;
					LPSTR pImagePtr = (LPSTR)d3dlr.pBits;
					float fY=0.0f;
					for(DWORD y=0; y<dwClipHeight; y++)
					{
						LPSTR pImgPtr = pImagePtr + (y*d3dlr.Pitch);
						LPSTR pPtr = (LPSTR)backlock.pBits + ((int)fY*backlock.Pitch);
						float fX=0.0f;
						for(DWORD x=0; x<dwClipWidth; x++)
						{
							switch(dwPixelSize)
							{
								case 2 : *(WORD*)(pImgPtr) = *(WORD*)(pPtr+((int)fX*dwPixelSize));	break;
								case 4 : *(DWORD*)(pImgPtr) = *(DWORD*)(pPtr+((int)fX*dwPixelSize));	break;
							}
							pImgPtr+=dwPixelSize;
							fX+=fXBit;
						}
						fY+=fYBit;
					}
				}
				else
				{
					LPSTR pImagePtr = (LPSTR)d3dlr.pBits;
					for(int y=0; y<iImageHeight; y++)
					{
						memcpy(pImagePtr, pPtr, iImageWidth*dwPixelSize);
						pImagePtr+=d3dlr.Pitch;
						pPtr+=backlock.Pitch;
					}
				}
				pBackBuffer->UnlockRect();

				// unlock texture
				pTempTexture->UnlockRect(NULL);
			}

			// create image
			if(m_ptr==NULL) MakeFormat ( iID, iImageWidth, iImageHeight, g_DefaultD3Dformat, 0 );
			if ( UpdatePtrImage ( iID ) )
			{
				// load grabbed surface data into destination texture
				LPDIRECT3DSURFACE9 pNewSurface = NULL;
				pTempTexture->GetSurfaceLevel(0, &pNewSurface);
				LPDIRECT3DSURFACE9 pTexSurface = NULL;
				m_ptr->lpTexture->GetSurfaceLevel(0, &pTexSurface);

				// LEEFIX - 071002 - No srtetching or filtering if DBV1 mode used
				if ( iTextureFlagForGrab==0 )
				{
					// leefix - dx8->dx9 - if texture exact size of image, no scaling required
					D3DSURFACE_DESC ddsdNewTexture;
					HRESULT hRes = pTexSurface->GetDesc(&ddsdNewTexture);
					if ( iImageWidth==(int)ddsdNewTexture.Width && iImageHeight==(int)ddsdNewTexture.Height )
						hRes = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_FILTER_NONE, m_Color);
					else
						hRes = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_DEFAULT, m_Color);
				}
				else
				{
					if ( iTextureFlagForGrab==2 )
						hRes = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_FILTER_NONE, NULL );
					else
					{
						// leeadd - 201008 - u71 - should not have used color key (mode3)
						if ( iTextureFlagForGrab==3 )
							hRes = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_FILTER_NONE, NULL );
						else
							hRes = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_FILTER_NONE, m_Color);
					}
				}

				SAFE_RELEASE(pNewSurface);
				SAFE_RELEASE(pTexSurface);

				// get actual dimensions of texture/image
				D3DSURFACE_DESC imageddsd;
				m_ptr->lpTexture->GetLevelDesc(0, &imageddsd);
				m_ptr->fTexUMax=(float)m_ptr->iWidth/(float)imageddsd.Width;
				m_ptr->fTexVMax=(float)m_ptr->iHeight/(float)imageddsd.Height;

				// Ensure smalltextres are handled
				if(m_ptr->fTexUMax>1.0f) m_ptr->fTexUMax=1.0f;
				if(m_ptr->fTexVMax>1.0f) m_ptr->fTexVMax=1.0f;
			}
			else
			{
				RunTimeError(RUNTIMEERROR_IMAGEERROR);
				SAFE_RELEASE(pTempTexture);
				return false;
			}

			// free work-newsurface
			SAFE_RELEASE(pTempTexture);
		}
	}
	else
	{
		RunTimeError(RUNTIMEERROR_IMAGEERROR);
		return false;
	}

	// An additional feature is that images by default are stretched to complete fit a texture
	if ( iTextureFlagForGrab==0 || iTextureFlagForGrab==2 )
	{
		Stretch ( iID, GetPowerSquareOfSize(iImageWidth), GetPowerSquareOfSize(iImageHeight) );
	}

	// Complete
	return true;
}

DARKSDK bool GrabImage ( int iID, int iX1, int iY1, int iX2, int iY2 )
{
	// Stretch image to fit texture by default (0)
	return GrabImage ( iID, iX1, iY1, iX2, iY2, 0 );
}

DARKSDK void TransferImage ( int iDestImgID, int iSrcImageID, int iTransferMode, int iMemblockAssistor )
{
	// iTransferMode:
	// 1 = blue channel specifies one of five IDs, each representing a small 4x4 min-texture described in the indexed memblock passed in

	// validate
	if(iDestImgID<1 || iDestImgID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}
	if(iSrcImageID<1 || iSrcImageID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}

	// get destination details
	D3DSURFACE_DESC ddsd;
	LPDIRECT3DSURFACE9 pDstSurface = NULL;
	if ( !UpdatePtrImage ( iDestImgID ) ) return;
	tagData* pDestImagePtr = m_ptr;
	pDestImagePtr->lpTexture->GetSurfaceLevel(0, &pDstSurface);
	pDstSurface->GetDesc(&ddsd);
	DWORD dwDestImgWidth = ddsd.Width;
	DWORD dwDestImgHeight = ddsd.Height;

	// get source details
	LPDIRECT3DSURFACE9 pSrcSurface = NULL;
	if ( !UpdatePtrImage ( iSrcImageID ) ) return;
	tagData* pSrcImagePtr = m_ptr;
	pSrcImagePtr->lpTexture->GetSurfaceLevel(0, &pSrcSurface);
	pSrcSurface->GetDesc(&ddsd);
	DWORD dwSrcImgWidth = ddsd.Width;
	DWORD dwSrcImgHeight = ddsd.Height;

	// must be same size 
	if ( dwSrcImgWidth!=dwDestImgWidth || dwSrcImgHeight!=dwDestImgHeight ) return;

	// go through each pixel and apply transfer logic
	D3DLOCKED_RECT d3dDstlock;
	RECT rc = { 0, 0, dwSrcImgWidth, dwSrcImgHeight };
	if(SUCCEEDED(pDstSurface->LockRect ( &d3dDstlock, &rc, 0 ) ) )
	{
		D3DLOCKED_RECT d3dSrclock;
		if(SUCCEEDED(pSrcSurface->LockRect ( &d3dSrclock, &rc, 0 ) ) )
		{
			for(DWORD y=0; y<dwSrcImgHeight; y++)
			{
				LPSTR pDst = (LPSTR)d3dDstlock.pBits + (y*d3dDstlock.Pitch);
				LPSTR pSrc = (LPSTR)d3dSrclock.pBits + (y*d3dSrclock.Pitch);
				for(DWORD x=0; x<dwSrcImgWidth; x++)
				{
					if ( iTransferMode==1 )
					{
						// first get the pixel to work on
						DWORD dwPixelValue = *(DWORD*)pSrc;
						float fTexSelectorV = ((dwPixelValue & 0x00000FF))/255.0f;
						float fTexSelectorCol1 = (0.25f-fabs(fTexSelectorV-0.00))*4.0f;
						if ( fTexSelectorCol1 < 0 ) fTexSelectorCol1 = 0;
						float fTexSelectorCol2 = (0.25f-fabs(fTexSelectorV-0.25))*4.0f;
						if ( fTexSelectorCol2 < 0 ) fTexSelectorCol2 = 0;
						float fTexSelectorCol3 = (0.25f-fabs(fTexSelectorV-0.50))*4.0f;
						if ( fTexSelectorCol3 < 0 ) fTexSelectorCol3 = 0;
						float fTexSelectorCol4 = (0.25f-fabs(fTexSelectorV-0.75))*4.0f;
						if ( fTexSelectorCol4 < 0 ) fTexSelectorCol4 = 0;
						float fTexSelectorCol5 = (0.25f-fabs(fTexSelectorV-1.00))*4.0f;
						if ( fTexSelectorCol5 < 0 ) fTexSelectorCol5 = 0;

						// get reference into memblock mini-texture lookup
						int tx = x-(int(x/4)*4);
						int tz = y-(int(y/4)*4);

						// get pointer to memblock
						DWORD* pMemBlockPot = (DWORD*)g_Memblock_GetMemblockPtr ( iMemblockAssistor );

						// get weighted contrib from each pot
						DWORD dwTexPartD1 = *(pMemBlockPot+(2*16)+(tz*4)+tx);
						int texpartd1r = ((dwTexPartD1 & 0x00FF0000) >> 16) * fTexSelectorCol1;
						int texpartd1g = ((dwTexPartD1 & 0x0000FF00) >> 8 ) * fTexSelectorCol1;
						int texpartd1b = ((dwTexPartD1 & 0x000000FF)      ) * fTexSelectorCol1;
						DWORD dwTexPartD2 = *(pMemBlockPot+(3*16)+(tz*4)+tx);
						int texpartd2r = ((dwTexPartD2 & 0x00FF0000) >> 16) * fTexSelectorCol2;
						int texpartd2g = ((dwTexPartD2 & 0x0000FF00) >> 8 ) * fTexSelectorCol2;
						int texpartd2b = ((dwTexPartD2 & 0x000000FF)      ) * fTexSelectorCol2;
						DWORD dwTexPartD3 = *(pMemBlockPot+(4*16)+(tz*4)+tx);
						int texpartd3r = ((dwTexPartD3 & 0x00FF0000) >> 16) * fTexSelectorCol3;
						int texpartd3g = ((dwTexPartD3 & 0x0000FF00) >> 8 ) * fTexSelectorCol3;
						int texpartd3b = ((dwTexPartD3 & 0x000000FF)      ) * fTexSelectorCol3;
						DWORD dwTexPartD4 = *(pMemBlockPot+(5*16)+(tz*4)+tx);
						int texpartd4r = ((dwTexPartD4 & 0x00FF0000) >> 16) * fTexSelectorCol4;
						int texpartd4g = ((dwTexPartD4 & 0x0000FF00) >> 8 ) * fTexSelectorCol4;
						int texpartd4b = ((dwTexPartD4 & 0x000000FF)      ) * fTexSelectorCol4;
						DWORD dwTexPartD5 = *(pMemBlockPot+(1*16)+(tz*4)+tx);
						int texpartd5r = ((dwTexPartD5 & 0x00FF0000) >> 16) * fTexSelectorCol5;
						int texpartd5g = ((dwTexPartD5 & 0x0000FF00) >> 8 ) * fTexSelectorCol5;
						int texpartd5b = ((dwTexPartD5 & 0x000000FF)      ) * fTexSelectorCol5;

						// combine to a single colour
						int diffusemapr = texpartd1r+texpartd2r+texpartd3r+texpartd4r+texpartd5r;
						int diffusemapg = texpartd1g+texpartd2g+texpartd3g+texpartd4g+texpartd5g;
						int diffusemapb = texpartd1b+texpartd2b+texpartd3b+texpartd4b+texpartd5b;
						if ( diffusemapr>255 ) diffusemapr=255;
						if ( diffusemapg>255 ) diffusemapg=255;
						if ( diffusemapb>255 ) diffusemapb=255;

						// write result into destination, preserve the alpha channel of the dest
						DWORD trgb = (diffusemapr<<16)+(diffusemapg<<8)+(diffusemapb);
						DWORD trgba = *(DWORD*)pDst;
						DWORD talpha = trgba & 0xFF000000;
						*(DWORD*)pDst = talpha + trgb;
					}
					pSrc+=4;
					pDst+=4;
				}
			}
			pSrcSurface->UnlockRect();
		}
		pDstSurface->UnlockRect();
	}
	SAFE_RELEASE(pDstSurface);
	SAFE_RELEASE(pSrcSurface);
}

DARKSDK void Paste ( int iID, int iX, int iY, int iFlag )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtrImage ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);
		return;
	}

	// use sprite library to paste image(texture) with polys!
	if(g_Sprite_PasteImage) g_Sprite_PasteImage( iID, iX, iY, m_ptr->fTexUMax, m_ptr->fTexVMax, iFlag );

	return;
}

DARKSDK void Stretch ( int iID, int Width, int Height )
{
	// returns true if the image exists
	if ( !UpdatePtrImage ( iID ) )
		return;

	// First ensure texture is not already stretched
	D3DSURFACE_DESC ddsd;
	LPDIRECT3DSURFACE9 pSurface = NULL;
	m_ptr->lpTexture->GetSurfaceLevel(0, &pSurface);
	pSurface->GetDesc(&ddsd);
	SAFE_RELEASE(pSurface);
	if ( ddsd.Width==(DWORD)GetPowerSquareOfSize(ddsd.Width)
	&& ddsd.Height==(DWORD)GetPowerSquareOfSize(ddsd.Height)
	&& m_ptr->fTexUMax==1.0f && m_ptr->fTexVMax==1.0f)
	{
		// Image is already power of two and fills entire surface
		return;
	}

	// Record original Size
	int iOrigWidth = m_ptr->iWidth;
	int iOrigHeight = m_ptr->iHeight;

	// Create temp store texture
	LPDIRECT3DTEXTURE9 pNewTexture = NULL;
	HRESULT hr = D3DXCreateTexture (  m_pD3D,
									  m_ptr->iWidth,
									  m_ptr->iHeight,
									  1,
									  D3DX_DEFAULT,
									  g_DefaultD3Dformat,
									  D3DPOOL_MANAGED,
									  &pNewTexture			);

	// leave if failed
	if ( pNewTexture==NULL )
		return;

	// Copy this image to store
	LPDIRECT3DSURFACE9 pNewSurface = NULL;
	pNewTexture->GetSurfaceLevel(0, &pNewSurface);
	LPDIRECT3DSURFACE9 pTexSurface = NULL;
	m_ptr->lpTexture->GetSurfaceLevel(0, &pTexSurface);
	RECT scrRect = { 0, 0, m_ptr->iWidth, m_ptr->iHeight };
	hr = D3DXLoadSurfaceFromSurface(pNewSurface, NULL, NULL, pTexSurface, NULL, &scrRect, D3DX_DEFAULT, 0);
	SAFE_RELEASE(pNewSurface);
	SAFE_RELEASE(pTexSurface);

	// Create new Image, deleting the old one
	Delete ( iID );
	MakeFormat ( iID, Width, Height, g_DefaultD3Dformat, 0 );

	// LEEFIX : 140902 : When created a new texture, the m_ptr can be different so update it
	if ( !UpdatePtrImage ( iID ) )
	{
		SAFE_RELEASE(pNewSurface);
		SAFE_RELEASE(pTexSurface);
		SAFE_RELEASE(pNewTexture);
		return;
	}

	// Copy store to new image
	pNewTexture->GetSurfaceLevel(0, &pNewSurface);
	m_ptr->lpTexture->GetSurfaceLevel(0, &pTexSurface);
	hr = D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pNewSurface, NULL, NULL, D3DX_DEFAULT, 0);
	SAFE_RELEASE(pNewSurface);
	SAFE_RELEASE(pTexSurface);

	// Image is still regarded as original size
	m_ptr->iWidth = iOrigWidth;			// store the width
	m_ptr->iHeight  = iOrigHeight;		// store the height

	// Texture takes up entire area
	m_ptr->fTexUMax=1.0f;
	m_ptr->fTexVMax=1.0f;

	// Free temp store texture
	SAFE_RELEASE(pNewTexture);

	// Complete
	return;
}

DARKSDK void Paste ( int iID, int iX, int iY )
{
	Paste ( iID, iX, iY, 0 );
}

DARKSDK void Delete ( int iID )
{
	if ( !UpdatePtrImage ( iID ) )
		return;

	// before release, remove the reference from ALL objects
	if ( g_Basic3D_ClearObjectsOfTextureRef )
	{
		// leeadd - 220604 - u54 - scans every object that uses this texture address
		g_Basic3D_ClearObjectsOfTextureRef ( m_ptr->lpTexture );
	}

	// clear cube details
	m_ptr->pCubeMapRef = NULL;
	m_ptr->iCubeMapFace = 0;

    RemoveImage( iID );
}

//
// Command Functions
//

DARKSDK void LoadEx	( LPSTR szFilename, int iID, int iTextureFlag, int iDivideTextureSize, int iSilentError )
{
	if(Load( szFilename, iID, iTextureFlag, false, iDivideTextureSize )==false)
	{
		if ( iSilentError==0 )
		{
			char pCWD[256]; _getcwd ( pCWD, 256 );
			char pErr[256]; sprintf ( pErr, "CWD:%s\nLOAD IMAGE %s,%d,%d,%d", pCWD, szFilename, iID, iTextureFlag, iDivideTextureSize);
			RunTimeError(RUNTIMEERROR_IMAGELOADFAILED,pErr);
		}
	}
}

DARKSDK void LoadEx	( LPSTR szFilename, int iID, int iKindOfTexture, int iDivideTextureSize )
{
	return LoadEx ( szFilename, iID, iKindOfTexture, iDivideTextureSize, 0 );
}

DARKSDK void LoadEx	( LPSTR szFilename, int iID, int iKindOfTexture )
{
    LoadEx ( szFilename, iID, iKindOfTexture, 0 );
}

DARKSDK void LoadEx	( LPSTR szFilename, int iID )
{
	int iKindOfTexture = 0; // U67 - zero, U69 - one (FPSC interference?), U70 BETA2 - zero again 
    LoadEx ( szFilename, iID, iKindOfTexture, 0 );
}

DARKSDK void SaveEx	( LPSTR szFilename, int iID )
{
	Save( szFilename, iID );
}

DARKSDK void SaveEx	( LPSTR szFilename, int iID, int iCompressionMode )
{
	Save( szFilename, iID, iCompressionMode );
}

DARKSDK void GrabImageEx ( int iID, int iX1, int iY1, int iX2, int iY2 )
{
	GrabImage( iID, iX1, iY1, iX2, iY2 );
}

DARKSDK void GrabImageEx ( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlag )
{
	GrabImage( iID, iX1, iY1, iX2, iY2, iTextureFlag );
}

DARKSDK void PasteEx ( int iID, int iX, int iY )
{
	Paste( iID, iX, iY, 0 );
}

DARKSDK void PasteEx ( int iID, int iX, int iY, int iFlag )
{
	Paste( iID, iX, iY, iFlag );
}

DARKSDK void DeleteEx ( int iID )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtrImage ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);
		return;
	}
	Delete( iID );
}

DARKSDK void RotateImageEx ( int iID, int iAngle )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK int GetExistEx ( int iID )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return 0;
	}

	// returns true if the image exists
	if ( !UpdatePtrImage ( iID ) )
		return 0;

	// return true
	return 1;
}

DARKSDK LPSTR GetName ( int iID )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return NULL;
	}

	// returns true if the image exists
	if ( !UpdatePtrImage ( iID ) )
		return NULL;

	// return true
	return m_ptr->szShortFilename;
}

//
// New Command Functions
//

DARKSDK void SetColorKey ( int iR, int iG, int iB )
{
	// set the color key of an image
	m_Color = D3DCOLOR_ARGB ( 255, iR, iG, iB );
}

DARKSDK bool FileExist ( LPSTR szFilename )
{
	D3DXIMAGE_INFO info;

	HRESULT hRes = 0;
	if ( g_bImageBlockActive && g_iImageBlockMode==1 )
	{
		DWORD dwFileInMemorySize = 0;
		LPVOID pFileInMemoryData = NULL;
		char pFinalRelPathAndFile[512];
		GetFileInMemory ( szFilename, &pFileInMemoryData, &dwFileInMemorySize, pFinalRelPathAndFile );
		hRes = D3DXGetImageInfoFromFileInMemory( pFileInMemoryData, dwFileInMemorySize, &info );
	}
	else
		hRes = D3DXGetImageInfoFromFile( szFilename, &info );

	if ( hRes==D3D_OK )
		return true;
	else
		return false;
}

DARKSDK DWORD LoadIcon ( LPSTR pFilename )
{
	// load icon
	HICON hIconHandle = (HICON)LoadImage ( NULL, pFilename, IMAGE_ICON, 48, 48, LR_LOADFROMFILE );

	// complete
	return (DWORD)hIconHandle;
}

DARKSDK void FreeIcon ( DWORD dwIcon )
{
	// free icon handle
    CloseHandle ( (HICON)dwIcon );
}

//
// Data Access Functions
//

DARKSDK void GetImageData( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	// Read Data
	if(bLockData==true)
	{
		if ( !UpdatePtrImage ( iID ) )
			return;

		if ( m_ptr->lpTexture==NULL )
			return;

		// data
		*dwWidth = m_ptr->iWidth;
		*dwHeight = m_ptr->iHeight;
		*dwDepth = m_ptr->iDepth;
		DWORD bitdepth = m_ptr->iDepth/8;

		// use actual size, not image size
		D3DSURFACE_DESC desc;
		m_ptr->lpTexture->GetLevelDesc(0,&desc);
		if(desc.Width<*dwWidth) *dwWidth=desc.Width;
		if(desc.Height<*dwHeight) *dwHeight=desc.Height;

		// create system memory version
		LPDIRECT3DTEXTURE9 pTempSysMemTexture = NULL;
		D3DXCreateTexture( m_pD3D, desc.Width, desc.Height, 1, 0, desc.Format, D3DPOOL_SYSTEMMEM, &pTempSysMemTexture );

		// and copy texture image to it
		LPDIRECT3DSURFACE9 pTempSysMemSurface = NULL;
		LPDIRECT3DSURFACE9 pTempVidMemSurface = NULL;
		pTempSysMemTexture->GetSurfaceLevel ( 0, &pTempSysMemSurface );
		m_ptr->lpTexture->GetSurfaceLevel ( 0, &pTempVidMemSurface );
		//m_pD3D->UpdateSurface ( pTempVidMemSurface, NULL, pTempSysMemSurface, NULL);
		D3DXLoadSurfaceFromSurface ( pTempSysMemSurface, 0, NULL, pTempVidMemSurface, 0, NULL, D3DX_DEFAULT, 0);

		// lock
		D3DLOCKED_RECT d3dlock;
		RECT rc = { 0, 0, *dwWidth, *dwHeight };
		if(SUCCEEDED(pTempSysMemTexture->LockRect ( 0, &d3dlock, &rc, 0 ) ) )
		{
			// create memory
			DWORD dwSizeOfBitmapData = (*dwWidth)*(*dwHeight)*bitdepth;
			*pData = new char[dwSizeOfBitmapData];
			*dwDataSize = dwSizeOfBitmapData;

			// copy from surface
			LPSTR pSrc = (LPSTR)d3dlock.pBits;
			LPSTR pPtr = *pData;
			DWORD dwDataWidth=*(dwWidth)*bitdepth;
			for(DWORD y=0; y<*dwHeight; y++)
			{
				memcpy(pPtr, pSrc, dwDataWidth);
				pPtr+=dwDataWidth;
				pSrc+=d3dlock.Pitch;
			}
			pTempSysMemTexture->UnlockRect(0);
		}
		else
		{
			// leefix - 250604 - u54 - place a one in size to indicate could not lock (exists but protected)
			*dwDataSize = 1;
		}

		// free temp system surface
		SAFE_RELEASE(pTempVidMemSurface);
		SAFE_RELEASE(pTempSysMemSurface);
		SAFE_RELEASE(pTempSysMemTexture);
	}
	else
	{
		// free memory
		delete *pData;
	}
}

DARKSDK void SetImageData( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize )
{
	if ( UpdatePtrImage ( iID ) )
	{
		if ( m_ptr->lpTexture==NULL )
			return;

		// Check new specs with existing one
		if(dwWidth==(DWORD)m_ptr->iWidth && dwHeight==(DWORD)m_ptr->iHeight && dwDepth==(DWORD)m_ptr->iDepth)
		{
			// Same size
		}
		else
		{
			// Recreate
			Delete ( iID );
			m_ptr=NULL;
		}
	}

	// new image
	if(m_ptr==NULL)
	{
		D3DFORMAT d3dformat;
		if(dwDepth==16) d3dformat=D3DFMT_A1R5G5B5;
		if(dwDepth==32) d3dformat=D3DFMT_A8R8G8B8;
		MakeFormat ( iID, dwWidth, dwHeight, d3dformat, 0 );
	}

	// may have changed
	if ( !UpdatePtrImage ( iID ) ) return;
	if ( m_ptr->lpTexture==NULL ) return;

	// Write Data
	D3DLOCKED_RECT d3dlock;
	DWORD bitdepth = m_ptr->iDepth/8;
	RECT rc = { 0, 0, dwWidth, dwHeight };
	if(SUCCEEDED(m_ptr->lpTexture->LockRect ( 0, &d3dlock, &rc, 0 ) ) )
	{
		// copy from surface
		LPSTR pSrc = (LPSTR)d3dlock.pBits;
		LPSTR pPtr = pData;
		DWORD dwDataWidth=dwWidth*bitdepth;
		for(DWORD y=0; y<dwHeight; y++)
		{
			memcpy(pSrc, pPtr, dwDataWidth);
			pPtr+=dwDataWidth;
			pSrc+=d3dlock.Pitch;
		}
		m_ptr->lpTexture->UnlockRect(0);
	}

	// ensure sprites all updated
	g_Update_All_Sprites();
}

//
// IMAGE BLOCK CODE
//

void OpenImageBlock	( char* szFilename, int iMode )
{
	// cannot open if already open
	if ( g_iImageBlockMode!=-1 )
		return;

	// Reset exclude path
	strcpy ( g_pImageBlockExcludePath, "" );

	// Create image block details
	g_iImageBlockFilename = new char [ strlen ( szFilename ) + 1 ];
	strcpy ( g_iImageBlockFilename, szFilename );

	// Create path to image block
	char current [ 512 ];
	_getcwd ( current, 512 );
	g_iImageBlockRootPath = new char [ strlen ( current ) + 2 ];
	strcpy ( g_iImageBlockRootPath, current );
	if ( g_iImageBlockRootPath [ strlen(g_iImageBlockRootPath)-1 ]!='\\' )
	{
		// add folder divide at end of path string
		int iLen = strlen(g_iImageBlockRootPath);
		g_iImageBlockRootPath [ iLen+0 ] = '\\';
		g_iImageBlockRootPath [ iLen+1 ] = 0;
	}
	
	// Set the imageblock mode (0-write, 1-read)
	g_bImageBlockActive = true;
	g_iImageBlockMode = iMode;

	// U77 - 060211 - does previously written image block exist, if so, we append to it
	bool bPreviousImageBlockExists = false;
	HANDLE hFile = CreateFile ( g_iImageBlockFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile!=INVALID_HANDLE_VALUE )
	{
		bPreviousImageBlockExists = true;
		CloseHandle ( hFile );
	}

	// Load imageblock (for reading or to load last imageblock from previous write)
	if ( g_iImageBlockMode==1 || bPreviousImageBlockExists==true )
	{
		// open to read
		HANDLE hFile = CreateFile ( g_iImageBlockFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		DWORD dwReadBytes = 0;

		// read file list
		int iListMax = 0;
		g_ImageBlockListFile.clear();
		ReadFile ( hFile, &iListMax, sizeof(int), &dwReadBytes, NULL );
		for ( int i = 0; i < iListMax; i++ )
		{
			// write filename length and string
			DWORD dwFilenameLength = 0;
			ReadFile ( hFile, &dwFilenameLength, sizeof(DWORD), &dwReadBytes, NULL );
			LPSTR pFile = new char [ dwFilenameLength ];
			ReadFile ( hFile, pFile, dwFilenameLength, &dwReadBytes, NULL );
			g_ImageBlockListFile.push_back ( pFile );

			// write file offset in data
			DWORD dwListOffset = 0;
			ReadFile ( hFile, &dwListOffset, sizeof(DWORD), &dwReadBytes, NULL );
			g_ImageBlockListOffset.push_back ( dwListOffset );

			// write file size in data
			DWORD dwListSize = 0;
			ReadFile ( hFile, &dwListSize, sizeof(DWORD), &dwReadBytes, NULL );
			g_ImageBlockListSize.push_back ( dwListSize );
		}

		// write the imageblock data itself
		ReadFile ( hFile, &g_dwImageBlockSize, sizeof(DWORD), &dwReadBytes, NULL );
		g_pImageBlockPtr = new char [ g_dwImageBlockSize ];
		ReadFile ( hFile, g_pImageBlockPtr, g_dwImageBlockSize, &dwReadBytes, NULL );
	
		// close file
		CloseHandle ( hFile );
	}
	else
	{
		// Create memory block for saving
		g_dwImageBlockSize = 0;
		g_pImageBlockPtr = NULL;

		// Clear file list
		g_ImageBlockListFile.clear();
		g_ImageBlockListOffset.clear();
		g_ImageBlockListSize.clear();
	}
}

void ExcludeFromImageBlock ( char* szExcludePath )
{
	// exclude any file starting with this string
    if (szExcludePath)
    	strcpy ( g_pImageBlockExcludePath, szExcludePath );
    else
        g_pImageBlockExcludePath[0] = 0;
}

bool AddToImageBlock ( LPSTR pAddFilename )
{
	// can only add in write mode
	if ( g_iImageBlockMode!=0 ) return true;

	// if exist
	if ( !pAddFilename ) return false;

	// exclude if path matches excluder, but only if excluder has a value
    if (g_pImageBlockExcludePath && g_pImageBlockExcludePath[0])
    	if ( strnicmp ( g_pImageBlockExcludePath, pAddFilename, strlen(g_pImageBlockExcludePath) )==NULL )
	    	return true;

	// ensure it does not already exist
	for ( int i = 0; i < (int)g_ImageBlockListFile.size ( ); i++ )
		if ( _stricmp ( g_ImageBlockListFile [ i ], pAddFilename )==NULL )
			return true;

	// open the file
	HANDLE hFile = CreateFile ( pAddFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile==INVALID_HANDLE_VALUE ) return false;

	// read the file data
	DWORD dwReadBytes = 0;
	DWORD dwFileSize = GetFileSize ( hFile, NULL );
	LPSTR pFileData = new char [ dwFileSize ];
	ReadFile ( hFile, pFileData, dwFileSize, &dwReadBytes, NULL );
	
	// close file
	CloseHandle ( hFile );

	// create space in the imageblock
	DWORD dwNewSize = g_dwImageBlockSize + dwFileSize;

	// U76 - 020710 - image blocks can get LARGE enough to overrun virtual address space
	// so catch the exeption raised in this case and end the image block creation process gracefully
	LPSTR pNewData = NULL;
	try
	{
		pNewData = new char [ dwNewSize ];
	}
	catch(...)
	{
		// failed to create a new "continuous" block of memory in virtual address space
		// so end image block creation and exit here
		SAFE_DELETE ( pFileData );
		CloseImageBlock();
		return false;
	}
	memcpy ( pNewData, g_pImageBlockPtr, g_dwImageBlockSize );

	// add data to the imageblock
	LPSTR pNewDataInsertPtr = pNewData + g_dwImageBlockSize;
	DWORD dwOffsetToDataInImageBlock = g_dwImageBlockSize;
	memcpy ( pNewDataInsertPtr, pFileData, dwFileSize );

	// free individual file data
	SAFE_DELETE ( pFileData );

	// erase old imageblock and use new one
	SAFE_DELETE ( g_pImageBlockPtr );
	g_dwImageBlockSize = dwNewSize;
	g_pImageBlockPtr = pNewData;

	// add it to list
	LPSTR pListFilename = new char [ strlen ( pAddFilename )+1 ];
	strcpy ( pListFilename, pAddFilename );
	g_ImageBlockListFile.push_back ( pListFilename );
	g_ImageBlockListOffset.push_back ( dwOffsetToDataInImageBlock );
	g_ImageBlockListSize.push_back ( dwFileSize );

	// success
	return true;
}

LPSTR RetrieveFromImageBlock ( LPSTR pRetrieveFilename, DWORD* pdwFileSize )
{
	// find the file
	int iIndexInListFound = -1;
	for ( int iIndexInList = 0; iIndexInList < (int)g_ImageBlockListFile.size ( ); iIndexInList++ )
	{
		if ( _stricmp ( g_ImageBlockListFile [ iIndexInList ], pRetrieveFilename )==NULL )
		{
			iIndexInListFound = iIndexInList;
			break;
		}
	}
	if ( iIndexInListFound==-1 )
	{
		// not found 
		return NULL;
	}

	// locate file within imageblock
	DWORD dwOffset = g_ImageBlockListOffset [ iIndexInListFound ];
	DWORD dwSize = g_ImageBlockListSize [ iIndexInListFound ];

	// return ptr and size
	if ( pdwFileSize ) *pdwFileSize = dwSize;
	return g_pImageBlockPtr + dwOffset;
}

void CloseImageBlock ( void )
{
	// cannot close if already closed
	if ( g_iImageBlockMode==-1 )
		return;

	// Save imageblock
	if ( g_iImageBlockMode==0 )
	{
		// set original path
		char storedir [ 512 ];
		_getcwd ( storedir, 512 );
		_chdir ( g_iImageBlockRootPath );

		// open to write
		HANDLE hFile = CreateFile ( g_iImageBlockFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		DWORD dwWrittenBytes = 0;

		// write file list
		int iListMax = g_ImageBlockListFile.size ( );
		WriteFile ( hFile, &iListMax, sizeof(int), &dwWrittenBytes, NULL );
		for ( int i = 0; i < iListMax; i++ )
		{
			// write filename length and string
			LPSTR pListPtr = g_ImageBlockListFile [ i ];
			DWORD dwFilenameLength = strlen(pListPtr)+1;
			WriteFile ( hFile, &dwFilenameLength, sizeof(DWORD), &dwWrittenBytes, NULL );
			WriteFile ( hFile, pListPtr, dwFilenameLength, &dwWrittenBytes, NULL );

			// write file offset in data
			DWORD dwListOffset = g_ImageBlockListOffset [ i ];
			WriteFile ( hFile, &dwListOffset, sizeof(DWORD), &dwWrittenBytes, NULL );

			// write file size in data
			DWORD dwListSize = g_ImageBlockListSize [ i ];
			WriteFile ( hFile, &dwListSize, sizeof(DWORD), &dwWrittenBytes, NULL );
		}

		// write the imageblock data itself
		WriteFile ( hFile, &g_dwImageBlockSize, sizeof(DWORD), &dwWrittenBytes, NULL );
		WriteFile ( hFile, g_pImageBlockPtr, g_dwImageBlockSize, &dwWrittenBytes, NULL );
	
		// close file
		CloseHandle ( hFile );

		// restore folder
		_chdir ( storedir );
	}

	// free strings in imageblock file list
	for ( int i = 0; i < (int)g_ImageBlockListFile.size(); i++ )
		SAFE_DELETE ( g_ImageBlockListFile [ i ] );
	g_ImageBlockListFile.clear();
	g_ImageBlockListOffset.clear();
	g_ImageBlockListSize.clear();

	// free filename
	SAFE_DELETE ( g_iImageBlockFilename );
	SAFE_DELETE ( g_iImageBlockRootPath );

	// Close imageblock
	SAFE_DELETE ( g_pImageBlockPtr );

	// Switch off imageblock
	g_bImageBlockActive = false;
	g_iImageBlockMode = -1;
}

void PerformChecklistForImageBlockFiles ( void )
{
	// Generate Checklist
	DWORD dwMaxStringSizeInEnum=0;
	bool bCreateChecklistNow=false;
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, dwMaxStringSizeInEnum);
		}

		// Look at parameters
		g_pGlob->checklistqty=0;
		for ( int i = 0; i < (int)g_ImageBlockListFile.size ( ); i++ )
		{
			// write filename length and string
			LPSTR pListPtr = g_ImageBlockListFile [ i ];
			if ( !pListPtr ) continue;

			// Add to checklist
			DWORD dwSize = strlen(pListPtr);
			if(dwSize>dwMaxStringSizeInEnum) dwMaxStringSizeInEnum=dwSize;
			if(bCreateChecklistNow)
			{
				// New checklist item
				strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, pListPtr);
				g_pGlob->checklist[g_pGlob->checklistqty].valuea = 0;
				g_pGlob->checklist[g_pGlob->checklistqty].valueb = 0;
				g_pGlob->checklist[g_pGlob->checklistqty].valuec = 0;
				g_pGlob->checklist[g_pGlob->checklistqty].valued = 0;
			}
			g_pGlob->checklistqty++;
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

int GetImageFileExist ( LPSTR pFilename )
{
	// If no string, no file
	if ( pFilename==NULL ) return 0;
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, pFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// if image block file, quick early out using imageblock
	if ( g_bImageBlockActive && g_iImageBlockMode==1 )
	{
		// final storage string of path and file resolver (makes the filename and path uniform for imageblock retrieval)
		// and work out true file and path, then look for it in imageblock
		char pFinalRelPathAndFile[512];
		LPVOID pFileInMemoryData = 0;
		DWORD dwFileInMemorySize = 0;
		GetFileInMemory ( VirtualFilename, &pFileInMemoryData, &dwFileInMemorySize, pFinalRelPathAndFile );
		if ( pFileInMemoryData )
			return 1;
	}

	// real file
	HANDLE hfile = CreateFile ( VirtualFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return 0;

	// success, it exists
	CloseHandle(hfile);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorImage ( HINSTANCE hSetup )
{
	Constructor ( hSetup );
}

void DestructorImage ( void )
{
	Destructor ( );
}

void SetErrorHandlerImage ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassSpriteInstanceImage ( HINSTANCE hSprite )
{
	PassSpriteInstance ( hSprite );
}

void RefreshD3DImage ( int iMode )
{
	RefreshD3D ( iMode );
}

void PassCoreDataImage( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void dbLoadImage ( char* szFilename, int iID )
{
	LoadEx ( szFilename, iID );
}

void dbLoadImage ( char* szFilename, int iID, int TextureFlag )
{
	LoadEx ( szFilename, iID, TextureFlag );
}

void dbLoadImage ( char* szFilename, int iID, int TextureFlag, int iDivideTextureSize )
{
	LoadEx ( szFilename, iID, TextureFlag, iDivideTextureSize );
}

void dbSaveImage ( char* szFilename, int iID )
{
	SaveEx ( szFilename, iID );
}

void dbSaveImage ( char* szFilename, int iID, int iCompressionMode )
{
	SaveEx ( szFilename, iID, iCompressionMode );
}

void dbGetImage ( int iID, int iX1, int iY1, int iX2, int iY2 )
{
	GrabImageEx ( iID, iX1, iY1, iX2, iY2 );
}

void dbGetImage ( int iID, int iX1, int iY1, int iX2, int iY2, int iTextureFlag )
{
	GrabImageEx ( iID, iX1, iY1, iX2, iY2, iTextureFlag );
}

void dbPasteImage ( int iID, int iX, int iY )
{
	PasteEx ( iID, iX, iY );
}

void dbPasteImage ( int iID, int iX, int iY, int iFlag )
{
	PasteEx ( iID, iX, iY, iFlag );
}

void dbDeleteImage ( int iID )
{
	DeleteEx ( iID );
}
 
void dbRotateImage ( int iID, int iAngle )
{
	RotateImageEx ( iID, iAngle );
}
 
int dbImageExist ( int iID )
{
	return GetExistEx ( iID );
}

void dbSetImageColorKey ( int iR, int iG, int iB )
{
	SetColorKey ( iR, iG, iB );
}

void dbSaveIconFromImage ( char* pFilename, int iID )
{
	//SaveIconFromImage ( pFilename, iID );
}

DWORD dbLoadIcon ( char* pFilename )
{
	return LoadIcon ( pFilename );
}

void dbFreeIcon ( DWORD dwIconHandle )
{
	FreeIcon ( dwIconHandle );
}

void dbGetImageData ( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	GetImageData ( iID, dwWidth, dwHeight, dwDepth, pData, dwDataSize, bLockData );
}

void dbSetImageData ( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize )
{
	SetImageData ( iID, dwWidth, dwHeight, dwDepth, pData, dwDataSize );
}

LPDIRECT3DTEXTURE9 dbMakeImage ( int iID, int iWidth, int iHeight )
{
	return Make ( iID, iWidth, iHeight );
}

LPDIRECT3DTEXTURE9 dbMakeImageUsage ( int iID, int iWidth, int iHeight, DWORD dwUsage )
{
	return MakeUsage ( iID, iWidth, iHeight, dwUsage );
}

LPDIRECT3DTEXTURE9 dbMakeImageJustFormat ( int iID, int iWidth, int iHeight, D3DFORMAT Format )
{
	return MakeUsage ( iID, iWidth, iHeight, Format );
}

LPDIRECT3DTEXTURE9 dbMakeImageRenderTarget ( int iID, int iWidth, int iHeight, D3DFORMAT Format )
{
	return MakeRenderTarget	( iID, iWidth, iHeight, Format );
}

LPDIRECT3DTEXTURE9 dbGetImagePointer ( int iID )
{
	return GetPointer ( iID );
}

int dbGetImageWidth ( int iID )
{
	return GetWidth ( iID );
}

int dbGetImageHeight ( int iID )
{
	return GetHeight ( iID );
}

int dbLoadImageInternal ( char* szFilename )
{
	return LoadInternal ( szFilename );
}

int	dbLoadImageInternal	( char* szFilename, int iDivideTextureSize )
{
	return LoadInternal ( szFilename, iDivideTextureSize );
}

bool dbGetImageExist ( int iID )
{
	return GetExist ( iID );
}

LPSTR dbGetImageName ( int iID )
{
	return GetName ( iID );
}

void dbSetImageSharing ( bool bMode )
{
	SetSharing ( bMode );
}

void dbSetImageMemory ( int iMode )
{
	SetMemory ( iMode );
}

void dbLockImage ( int iID )
{
	Lock ( iID );
}

void dbUnlockImage ( int iID )
{
	Unlock ( iID );
}

void dbWriteImage ( int iID, int iX, int iY, int iA, int iR, int iG, int iB )
{
	Write ( iID, iX, iY, iA, iR, iG, iB );
}

void dbGetImage ( int iID, int iX, int iY, int* piR, int* piG, int* piB )
{
	Get ( iID, iX, iY, piR, piG, piB );
}

void dbSetImageMipmapMode ( bool bMode )
{
	SetMipmapMode ( bMode );
}

void dbSetImageMipmapType ( int iType )
{
	SetMipmapType ( iType );
}

void dbSetImageMipmapBias ( float fBias )
{
	SetMipmapBias ( fBias );
}

void dbSetImageMipmapNum ( int iNum )
{
	SetMipmapNum ( iNum );
}

void dbSetImageTranslucency ( int iID, int iPercent )
{
	SetTranslucency ( iID, iPercent );
}

void dbSetImageCubeFace	( int iID, LPDIRECT3DCUBETEXTURE9 pCubeMap, int iFace )
{
	SetCubeFace ( iID, pCubeMap, iFace );
}

void dbGetImageCubeFace	( int iID, LPDIRECT3DCUBETEXTURE9* ppCubeMap, int* piFace )
{
	GetCubeFace ( iID, ppCubeMap, piFace );
}

bool dbLoadImageEx ( char* szFilename, int iID )
{
	return Load ( szFilename, iID );
}

float dbGetImageUMax ( int iID )
{
	return GetUMax ( iID );
}

float dbGetImageVMax ( int iID )
{
	return GetVMax ( iID );
}

void dbStretchImage ( int iID, int iSizeX, int iSizeY )
{
	Stretch ( iID, iSizeX, iSizeY );
}

void dbLoadImageTextureFlag ( char* szFilename, int iID, int TextureFlag )
{
	LoadEx ( szFilename, iID, TextureFlag );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////