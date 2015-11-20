#include "Textures.h"

extern LPDIRECT3DDEVICE9		m_pD3D;

struct _Textures Textures;

extern files_found Q3A_Resources;


BOOL _Textures::Reset ( )
{
	Tex_Resize ( 0 );
	SAFE_DELETE ( Tex );
	Tex_count = 0;

	LightMap_Resize ( 0 );
	SAFE_DELETE ( LightMap );
	LightMap_count = 0;

	for ( int x = 0; x < NotFound_count; x++ )
		SAFE_DELETE ( NotFound_FileName [ x ] );

	NotFound_size  = 0;
	NotFound_count = 0;

	free ( NotFound_FileName );
	NotFound_FileName=NULL;

	return TRUE;
}

int _Textures::Load_By_FileName ( LPSTR FileName )
{
	 return Load_By_FileName ( FileName, FALSE );
}

int _Textures::Load_By_FileName ( LPSTR FileName, BOOL forceload )
{

	if ( strlen ( FileName ) < 1 )
		return -1;

	int i = Get_ID_For_FileName ( FileName, forceload );
	
	if ( i != -2 )
		return i;

	char RootDir [ MAX_PATH ];
	sprintf ( RootDir, "%s\\Textures", "" );
	_chdir ( RootDir );

	byte* data = NULL; 
	int length;

	int x = 0;

	for ( x = 0; x < Textures.cache.entry_count; x++ )
	{
		if ( _stricmp ( Textures.cache.entry [ x ].filename, FileName ) == 0 )
		{
			if ( file_loader::Load::Direct ( Textures.cache.entry [ x ].fullname, &data, &length ) )
			{
				files_cache.AddFile ( Textures.cache.entry [ x ].fullname, Textures.cache.entry [ x ].filename, Textures.cache.entry [ x ].path );
				break;
			}
			else
			{
				x = Textures.cache.entry_count;
				break;		
			}
		}
	}

	if ( x == Textures.cache.entry_count )
	{
		i = Textures.Load_By_FileName_Q3A ( FileName );

		if ( i == -1 )
		{
			SAFE_DELETE ( data );
			Textures.NotFound_Resize ( Textures.NotFound_count + 1 );
			strcpy2 ( &Textures.NotFound_FileName [ Textures.NotFound_count ], FileName );
			Textures.NotFound_count++;
		}

		return i;
	}

	i = Load_By_RAW_Data ( data, length, FileName, GetTexOptions ( FileName ), forceload );

	SAFE_DELETE ( data );

	return i;
}

int _Textures::Load_By_RAW_Data ( byte* data, int length, LPSTR FileName, TexOptions options )
{
	return Load_By_RAW_Data ( data, length, FileName, options, FALSE );
}

int _Textures::Load_By_RAW_Data ( byte* data, int length, LPSTR FileName, TexOptions options, BOOL forceload )
{
	if ( strlen ( FileName ) < 1 )
		return -1;

	int i = Get_ID_For_FileName ( FileName, forceload );
	
	if ( i != -2 )
		return i;

	int n = Textures.Tex_count;
	Tex_Resize ( n + 1 );
	Tex [ n ].Options = options;

	D3DFORMAT format = D3DFMT_UNKNOWN;

	if ( options.ColorKey != 0 )
		format = D3DFMT_A8R8G8B8;

	D3DXIMAGE_INFO info;

	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx (
														m_pD3D, data, length,
														D3DX_DEFAULT, D3DX_DEFAULT,
//														D3DX_DEFAULT, 0, format, D3DPOOL_MANAGED,
														2, 0, format, D3DPOOL_MANAGED,
														D3DX_DEFAULT, D3DX_DEFAULT,
														options.ColorKey,
														&info, NULL, &Textures.Tex[n].lpD3DTex
													 );
									
	Tex [ n ].original_width  = info.Width;
	Tex [ n ].original_height = info.Height;
						
	if ( hr == D3D_OK )
	{
		strcpy2 ( &Textures.Tex [ n ].FileName, FileName );
		Textures.Tex_count++;
		return n;
	}
	else
		return -1;
}

int _Textures::Get_ID_For_FileName ( LPSTR FileName, BOOL forceload )
{
	for ( int x = 0; x < Textures.Tex_count; x++ )
	{
		if ( _stricmp ( Textures.Tex [ x ].FileName, FileName ) == 0 )
			return x;
	}

	if ( forceload )
		return -2;
		
	for ( int x = 0; x < Textures.NotFound_count; x++ )
	{
		if ( _stricmp ( Textures.NotFound_FileName [ x ], FileName ) == 0 )
			return -1;
	}
	
	return -2;
}

