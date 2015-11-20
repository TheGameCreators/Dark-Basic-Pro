/****************************************************************************\
*            
*     FILE:     ICONPRO.C
*
*     PURPOSE:  IconPro Project main C file
*
*     COMMENTS: This file contains the main window and instance handing
*               for this project.
*
*     FUNCTIONS:
*      EXPORTS: 
*               GetSaveIconFileName     - Get the name of the file to write
*               GetOpenIconFileName     - Get the name of a file to open
*      LOCALS:
*               WinMain                 - Program entry point
*               InitApplication         - Register classes
*               InitInstance            - Create windows
*               WndProc                 - Main Window's 'Window Procedure'
*               OnCommand               - Handle command messages (menu items)
*               AboutDlgProc            - About Dialog Box's 'Window Procedure'
*               CreateNewMDIChildWindow - Creates a new child MDI window
*               UpdateMenuState         - Grays/enables appropriate menu items
*               QueryCloseAllChildren   - Close all children if possible
*
*     Copyright 1995 Microsoft Corp.
*
*
* History:
*                July '95 - Created
*
\****************************************************************************/
#include <Windows.H>
#include "Resource.h"
#include "Icons.H"
#include "IconPro.h"

/****************************************************************************/
/* Local Function Prototypes */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
BOOL InitApplication( HANDLE hInstance );
BOOL InitInstance( HANDLE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
LRESULT OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
HWND CreateNewMDIChildWindow( LPSTR szTitle, LPVOID lpData );
BOOL UpdateMenuState( HWND hWnd, HMENU hMenu );
BOOL QueryCloseAllChildren( void );
/****************************************************************************/


/****************************************************************************/
/* Global Variables */
HINSTANCE	hInst;
HWND        hWndMain, hMDIClientWnd;
char        szAppName[] = TEXT("IconPro");
char        szTitle[] = TEXT("IconPro Icon Handler");
char        szChildClassName[] = TEXT("IconChildClass");
char        szHelpFileName[] = "IconPro.HLP";
/****************************************************************************/


/****************************************************************************
*
*     FUNCTION: WinMain
*
*     PURPOSE:  Main entry point for this app
*
*     PARAMS:   HANDLE hInstance     - This instance
*               HANDLE hPrevInstance - Previous instance
*               LPSTR  lpszCmdLine   - Command Line
*               int    nCmdShow      - Desired window status
*
*     RETURNS:  int - return code
*
* History:
*                July '95 - Created
*
\****************************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;

    // standard init stuff
    if( ! hPrevInstance )
    {
        if( ! InitApplication( hInstance ) )
        {
            return FALSE;
        }
    }
    if( ! InitInstance( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    // Standard message loop
    while (GetMessage(&msg, (HWND) NULL, 0, 0)) 
    {
        if( !TranslateMDISysAccel(hMDIClientWnd, &msg) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}
/* End WinMain() ************************************************************/



/****************************************************************************
*
*     FUNCTION: InitApplication
*
*     PURPOSE:  Register classes
*
*     PARAMS:   HANDLE hInstance     - This instance
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL InitApplication( HANDLE hInstance )
{
    return FALSE;
}
/* End InitApplication() ***************************************************/



/****************************************************************************
*
*     FUNCTION: InitInstance
*
*     PURPOSE:  Create and show the main window
*
*     PARAMS:   HANDLE hInstance  - This instance
*               int    nCmdShow   - desired show state
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL InitInstance( HANDLE hInstance, int nCmdShow )
{
    hInst = hInstance;
    hWndMain = CreateWindow(
        szAppName,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        NULL,
        NULL,
        hInstance,
        NULL );

    if( hWndMain == NULL )
    {
        return FALSE;
    }

    ShowWindow( hWndMain, nCmdShow );
    UpdateWindow( hWndMain );

    return TRUE;
}
/* End InitInstance() *****************************************************/



/****************************************************************************
*
*     FUNCTION: WndProc
*
*     PURPOSE:  Window Procedure for the main window.
*
*     PARAMS:   HWND   hWnd    - This window
*               UINT   Message - Which message?
*               WPARAM wParam  - message parameter
*               LPARAM lParam  - message parameter
*
*     RETURNS:  LRESULT - depends on message
*
* History:
*                July '95 - Created
*
\****************************************************************************/
LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
    // which message are we handling?
    switch( Message )
    {
        // Menu is coming up, initialize it
        case WM_INITMENU:
            UpdateMenuState( hWnd, (HMENU)wParam );
            return 1;
        break; // end WM_INITMENU

        // Window is being created, create the MDI client also
        case WM_CREATE:
        {
            CLIENTCREATESTRUCT ccs;

            // Retrieve the handle of the Window menu and assign the
            // first child window identifier.
            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 2 );
            ccs.idFirstChild = IDM_WINDOWCHILD;

            // Create the MDI client window
            hMDIClientWnd = CreateWindow(	TEXT("MDICLIENT"), (LPCTSTR) NULL,
                                            WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                                            0, 0, 0, 0, hWnd, (HMENU) 0xCAC, hInst, (LPSTR) &ccs );

            ShowWindow(hMDIClientWnd, SW_SHOW);
        }
        break; // End WM_CREATE

        // Command Messages (menu items, etc)
        case WM_COMMAND:
            OnCommand( hWnd, wParam, lParam );
            return DefFrameProc( hWnd, hMDIClientWnd, Message, wParam, lParam );
        break; // End WM_COMMAND

        // Time to close down now :(
        case WM_CLOSE:
        {
            // Will the children allow it? (Give 'em a chance to cancel)
            if( QueryCloseAllChildren() )
            {
                WinHelp( hWnd, szHelpFileName, HELP_QUIT, 0 );
                DestroyWindow( hWnd );
                PostQuitMessage( 0 );
                return DefFrameProc( hWnd, hMDIClientWnd, Message, wParam, lParam );
            }
            return 0;
        }
        break; // End WM_CLOSE

        // Pass it on to the default window procedure
        default:
            return DefFrameProc( hWnd, hMDIClientWnd, Message, wParam, lParam );
        break; // end default
    }
    return DefFrameProc( hWnd, hMDIClientWnd, Message, wParam, lParam );
}
/* End WndProc() ***********************************************************/



