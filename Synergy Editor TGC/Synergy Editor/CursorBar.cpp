#include "StdAfx.h"
#include "CursorBar.h"
#include "Synergy Editor.h"
#include "Utilities.h"
#include "Variables.h"


BEGIN_MESSAGE_MAP(CursorBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_OPEN_CURSOR, OnImport)
	ON_COMMAND(ID_DELETE_CURSOR, OnDelete)
	ON_COMMAND(ID_DEFAULT_CURSOR, OnDefault)

	ON_UPDATE_COMMAND_UI(ID_DELETE_CURSOR, OnUpdateDelete)
	ON_UPDATE_COMMAND_UI(ID_DEFAULT_CURSOR, OnUpdateDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CursorBar::OnContextMenu(CWnd*, CPoint){} // Supress

// Adds a new media file to the tree (relative path) and uses the full path to add to the list
void CursorBar::AddNewFile(CString file)
{
	CString path(Utilities::GetRelativePath(file));

	m_ListBox.InsertItem(m_ListBox.GetItemCount(), path);
	m_ListBox.EnsureVisible(m_ListBox.GetItemCount()-1, TRUE);
	Variables::AddCursor(file);
}

void CursorBar::Clear()
{
	m_ListBox.DeleteAllItems();
}

void CursorBar::OnUpdateDelete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetSelectedCount() > 0);
}

void CursorBar::OnUpdateDefault(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetItemCount() == 0);
}

void CursorBar::OnImport()
{
	TCHAR strFilter[] = { _T("Cursor Files (*.cur)|*.cur|Bitmap Files (*.bmp)|*.bmp||") };

	CFileDialog FileDlg(TRUE, _T(".*"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, strFilter);
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		POSITION pos = FileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString file = FileDlg.GetNextPathName(pos);

			m_DefaultPath = Utilities::ExtractPath(file);

			if(Utilities::ExtractPath(file) == Variables::m_ProjectPath)
			{
				file = Utilities::ExtractFilename(file);
			}

			bool found = Variables::CursorExists(file) > -1;

			if(found)
			{
				AfxMessageBox(_T("The file ") + file + _T(" already exists in the project"), MB_ICONEXCLAMATION | MB_OK);
			}
			else
			{
				CString path(Utilities::GetRelativePath(file));

				m_ListBox.InsertItem(m_ListBox.GetItemCount(), path);
				Variables::AddCursor(file);
			}
		}
	}
}

void CursorBar::OnDelete()
{
	UINT i, uSelectedCount = m_ListBox.GetSelectedCount();
	int  nItem = -1;

	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
	   for (i=0;i < uSelectedCount;i++)
	   {
			nItem = m_ListBox.GetNextItem(nItem, LVNI_SELECTED);
			ASSERT(nItem != -1);
			TCHAR szBuffer[1024];
			DWORD cchBuf(1024);
			LVITEM pItem;
			pItem.iItem = nItem;
			pItem.iSubItem = 0;
			pItem.mask = LVIF_TEXT;
			pItem.pszText = szBuffer;
			pItem.cchTextMax = cchBuf;
			m_ListBox.GetItem(&pItem);
			CString name(szBuffer);
			if(AfxMessageBox(_T("Are you sure you want to remove the cursor file named ") + Utilities::ExtractFilename(name),MB_YESNO) == IDYES)
			{
				if(name.Mid(1,1) != _T(":")) // Full path
				{
					name = Variables::m_ProjectPath + name;
				}
				Variables::RemoveCursor(name);

				m_ListBox.DeleteItem(nItem);
				nItem = -1;
			}
	   }
	}
}