LPSTR _Textures::Get_FileName_For_ID ( int id )
{
	if ( id < 0 || id >= Tex_count )
		return "";

	return Textures.Tex [ id ].FileName;
}

struct _option_cache
{
	LPSTR FileName [ 1024 ];
	TexOptions Options [ 1024 ];
	int count;
} option_cache;

void InitOptions ( )
{
	FILE* file;
	char temp [ 1024 ];
	
	sprintf ( temp, "%s\\Textures\\options.ini", "" );
	
	file = fopen ( temp, "r" );
	
	while ( feof ( file ) == 0 )
	{
		int pos = 0;

		fgets ( temp, 1024, file );

		if ( temp [ 0 ] == '"' )
		{
			int n = option_cache.count;
			pos++;
			char strg [ 1024 ];

			for ( int x = 0; x != 1024; x++, pos++ )
			{
				strg [ x ] = temp [ pos ];

				if ( strg [ x ] == '"' )
				{
					strg [ x ] = '\0'; 
					break;
				}
			}

			SAFE_DELETE ( option_cache.FileName [ n ] );
			option_cache.FileName [ n ] = new char [ strlen ( strg ) + 1 ];
			strcpy ( option_cache.FileName [ n ], strg );
			pos += 4;
			option_cache.Options[n].ColorKey = 0;

			if ( _strnicmp ( temp + pos, "ColorKey", 8 ) == 0 )
				option_cache.Options [ n ].ColorKey = D3DCOLOR_XRGB ( 255, 0, 255 );

			option_cache.count++;
		}
	}

	fclose ( file );
}

TexOptions _Textures::GetTexOptions ( LPSTR FileName )
{
	if ( option_cache.count == 0 )
		InitOptions ( );

	char temp1 [ MAX_PATH ];
	strcpy ( temp1, FileName );
	
	for ( int y = 0; y < (int)strlen ( temp1 ); y++ )
	{
		if ( temp1 [ y ] == '/' )
			temp1 [ y ] = '\\';
	}

	TexOptions opt;
	opt.ColorKey = 0;
		
	for ( int x = 0; x < option_cache.count; x++ )
	{
		char temp2 [ MAX_PATH ];

		strcpy ( temp2, option_cache.FileName [ x ] );

		for ( int y = 0; y < (int)strlen ( temp2 ); y++ )
		{
			if ( temp2 [ y ] == '/' )
				temp2 [ y ] = '\\';
		}
		
		if ( _stricmp ( temp1, temp2 ) == 0 )
			opt = option_cache.Options [ x ];
	}

	return opt;
}

int _Textures::Load_LightMap_By_FileName ( LPSTR FileName )
{
	if ( strlen ( FileName ) < 1 )
		return -1;

	int i = Get_LightMap_ID_For_FileName ( FileName );

	if ( i != -2 )
		return i;
		
	byte* data = NULL;
	int length;
	LPSTR FullName;
	
	if ( file_loader::Find::File ( FileName, "", &FullName ) )
	{
		if ( !file_loader::Load::Direct ( FullName, &data, &length ) )
		{
			SAFE_DELETE ( data );
			return -1;
		}
	}

	int n = LightMap_count;

	LightMap_Resize ( n + 1 );

	D3DXIMAGE_INFO info;

	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx (
													//	m_pD3D, data, length, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
														m_pD3D, data, length, D3DX_DEFAULT, D3DX_DEFAULT, 2, 0,
														D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL, &LightMap [ n ].lpD3DTex
													 );

	LightMap [ n ].original_width  = info.Width;
	LightMap [ n ].original_height = info.Height;
				
	SAFE_DELETE ( data );

	if ( hr == D3D_OK )
	{
		strcpy2 ( &LightMap [ n ].FileName, FileName );
		LightMap_count++;
		return n;
	}
	else
		return -1;
}


int _Textures::Get_LightMap_ID_For_FileName ( LPSTR FileName )
{
	for ( int x = 0; x < LightMap_count; x++ )
	{
		if ( strcmp ( LightMap [ x ].FileName, FileName ) == 0 )
			return x;
	}

	return -2;
}

LPSTR _Textures::Get_LightMap_FileName_For_ID ( int id )
{
	if ( id < 0 || id >= LightMap_count )
		return "";

	return LightMap [ id ].FileName;
}