/****************************************************************************
*
*     FUNCTION: OnCommand
*
*     PURPOSE:  Handles command messages for main window
*
*     PARAMS:   HWND   hWnd    - This window
*               WPARAM wParam  - message parameter
*               LPARAM lParam  - message parameter
*
*     RETURNS:  LRESULT - depends on message
*
* History:
*                July '95 - Created
*
\****************************************************************************/
LRESULT OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    // Which command is it?
    switch( LOWORD( wParam ) )
    {
        // File->New menu option - spawn a new child
        case ID_FILE_NEW:
        {
            HWND    hWndNew;

            hWndNew = CreateNewMDIChildWindow( TEXT("Untitled"), NULL );
            SendMessage( hMDIClientWnd, WM_MDIACTIVATE, (WPARAM)hWndNew, 0 );
        }
        break; // End ID_FILE_NEW

        // File->Open menu option - open an ICO file
        case ID_FILE_OPEN:
        {
            HWND            hWndNew;
            LPICONRESOURCE	lpIR;
            TCHAR        	szFileName[MAX_PATH];
            TCHAR        	szFileTitle[MAX_PATH];

            // Get the name of the file to open
            if( GetOpenIconFileName( szFileName, IDS_FILTERSTRING, "Open Icon File" ) )
            {
                // Read in the icon data
                if( (lpIR = ReadIconFromICOFile( szFileName )) == NULL )
                    break;
                // Get the name of the file for the window title
                if( GetFileTitle( szFileName, szFileTitle, MAX_PATH ) )
                    break;
                // Make a new child to handle this icon
                hWndNew = CreateNewMDIChildWindow( szFileTitle, lpIR );
                SendMessage( hMDIClientWnd, WM_MDIACTIVATE, (WPARAM)hWndNew, 0 );
            }
        }
        break; // End ID_FILE_OPEN

        // File->Extract menu option - extract icon data from a DLL or EXE
        case ID_FILE_EXTRACT:
        {
            HWND            hWndNew;
            LPICONRESOURCE	lpIR;
            TCHAR        	szFileName[MAX_PATH];

            // Get the name of the file from which to extract the icon
            if( GetOpenIconFileName( szFileName, IDS_EXEFILTERSTRING, "Extract Icon from File" ) )
            {
                // Extract the icon data
                if( (lpIR = ReadIconFromEXEFile( szFileName )) == NULL )
                    break;
                // Make a new child to handle this icon
                hWndNew = CreateNewMDIChildWindow( ("Untitled"), lpIR );
                SendMessage( hMDIClientWnd, WM_MDIACTIVATE, (WPARAM)hWndNew, 0 );
            }
        }
        break; // End ID_FILE_EXTRACT

        // File->Save and File->SaveAs menu options - save current ICO file to disk
        case ID_FILE_SAVEAS:
        case ID_FILE_SAVE:
        {
            HWND    hWndActive;
            
            // Get the active MDI child window
            if( (hWndActive = (HWND)SendMessage( hMDIClientWnd, WM_MDIGETACTIVE, 0, 0 )) != NULL )
            {
                // Tell it to write its icon to disk
                SendMessage( hWndActive, WM_COMMAND, wParam, 0 );
            }
            else
                MessageBox( hWnd, TEXT("Error Getting Active Window"), TEXT("Error"), MB_OK );
        }
        break; // End ID_FILE_SAVE/ID_FILE_SAVEAS

        // File->Close menu option - close the current MDI child window
        case ID_FILE_CLOSE:
        {
            HWND    hWndActive;

            // Get the active MDI child window and tell it to close itself
            if( (hWndActive = (HWND)SendMessage( hMDIClientWnd, WM_MDIGETACTIVE, 0, 0)) != NULL )
                SendMessage( hWndActive, WM_CLOSE, 0, lParam );
        }
        break; // End ID_FILE_CLOSE

        // File->Exit menu option - shut 'er down
        case ID_F_EXIT:
            // Just send a WM_CLOSE - code there will handle it
            PostMessage( hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0 );
        break; // End ID_F_EXIT

        // Edit->Import BMP and Edit->Stretch-Import BMP - convert BMP file to icon
        // Edit->Copy, Edit->Paste, Edit->StretchPaste, Edit->AddFormat, Edit->RemoveFormat
        // menu options. Just pass 'em on to the currently active MDI child window
        case ID_EDIT_EXPORTBMP:
        case ID_EDIT_IMPORTBMP:
        case ID_EDIT_STRETCHIMPORTBMP:
        case ID_EDIT_COPY:
        case ID_EDIT_PASTE:
        case ID_EDIT_STRETCHPASTE:
        case ID_EDIT_ADDFORMAT:
        case ID_EDIT_REMOVEFORMAT:
        {
            HWND    hWndActive;
            
            // Get active MDI child
            if( (hWndActive = (HWND)SendMessage( hMDIClientWnd, WM_MDIGETACTIVE, 0, 0 )) != NULL )
            {
                // Pass the message along
                SendMessage( hWndActive, WM_COMMAND, wParam, 0 );
            }
        }
        break; // End ID_EDIT_XXXX

        // Window->ArrangeIcons menu option
        case ID_WINDOW_ARRANGEICONS:
            // Ask the MDIClient to do it
            SendMessage( hMDIClientWnd, WM_MDIICONARRANGE, 0, 0);
        break; // End ID_WINDOW_ARRANGEICONS

        // Window->Cascade menu option
        case ID_WINDOW_CASCADE:
            // Ask the MDIClient to do it
            SendMessage( hMDIClientWnd, WM_MDICASCADE, 0, 0);
        break; // End ID_WINDOW_CASCADE

        // Window->Tile menu option
        case ID_WINDOW_TILE:
            // Ask the MDIClient to do it
            SendMessage( hMDIClientWnd, WM_MDITILE, (WPARAM)MDITILE_HORIZONTAL, 0);
        break; // End ID_WINDOW_TILE

        // Window->TileVertical menu option
        case ID_WINDOW_TILEVERTICAL:
            // Ask the MDIClient to do it
            SendMessage( hMDIClientWnd, WM_MDITILE, (WPARAM)MDITILE_VERTICAL, 0);
        break; // End ID_WINDOW_TILEVERTICAL
        
        // Help->About menu option
        case ID_HELP_ABOUT:
            // Launch the About Dialog
            DialogBox( hInst, MAKEINTRESOURCE(ICONPRO_ABOUT_DLG), hWnd, AboutDlgProc );
        break;

        case ID_HELP_CONTENTS:
                WinHelp( hWnd, szHelpFileName, HELP_CONTENTS, 0 );
        break;
    }
    return 0l;
}
/* End OnCommand() *********************************************************/



