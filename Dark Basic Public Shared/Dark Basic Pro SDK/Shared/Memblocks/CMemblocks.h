#ifndef _CMEMBLOCKS_H_
#define _CMEMBLOCKS_H_

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
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void	ConstructorMemblocks		( void );
		void	DestructorMemblocks			( void );
		void	SetErrorHandlerMemblocks	( LPVOID pErrorHandlerPtr );
		void	PassCoreDataMemblocks		( LPVOID pGlobPtr );
		void	RefreshD3DMemblocks			( int iMode );
#endif

DARKSDK void	Constructor  				( void );
DARKSDK void	Destructor   				( void );
DARKSDK void	SetErrorHandler 			( LPVOID pErrorHandlerPtr );
DARKSDK void	PassCoreData				( LPVOID pGlobPtr );
DARKSDK void	RefreshD3D					( int iMode );

DARKSDK void	ReturnMemblockPtrs			( LPVOID* pSizeArray, LPVOID* pDataArray );

// memblock external access
DARKSDK LPSTR	ExtMakeMemblock				( int mbi, DWORD size );
DARKSDK void	ExtFreeMemblock				( int mbi );
DARKSDK DWORD	ExtGetMemblockSize			( int mbi );
DARKSDK void	ExtSetMemblockSize			( int mbi, DWORD size );
DARKSDK LPSTR	ExtGetMemblockPtr			( int mbi );

// memblock creation functions
DARKSDK void	MakeMemblock				( int mbi, int size );
DARKSDK void	DeleteMemblock				( int mbi );
DARKSDK int		MemblockExist				( int mbi );
DARKSDK DWORD	GetMemblockPtr				( int mbi );
DARKSDK int		GetMemblockSize				( int mbi );
DARKSDK void	CopyMemblock				( int mbiF, int mbiT, int posF, int posT, int size );
DARKSDK void	WriteMemblockByte			( int mbi, int pos, int data );
DARKSDK int		ReadMemblockByte			( int mbi, int pos );
DARKSDK void	WriteMemblockWord			( int mbi, int pos, int data );
DARKSDK int		ReadMemblockWordLLL			( int mbi, int pos );
DARKSDK void	WriteMemblockDWord			( int mbi, int pos, DWORD data );
DARKSDK DWORD	ReadMemblockDWord			( int mbi, int pos );
DARKSDK void	WriteMemblockFloat			( int mbi, int pos, float data );
DARKSDK DWORD	ReadMemblockFloat			( int mbi, int pos );
DARKSDK void	CreateMemblockFromBitmap	( int mbi, int bitmapindex );
DARKSDK void	CreateBitmapFromMemblock	( int bitmapindex, int mbi );
DARKSDK void	CreateMemblockFromMesh		( int mbi, int meshid );
DARKSDK void	CreateMeshFromMemblock		( int meshid, int mbi );
DARKSDK void	ChangeMeshFromMemblock		( int meshid, int mbi );
DARKSDK void	CreateMemblockFromSound		( int mbi, int soundindex );
DARKSDK void	CreateSoundFromMemblock		( int soundindex, int mbi );
DARKSDK void	CreateMemblockFromImage		( int mbi, int imageid );
DARKSDK void	CreateImageFromMemblock		( int imageid, int mbi );
DARKSDK void	CreateMemblockFromArray		( int mbi, DWORD arrayptr );
DARKSDK void	CreateArrayFromMemblock		( DWORD arrayptr, int mbi );

#ifdef DARKSDK_COMPILE
		void	dbReturnMemblockPtrs		( LPVOID* pSizeArray, LPVOID* pDataArray );

		LPSTR	dbExtMakeMemblock			( int mbi, DWORD size );
		void	dbExtFreeMemblock			( int mbi );
		DWORD	dbExtGetMemblockSize		( int mbi );
		void	dbExtSetMemblockSize		( int mbi, DWORD size );
		LPSTR	dbExtGetMemblockPtr			( int mbi );

		void	dbMakeMemblock				( int mbi, int size );
		void	dbDeleteMemblock			( int mbi );
		int		dbMemblockExist				( int mbi );
		DWORD	dbGetMemblockPtr			( int mbi );
		int		dbGetMemblockSize			( int mbi );
		void	dbCopyMemblock				( int mbiF, int mbiT, int posF, int posT, int size );
		void	dbWriteMemblockByte			( int mbi, int pos, int data );
		int		dbMemblockByte				( int mbi, int pos );
		void	dbWriteMemblockWord			( int mbi, int pos, int data );
		int		dbMemblockWord				( int mbi, int pos );
		void	dbWriteMemblockDWORD		( int mbi, int pos, DWORD data );
		DWORD	dbMemblockDWORD				( int mbi, int pos );
		void	dbWriteMemblockFloat		( int mbi, int pos, float data );
		float	dbMemblockFloat				( int mbi, int pos );
		void	dbMakeMemblockFromBitmap	( int mbi, int bitmapindex );
		void	dbMakeBitmapFromMemblock	( int bitmapindex, int mbi );
		void	dbMakeMemblockFromMesh		( int mbi, int meshid );
		void	dbMakeMeshFromMemblock		( int meshid, int mbi );
		void	dbChangeMeshFromMemblock	( int meshid, int mbi );
		void	dbMakeMemblockFromSound		( int mbi, int soundindex );
		void	dbMakeSoundFromMemblock		( int soundindex, int mbi );
		void	dbMakeMemblockFromImage		( int mbi, int imageid );
		void	dbMakeImageFromMemblock		( int imageid, int mbi );
		void	dbMakeMemblockFromArray		( int mbi, DWORD arrayptr );
		void	dbMakeArrayFromMemblock		( DWORD arrayptr, int mbi );

		// lee - 300706 - GDK fixes
		void	dbWriteMemblockByte			( int mbi, int pos, unsigned char data );
		void	dbWriteMemblockWord			( int mbi, int pos, WORD data );
		void	dbWriteMemblockDword		( int mbi, int pos, DWORD data );
		DWORD	dbMemblockDword				( int mbi, int pos );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


#endif _CMEMBLOCKS_H_