#define MIPLEVELS 4
typedef struct wadtex_s
{
	char     name [ 16 ];
	unsigned width, height;
	unsigned offsets [ MIPLEVELS ];
} wadtex_t;

LPDIRECT3DTEXTURE9 _Textures::Create_From_RAW_Data ( byte* data, UINT width, UINT height, int bpp )
{
	D3DFORMAT format;

	switch ( bpp )
	{
		case 24: format = D3DFMT_R8G8B8;   break;
		case 32: format = D3DFMT_A8R8G8B8; break;
		case 16: format = D3DFMT_R5G6B5;   break;
		case 8:  format = D3DFMT_L8;       break;
	}

	// leefix - 290703 - completely changed the way raw data is made into a texture (DX9)
	LPDIRECT3DTEXTURE9 tex;
	D3DXCreateTexture ( m_pD3D, width, height, 1, NULL, format, D3DPOOL_DEFAULT, &tex );
	D3DSURFACE_DESC pDesc;
	tex->GetLevelDesc ( 0, &pDesc );

	// Create and copy texture data to lpSur1
	LPDIRECT3DSURFACE9 lpSur1;
	m_pD3D->CreateOffscreenPlainSurface ( width, height, format, D3DPOOL_SCRATCH, &lpSur1, NULL );
	D3DLOCKED_RECT lock;
	lpSur1->LockRect ( &lock, NULL, NULL );
	CopyMemory ( lock.pBits, data, width * height * ( bpp / 8 ) );
	lpSur1->UnlockRect ( );

	// Create temp store texture
	LPDIRECT3DTEXTURE9 pNewTexture = NULL;
	HRESULT hr = D3DXCreateTexture (  m_pD3D,
									  pDesc.Width,
									  pDesc.Height,
									  1,
									  D3DX_DEFAULT,
									  pDesc.Format,
									  D3DPOOL_SYSTEMMEM,
									  &pNewTexture			);

	// Copy this image to store
	LPDIRECT3DSURFACE9 pNewSurface = NULL;
	pNewTexture->GetSurfaceLevel(0, &pNewSurface);
	RECT scrRect = { 0, 0, width, height };
	hr = D3DXLoadSurfaceFromSurface(pNewSurface, NULL, NULL, lpSur1, NULL, &scrRect, D3DX_DEFAULT, 0);
	SAFE_RELEASE(pNewSurface);

	// copy loaded texture to final texture
	m_pD3D->UpdateTexture ( pNewTexture, tex );
	pNewTexture->Release ( );
	lpSur1->Release   ( );

	return tex;
}

int _Textures::Load_LightMap_By_Data_Q2 ( byte* data, UINT width, UINT height, int bpp )
{
	int n = Textures.LightMap_count;
	LightMap_Resize ( n + 1 );

	LightMap [ n ].original_width  = width;
	LightMap [ n ].original_height = height;

	int size = width * height * ( bpp / 8 );

	byte* tex_data = new byte [ size ];

	if ( bpp == 8 || bpp == 16 )
		memcpy ( tex_data, data, size );

	if ( bpp == 24 )
	{
		for ( int x = 0; x < size; x += 3 )
		{
			tex_data [ x + 0 ] = data [ x + 2 ];
			tex_data [ x + 1 ] = data [ x + 1 ];
			tex_data [ x + 2 ] = data [ x + 0 ];
		}
	}

	if ( bpp == 32 )
	{
		for ( int x = 0; x < size; x += 4 )
		{
			tex_data [ x + 0 ] = data [ x + 3 ];
			tex_data [ x + 1 ] = data [ x + 2 ];
			tex_data [ x + 2 ] = data [ x + 1 ];
			tex_data [ x + 3 ] = data [ x + 0 ];
		}
	}

	LightMap [ n ].lpD3DTex = Create_From_RAW_Data ( tex_data, width, height, bpp );
	SAFE_DELETE ( tex_data );

	if ( LightMap [ n ].lpD3DTex != NULL )
	{
		char text [ 256 ];
		sprintf ( text,"$Q1/2lightmap%d", n );
		strcpy2 ( &Textures.LightMap [ n ].FileName, text );
		Textures.LightMap_count++;
		return n;
	}
	else
		return -1;
}

int _Textures::Load_By_FileName_Q3A ( LPSTR FileName )
{
	return Load_By_FileName_Q3A ( FileName, FALSE );
}

