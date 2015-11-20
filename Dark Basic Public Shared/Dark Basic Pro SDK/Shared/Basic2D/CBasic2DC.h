#ifndef _CBASIC2D_H_
#define _CBASIC2D_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3dx9.lib" )
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
#define D3DFVF_LINES ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct tagLinesVertexDesc
{
	float x, y, z, rhw;
	DWORD dwColour;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void	ConstructorBasic2D		( HINSTANCE );
		void	DestructorBasic2D		( void );
		void	SetErrorHandlerBasic2D	( LPVOID pErrorHandlerPtr );
		void	PassCoreDataBasic2D		( LPVOID pGlobPtr );
		void	RefreshD3DBasic2D		( int iMode );
		void	UpdateBPPBasic2D		( void );
#endif

DARKSDK void	Constructor				( HINSTANCE );
DARKSDK void	Destructor				( void );
DARKSDK void	SetErrorHandler			( LPVOID pErrorHandlerPtr );
DARKSDK void	PassCoreData			( LPVOID pGlobPtr );
DARKSDK void	RefreshD3D				( int iMode );
DARKSDK void	UpdateBPP				( void );

DARKSDK void	Clear					( void );
DARKSDK void	Clear					( int iRed, int iGreen, int iBlue );
DARKSDK void	Ink						( int iRedF, int iGreenF, int iBlueF, int iRedB, int iGreenB, int iBlueB );
DARKSDK bool	GetLockable				( void );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void	ClearRgb				( DWORD RGBBackColor );
DARKSDK void	Ink						( DWORD RGBForeColor, DWORD RGBBackColor );
DARKSDK void	Dot						( int iX, int iY );
DARKSDK void	Box						( int iLeft, int iTop, int iRight, int iBottom );
DARKSDK void	Line					( int iXa, int iYa, int iXb, int iYb );
DARKSDK void	Circle					( int iX, int iY, int iRadius );
DARKSDK void	Elipse					( int iX, int iY, int iXRadius, int iYRadius );
DARKSDK DWORD	Rgb						( int iRed, int iGreen, int iBlue );
DARKSDK int		RgbR					( DWORD iRGB );
DARKSDK int		RgbG					( DWORD iRGB );
DARKSDK int		RgbB					( DWORD iRGB );
DARKSDK DWORD	GetPoint				( int iX, int iY );

DARKSDK void	Dot						( int iX, int iY, DWORD dwColor );
DARKSDK void	BoxGradient				( int iLeft, int iTop, int iRight, int iBottom, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4 );
DARKSDK void	Lock					( void );
DARKSDK void	Unlock					( void );
DARKSDK DWORD	GetPixelPtr				( void );
DARKSDK DWORD	GetPixelPitch			( void );

#ifdef DARKSDK_COMPILE
		void	dbCLS					( DWORD RGBBackColor );
		void	dbInk					( DWORD RGBForeColor, DWORD RGBBackColor );
		void	dbDot					( int iX, int iY );
		void	dbBox					( int iLeft, int iTop, int iRight, int iBottom );
		void	dbLine					( int iXa, int iYa, int iXb, int iYb );
		void	dbCircle				( int iX, int iY, int iRadius );
		void	dbElipse				( int iX, int iY, int iXRadius, int iYRadius );
		DWORD	dbRgb					( int iRed, int iGreen, int iBlue );
		int		dbRgbR					( DWORD iRGB );
		int		dbRgbG					( DWORD iRGB );
		int		dbRgbB					( DWORD iRGB );
		DWORD	dbPoint					( int iX, int iY );

		void	dbDot					( int iX, int iY, DWORD dwColor );
		void	dbBox					( int iLeft, int iTop, int iRight, int iBottom, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4 );
		void	dbLockPixels			( void );
		void	dbUnLockPixels			( void );
		DWORD	dbGetPixelsPointer		( void );
		DWORD	dbGetPixelsPitch		( void );
		void	dbCopyArea				( int iDestX, int iDestY, int iWidth, int iHeight, int iSourceX, int iSourceY );

		// lee - 300706 - GDK fixes
		DWORD	dbRGB					( int iRed, int iGreen, int iBlue );
		int		dbRGBR					( DWORD iRGB );
		int		dbRGBG					( DWORD iRGB );
		int		dbRGBB					( DWORD iRGB );
		void	dbEllipse				( int iX, int iY, int iXRadius, int iYRadius );
		void	dbUnlockPixels			( void );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CBASIC2D_H_
