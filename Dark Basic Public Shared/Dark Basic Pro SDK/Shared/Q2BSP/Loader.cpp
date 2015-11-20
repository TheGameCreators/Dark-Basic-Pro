#include "loader.h"
#include <io.h>


struct _externfiles externfiles;
files_found files_cache;


files_found Q3A_Resources;
files_found Q2_Resources;

BOOL strcpy2 ( LPSTR* dest, LPSTR src )
{
	
	if ( src == NULL )
		return FALSE;

	if(dest) SAFE_DELETE(*dest);
	*dest = new char [ strlen ( src ) + 1 ];
	
	strcpy ( *dest, src );
	
	return TRUE;
}

BOOL file_loader::GetRelativePath ( LPSTR fullpath, LPSTR start, LPSTR* out )
{
	for ( int x = 0; x != ( int ) strlen ( fullpath ); x++ )
	{
		if ( fullpath [ x ] == '\\' )
			fullpath [ x ] = '/';
	}
	
	if ( fullpath [ strlen ( fullpath ) - 1 ] == '/' )
		fullpath [ strlen ( fullpath ) - 1 ] = '\0';

	for ( x = 0; x != ( int ) strlen ( start ); x++ )
	{
		if ( start [ x ] == '\\' )
			start [ x ] = '/';
	}
	
	if ( start [ strlen ( start ) - 1 ] == '/' )
		start [ strlen ( start ) - 1 ] = '\0';

	if ( _stricmp ( fullpath, start ) == 0 )
	{
		*out       = new char [ 1 ];
		*out [ 0 ] = '\0';

		return TRUE;
	}

	for ( x = 0; x != ( int ) strlen ( start ); x++ )
	{
		if ( start [ x ] != fullpath [ x ] )
			return FALSE;
	}

	char temp [ 1024 ];
	strncpy ( temp, fullpath + x + 1, strlen ( fullpath ) -x );

	*out = new char [ strlen ( temp ) + 1 ];
	
	strcpy ( *out, temp );

	return TRUE;
}

typedef struct
{
	char	name [ 56 ];
	int		filepos, 
			filelen;
} dpackfile_t;

typedef struct
{
	char id [ 4 ];
	int  dirofs;
	int  dirlen;
} dpackheader_t;

#define MAX_FILES_IN_PACK 32768  // 2048 is default for "Quake I", but that's not enough for others (e.g. Half-Life)


dpackheader_t  packheader;
dpackfile_t    info [ MAX_FILES_IN_PACK ];

BOOL file_loader::Load::From_PAK ( LPSTR pakname, LPSTR filename, byte** data, int* length )
{
	if ( data != NULL )
		*data = NULL;

	FILE* file = fopen ( pakname, "rb" );
	
	if ( !file )
	{
		return FALSE;
	}

	if ( fread ( &packheader, 1, sizeof ( packheader ), file ) != sizeof ( packheader ) )
	{
		fclose ( file );
		return FALSE;
	}
	
	if ( packheader.id [ 0 ] != 'P' || packheader.id [ 1 ] != 'A' || packheader.id [ 2 ] != 'C' || packheader.id [ 3 ] != 'K' )
	{
		fclose ( file );
		return FALSE;
	}
	
	int numpackfiles = packheader.dirlen / sizeof ( dpackfile_t );

	if ( numpackfiles > MAX_FILES_IN_PACK )
	{
		numpackfiles = MAX_FILES_IN_PACK;
	}

	fseek ( file, packheader.dirofs, SEEK_SET );
	fread ( &info, 1, packheader.dirlen, file );

	for ( int x = 0; x < numpackfiles; x++ )
	{
		if ( strlen ( info [ x ].name ) < strlen ( filename ) )
			continue;

		/*
		// Check if file is a BSP file
		fseek ( file, info [ x ].filepos, SEEK_SET );
		byte* pSeeData = ( byte* )malloc ( info [ x ].filelen );
		if ( fread ( pSeeData, 1, info [ x ].filelen, file ) == info [ x ].filelen )
		{
			if ( pSeeData[0] == 'I' && pSeeData[1] == 'B' && pSeeData[2] == 'S' && pSeeData[3] == 'P' )
			{
				pSeeData=pSeeData;
			}
		}
		free(pSeeData);
		*/

		if ( _strnicmp ( filename, info [ x ].name + ( strlen ( info [ x ].name ) - strlen ( filename ) ), strlen ( filename ) ) == 0 )
		{
			if ( length != NULL )
				*length = info [ x ].filelen;

			if ( data != NULL )
			{
				fseek ( file, info [ x ].filepos, SEEK_SET );

				*data = ( byte* ) malloc ( info [ x ].filelen );

				if ( fread ( *data, 1, info [ x ].filelen, file ) != info [ x ].filelen )
				{
					fclose ( file );
					SAFE_DELETE ( *data );
					return FALSE;
				}
			}

			fclose ( file );
			return TRUE;
		}
	}
	
	fclose ( file );

	if ( data != NULL )
		SAFE_DELETE ( *data );

	return FALSE;
}