void CursorBar::OnDefault()
{
	CString path;
	if(!Utilities::ShowBrowseWindow(path, _T("Please select a folder to copy the cursor files to and press 'OK'. It is recommended that you use your project directory."), Variables::m_ProjectPath))
	{
		return;
	}

	// Arrow Cursor
	CString outputA = path + _T("Arrow.cur");
	HICON  hIconA = LoadCursor(NULL,IDC_ARROW);
	SaveIconToFile(hIconA, outputA);

	CString relPathA(Utilities::GetRelativePath(outputA));
	m_ListBox.InsertItem(m_ListBox.GetItemCount(), relPathA);
	Variables::AddCursor(relPathA);

	// Wait Cursor
	CString outputB = path + _T("Wait.cur");
	HICON  hIconB = LoadCursor(NULL,IDC_APPSTARTING);
	SaveIconToFile(hIconB, outputB);

	CString relPathB(Utilities::GetRelativePath(outputB));
	m_ListBox.InsertItem(m_ListBox.GetItemCount(), relPathB);
	Variables::AddCursor(relPathB);
}

HRESULT CursorBar::SaveIconToFile(HICON hIcon, LPCTSTR lpFileName)
{
   IPicture* pPicture = NULL;
   IStream*  pStream  = NULL;
   PICTDESC  pict;
   HGLOBAL   hGlobal  = NULL;
   HRESULT   hr, hrIni;
   HANDLE    hFile;
   PVOID     pData;
   DWORD     dwWritten;
   LONG      lSize;

   if((!hIcon) || (!lpFileName))
      return(E_INVALIDARG);

   pict.cbSizeofstruct = sizeof(pict);
   pict.picType = PICTYPE_ICON;
   pict.icon.hicon = hIcon;

   hrIni = OleInitialize(NULL);

   if(SUCCEEDED((hr = OleCreatePictureIndirect(&pict, IID_IPicture, 
                                FALSE, (void**)&pPicture))))
   {
      if(SUCCEEDED((hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream))))
      {
         if(SUCCEEDED((hr = pPicture->SaveAsFile(pStream, TRUE, &lSize))))
         {
            GetHGlobalFromStream(pStream, &hGlobal);
            pData = GlobalLock(hGlobal);

            if(INVALID_HANDLE_VALUE != (hFile = CreateFile(lpFileName, GENERIC_WRITE, 
                            0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)))
            {
               hr = (WriteFile(hFile, pData, lSize, &dwWritten, NULL)) ? S_OK : 
                     HRESULT_FROM_WIN32(GetLastError());
               CloseHandle(hFile);
            }
            else
            {
               hr = HRESULT_FROM_WIN32(GetLastError());
            }

            GlobalUnlock(hGlobal);
         }
         pStream->Release();
      }
      pPicture->Release();
   }

   if(SUCCEEDED(hrIni))
   OleUninitialize();

   return(hr);
}

int CursorBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create listbox
	const DWORD dwListViewStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_NOCOLUMNHEADER;
	
	if (!m_ListBox.Create (dwListViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create list view\n");
		return -1;      // fail to create
	}
	
	m_ListBox.SetView(LV_VIEW_LIST);

	m_ListBox.SetExtendedStyle(m_ListBox.GetExtendedStyle() | LVS_EX_INFOTIP);

	m_wndToolBar.Create (this, dwDefaultToolbarStyle, IDR_CURSOR);
	m_wndToolBar.LoadToolBar (IDR_CURSOR, 0, 0, TRUE /* Is locked */);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
		
	m_wndToolBar.SetBarStyle (
		m_wndToolBar.GetBarStyle () & 
			~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner (this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);


	OnChangeVisualStyle ();
	Clear();

	return 0;
}

//************************************************************************************
void CursorBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
								rectClient.Width (), cyTlb,
								SWP_NOACTIVATE | SWP_NOZORDER);
	m_ListBox.SetWindowPos (NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,
								rectClient.Width () - 2, rectClient.Height () - cyTlb - 2,
								SWP_NOACTIVATE | SWP_NOZORDER);
	
}

//************************************************************************************
void CursorBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CursorBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

void CursorBar::OnChangeVisualStyle ()
{
	m_wndToolBar.CleanUpLockedImages ();
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	m_wndToolBar.LoadBitmap (bIsHighColor ? 
		 IDB_CURSOR : IDR_CURSOR, 0, 0, TRUE /* Locked */);
}

