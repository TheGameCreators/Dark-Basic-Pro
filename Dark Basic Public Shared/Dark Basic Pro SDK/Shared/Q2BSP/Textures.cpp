#include "Textures.h"

extern LPDIRECT3DDEVICE9		m_pD3D;

struct _Textures Textures;

extern files_found Q3A_Resources;
extern files_found Q2_Resources;


BOOL _Textures::Reset ( )
{
	for ( int i = 0; i < Tex_size; i++ )
	{
		SAFE_DELETE_ARRAY ( Tex [ i ].FileName );
	}

	for ( i = 0; i < LightMap_size; i++ )
	{
		SAFE_DELETE_ARRAY ( LightMap [ i ].FileName );
	}

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

	SAFE_DELETE_ARRAY ( NotFound_FileName );

	cache.Release ( );

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

	for ( int x = 0; x < Textures.cache.entry_count; x++ )
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
														//D3DX_DEFAULT, 0, format, D3DPOOL_MANAGED,
														D3DX_DEFAULT, 0, format, D3DPOOL_DEFAULT,
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
		
	for ( x = 0; x < Textures.NotFound_count; x++ )
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

		for ( y = 0; y < (int)strlen ( temp2 ); y++ )
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
														m_pD3D, data, length, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
														//D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL, &LightMap [ n ].lpD3DTex
														D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL, &LightMap [ n ].lpD3DTex
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

bool Export ( LPDIRECT3DTEXTURE9 lpTemp, int iWidth, int iHeight, char* szFilename, BYTE* pbData );
int iNum = 0;

int _Textures::Load_LightMap_By_Data_Q2 ( byte* data, UINT width, UINT height, int bpp )
{
	int n = Textures.LightMap_count;
	LightMap_Resize ( n + 1 );

	LightMap [ n ].original_width  = width;
	LightMap [ n ].original_height = height;

	char szFilename [ 256 ];
	_itoa ( iNum++, szFilename, 10 );

	GetTempPath ( 256, szFilename );
	strcat ( szFilename, "xx.bmp" );
	
	Export ( NULL, width, height, szFilename, data );

	D3DXCreateTextureFromFileEx 
								(
								  m_pD3D,
								  szFilename,
								  D3DX_DEFAULT,
								  D3DX_DEFAULT,
								  //1,
								  0,
								  0,
								  D3DFMT_UNKNOWN,
								  D3DPOOL_DEFAULT,
								  D3DX_DEFAULT,
								  D3DX_DEFAULT,
								  0xFF000000,
								  NULL,
								  NULL,
								  &LightMap [ n ].lpD3DTex
								);
	
	DeleteFile ( szFilename );

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

bool Export ( LPDIRECT3DTEXTURE9 lpTemp, int iWidth, int iHeight, char* szFilename, BYTE* pbData )
{
	// old export function, takes a texture and saves it out

	BITMAPFILEHEADER	filehdr;	// bitmap file header
	BITMAPINFOHEADER	infohdr;	// bitmap info header

	DWORD				dwWidth,	// width of bitmap
						dwHeight,	// height of bitmap
						dwDepth;	// depth of bitmap

	FILE*				fp = NULL;	// file access (don't use windows functions this time)

	dwWidth		= iWidth;
	dwHeight	= iHeight;
	dwDepth		= 16;

	// setup header information
	memset(&filehdr, 0, sizeof(filehdr));	// clear out the structure

	*((char*)&filehdr.bfType)		='B';	// add b
	*(((char*)&filehdr.bfType) + 1)	='M';	// add m (this takes care of bitmap id)

	filehdr.bfOffBits = sizeof(filehdr) + sizeof(infohdr);	// data offset
	
	memset(&infohdr, 0, sizeof(infohdr));	// clear out another structure 

	infohdr.biSize			= sizeof(infohdr);			// set size
	infohdr.biWidth			= dwWidth;					// width of bitmap
	infohdr.biHeight		= dwHeight;					// height of bitmap
	infohdr.biPlanes		= 1;						// just use 1 plane
	infohdr.biBitCount		= dwDepth == 8 ? 8 : 24;	// depth
	infohdr.biCompression	= BI_RGB;					// say no to compression and just plain RGB values
	infohdr.biSizeImage		= 0;						// set to 0
	infohdr.biClrUsed		= 0;						// unused so set to 0

	// create an empty file using binary mode
	fp = fopen(szFilename, "wb");

	fwrite(&filehdr, sizeof(filehdr), 1, fp);	// write out the file header
	fwrite(&infohdr, sizeof(infohdr), 1, fp);	// now write out the info header

	// the file data is written dependant
	// on the bit depth of the surface
	//switch (dwDepth)
	{
	//	case 16:
		{
				int			bytesrequired		= dwWidth * 3;
				int			bytesgiven			= ( bytesrequired + 3 ) & ~ 3;
				int			fillerbytes			= bytesgiven - bytesrequired;
				BYTE		filler [ ]			= { 0, 0, 0 };
				RGBTRIPLE*  bmpbuffer			= new RGBTRIPLE [ dwWidth ];
				int			iCount				= 0;

					
				int iLine  = iHeight - 1;
				int iStart = iWidth * iLine * 3;
			
				for ( int y = 0; y < (int)dwHeight; y++ )
				{
					RGBTRIPLE*	bmppixel  = bmpbuffer;

					for ( int x = 0; x < (int)dwWidth; x++ )
					{
						//bmppixel->rgbtRed   = pbData [ iStart + 2 ];
						//bmppixel->rgbtGreen = pbData [ iStart + 1 ];
						//bmppixel->rgbtBlue  = pbData [ iStart + 0 ];

						bmppixel->rgbtRed   = pbData [ iStart + 0 ];
						bmppixel->rgbtGreen = pbData [ iStart + 1 ];
						bmppixel->rgbtBlue  = pbData [ iStart + 2 ];

						{
							float	r,
									g,
									b;
							float	scale = 1.0f;
							float	tmp;
							float factor = 0.7f;

							r = ( float ) bmppixel->rgbtRed;
							g = ( float ) bmppixel->rgbtGreen;
							b = ( float ) bmppixel->rgbtBlue;

							r = r * factor / 255.0f;
							g = g * factor / 255.0f;
							b = b * factor / 255.0f;

							if ( r > 1.0f && ( tmp = ( 1.0f / r ) ) < scale )
								scale = tmp;

							if ( g > 1.0f && ( tmp = ( 1.0f / g ) ) < scale ) 
								scale = tmp;

							if ( b > 1.0f && ( tmp = ( 1.0f / b ) ) < scale ) 
								scale = tmp;

							scale *= 255.0f;
							r     *= scale;
							g     *= scale;
							b     *= scale;

							bmppixel->rgbtRed = ( byte ) r;
							bmppixel->rgbtGreen = ( byte ) g;
							bmppixel->rgbtBlue = ( byte ) b;
						}

						iStart += 3;

						//bmppixel->rgbtRed   = pbData [ iStart++ ];
						//bmppixel->rgbtGreen = pbData [ iStart++ ];
						//bmppixel->rgbtBlue  = pbData [ iStart++ ];

						bmppixel++;
					}

					iLine--;
					iStart = iWidth * iLine * 3;
					
					fwrite ( bmpbuffer, sizeof ( RGBTRIPLE ), dwWidth, fp );
					fwrite ( filler, 1, fillerbytes, fp );
					
				}

				delete [ ] bmpbuffer;
		}
	//	break;
	}

	fclose ( fp );

	return true;
}


int _Textures::Load_By_FileName_Q2 ( LPSTR FileName )
{
	return Load_By_FileName_Q2 ( FileName, FALSE );
}

int _Textures::Load_By_FileName_Q2 ( LPSTR FileName, BOOL forceload )
{
	if ( FileName == NULL || strlen ( FileName ) < 1 )
		return -1;

	char fn [ MAX_PATH ];

	strcpy ( fn, FileName );

	int i = Get_ID_For_FileName ( fn, forceload );

	if ( i == -2 )
	{
		char fn2 [ 256 ];
		
		strcpy ( fn2, fn );
		strcat ( fn2, ".wadtex" );

		i = Get_ID_For_FileName ( fn2, forceload );
	}

	if ( i == -2 )
	{
		char fn2 [ 256 ];
		
		strcpy ( fn2, fn );
		strcat ( fn2, ".wal" );

		i = Get_ID_For_FileName ( fn2, forceload );
	}

	if ( i != -2 )
		return i;

	// this temporary texture will contain the final texture
	LPDIRECT3DTEXTURE9 texture = NULL;

	UINT w, h;
	byte* data = NULL;
	int length;

	for ( int x = 0; x < Q2_Resources.num_files; x++ )
	{
		// convert to lowercase
		char lname [ _MAX_PATH ];
		
		strcpy ( lname, Q2_Resources.files [ x ].fullname );
		strcpy ( lname, _strlwr ( lname ) );
		
		// is current resource a PAK file?
		if ( _strnicmp ( lname + strlen ( lname ) - 4, ".pak", 4 ) == 0 )
		{
			// try to load a file with exactly the same name ( should not work )
			if ( file_loader::Load::From_PAK ( lname, fn, &data, &length ) )
			{
				files_cache.AddFile ( fn, fn, lname );
				
				// try to load texture ( if this fails, it's an unsupported filetype )
				D3DXIMAGE_INFO info;

				HRESULT hr = D3DXCreateTextureFromFileInMemoryEx
																(
																	m_pD3D,
																	data,
																	length,
																	D3DX_DEFAULT,
																	D3DX_DEFAULT,
																	//D3DX_DEFAULT,
																	// mike - 230604 - changed mip map levels
																	6,
																	0,
																	D3DFMT_UNKNOWN,
																	//D3DPOOL_MANAGED,
																	D3DPOOL_DEFAULT,
																	D3DX_FILTER_NONE,
																	D3DX_DEFAULT,
																	0,
																	&info,
																	NULL,
																	&texture
																);

				if ( hr == D3D_OK )
				{
					w = info.Width;
					h = info.Height;
					break;
				}
			}
			
			// this is the normal case: it's a file with the ".wal" extension
			char fn2 [ 256 ];
			
			strcpy ( fn2, fn );

			if ( _strnicmp ( fn2 + strlen ( fn2 ) - 4, ".wal", 4 ) != 0 )
				strcat ( fn2, ".wal" );

			if ( file_loader::Load::From_PAK ( lname, fn2, &data, &length ) )
			{
				strcpy ( fn, fn2 );
				files_cache.AddFile ( fn, fn, lname );
				texture = Textures.Create_From_WAL_Data ( data, length, &w, &h );
				break;
			}
		}
		
		// is current resource a WAD file?
		if ( _strnicmp ( lname + strlen ( lname ) - 4, ".wad", 4 ) == 0 )
		{
			if ( file_loader::Load::From_WAD ( lname, fn, &data, &length ) )
			{
				if ( _strnicmp ( fn + strlen ( fn ) - 7, ".wadtex", 7 ) != 0 )
					strcat ( fn, ".wadtex" );

				files_cache.AddFile ( fn, fn, lname );

				texture = Textures.Create_From_WAD_Data ( data, length, &w, &h );
				break;
			}			
		}
		
		// it's not a PAK or WAD... maybe it's another format?
		if (
				_strnicmp ( lname + strlen ( lname ) - 4, ".tga", 4 ) == 0 ||
				_strnicmp ( lname + strlen ( lname ) - 4, ".jpg", 4 ) == 0 ||
				_strnicmp ( lname + strlen ( lname ) - 4, ".bmp", 4 ) == 0
		   )
		{
			// check filename
			if ( strstr ( lname, fn ) != 0 )
			{
				if ( file_loader::Load::Direct ( lname, &data, &length ) )
				{
					D3DXIMAGE_INFO info;
					HRESULT hr = D3DXCreateTextureFromFileInMemoryEx
																	(
																		m_pD3D, 
																		data,
																		length,
																		D3DX_DEFAULT,
																		D3DX_DEFAULT,
																		//D3DX_DEFAULT,
																		4,
																		0,
																		D3DFMT_UNKNOWN,
																		D3DPOOL_MANAGED,
																		D3DX_DEFAULT,
																		D3DX_DEFAULT,
																		0,
																		&info,
																		NULL,
																		&texture
																	 );

					if ( hr == D3D_OK )
					{
						LPSTR	fn2 = NULL,
								p   = NULL;
						
						SolveFullName ( lname, &fn2, &p );

						files_cache.AddFile ( lname, fn, p );

						SAFE_DELETE ( fn2 );
						SAFE_DELETE ( p );
						
						w = info.Width;
						h = info.Height;
					}

					break;
				}
			}
		}
	}

	// mike - 240604 - changed from free to delete as it causes
	//				 - conflict because memory is allocated with new
	if ( data )
	{
		//free ( data );
		delete [ ] data;
		data=NULL;
	}

	if ( texture != NULL )
	{
		int n = Textures.Tex_count;

		// get mip maps by saving out the file and then reloading
		// slows down loading but it works and looks nicer

		char buf [ 512 ];
		GetTempPath ( 512, buf );
		strcat ( buf, "temp.bmp" );

		D3DXSaveTextureToFile ( buf, D3DXIFF_BMP, texture, NULL );

		texture->Release ( );

		D3DXCreateTextureFromFileExA
									(
										m_pD3D,
										buf,
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										//D3DX_DEFAULT,
										4,
										0,
										D3DFMT_UNKNOWN,
										D3DPOOL_DEFAULT,
										D3DX_DEFAULT,
										D3DX_DEFAULT,
										0,
										NULL,
										NULL,
										&texture
									);
	
		DeleteFile ( buf );
		
		Tex_Resize ( n + 1);

		Tex [ n ].lpD3DTex         = texture;
		Tex [ n ].original_width   = w;
		Tex [ n ].original_height  = h;
		Tex [ n ].Options.ColorKey = 0;

		strcpy2 ( &Tex [ n ].FileName, fn );
		Textures.Tex_count++;

		return n;
	}

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

	for ( int x = 0; x < Q3A_Resources.num_files; x++ )
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

	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx
		(
		m_pD3D, data, length,
		//D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL,
		D3DX_DEFAULT, D3DX_DEFAULT, 4, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL,
		&Textures.Tex [ n ].lpD3DTex
	);
	
	Tex [ n ].original_width  = info.Width;
	Tex [ n ].original_height = info.Height;

	SAFE_DELETE ( data );

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
	
	for ( x = 0; x < Textures.NotFound_count; x++ )
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

int _Textures::Load_By_Data_Q2 ( LPSTR FileName, byte* data, UINT width, UINT height, int bpp )
{
	return Load_By_Data_Q2 ( FileName, data, width, height, bpp, FALSE );
}

int _Textures::Load_By_Data_Q2 ( LPSTR FileName, byte* data, UINT width, UINT height, int bpp, BOOL forceload )
{
	int i = Get_ID_For_FileName ( FileName, forceload );

	if ( i != -2 )
		return i;

	int n = Textures.Tex_count;

	Tex_Resize ( n + 1 );

	Tex [ n ].Options.ColorKey = 0;
	Tex [ n ].original_width   = width;
	Tex [ n ].original_height  = height;

	// mike - 240604 - set pointer to null to stop crash
	byte* pal_data = NULL;
	int   pal_length;

	//chdir ( setup.state.RootPath );

	file_loader::Load::Direct ( "Media\\Quake1.pal", &pal_data, &pal_length );

	int size = width * height * ( bpp / 8 );

	byte* tex_data = new byte [ width * height * ( bpp / 8 ) ];

	if ( tex_data == NULL )
		return -1;

	if ( bpp == 24 )
	{
		int c = 0;

		for ( i = 0; i < size; i++ )
		{
			tex_data [ c++ ] = pal_data [ data [ i ] * 3 + 2 ];  // converts from BGR to RGB
			tex_data [ c++ ] = pal_data [ data [ i ] * 3 + 1 ];
			tex_data [ c++ ] = pal_data [ data [ i ] * 3 + 0 ];
		}
	}

	
	SAFE_DELETE ( pal_data );

	Tex [ n ].lpD3DTex = Create_From_RAW_Data ( tex_data, width, height, bpp );

	SAFE_DELETE ( tex_data );

	if ( Tex [ n ].lpD3DTex != NULL )
	{
		strcpy2 ( &Textures.Tex [ n ].FileName, FileName );
		Textures.Tex_count++;
		return n;
	}
	else
		return -1;
}


LPDIRECT3DTEXTURE9 _Textures::Create_From_WAD_Data ( byte* data, int length, UINT* w, UINT* h )
{
	LPDIRECT3DTEXTURE9 tex = NULL;

	wadtex_t* t = ( wadtex_t* ) data;
	UINT width  = t->width;
	UINT height = t->height;

	*w = width;
	*h = height;

	// palette starts after the fourth mipmap ( 1 / 8 size )
	byte* pal  = data + t->offsets [ 3 ] + ( ( width / 8 ) * ( height / 8 ) ) + 2;
	byte* idx  = data + t->offsets [ 0 ];
	int   size = width * height;

	byte* tex_data = new byte [ size * 3 ];
	int c = 0;

	for ( int i = 0; i < size; i++ )
	{
		tex_data [ c++ ] = pal [ idx [ i ] * 3 + 2 ];  // converts from BGR to RGB
		tex_data [ c++ ] = pal [ idx [ i ] * 3 + 1 ];
		tex_data [ c++ ] = pal [ idx [ i ] * 3 + 0 ];
	}

	tex = Create_From_RAW_Data ( tex_data, width, height, 24 );

	SAFE_DELETE ( tex_data );

	return tex;
}

struct wal_header
{
	char			name [ 32 ];

	unsigned int	width;
	unsigned int	height;

	unsigned int	offset [ 4 ];

	char			next_name [ 32 ];

	unsigned int	flags;
	unsigned int	contents;
	unsigned int	value;
};

LPDIRECT3DTEXTURE9 _Textures::Create_From_WAL_Data ( byte* data, int length, UINT* w, UINT* h )
{
	wal_header header;

	memcpy ( &header, data, sizeof ( header ) );

	LPDIRECT3DTEXTURE9 tex = NULL;

	int width  = header.width;
	int height = header.height;
	int size   = width * height;

	*w = width;
	*h = height;

	byte* pal_data;
	int   pal_length;
	
	//file_loader::Load::Direct ( "Media\\Quake2.pal", &pal_data, &pal_length );
	file_loader::Load::Direct ( "Quake2.pal", &pal_data, &pal_length );

	byte* tex_data = new byte [ size * 3 ];
	int c = 0;

	for ( int i = 0; i < size; i++ )
	{
		tex_data [ c++ ] = pal_data [ data [ header.offset [ 0 ] + i ] * 3 + 2 ];  // converts from BGR to RGB
		tex_data [ c++ ] = pal_data [ data [ header.offset [ 0 ] + i ] * 3 + 1 ];
		tex_data [ c++ ] = pal_data [ data [ header.offset [ 0 ] + i ] * 3 + 0 ];
	}

	SAFE_DELETE ( pal_data );

	tex = Create_From_RAW_Data ( tex_data, width, height, 24 );

	SAFE_DELETE ( tex_data );

	return tex;
}
