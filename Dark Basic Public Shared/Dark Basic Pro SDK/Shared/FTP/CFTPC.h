#ifndef _CFTP_H_
#define _CFTP_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void	ConstructorFTP		( void );
		void	DestructorFTP		( void );
		void	SetErrorHandlerFTP	( LPVOID pErrorHandlerPtr );
		void	PassCoreDataFTP		( LPVOID pGlobPtr );
#endif

DARKSDK void	Constructor			( void );
DARKSDK void	Destructor			( void );
DARKSDK void	SetErrorHandler		( LPVOID pErrorHandlerPtr );
DARKSDK void	PassCoreData		( LPVOID pGlobPtr );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// FTP

DARKSDK void	ConnectEx			( DWORD dwString, DWORD dwString2, DWORD dwString3, int iUseWindow );
DARKSDK void	Connect				( DWORD dwString, DWORD dwString2, DWORD dwString3 );
DARKSDK void	Disconnect			( void );
DARKSDK void	DisconnectEx		( int iFlag );

DARKSDK void	SetDir				( DWORD dwString );
DARKSDK void	PutFile				( DWORD dwString );
DARKSDK void	GetFile				( DWORD dwString, DWORD dwString2 );
DARKSDK void	GetFile				( DWORD dwString, DWORD dwString2, int iFlag );
DARKSDK void	DeleteFile			( DWORD dwString );
DARKSDK void	FindFirst			( void );
DARKSDK void	FindNext			( void );
DARKSDK void	Terminate			( void );
DARKSDK void	Proceed				( void );

DARKSDK DWORD	GetError			( DWORD pDestStr );
DARKSDK DWORD	GetDir				( DWORD pDestStr );
DARKSDK DWORD	GetFileName			( DWORD pDestStr );
DARKSDK int		GetFileType			( void );
DARKSDK int		GetFileSize			( void );
DARKSDK int		GetProgress			( void );
DARKSDK int		GetStatus			( void );
DARKSDK int		GetFailure			( void );

DARKSDK void	HTTPConnect			( DWORD dwUrl );
DARKSDK DWORD	HTTPRequestData		( DWORD pDestStr, DWORD dwVerb, DWORD dwObjectName, DWORD dwPostData );
DARKSDK void	HTTPDisconnect		( void );

#ifdef DARKSDK_COMPILE
		 void	dbFTPConnect		( char* szString, char* szString2, char* szString3, int iUseWindow );
		 void	dbFTPConnect		( char* szString, char* szString2, char* szString3 );
		 void	dbFTPDisconnect		( void );
		 void	dbFTPDisconnect		( int iFlag );

		 void	dbFTPSetDir			( char* dwString );
		 void	dbFTPPutFile		( char* dwString );
		 void	dbFTPGetFile		( char* dwString, char* dwString2 );
		 void	dbFTPGetFile		( char* dwString, char* dwString2, int iFlag );
		 void	dbFTPDeleteFile		( char* dwString );
		 void	dbFTPFindFirst		( void );
		 void	dbFTPFindNext		( void );
		 void	dbFTPTerminate		( void );
		 void	dbFTPProceed		( void );

		 char*	dbGetFTPError		( void );
		 char*	dbGetFTPDir			( void );
		 char*	dbGetFTPFileName	( void );
		 int	dbGetFTPFileType	( void );
		 int	dbGetFTPFileSize	( void );
		 int	dbGetFTPProgress	( void );
		 int	dbGetFTPStatus		( void );
		 int	dbGetFTPFailure		( void );
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CFTP_H_