int _Textures::Load_By_FileName_Q3A ( LPSTR FileName, BOOL forceload )
{
	if ( strlen ( FileName ) < 1 )
		return -1;

	int i = Get_ID_For_FileNameQ3A ( FileName, forceload );
	
	if (i != -2 )
		return i;
		
	byte* data = NULL; 
	int length;

	char newname [ 256 ];
	strcpy ( newname, FileName );

	int x = 0;

	for ( x = 0; x < Q3A_Resources.num_files; x++ )
	{
		char name [ 256 ];
		strcpy ( name, FileName );
		
		if ( IsDirectory ( name ) )
			strcat ( name, ".tga" );

		if ( file_loader::Load::From_PK3 ( Q3A_Resources.files [ x ].fullname, name, &data, &length ) )
		{
			files_cache.AddFile ( name, name, Q3A_Resources.files [ x ].fullname );
			strcpy ( newname, name );
			break;
		}

		SAFE_DELETE ( data );

		strncpy ( name + strlen ( name ) - 4, ".jpg", 4 );

		if ( file_loader::Load::From_PK3 ( Q3A_Resources.files [ x ].fullname, name, &data, &length ) )
		{
			files_cache.AddFile ( name, name, Q3A_Resources.files [ x ].fullname );
			strcpy ( newname, name );
			break;
		}

		SAFE_DELETE ( data );
	}

	if ( x == Q3A_Resources.num_files )
	{
		SAFE_DELETE ( data );

		Textures.NotFound_Resize ( Textures.NotFound_count + 1 );

		strcpy2 ( &Textures.NotFound_FileName [ Textures.NotFound_count ], FileName );
		Textures.NotFound_count++;
		return -1;
	}

	int n = Textures.Tex_count;
	Tex_Resize ( n + 1 );

	D3DXIMAGE_INFO info;

//	// LEE FIX : 130902 : Texture did not fill the texture plate, must use DEFAULT
//	// Do not use D3DX_FILTER_NONE as it only copies toa subset of the texture!!
//	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx
//		(
//		m_pD3D, data, length,
//		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL,
//		&Textures.Tex [ n ].lpD3DTex
//	);
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx
		(
		m_pD3D, data, length,
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL,
		&Textures.Tex [ n ].lpD3DTex
	);
	
	Tex [ n ].original_width  = info.Width;
	Tex [ n ].original_height = info.Height;

	free ( data );
	data=NULL;

	if ( hr == D3D_OK )
	{
		strcpy2 ( &Textures.Tex [ n ].FileName, newname );
		Textures.Tex_count++;
		return n;
	}
	else
		return -1;
}

int _Textures::Get_ID_For_FileNameQ3A ( LPSTR FileName, BOOL forceload )
{
	for ( int x = 0; x < Textures.Tex_count; x++ )
	{
		char name [ 256 ];
		strcpy ( name, FileName );

		if ( _stricmp ( name, Textures.Tex [ x ].FileName ) == 0 )
			return x;

		if ( _strnicmp ( name + strlen ( name ) - 4, ".tga", 4 ) == 0 )
		{
			strncpy ( name + strlen ( name ) - 4, ".jpg", 4 );

			if ( _stricmp ( name, Textures.Tex [ x ].FileName ) == 0 )
				return x;

		}
		else if ( _strnicmp ( name + strlen ( name ) - 4, ".jpg", 4 ) == 0 )
		{
			strncpy ( name + strlen ( name ) - 4, ".tga", 4 );

			if ( _stricmp ( name, Textures.Tex [ x ].FileName ) == 0 )
				return x;
		}
		else
		{
			strcat ( name, ".tga" );

			if ( _stricmp ( name, Textures.Tex [ x ].FileName ) == 0 )
				return x;

			strncpy ( name + strlen ( name ) - 4, ".jpg", 4 );

			if ( _stricmp ( name, Textures.Tex [ x ].FileName ) == 0 )
				return x;
		}
	}

	if ( forceload )
		return -2;
	
	for ( int x = 0; x < Textures.NotFound_count; x++ )
	{
		char name [ 256 ];
		strcpy ( name, FileName );

		if ( _stricmp ( name, Textures.NotFound_FileName [ x ] ) == 0 )
			return -1;

		strcat ( name, ".tga" );
		
		if ( _stricmp ( name, Textures.NotFound_FileName [ x ] ) == 0 )
			return -1;

		strncpy ( name + strlen ( name ) - 4, ".jpg", 4 );

		if ( _stricmp ( name, Textures.NotFound_FileName [ x ] ) == 0 )
			return -1;
	}
	
	return -2;
}