BOOL file_loader::Load::Direct ( LPSTR filename, byte** data, int* length )
{
	FILE* file;

	file = fopen ( filename, "rb" );

	if ( file == NULL )
		return FALSE;

	*length = file_loader::GetLength::File ( file );
	
	if ( *length == -1 )
		return FALSE;

	*data = new byte [ *length ];

	int count = fread ( *data, 1, *length, file );
		
	if ( count != *length )
		return FALSE;
		
	fclose ( file );

	return TRUE;
}

BOOL file_loader::Find::File ( LPSTR filename, LPSTR path, LPSTR* fullname )
{
	char dir_save [ _MAX_PATH ];

	GetCurrentDirectory ( _MAX_PATH, dir_save );

	_chdir ( path );

	struct _finddata_t c_files;
	long	hFile;
	struct _finddata_t c_dirs [ 100 ];
    long	hDir [ 100 ];

	int x = 0;

	next:
		// search new directory
		if ( ( hDir [ x ] = _findfirst ( "*.*", &c_dirs[x] ) ) != -1 )
		{
			if ( c_dirs [ x ].attrib & _A_SUBDIR )
			{
				if ( strcmp ( c_dirs [ x ].name, "." ) == 0 || strcmp ( c_dirs [ x ].name,".." ) == 0 ) 
					goto last;

				_chdir ( c_dirs [ x ].name );
				
				x++;

				goto next;
			}
		}
	
	last:
		while ( ( _findnext ( hDir [ x ], &c_dirs [ x ] ) ) != -1 )
		{
			if ( c_dirs [ x ].attrib & _A_SUBDIR )
			{
				if ( strcmp ( c_dirs [ x ].name, "." ) == 0 || strcmp ( c_dirs [ x ].name, ".." ) == 0 )
					goto last;

				_chdir ( c_dirs [ x ].name );
				
				if ( ( hFile = _findfirst ( filename, &c_files ) ) != -1L )
				{
					char dir [ 1024 ];

					GetCurrentDirectory ( _MAX_PATH, dir );

					strcat ( dir, "/" );
					strcat ( dir, c_files.name );

					*fullname = new char [ strlen ( dir ) + 1 ];
					strcpy ( *fullname, dir );

					_chdir ( dir_save );

					_findclose ( hFile );

					for ( int y = 0; y <= x; y++ )
						_findclose ( hDir [ y ] );

					return TRUE;
				}

				_findclose ( hFile );
				
				x++;
				goto next;
			}
		}

		_findclose ( hDir [ x ] );

		if ( x == 0 )
		{
			_chdir ( dir_save );
			return FALSE;
		}

		_chdir ( ".." );
		x--;
		goto last;

	return TRUE;
}

