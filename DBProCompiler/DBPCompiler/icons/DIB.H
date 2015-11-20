/****************************************************************************\
*			 
*     FILE:     DIB.H
*
*     PURPOSE:  IconPro Project DIB handling header file
*
*     COMMENTS: Icons are stored in something almost identical to DIB
*               format, which makes it real easy to treat them as DIBs
*               when manipulating them.
*
*     Copyright 1995 Microsoft Corp.
*
*
* History:
*                July '95 - Created
*
\****************************************************************************/


/****************************************************************************/
// local #defines

// How wide, in bytes, would this many bits be, DWORD aligned?
#define WIDTHBYTES(bits)      ((((bits) + 31)>>5)<<2)
/****************************************************************************/


/****************************************************************************/
// Exported function prototypes
LPSTR FindDIBBits (LPSTR lpbi);
WORD DIBNumColors (LPSTR lpbi);
WORD PaletteSize (LPSTR lpbi);
DWORD BytesPerLine( LPBITMAPINFOHEADER lpBMIH );
LPBYTE ConvertDIBFormat( LPBITMAPINFO lpSrcDIB, UINT nWidth, UINT nHeight, UINT nColors, BOOL bStretch );
void SetMonoDIBPixel( LPBYTE pANDBits, DWORD dwWidth, DWORD dwHeight, DWORD x, DWORD y, BOOL bWhite );
LPBYTE ReadBMPFile( LPCTSTR szFileName );
BOOL WriteBMPFile( LPCTSTR szFileName, LPBYTE lpDIB );
/****************************************************************************/
