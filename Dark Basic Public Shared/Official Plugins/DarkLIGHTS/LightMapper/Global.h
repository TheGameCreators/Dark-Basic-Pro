
#ifndef _DBPROGLOBAL_H_
#define _DBPROGLOBAL_H_

#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#define SAFE_MEMORY( p )		{ if ( p == NULL ) return false; }
#define SAFE_CLOSE(x)			if(x) { CloseHandle(x); x=NULL; }
#define SAFE_FREE(x)			if(x) { GlobalFree(x); x=NULL; }

#ifndef MAX_STRING
 #define MAX_STRING				256
#endif

#ifdef DARKSDK_COMPILE
	#undef DARKSDK 
	#define DARKSDK
	#define DBPRO_GLOBAL
	#define DARKSDK_DLL
#else
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
	#define DARKSDK_DLL 
#endif

/*
#ifdef DARKSDK_COMPILE
	#define DARKSDK 
	#define DBPRO_GLOBAL static
	#define DARKSDK_DLL static
#else
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
	#define DARKSDK_DLL 
#endif
*/

#endif