/****************************************************************************
*
*     FUNCTION: AboutDlgProc
*
*     PURPOSE:  Handles messages for About Dialog
*
*     PARAMS:   HWND   hWnd    - This window
*               UINT   Msg     - Which message?
*               WPARAM wParam  - message parameter
*               LPARAM lParam  - message parameter
*
*     RETURNS:  BOOL - depends on message
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
        case WM_INITDIALOG:
        break;

        case WM_CLOSE:
            PostMessage( hWnd, WM_COMMAND, IDCANCEL, 0l );
        break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                    EndDialog( hWnd, TRUE );
                break;

                case IDCANCEL:
                    EndDialog( hWnd, FALSE );
                break;

            }
        break;
        default:
            return FALSE;
        break;
    }
    return TRUE;
}
/* End AboutDlgProc() ******************************************************/



/****************************************************************************
*
*     FUNCTION: CreateNewMDIChildWindow
*
*     PURPOSE:  Creates and shows a new MDI child window
*
*     PARAMS:   LPSTR  szTitle - The title for the new window
*               LPVOID lpData  - Data to attach to the window (icon info)
*
*     RETURNS:  HWND - Handle to the new MDI child window
*
* History:
*                July '95 - Created
*
\****************************************************************************/
HWND CreateNewMDIChildWindow( LPTSTR szTitle, LPVOID lpData )
{
    HWND    hReturnWnd;

    // Create the MDI child window
    hReturnWnd = CreateMDIWindow( szChildClassName, szTitle, 0,
                    CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
                    hMDIClientWnd, hInst, (LPARAM)lpData );
    // Show it
    ShowWindow( hReturnWnd, SW_SHOW );
    return hReturnWnd;
}
/* End CreateNewMDIChildWindow() ********************************************/



