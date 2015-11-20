#ifndef _DBPROGLOBAL_H_
#define _DBPROGLOBAL_H_

#pragma once

//
//	aaron - 20120811 - 'delete' automatically checks for a nullptr
//
//#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
#undef SAFE_DELETE
#undef SAFE_RELEASE
#undef SAFE_DELETE_ARRAY
#undef SAFE_MEMORY
#undef SAFE_CLOSE
#undef SAFE_FREE

#define SAFE_DELETE( p )		delete( p ), ( p ) = nullptr
#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
//#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p )	delete[]( p ), ( p ) = nullptr
#define SAFE_MEMORY( p )		{ if ( p == NULL ) return false; }
#define SAFE_CLOSE(x)			if(x) { CloseHandle(x); x=NULL; }
#define SAFE_FREE(x)			if(x) { GlobalFree(x); x=NULL; }

#ifndef MAX_STRING
# define MAX_STRING				256 
#endif

//
//	aaron - 20120811 - DB_EXPORT for 'extern "C"' dllexports
//
#ifdef DARKSDK_COMPILE
# undef DARKSDK 
# define DARKSDK
# define DB_EXPORT				//static
# define DBPRO_GLOBAL			//static
# define DARKSDK_DLL			//static
#else
# define DARKSDK				__declspec ( dllexport )
# define DB_EXPORT				extern "C" __declspec(dllexport)
# define DBPRO_GLOBAL 
# define DARKSDK_DLL 
#endif

#endif