BOOL file_loader::Find::Files ( LPSTR common, LPSTR path, files_found* files )
{
	char dir_save [ _MAX_PATH ];

	GetCurrentDirectory ( _MAX_PATH, dir_save );

	_chdir ( path );

	( *files ).Init ( );

	struct _finddata_t c_files;
	long	hFile;
	struct	_finddata_t c_dirs [ 256 ];
    long	hDir [ 100 ];
	int		n = 0;
	int		x = -1;
	goto	start;
	
	next:
		if ( ( hDir [ x ] = _findfirst ( "*.*", &c_dirs [ x ] ) ) != -1 )
		{
			if ( c_dirs [ x ].attrib & _A_SUBDIR )
			{
				if ( strcmp ( c_dirs [ x ].name, "." ) == 0 || strcmp ( c_dirs [ x ].name, ".." ) == 0 ) 
					goto last;

				_chdir ( c_dirs [ x ].name );

				goto start;
			}
		}

	last:
		while ( ( _findnext ( hDir [ x ], &c_dirs [ x ] ) ) != -1 )
		{
			if ( c_dirs [ x ].attrib & _A_SUBDIR )
			{
				if ( strcmp ( c_dirs [ x ].name, "." ) == 0 || strcmp ( c_dirs [ x ].name, ".." ) == 0 )
					goto last;

				_chdir ( c_dirs [ x ].name );

				start:
					if ( ( hFile = _findfirst ( common, &c_files ) ) != -1L )
					{
						if ( c_files.attrib & _A_SUBDIR )
							goto jump;

						char dir  [ _MAX_PATH ];
						char name [ _MAX_PATH ];
		
						GetCurrentDirectory ( _MAX_PATH, dir );

						( *files ).Resize ( n + 1);

						strcpy2 ( &( *files ).files [ n ].filename, c_files.name );
						strcpy2 ( &( *files ).files [ n ].path, dir);
						sprintf ( name, "%s/%s", dir, c_files.name );
						strcpy2 ( &( *files ).files [ n ].fullname, name );

						( *files ).num_files++;
						n++;
						
					jump:
						while ( ( _findnext ( hFile, &c_files ) ) != -1 )
						{
							if ( c_files.attrib & _A_SUBDIR )
								continue;

							( *files ).Resize ( n + 1 );

							strcpy2 ( &( *files ).files [ n ].filename, c_files.name );
							strcpy2 ( &( *files ).files [ n ].path, dir );
							sprintf ( name, "%s/%s", dir, c_files.name );
							strcpy2 ( &( *files ).files [ n ].fullname, name );

							( *files ).num_files++;
							n++;
						}
					}

					_findclose ( hFile );
					x++;
					goto next;
			}
		}

		_findclose ( hDir [ x ] );
		
		if ( x == 0 )
		{
			_chdir ( dir_save );

			if ( ( *files ).num_files > 0 )
				return TRUE;

			return FALSE;
		}

		_chdir ( ".." );
		x--;
		goto last;			
}


#pragma pack(push)
#pragma pack(1)

struct zip_dir_t{
    unsigned long   signature;
    unsigned short  disk;
    unsigned short  cdisk;
    unsigned short  count;
    unsigned short  total;
    unsigned long   size;
    unsigned long   offset;
    unsigned short  comment_len;
};

struct zip_file_t{
    unsigned long   signature;  // signature (0x02014b50)
	unsigned short  made,       // version made by
					extract,    // vers needed to extract
					flag,       // general purpose flag
					method,     // compression method
					time,       // file time
					date;       // ..and file date
	unsigned long   crc,        // CRC-32 for file
					csize,      // compressed size
					size;       // uncompressed size
	unsigned short  name_len,   // file name length
					extra_len,  // extra field length
					comment_len,// file comment length
					disk,       // disk number
					attr;       // internal file attrib
	unsigned long   eattr,      // external file attrib
					offset;     // offset of local header
};

struct zfile_entry_t{
    char name[256];
    unsigned long   offset;
    unsigned long   csize;
    unsigned long   size;
};

#pragma pack(pop)


#define MAX_ZIPS 1024
struct _zip_optimize{
	LPSTR zipname[MAX_ZIPS];
	FILE* file[MAX_ZIPS];
	zip_dir_t dir[MAX_ZIPS];
	zfile_entry_t *files[MAX_ZIPS];
	int count;
}zip_optimize;


