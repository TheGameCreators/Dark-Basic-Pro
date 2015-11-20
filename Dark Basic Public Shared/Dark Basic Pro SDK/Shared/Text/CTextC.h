#ifndef _CTEXT_H_
#define _CTEXT_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3dx9.lib" )
#include <D3DX9.h>
#include "..\\Position\\cpositionc.h"

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

#define MAX_NUM_VERTICES 50 * 6
#define D3DFVF_FONT2DVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct FONT2DVERTEX 
{ 
	D3DXVECTOR4 p;
	DWORD		color;
	float		tu;
	float		tv;
};

struct FONT3DVERTEX 
{ 
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	float		tu;
	float		tv;
};

inline FONT2DVERTEX InitFont2DVertex ( const D3DXVECTOR4& p, D3DCOLOR color, FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   
	
	v.p     = p;
	v.color = color;
	v.tu    = tu;
	v.tv    = tv;

    return v;
}

inline FONT3DVERTEX InitFont3DVertex ( const D3DXVECTOR3& p, const D3DXVECTOR3& n, FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;
	
	v.p  = p;
	v.n  = n;
	v.tu = tu;
	v.tv = tv;

    return v;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void TextConstructor ( HINSTANCE );
DARKSDK void TextDestructor  ( void );
DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData( LPVOID pGlobPtr );
DARKSDK void RefreshD3D ( int iMode );

#ifdef DARKSDK_COMPILE
	void ConstructorText ( HINSTANCE );
	void DestructorText  ( void );
	void SetErrorHandlerText ( LPVOID pErrorHandlerPtr );
	void PassCoreDataText( LPVOID pGlobPtr );
	void RefreshD3DText ( int iMode );
#endif

DARKSDK void SetupFont   ( void );
DARKSDK void SetupStates ( void );
DARKSDK void Recreate    ( void );

DARKSDK void SetTextColor ( int iAlpha, int iRed, int iGreen, int iBlue );
DARKSDK void SetTextFont          ( char* szTypeface, int iCharacterSet );		// set the text font and character set
DARKSDK void Text ( int iX, int iY, char* szText );
DARKSDK int  GetTextWidth          ( char* szString );
DARKSDK int  GetTextHeight         ( char* szString );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK int	  Asc	( DWORD dwSrcStr);
DARKSDK DWORD Bin	( DWORD pDestStr, int iValue );
DARKSDK DWORD Chr	( DWORD pDestStr, int iValue );
DARKSDK DWORD Hex	( DWORD pDestStr, int iValue );
DARKSDK DWORD Left	( DWORD pDestStr, DWORD szText, int iValue );
DARKSDK int	  Len	( DWORD dwSrcStr );
DARKSDK DWORD Lower ( DWORD szText, DWORD pDestStr );
DARKSDK DWORD Mid	( DWORD pDestStr, DWORD szText, int iValue );
DARKSDK DWORD Right ( DWORD pDestStr, DWORD szText, int iValue );
DARKSDK DWORD Str	( DWORD pDestStr, float fValue );
DARKSDK DWORD StrEx	( DWORD pDestStr, float fValue, int iDecPlaces );
DARKSDK DWORD Str	( DWORD pDestStr, int iValue );
DARKSDK DWORD Upper ( DWORD pDestStr, DWORD szText );
//DARKSDK int	  ValL	( DWORD dwSrcStr );  LEEFIX - 041002 - removed in favour of casted Float version
DARKSDK DWORD ValF	( DWORD dwSrcStr );
DARKSDK DWORD StrDouble ( DWORD pDestStr, double dValue );
DARKSDK DWORD StrDoubleInt ( DWORD pDestStr, LONGLONG lValue );
DARKSDK LONGLONG ValR ( DWORD dwSrcStr );

DARKSDK void PerformChecklistForFonts ( void );
DARKSDK void BasicText ( int iX, int iY, DWORD szText );
DARKSDK void CenterText ( int iX, int iY, DWORD szText );						// centre text at x and y coordinates
DARKSDK void SetBasicTextFont     ( DWORD szTypeface );							// set the text font
DARKSDK void SetBasicTextFont     ( DWORD szTypeface, int iCharacterSet );		// set the text font and character set
DARKSDK void SetTextSize          ( int iSize );								// set the size
DARKSDK void SetTextToNormal      ( void );										// set text to normal style
DARKSDK void SetTextToItalic      ( void );										// set text to italic
DARKSDK void SetTextToBold        ( void );										// set text to bold
DARKSDK void SetTextToBoldItalic  ( void );										// set text to bold italic
DARKSDK void SetTextToOpaque      ( void );										// set text to opaque
DARKSDK void SetTextToTransparent ( void );										// set text to transparent

DARKSDK int   TextBackgroundType ( void );
DARKSDK DWORD TextFont			 ( DWORD pDestStr );
DARKSDK int   TextSize           ( void );
DARKSDK int   TextStyle          ( void );
DARKSDK int   TextWidth          ( DWORD szString );
DARKSDK int   TextHeight         ( DWORD szString );

DARKSDK void Text3D     ( char* szText );										// print 3D text at x and y coordinates

DARKSDK DWORD	  Spaces	( DWORD dwSrcStr, int iSpaces );

// u74b7 - removed append statement as not fixable
//DARKSDK void  Append        ( DWORD dwA, DWORD dwB );
DARKSDK void  Reverse       ( DWORD dwA );
DARKSDK int   FindFirstChar ( DWORD dwSource, DWORD dwChar );
DARKSDK int   FindLastChar  ( DWORD dwSource, DWORD dwChar );
DARKSDK int   FindSubString ( DWORD dwSource, DWORD dwString );
DARKSDK int   CompareCase   ( DWORD dwA, DWORD dwB );
DARKSDK DWORD FirstToken    ( DWORD dwReturn, DWORD dwSource, DWORD dwDelim );
DARKSDK DWORD NextToken     ( DWORD dwReturn, DWORD dwDelim );

#ifdef DARKSDK_COMPILE
		int			dbAsc						( char* dwSrcStr);
		char* 		dbBin						( int iValue );
		char* 		dbChr						( int iValue );
		char* 		dbHex						( int iValue );
		char* 		dbLeft						( char* szText, int iValue );
		int			dbLen						( char* dwSrcStr );
		char* 		dbLower 					( char* szText );
		char* 		dbMid						( char* szText, int iValue );
		char* 		dbRight 					( char* szText, int iValue );
		char* 		dbStr						( float fValue );
		char* 		dbStr						( int iValue );
		char* 		dbUpper 					( char* szText );
		float 		dbValF						( char* dwSrcStr );
		double		dbStrDouble					( double dValue );
		LONGLONG	dbValR						( char* dwSrcStr );

		void		dbPerformChecklistForFonts	( void );
		void		dbText						( int iX, int iY, char* szText );
		void 		dbCenterText				( int iX, int iY, char* szText );
		void 		dbSetTextFont				( char* szTypeface );
		void 		dbSetTextFont		  		( char* szTypeface, int iCharacterSet );	
		void 		dbSetTextSize          		( int iSize );							
		void 		dbSetTextToNormal      		( void );								
		void 		dbSetTextToItalic      		( void );								
		void 		dbSetTextToBold        		( void );								
		void 		dbSetTextToBoldItalic  		( void );								
		void 		dbSetTextToOpaque      		( void );								
		void 		dbSetTextToTransparent 		( void );								
		
		int   		dbTextBackgroundType 		( void );
		char* 		dbTextFont			 		( void );
		int   		dbTextSize           		( void );
		int   		dbTextStyle          		( void );
		int			dbTextWidth          		( char* szString );
		int			dbTextHeight         		( char* szString );

		void		dbText3D     				( char* szText );									

		char*		dbSpaces					( int iSpaces );

		void		dbAppend        			( char* dwA, char* dwB );
		void		dbReverse       			( char* dwA );
		int			dbFindFirstChar 			( char* dwSource, char* dwChar );
		int			dbFindLastChar  			( char* dwSource, char* dwChar );
		int			dbFindSubString 			( char* dwSource, char* dwString );
		// mike - 220107 - make second param char*
		int			dbCompareCase   			( char* dwA, char* dwB );
		char*		dbFirstToken    			( char* dwSource, char* dwDelim );
		char*		dbNextToken     			( char* dwDelim );

		void		dbSetTextColor				( int iAlpha, int iRed, int iGreen, int iBlue );
		void		dbSetTextFont				( char* szTypeface, int iCharacterSet );
		void		dbText						( int iX, int iY, char* szText );
		int			dbGetTextWidth				( char* szString );
		int			dbGetTextHeight				( char* szString );

		// lee - 300706 - GDK fixes
		void 		dbSetTextOpaque      		( void );								
		void 		dbSetTextTransparent 		( void );								
		float 		dbVal						( char* dwSrcStr );

#endif

//////////////////////////////////////////////////////////////////////////////////
#endif _CTEXT_H_