/****************************************************************************
*
*     FUNCTION: GetOpenIconFileName
*
*     PURPOSE:  Prompts user for the filename for opening
*
*     PARAMS:   LPSTR   szFileName     - Buffer to receive name of file
*               UINT    FilterStringID - ID of filter string
*               LPCTSTR szTitle        - Title for the Dialog
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL GetOpenIconFileName( LPTSTR szFileName, UINT FilterStringID, LPCTSTR szTitle )
{
    OPENFILENAME    ofn;
    UINT            i, cbString;
    TCHAR        	chReplace;
    TCHAR        	szFilter[256];

    // Get the filter string
    if ((cbString = LoadString( hInst, FilterStringID, szFilter, sizeof(szFilter))) == 0 ) 
        return FALSE;
    // Replace the wildcard with NULL
    chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
    for (i = 0; szFilter[i] != '\0'; i++) 
    {
        if (szFilter[i] == chReplace)
            szFilter[i] = '\0';
    }

    // Fill out the OPENFILENAME struct
    ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = hWndMain;
    ofn.hInstance = hInst;
    lstrcpy( szFileName, TEXT("") );
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrTitle = szTitle;

    // Use the common dialog
    return GetOpenFileName( &ofn );
}
/* End GetOpenIconFileName() ***********************************************/



/****************************************************************************
*
*     FUNCTION: GetSaveIconFileName
*
*     PURPOSE:  Prompts user for the filename for saving
*
*     PARAMS:   LPSTR   szFileName     - Buffer to receive name of file
*               UINT    FilterStringID - ID of filter string
*               LPCTSTR szTitle        - Title for the Dialog
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL GetSaveIconFileName( LPTSTR szFileName, UINT FilterStringID, LPCTSTR szTitle )
{
    OPENFILENAME    ofn;
    UINT            i, cbString;
    TCHAR        	chReplace;
    TCHAR        	szFilter[256];

    // Get the filter string
    if ((cbString = LoadString( hInst, FilterStringID, szFilter, sizeof(szFilter))) == 0 ) 
        return FALSE;
    // Replace the wildcard with NULL
    chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
    for (i = 0; szFilter[i] != '\0'; i++) 
    {
        if (szFilter[i] == chReplace)
            szFilter[i] = '\0';
    }

    // Fill out the OPENFILENAME struct
    ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = hWndMain;
    ofn.hInstance = hInst;
    lstrcpy( szFileName, TEXT("") );
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrDefExt = TEXT("ICO");
    ofn.lpstrTitle = szTitle;

    // Use the common dialog
    return GetSaveFileName( &ofn );
}
/* End GetSaveIconFileName() ***********************************************/