BOOL file_loader::Load::From_ZIP ( LPSTR zipname, LPSTR filename, byte** data, int* length )
{
	int current_zip;

	for ( int x = 0; x != zip_optimize.count; x++ )
	{
		if ( strcmp ( zip_optimize.zipname [ x ], zipname ) == 0 )
		{
			current_zip = x;
			goto jump;
		}
	}

	back:
		zip_optimize.zipname [ zip_optimize.count ] = new char [ strlen ( zipname ) + 1 ];
		
		strcpy ( zip_optimize.zipname [ zip_optimize.count ], zipname );

		current_zip = zip_optimize.count;
		
		if ( zip_optimize.count == MAX_ZIPS )
		{
			zip_optimize.count--;
			delete zip_optimize.files [ zip_optimize.count ];
			goto back;
		}

		zip_optimize.count++;		
		
		zip_optimize.file [ current_zip ] = fopen ( zipname, "rb" );

		if ( zip_optimize.file [ current_zip ] == NULL )
			return FALSE;

		// read header
		fseek ( zip_optimize.file [ current_zip ], 0 - sizeof ( zip_dir_t ), SEEK_END );

		if ( fread ( &zip_optimize.dir [ current_zip ], 1, sizeof ( zip_dir_t ), zip_optimize.file [ current_zip ] ) != sizeof ( zip_dir_t ) )
			return FALSE;

		zip_optimize.files [ current_zip ] = new zfile_entry_t [ zip_optimize.dir [ current_zip ].count ];
		
		// navigate to file entries
		fseek ( zip_optimize.file [ current_zip ], zip_optimize.dir [ current_zip ].offset,SEEK_SET );

		int i;
		
		for ( i = 0; i < zip_optimize.dir [ current_zip ].count; i++ )
		{
			// read header followed by file name
			zip_file_t h;

			if ( fread ( &h, 1, sizeof ( zip_file_t ), zip_optimize.file [ current_zip ] ) != sizeof ( zip_file_t ) )
				return false;
			
			char name [ 256 ];
			memset ( name, 0, sizeof ( name ) );

			if ( fread ( name, 1, h.name_len, zip_optimize.file [ current_zip ] ) != h.name_len )
				return false;
			
			fseek ( zip_optimize.file [ current_zip ], h.extra_len + h.comment_len, SEEK_CUR );
					
			strcpy(zip_optimize.files [ current_zip ] [ i ].name, _strlwr ( name ) );

			zip_optimize.files [ current_zip ] [ i ].offset = h.offset;
			zip_optimize.files [ current_zip ] [ i ].size   = h.size;
			zip_optimize.files [ current_zip ] [ i ].csize  = h.csize;
		}


	jump:
		for ( i = 0; i < zip_optimize.dir [ current_zip ].count; i++ )
		{
			if ( strlen ( zip_optimize.files [ current_zip ] [ i ].name ) < strlen ( filename ) )
				continue;

			if ( _strnicmp (
							filename,
							zip_optimize.files [ current_zip ] [ i ].name + ( strlen ( zip_optimize.files [ current_zip ] [ i ].name ) - strlen ( filename ) ),
							strlen ( filename ) ) == 0
						  )
			{
				goto found;
			}
		}

		return FALSE;

	found:

		int idx = i;

		const zfile_entry_t& f = zip_optimize.files [ current_zip ] [ idx ];

		extern char* unzip_file ( const char* p, int& size );

		if ( f.size == 0 || f.csize == 0 )
			return FALSE;
		
		LF lf;
		fseek ( zip_optimize.file [ current_zip ], f.offset, SEEK_SET );

		if ( fread ( &lf, 1, sizeof ( LF ), zip_optimize.file [ current_zip ] ) != sizeof ( LF ) )
		{
			return false;
		}

		// compute the complete file size
		int size = f.csize + sizeof ( LF ) + lf.lf_fn_len + lf.lf_ef_len;

		// get temp buffer
		char* tmp = new char [ size ];
		
		if ( !tmp )
		{
			return false;
		}

		// back up a little and read the whole file
		fseek ( zip_optimize.file [ current_zip ], 0 - sizeof ( LF ), SEEK_CUR );

		if ( fread ( tmp, 1, size, zip_optimize.file [ current_zip ] ) != ( unsigned ) size )
		{
			// return false;
		}

		// unzip
		size = 0;
		char* unzippeddata = unzip_file ( tmp, size );
		delete tmp;

		if ( !unzippeddata || ( unsigned ) size != f.size )
		{
			// mismatch
			if ( unzippeddata )
			{
				
			}

			return false;
		}


		if ( data != NULL )
			*data = ( byte* ) unzippeddata;

		if ( length != NULL) 
			*length = size;

		return TRUE;
}


