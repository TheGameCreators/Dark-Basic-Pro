#ifndef _CBITMAP_H_
#define _CBITMAP_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "gdi32.lib" )

#include <D3D9.h>
#include <D3DX9.h>

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
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct tagData
{
	LPDIRECT3DSURFACE9		lpSurface;					// render target surface
	LPDIRECT3DSURFACE9		lpDepth;					// render target depthbuffer

	int						iWidth;						// width of texture
	int						iHeight;					// height of texture
	int						iDepth;						// depth of texture

	int						iMirrored;					// mirror
	int						iFlipped;					// flip
	int						iFadeValue;					// fade of fade setting

	bool					bLocked;					// is locked
	D3DLOCKED_RECT			d3dlr;						// information structure

	char					szLongFilename  [ 256 ];	// long filename  e.g. "c:\\images\\a.bmp"
	char					szShortFilename [ 256 ];	// short filename e.g. ".\\a.bmp"
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void				ConstructorBitmap						( HINSTANCE );
		void				DestructorBitmap						( void );
		void				SetErrorHandlerBitmap					( LPVOID pErrorHandlerPtr );
		void				PassCoreDataBitmap						( LPVOID pGlobPtr );
		void				RefreshD3DBitmap						( int iMode );
		void				UpdateBitmapZeroOfNewBackbufferBitmap	( void );
#endif

DARKSDK void				Constructor								( HINSTANCE );
DARKSDK void				Destructor								( void );
DARKSDK void				SetErrorHandler							( LPVOID pErrorHandlerPtr );
DARKSDK void				PassCoreData							( LPVOID pGlobPtr );
DARKSDK void				RefreshD3D								( int iMode );
DARKSDK void				UpdateBitmapZeroOfNewBackbuffer			( void );
DARKSDK LPDIRECT3DSURFACE9	Make									( int iID, int iWidth, int iHeight );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void				CreateBitmap							( int iID, int iWidth, int iHeight );
DARKSDK void 				LoadBitmap 								( char* szFilename, int iID );
DARKSDK void 				SaveBitmap 								( char* szFilename, int iID );
DARKSDK void 				LoadBitmapEx 							( char* szFilename );
DARKSDK void 				SaveBitmapEx 							( char* szFilename );
DARKSDK void 				CopyBitmap 								( int iID, int iToID );
DARKSDK void 				CopyBitmapEx							( int iID, int iX1, int iY1, int iX2, int iY2, int iToID, int iToX1, int iToY1, int iToX2, int iToY2 );
DARKSDK void 				DeleteBitmapEx 							( int iID );
DARKSDK void 				FlipBitmap 								( int iID );
DARKSDK void 				MirrorBitmap 							( int iID );
DARKSDK void 				FadeBitmap 								( int iID, int iFade );
DARKSDK void 				BlurBitmap 								( int iID, int iBlurLevel );
DARKSDK void 				SetCurrentBitmap 						( int iID );

DARKSDK int 				CurrentBitmap 							( void );
DARKSDK int 				BitmapExist 							( int iID );
DARKSDK int 				BitmapWidth 							( int iID );
DARKSDK int					BitmapHeight 							( int iID );
DARKSDK int 				BitmapDepth 							( int iID );
DARKSDK int 				BitmapMirrored 							( int iID );
DARKSDK int 				BitmapFlipped 							( int iID );
DARKSDK int 				BitmapExist 							( void );
DARKSDK int 				BitmapWidth 							( void );
DARKSDK int 				BitmapHeight 							( void );
DARKSDK int 				BitmapDepth 							( void );
DARKSDK int 				BitmapMirrored 							( void );
DARKSDK int 				BitmapFlipped 							( void );

DARKSDK void 				GetBitmapData							( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
DARKSDK void 				SetBitmapData							( int bitmapindex, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize );

// mike - 061005 - new function to change bitmap format for alpha etc.
DARKSDK void				SetBitmapFormat							( int iFormat );

#ifdef DARKSDK_COMPILE
		void				dbCreateBitmap							( int iID, int iWidth, int iHeight );
		void 				dbLoadBitmap 							( char* szFilename, int iID );
		void 				dbSaveBitmap 							( char* szFilename, int iID );
		void 				dbLoadBitmap 							( char* szFilename );
		void 				dbSaveBitmap 							( char* szFilename );
		void 				dbCopyBitmap 							( int iID, int iToID );
		void 				dbCopyBitmap							( int iID, int iX1, int iY1, int iX2, int iY2, int iToID, int iToX1, int iToY1, int iToX2, int iToY2 );
		void 				dbDeleteBitmap	 						( int iID );
		void 				dbFlipBitmap 							( int iID );
		void 				dbMirrorBitmap 							( int iID );
		void 				dbFadeBitmap 							( int iID, int iFade );
		void 				dbBlurBitmap 							( int iID, int iBlurLevel );
		void 				dbSetCurrentBitmap 						( int iID );

		int 				dbCurrentBitmap 						( void );
		int 				dbBitmapExist 							( int iID );
		int 				dbBitmapWidth 							( int iID );
		int					dbBitmapHeight 							( int iID );
		int 				dbBitmapDepth 							( int iID );
		int 				dbBitmapMirrored 						( int iID );
		int 				dbBitmapFlipped 						( int iID );
		int 				dbBitmapExist 							( void );
		int 				dbBitmapWidth 							( void );
		int 				dbBitmapHeight 							( void );
		int 				dbBitmapDepth 							( void );
		int 				dbBitmapMirrored 						( void );
		int 				dbBitmapFlipped 						( void );

		void 				dbGetBitmapData							( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
		void 				dbSetBitmapData							( int bitmapindex, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize );

		void				dbSetBitmapFormat							( int iFormat );
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CBITMAP_H_
