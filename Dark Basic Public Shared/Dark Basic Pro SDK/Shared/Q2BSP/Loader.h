
#ifndef _FILE_LOADER
#define _FILE_LOADER

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "unzip.h"
#include <direct.h>

#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }




BOOL SolveFullName ( LPSTR fullname, LPSTR* filename, LPSTR* path );
BOOL strcpy2       ( LPSTR*dest, LPSTR src );

struct files_found
{
	// structure used for "file_loader::Find::Files ( ... )" to save results

	int num_files;
	
	LPSTR zipname;

	struct _files
	{
		LPSTR filename;
		LPSTR path;
		LPSTR fullname;
	} *files;

	void Init ( )
	{
		zipname   = NULL;
		files     = NULL;
		num_files = 0;
		size     = 0;
	}

	int size;

	void Resize ( int num )
	{
		if ( num > size )
		{
			num += 100;

			files = ( _files* ) realloc ( files, sizeof ( _files ) * num );
			size  = num;
			
			for ( int x = num_files; x < size; x++ )
			{
				files [ x ].filename = NULL;
				files [ x ].path     = NULL;
				files [ x ].fullname = NULL;
			}
		}
		else if ( size - num > 100 )
		{
			for ( int x = num; x < size; x++ )
			{
				SAFE_DELETE ( files [ x ].filename );
				SAFE_DELETE ( files [ x ].path );
				SAFE_DELETE ( files [ x ].fullname );
			}

			files = ( _files* ) realloc ( files, sizeof ( _files ) * num );
			size  = num;
		}
	}
	
	void Release ( )
	{

		SAFE_DELETE ( zipname );

		if ( !files )
			Resize ( 0 );
		
		if ( files )
		{
			for ( int i = 0; i < num_files; i++ )
			{
				SAFE_DELETE_ARRAY ( files [ i ].filename );
				SAFE_DELETE_ARRAY ( files [ i ].path );
				SAFE_DELETE_ARRAY ( files [ i ].fullname );
			}
		}

		SAFE_DELETE_ARRAY ( zipname );

		if(files)
		{
			free ( files );
			files=NULL;
		}
		num_files = 0;

		// MIKE FIX 190303 - reset size value
		size = 0;
	}

	void AddFile ( LPSTR fullname )
	{
		if ( fullname == NULL )
			return;

		LPSTR	filename,
				path;

		SolveFullName ( fullname, &filename, &path );

		for ( int x = 0; x < num_files; x++ )
		{
			if ( _stricmp ( files [ x ].filename, filename ) == 0 )
			{
				SAFE_DELETE ( filename );
				SAFE_DELETE ( path );
				return;
			}
		}

		Resize ( num_files + 1 );
		strcpy2 ( &files [ num_files ].fullname, fullname );

		files [ num_files ].filename = filename;
		files [ num_files ].path     = path;

		SAFE_DELETE_ARRAY ( filename );

		num_files++;
	}

	void AddFile ( LPSTR fullname, LPSTR filename, LPSTR path )
	{
		if ( filename == NULL )
			return;

		for ( int x = 0; x < num_files; x++ )
		{
			if ( _stricmp ( files [ x ].filename, filename ) == 0 ) 
				return;
		}

		Resize ( num_files + 1 );

		if ( fullname != NULL )
			strcpy2 ( &files [ num_files ].fullname, fullname );

		if ( filename != NULL )
			strcpy2 ( &files [ num_files ].filename, filename );

		if ( path != NULL )
			strcpy2 ( &files [ num_files ].path, path );

		num_files++;

	}
};


#define files_found_in_ZIP files_found
#define files_found_in_PK3 files_found_in_ZIP
#define files_found_in_PAK files_found_in_ZIP

class file_loader
{
	public:
		static BOOL GetRelativePath ( LPSTR fullpath, LPSTR start, LPSTR* out );

	class Load
	{
		public:
			static BOOL Direct   ( LPSTR filename, byte** data, int* length );
			static BOOL From_ZIP ( LPSTR zipname, LPSTR filename, byte** data, int* length );
			static BOOL From_PK3 ( LPSTR pk3name, LPSTR filename, byte** data, int* length );
			static BOOL From_PAK ( LPSTR pakname, LPSTR filename, byte** data, int* length );
			static BOOL From_WAD ( LPSTR wadname, LPSTR filename, byte** data, int* length );
	};

	class Find
	{
		public:
			static BOOL File         ( LPSTR filename, LPSTR path, LPSTR* fullname );
			static BOOL Files        ( LPSTR common, LPSTR path, files_found* files );
			static BOOL Files_in_ZIP ( LPSTR zipname, LPSTR common, files_found_in_ZIP* files_found );
			static BOOL Files_in_PK3 ( LPSTR pk3name, LPSTR common, files_found_in_PK3* files_found );
			static BOOL Files_in_PAK ( LPSTR pakname, LPSTR common, files_found_in_PAK* files_found );
	};

	class GetLength
	{
		public:
			static BOOL File        ( LPSTR filename, int* length );
			static int  File        ( FILE* f );
			static BOOL File_in_ZIP ( LPSTR zipname, LPSTR filename, int* length );
			static BOOL File_in_PK3 ( LPSTR pk3name, LPSTR filename, int* length );
	};

	static BOOL Init_External_Files ( );
	
	class Q3A
	{
		public:
			static BOOL Add_PK3 ( LPSTR filename );
	};

	class Q2
	{
		public:
		static BOOL Add_WAD(LPSTR filename);
	};
};

BOOL IsDirectory ( LPSTR str );


#define EXT_QUAKE12 0x1
#define EXT_QUAKE3  0x2

extern struct _externfiles
{
	int extmap;
	char extmap_name [ 256 ];
} externfiles;


extern files_found files_cache;

#endif