BOOL file_loader::Load::From_PK3 ( LPSTR pk3name, LPSTR filename, byte** data, int* length )
{
	return From_ZIP ( pk3name, filename, data, length );
}


BOOL IsDirectory ( LPSTR str )
{
	BOOL dir = TRUE;

	for ( int x = strlen ( str ) - 1; x >= 0; x-- )
	{
		if ( str [ x ] == '.' )
			dir = FALSE;

		if ( str [ x ] == '/' || str [ x ] == '\\' )
			break;
	}

	return dir;
}

BOOL file_loader::Q3A::Add_PK3 ( LPSTR filename )
{
	// search in existing entries
	for ( int x = 0; x < Q3A_Resources.num_files; x++ )
	{
		if ( _stricmp ( Q3A_Resources.files [ x ].fullname, filename ) == 0 )
			return TRUE;
	}

	FILE* file = fopen ( filename, "r" );

	if ( file == NULL )
		return FALSE;

	fclose ( file );

	int n = Q3A_Resources.num_files;

	Q3A_Resources.Resize ( n + 1 );

	strcpy2 ( &Q3A_Resources.files [ n ].fullname, filename );

	SolveFullName ( Q3A_Resources.files [ n ].fullname, &Q3A_Resources.files [ n ].filename, &Q3A_Resources.files [ n ].path );

	Q3A_Resources.num_files++;
	
	return TRUE;
}

BOOL file_loader::Q2::Add_WAD ( LPSTR filename )
{
	// search in existing entries
	for ( int x = 0; x < Q2_Resources.num_files; x++ )
	{
		if ( _stricmp ( Q2_Resources.files [ x ].fullname, filename ) == 0 )
			return TRUE;
	}

	FILE* file = fopen ( filename, "r" );

	if ( file == NULL )
		return FALSE;
	
	fclose ( file );

	int n = Q2_Resources.num_files;

	Q2_Resources.Resize ( n + 1 );

	strcpy2 ( &Q2_Resources.files [ n ].fullname, filename );

	SolveFullName ( Q2_Resources.files [ n ].fullname, &Q2_Resources.files [ n ].filename, &Q2_Resources.files [ n ].path );
	
	Q2_Resources.num_files++;
	
	return TRUE;
}


