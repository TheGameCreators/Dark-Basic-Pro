

#ifndef _TEXTURES_H
#define _TEXTURES_H

//////////////////////////////////////////////////////////////////////////
// include files /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS

#include < d3d9.h >
#include < Dxerr9.h >
#include < D3dx9tex.h >
#include < D3dx9core.h >
#include < basetsd.h >
#include < stdio.h >
#include < math.h >
#include < D3DX9.h >
#include < d3d9types.h >

#define WIN32_LEAN_AND_MEAN 
#include < windows.h >

#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////

#include "loader.h"

struct TexOptions
{
	D3DCOLOR ColorKey;
};

extern struct _Textures
{
	// variables for texture management
	
	// information for every single texture
	
	int Tex_count;

	struct _Tex
	{
		LPSTR				FileName;
		TexOptions			Options;
		LPDIRECT3DTEXTURE9	lpD3DTex;
	
		UINT original_width;
		UINT original_height;
	} *Tex;

	int Tex_size;

	void Tex_Init ( )
	{
		Tex       = NULL;
		Tex_count = 0;
		Tex_size  = 0;
	}

	void Tex_Resize ( int count )
	{
		if ( count > Tex_size )
		{
			count += 10;

			Tex      = ( _Tex* ) realloc ( Tex, sizeof ( _Tex ) * count );
			Tex_size = count;

			for ( int x = Tex_count; x < Tex_size; x++ )
			{
				Tex [ x ].FileName = NULL;
				Tex [ x ].lpD3DTex = NULL;
			}
		}
		else if ( Tex_size-count > 10 )
		{
			for ( int x = count; x < Tex_size; x++ )
			{
				SAFE_DELETE  ( Tex [ x ].FileName );
				SAFE_RELEASE ( Tex [ x ].lpD3DTex );
			}
			
			Tex      = ( _Tex* ) realloc ( Tex, sizeof ( _Tex ) * count );
			Tex_size = count;
		}
	}

	int LightMap_count;

	struct _LightMap
	{
		LPSTR				FileName;
		LPDIRECT3DTEXTURE9	lpD3DTex;
		UINT				original_width;
		UINT				original_height;
	} *LightMap;

	int LightMap_size;

	void LightMap_Init ( )
	{
		LightMap       = NULL;
		LightMap_count = 0;
		LightMap_size  = 0;
	}

	void LightMap_Resize ( int count )
	{
		if ( count > LightMap_size )
		{
			count += 10;

			LightMap      = ( _LightMap* ) realloc ( LightMap, sizeof ( _LightMap ) * count );
			LightMap_size = count;
			
			for ( int x = LightMap_count; x < LightMap_size; x++ )
			{
				LightMap [ x ].FileName = NULL;
				LightMap [ x ].lpD3DTex = NULL;
			}
		}
		else if ( LightMap_size-count > 10 )
		{
			for ( int x = count; x < LightMap_size; x++ )
			{
				SAFE_DELETE  ( LightMap [ x ].FileName );
				SAFE_RELEASE ( LightMap [ x ].lpD3DTex );
			}

			LightMap      = ( _LightMap* ) realloc ( LightMap, sizeof ( _LightMap ) * count );
			LightMap_size = count;
		}
	}

	int		NotFound_count;
	LPSTR*	NotFound_FileName;
	int		NotFound_size;

	void NotFound_Init ( )
	{
		NotFound_FileName = NULL;
		NotFound_count    = 0;
		NotFound_size     = 0;
	}

	void NotFound_Resize ( int count )
	{
		if ( count > NotFound_size )
		{
			count += 10;

			NotFound_FileName = ( LPSTR* ) realloc ( NotFound_FileName, sizeof ( LPSTR ) * count );
			NotFound_size     = count;
		}
	}

	struct _cache
	{
		int entry_count;

		struct _entry
		{
			LPSTR filename;
			LPSTR path;
			LPSTR fullname;
			LPSTR resource;
		} *entry;

		int size;

		void Init ( )
		{
			entry       = NULL;
			entry_count = 0;
			size        = 0;
		}

		void Resize ( int num )
		{
			if ( num > size )
			{
				num += 100;
				entry = ( _entry* ) realloc ( entry, sizeof ( _entry ) * num );
				size  = num;

				for ( int x = entry_count; x < size; x++ )
				{
					entry [ x ].filename = NULL;
					entry [ x ].path     = NULL;
					entry [ x ].fullname = NULL;
					entry [ x ].resource = NULL;
				}
			}
			else if ( size - num > 100 )
			{
				for ( int x = num; x < size; x++ )
				{ 
					SAFE_DELETE_ARRAY ( entry [ x ].filename );
					SAFE_DELETE_ARRAY ( entry [ x ].path );
					SAFE_DELETE_ARRAY ( entry [ x ].fullname );
					SAFE_DELETE_ARRAY ( entry [ x ].resource );
				}

				entry = ( _entry* ) realloc ( entry, sizeof ( _entry ) * num );
				size  = num;
			}
		}

		void Release ( )
		{
			if ( entry )
			{
				for ( int i = 0; i < size; i++ )
				{
					SAFE_DELETE_ARRAY ( entry [ i ].filename );
					SAFE_DELETE_ARRAY ( entry [ i ].path );
					SAFE_DELETE_ARRAY ( entry [ i ].fullname );
					SAFE_DELETE_ARRAY ( entry [ i ].resource );
				}
			}

			Resize ( 0 );
			SAFE_DELETE ( entry );
			entry_count = 0;
		}

		BOOL Get_Category_From_FileName ( LPSTR filename, LPSTR* cat )
		{
			for ( int x = 0; x < entry_count; x++ )
			{
				if ( strlen ( entry [ x ].filename ) == strlen ( filename ) && _stricmp ( entry [ x ].filename, filename ) == 0 )
				{
					strcpy2 ( cat, entry [ x ].path );
					return TRUE;
				}
			}
		
			return FALSE;
		}

		int Get_ID_From_FileName ( LPSTR filename )
		{
			for ( int x = 0; x < entry_count; x++ )
			{
				if ( strlen ( entry [ x ].filename ) == strlen ( filename ) && _stricmp ( entry [ x ].filename, filename ) == 0 )
					return x;
			}
		
			return -1;
		}

	} cache;

	BOOL Reset ( );
	
	int Load_By_FileName      ( LPSTR FileName );
	int Load_By_FileName      ( LPSTR FileName, BOOL forceload );
	int Load_By_RAW_Data      ( byte* data, int length, LPSTR FileName, TexOptions options );
	int Load_By_RAW_Data      ( byte* data, int length, LPSTR FileName, TexOptions options, BOOL forceload );
	int Get_ID_For_FileName   ( LPSTR FileName, BOOL forceload );
	LPSTR Get_FileName_For_ID ( int id );
	TexOptions GetTexOptions  ( LPSTR FileName );
	
	int   Load_LightMap_By_FileName    ( LPSTR FileName );
	int   Get_LightMap_ID_For_FileName ( LPSTR FileName );
	LPSTR Get_LightMap_FileName_For_ID ( int id );

	LPDIRECT3DTEXTURE9 Create_From_RAW_Data ( byte* data, UINT width, UINT height, int bpp );
	LPDIRECT3DTEXTURE9 Create_From_WAD_Data ( byte* data, int length, UINT* w, UINT* h );
	LPDIRECT3DTEXTURE9 Create_From_WAL_Data ( byte* data, int length, UINT* w, UINT* h );
	
	int Load_By_FileName_Q2      ( LPSTR FileName );
	int Load_By_FileName_Q2      ( LPSTR FileName, BOOL forceload );
	int Load_By_Data_Q2          ( LPSTR FileName, byte* data, UINT width, UINT height, int bpp );
	int Load_By_Data_Q2          ( LPSTR FileName, byte* data, UINT width, UINT height, int bpp, BOOL forceload );
	int Load_LightMap_By_Data_Q2 ( byte* data, UINT width, UINT height, int bpp );
	int Load_By_FileName_Q3A     ( LPSTR FileName );
	int Load_By_FileName_Q3A     ( LPSTR FileName, BOOL forceload );
	int Get_ID_For_FileNameQ3A   ( LPSTR FileName, BOOL forceload );

} Textures;

#endif