/****************************************************************************
*
*     FUNCTION: UpdateMenuState
*
*     PURPOSE:  Updates states of menu items (called on menu activation)
*
*     PARAMS:   HWND  hWnd  - Handle to the window
*               HMENU hMenu - Handle to the menu
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL UpdateMenuState( HWND hWnd, HMENU hMenu )
{
    UINT    nEnableFlag, nNumFormats;
    HWND    hWndActive;

    // Get a handle to the active MDI child window
    hWndActive=(HWND)SendMessage( hMDIClientWnd, WM_MDIGETACTIVE, 0, 0 );
    // Find out how many icon image formats it holds
    nNumFormats = SendMessage( hWndActive, WM_COMMAND, ID_GETNUMFORMATS, 0 );
    // Do we actually have an active window?
    if( hWndActive == NULL )
    {
        // No active MDI child...
        EnableMenuItem( hMenu, ID_FILE_NEW, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_OPEN, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_EXTRACT, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_F_EXIT, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_EDIT_COPY, MF_BYCOMMAND| MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_PASTE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_STRETCHPASTE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_ADDFORMAT, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_IMPORTBMP, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_STRETCHIMPORTBMP, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_EXPORTBMP, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_REMOVEFORMAT, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_WINDOW_ARRANGEICONS, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_WINDOW_CASCADE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_WINDOW_TILE, MF_BYCOMMAND | MF_GRAYED );
        EnableMenuItem( hMenu, ID_WINDOW_TILEVERTICAL, MF_BYCOMMAND | MF_GRAYED );
    }
    else
    {
        // We have an active MDI child...
        EnableMenuItem( hMenu, ID_FILE_NEW, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_OPEN, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_EXTRACT, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED );
        nEnableFlag = (BOOL)SendMessage( hWndActive, WM_COMMAND, ID_HASFILECHANGED, 0 ) ? MF_ENABLED : MF_GRAYED;
        EnableMenuItem( hMenu, ID_FILE_SAVE, MF_BYCOMMAND | nEnableFlag );
        EnableMenuItem( hMenu, ID_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_F_EXIT, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_EDIT_COPY, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        if( IsClipboardFormatAvailable( CF_DIB ) )
        {
            EnableMenuItem( hMenu, ID_EDIT_PASTE, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
            EnableMenuItem( hMenu, ID_EDIT_STRETCHPASTE, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        }
        else
        {
            EnableMenuItem( hMenu, ID_EDIT_PASTE, MF_BYCOMMAND | MF_GRAYED );
            EnableMenuItem( hMenu, ID_EDIT_STRETCHPASTE, MF_BYCOMMAND | MF_GRAYED );
        }
        EnableMenuItem( hMenu, ID_EDIT_IMPORTBMP, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_STRETCHIMPORTBMP, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_EXPORTBMP, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        EnableMenuItem( hMenu, ID_EDIT_ADDFORMAT, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_EDIT_REMOVEFORMAT, MF_BYCOMMAND | (nNumFormats>0)?MF_ENABLED:MF_GRAYED );
        EnableMenuItem( hMenu, ID_WINDOW_ARRANGEICONS, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_WINDOW_CASCADE, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_WINDOW_TILE, MF_BYCOMMAND | MF_ENABLED );
        EnableMenuItem( hMenu, ID_WINDOW_TILEVERTICAL, MF_BYCOMMAND | MF_ENABLED );
    }
    return TRUE;
}
/* End UpdateMenuState() ***************************************************/



/****************************************************************************
*
*  FUNCTION   : QueryCloseAllChildren()
*
*  PURPOSE    : Asks the child windows if it is ok to close up app. Nothing
*               is destroyed at this point. The z-order is not changed.
*
*  PARAMS:     : none
*
*
*  RETURNS    : TRUE - If all children agree to the query.
*               FALSE- If any one of them disagrees.
*
\***************************************************************************/
BOOL QueryCloseAllChildren( void )
{
    register HWND hwndT;

    for( hwndT = GetWindow(hMDIClientWnd, GW_CHILD); hwndT; hwndT = GetWindow (hwndT, GW_HWNDNEXT) )
    {
        if( GetWindow( hwndT, GW_OWNER ) )
            continue;

        if( SendMessage( hwndT, WM_CLOSE, 0, 0L ) )
            return FALSE;
    }
    return TRUE;
}
/* End QueryCloseAllChildren() **********************************************/