BOOL file_loader::Find::Files_in_ZIP ( LPSTR zipname, LPSTR common, files_found_in_ZIP* files_found )
{
	FILE* file = fopen ( zipname, "rb" );

    if ( file == NULL )
		return FALSE;

	// read header
	fseek ( file, 0 - sizeof ( zip_dir_t ), SEEK_END );

	zip_dir_t dir;

	if ( fread ( &dir, 1, sizeof ( zip_dir_t ), file ) != sizeof ( zip_dir_t ) )
		return FALSE;

	zfile_entry_t* files = new zfile_entry_t [ dir.count ];
	
    // navigate to file entries
    fseek ( file, dir.offset, SEEK_SET );

    int i;
	
	for ( i = 0; i < dir.count; i++ )
	{
        // read header followed by file name
        zip_file_t h;

        if ( fread ( &h, 1, sizeof ( zip_file_t ), file ) != sizeof ( zip_file_t ) )
			return false;

        char name[256];
		memset ( name, 0, sizeof ( name ) );

        if ( fread ( name, 1, h.name_len, file ) != h.name_len )
			return false;

        fseek ( file, h.extra_len+h.comment_len, SEEK_CUR );
				
		strcpy ( files [ i ].name, _strlwr ( name ) );

        files [ i ].offset = h.offset;
        files [ i ].size   = h.size;
        files [ i ].csize  = h.csize;
    }

	fclose ( file );

	// search requested files
	( *files_found ).Init ( );

	// save zipname
	strcpy2 ( &( *files_found ).zipname, zipname );

	// convert to lowercase
	char fcommon [ 256 ];
	
	strcpy ( fcommon, common );
	strcpy ( fcommon, _strlwr ( fcommon ) );

	for ( i = 0; i < dir.count; i++ )
	{
		if ( strstr ( files [ i ].name, fcommon ) != NULL )
		{
			int n = ( *files_found ).num_files;

			( *files_found ).Resize ( n + 1 );

			strcpy2 ( &( *files_found ).files [ n ].fullname, files [ i ].name );

			SolveFullName ( ( *files_found ).files [ n ].fullname, &( *files_found ).files [ n ].filename, &( *files_found ).files [ n ].path );

			( *files_found ).num_files++;
		}
	}

	SAFE_DELETE ( files );

	return TRUE;
}

BOOL file_loader::Find::Files_in_PK3 ( LPSTR zipname, LPSTR common, files_found_in_PK3* files_found )
{
	return file_loader::Find::Files_in_ZIP ( zipname, common, files_found );
}

BOOL SolveFullName ( LPSTR fullname, LPSTR* filename, LPSTR* path )
{
	char tmp1 [ MAX_PATH ],
		 tmp2 [ _MAX_DIR ],
		 tmp3 [ MAX_PATH ],
		 tmp4 [ _MAX_EXT ];

	_splitpath ( fullname, tmp1, tmp2, tmp3, tmp4 );

	if ( filename != NULL )
	{
		strcat ( tmp3, tmp4 );
		strcpy2 ( filename, tmp3 );
	}

	if ( path != NULL )
	{
		strcat ( tmp1, tmp2 );
		if ( strlen ( tmp1 ) > 0 )
			if ( tmp1 [ strlen ( tmp1 ) - 1 ] == '\\' || tmp1 [ strlen ( tmp1 ) - 1 ] == '/' ) 
				tmp1 [ strlen ( tmp1 ) - 1 ] = '\0';

		strcpy2 ( path, tmp1 );
	}

	return TRUE;
}


BOOL file_loader::GetLength::File ( LPSTR filename, int* length )
{
	if ( filename == NULL || strlen ( filename ) < 1 )
		return FALSE;

	FILE* f = fopen ( filename, "r" );

	if ( f == NULL )
		return FALSE;

	fseek ( f, 0, SEEK_END );

	*length = ftell ( f );

	fclose ( f );

	return TRUE;
}

int file_loader::GetLength::File ( FILE* f )
{
	if ( f == NULL )
		return -1;

	int	pos = ftell ( f );

	fseek ( f, 0, SEEK_END );

	int length = ftell ( f );

	fseek ( f, pos, SEEK_SET );

	return length;
}

BOOL file_loader::GetLength::File_in_ZIP ( LPSTR zipname, LPSTR filename, int* length )
{
	FILE* file = fopen ( zipname, "rb" );

    if ( file == NULL )
		return FALSE;

	// read header
	fseek ( file, 0 - sizeof ( zip_dir_t ), SEEK_END );

	zip_dir_t dir;

    if ( fread ( &dir, 1, sizeof ( zip_dir_t ), file ) != sizeof ( zip_dir_t ) )
		return FALSE;

	zfile_entry_t* files = new zfile_entry_t [ dir.count ];
	
    // navigate to file entries
    fseek ( file, dir.offset, SEEK_SET );

    int i;
	
	for ( i = 0; i < dir.count; i++ )
	{
        // read header followed by file name
        zip_file_t h;

        if ( fread ( &h, 1, sizeof ( zip_file_t ), file ) != sizeof ( zip_file_t ) )
			return false;

        char name [ 256 ]; 
		
		memset ( name, 0, sizeof ( name ) );

        if ( fread ( name, 1, h.name_len, file ) != h.name_len )
			return false;

        fseek ( file, h.extra_len + h.comment_len, SEEK_CUR );
				
		strcpy ( files [ i ].name, _strlwr ( name ) );

        files [ i ].offset = h.offset;
        files [ i ].size   = h.size;
        files [ i ].csize  = h.csize;
    }

	fclose ( file );

	// search requested files
	for ( i = 0; i < dir.count; i++ )
	{
		if ( strstr ( _strlwr ( files [ i ].name ), _strlwr ( filename ) ) != 0 )
		{
			*length = files [ i ].size;

			SAFE_DELETE ( files );
			return TRUE;
		}
	}

	*length = 0;

	SAFE_DELETE ( files );
	
	return FALSE;
}

