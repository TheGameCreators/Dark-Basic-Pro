/****************************************************************************\
*			 
*     FILE:     IconPro.H
*
*     PURPOSE:  IconPro Project main header file
*
*     COMMENTS: The child window size is determined here, based on the
*               hard coded maximum sizes of icons supported. Only one
*               function is exported from IconPro.C, and it is exported
*               to allow MDI child windows to query for a save file name
*               if they are asked to close and their data has changed.
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

// How large of icons will we support? This is really only important
// bacuase of the design of the UI. It needs to draw the entire icon
// 4 times, so we limit how big one can be. The ICO spec puts no limit
// on icon sizes.
#define MAX_ICON_WIDTH	128              // Max width
#define MIN_ICON_WIDTH	16               // Min width
#define MAX_ICON_HEIGHT	MAX_ICON_WIDTH   // Max height
#define MIN_ICON_HEIGHT	MIN_ICON_WIDTH   // Min height

// How big do the MDI child windows need to be to display the icon and
// the listbox?
#define WINDOW_WIDTH		( ( MAX_ICON_WIDTH * 2 ) + 30 )
#define WINDOW_HEIGHT		( ( MAX_ICON_HEIGHT * 2 ) + 150 )

// Utility macro to calculate a rectangle's width/height
#define RectWidth(r)		((r).right - (r).left + 1)
#define RectHeight(r)		((r).bottom - (r).top + 1)
/****************************************************************************/




/****************************************************************************/
// Exported function prototypes
BOOL GetSaveIconFileName( LPTSTR szFileName, UINT FilterStringID, LPCTSTR szTitle );
BOOL GetOpenIconFileName( LPTSTR szFileName, UINT FilterStringID, LPCTSTR szTitle );
/****************************************************************************/