BOOL file_loader::GetLength::File_in_PK3 ( LPSTR pk3name, LPSTR filename, int* length )
{
	return File_in_ZIP ( pk3name, filename, length );
}

typedef struct
{
	char id [ 4 ];  // should be WAD2 or 2DAW
	int  numlumps;
	int  infotableofs;
} wadinfo_t;

typedef struct
{
	int  filepos;
	int  disksize;
	int  size;
	char type;
	char compression;
	char pad1,
		 pad2;
	char name [ 16 ];
} lumpinfo_t;

BOOL file_loader::Load::From_WAD ( LPSTR wadname, LPSTR filename, byte** data, int* length )
{
	if ( data != NULL )
		*data = NULL;

	FILE* file = fopen ( wadname, "rb" );

	if ( !file )
	{
		return FALSE;
	}

	wadinfo_t w;

	if ( fread ( &w, 1, sizeof ( w ), file ) != sizeof ( w ) )
	{
		fclose ( file );
		return FALSE;
	}

	if ( w.id [ 0 ] != 'W' || w.id [ 1 ] != 'A' || w.id [ 2 ] != 'D' || w.id [ 3 ] != '3' )
	{
		if ( w.id [ 0 ] != 'W' || w.id [ 1 ] != 'A' || w.id [ 2 ] != 'D' || w.id [ 3 ] != '2' )
		{
			if ( w.id [ 0 ] != '2' || w.id [ 1 ] != 'D' || w.id [ 2 ] != 'A' || w.id [ 3 ] != 'W' )
			{
				fclose ( file );
				return FALSE;
			}
		}
	}

	int			lump_count; 
	lumpinfo_t* lumps = NULL;
	
	fseek ( file, w.infotableofs, SEEK_SET );

	lump_count = w.numlumps;
	lumps      = new lumpinfo_t [ w.numlumps ];

	if ( fread ( lumps, sizeof ( lumpinfo_t ), w.numlumps, file ) != w.numlumps )
	{
		fclose ( file );
		SAFE_DELETE ( lumps );
		return FALSE;
	}
	
	char fn [ MAX_PATH ];
	
	strcpy ( fn, filename );

	if ( _strnicmp ( fn + strlen ( fn ) - 7, ".wadtex", 7 ) == 0 )
	{
		fn [ strlen ( fn ) - 7 ] = '\0';
	}

	for ( int i = 0; i < lump_count; i++ )
	{
		if ( _strcmpi ( lumps [ i ].name, fn ) == 0 )
		{
			if ( length != NULL )
			{
				*length = lumps [ i ].disksize;
			}

			if ( data != NULL )
			{
				fseek ( file, lumps [ i ].filepos, SEEK_SET );
				
				*data = ( byte* ) malloc ( lumps [ i ].disksize );

				if ( fread ( *data, 1, lumps [ i ].disksize, file ) != lumps [ i ].disksize )
				{
					fclose ( file );
					SAFE_DELETE ( *data );
					return FALSE;
				}
			}

			fclose ( file );
			SAFE_DELETE ( lumps );
			return TRUE;
		}
	}

	fclose ( file );

	if ( data != NULL )
		SAFE_DELETE ( *data );

	SAFE_DELETE ( lumps );

	return FALSE;
}
