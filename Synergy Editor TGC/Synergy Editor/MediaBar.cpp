#include "stdafx.h"
#include "Synergy Editor.h"
#include "MediaBar.h"
#include "Utilities.h"
#include "Variables.h"
#include "ImageWindow.h"
#include "shlwapi.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar

BEGIN_MESSAGE_MAP(MediaBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_MEDIAOPEN, OnImport)
	ON_COMMAND(ID_MEDIADELETE, OnRemove)
	ON_COMMAND(ID_MEDIAPREVIEW, OnPreview)
	ON_UPDATE_COMMAND_UI(ID_MEDIADELETE, OnUpdateRemove)
	ON_UPDATE_COMMAND_UI(ID_MEDIAPREVIEW, OnUpdatePreview)
	ON_NOTIFY(LVN_ITEMCHANGED, 1, OnItemchangedLinksList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void MediaBar::OnContextMenu(CWnd*, CPoint){} // Supress

void MediaBar::OnItemchangedLinksList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	CString name = m_ListBox.GetItemText(pNMListView->iItem, 0);
	if(name.Mid(1,1) != _T(":")) // Full path
	{
		name = Variables::m_ProjectPath + name;
	}
	
	CString ext = name.Right(3);
	ext.MakeUpper();
	if(ext == "BMP" || ext == "JPG" || ext == "PNG" || ext == "GIF")
	{
		m_Filename = name;	
		m_BitmapView.SetFilename(name, true);
	}
	else
	{
		m_Filename = _T("");
	}
}

void MediaBar::OnUpdateRemove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetSelectedCount() > 0);
}

void MediaBar::OnUpdatePreview(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetSelectedCount() > 0 && m_Filename != _T(""));
}

CString MediaBarTitle = _T("Media");

void MediaBar::OnImport()
{
	TCHAR strFilter[] = { _T("All Files (*.*)|*.*||") };

	CFileDialog FileDlg(TRUE, _T(".*"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, strFilter);
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		POSITION pos = FileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString file = FileDlg.GetNextPathName(pos);

			m_DefaultPath = Utilities::ExtractPath(file);

			const int fileLocLength = m_DefaultPath.GetLength();
			const int projLocLength = Variables::m_ProjectPath.GetLength();
			bool error = false;
			if(fileLocLength < projLocLength || (fileLocLength >= projLocLength && _wcsnicmp(Variables::m_ProjectPath, m_DefaultPath, projLocLength) != 0))
			{
				error = true;
			}

			bool found = Variables::MediaExists(file) > -1;

			if(found)
			{
				AfxMessageBox(_T("The file ") + file + _T(" already exists in the project"), MB_ICONEXCLAMATION | MB_OK);
			}
			else
			{
				if(error)
				{
					int val = AfxMessageBox(_T("The media file is not under your project directory; subsequently it will not be stored in the final exe. Do you wish to copy the file to ") + Variables::m_ProjectPath + _T("media\\"), MB_ICONQUESTION | MB_YESNOCANCEL);
					if(val == IDCANCEL)
					{
						return;
					}
					else if(val == IDYES)
					{
						CString newLocation = Variables::m_ProjectPath + _T("media\\") + Utilities::ExtractFilename(file);
						if(!Utilities::CheckPathExists(Variables::m_ProjectPath + _T("media\\")) && !Utilities::EnsurePathExists(Variables::m_ProjectPath + _T("media\\")))
						{
							AfxMessageBox(_T("Could not create media directory, ensure that you have the correct permissions"), MB_ICONEXCLAMATION | MB_OK);
							return;
						}
						if(!Utilities::CheckFileExists(newLocation) && !CopyFile(file, newLocation, FALSE))
						{							
							AfxMessageBox(_T("Could not move the file, ensure that you have the correct permissions"), MB_ICONEXCLAMATION | MB_OK);
							return;
						}
						file = newLocation; // Update the file to the new location
					}
				}
				if(m_ListBox.GetItemCount() == 0 && Variables::m_DBPropertyBuildType != _T("media"))
				{
					if(AfxMessageBox(_T("Your project is not currently configured to allow media files to be added to the final exe, do you wish to change the Application Type to 'media'?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
					{
						MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
						pMainWnd->SetBuildTypeToMedia();
					}
				}

				CString path(Utilities::GetRelativePath(file));
				m_ListBox.InsertItem(m_ListBox.GetItemCount(), path);
				Variables::AddMedia(file);
			}
		}
	}
}

void MediaBar::OnClearAll()
{
	if(AfxMessageBox(_T("Are you sure you wish to remove all media entries?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		Variables::m_DBPropertyMedia.clear();
		Clear();
	}
}

void MediaBar::OnRemove()
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
			if(AfxMessageBox(_T("Are you sure you want to remove the media file named ") + Utilities::ExtractFilename(name), MB_YESNO) == IDYES)
			{
				if(name.Mid(1,1) != _T(":")) // Full path
				{
					name = Variables::m_ProjectPath + name;
				}
				Variables::RemoveMedia(name);

				m_ListBox.DeleteItem(nItem);

				if(m_Filename == name)
				{
					m_BitmapView.SetFilename(_T(""), true);
					m_Filename = "";
				}

				nItem = -1;
			}
	   }
	}
}

void MediaBar::Clear()
{
	m_ListBox.DeleteAllItems();
	m_BitmapView.SetFilename(_T(""), true);
}

void MediaBar::OnPreview()
{
	CRect rect(0, 0,500,500);
	ImageWindow* iw = new ImageWindow();
	iw->Create(_T("Image_Window"), _T("Image Preview"), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, rect);
	iw->SetPicture(m_Filename);
	iw->CenterWindow();
	iw->ShowWindow(SW_SHOW);
	iw->UpdateWindow();	
}

// Adds a new media file to the tree (relative path) and uses the full path to add to the list
void MediaBar::AddNewFile(CString file)
{
	CString path(Utilities::GetRelativePath(file));

	m_ListBox.InsertItem(m_ListBox.GetItemCount(), path);
	m_ListBox.EnsureVisible(m_ListBox.GetItemCount()-1, TRUE);
	Variables::AddMedia(file);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar construction/destruction

MediaBar::MediaBar()
{
	m_Filename = _T("");
	m_DefaultPath = Utilities::ExtractPath(Utilities::GetApplicationPath());
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar message handlers

int MediaBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	m_wndToolBar.Create (this, dwDefaultToolbarStyle, IDR_MEDIA);
	m_wndToolBar.LoadToolBar (IDR_MEDIA, 0, 0, TRUE /* Is locked */);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		
	m_wndToolBar.SetBarStyle (m_wndToolBar.GetBarStyle () & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner (this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);

	// Create preview window
	const DWORD bitmapViewStyle= WS_CHILD | WS_VISIBLE | WS_BORDER;
	m_BitmapView.Create(_T(""), bitmapViewStyle, rectDummy, this, 2);	

	OnChangeVisualStyle ();
	Clear();

	return 0;
}
//************************************************************************************
void MediaBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy + 1;

	int commentsHeight = 150;
	int commentsWidth = rectClient.Width() - 2;
	
	int listHeight = rectClient.Height () - (commentsHeight + 1) - cyTlb;
	int listWidth = rectClient.Width () - 2;

	int commentsX = 1;
	int commentsY = listHeight + cyTlb;

	if(rectClient.Height() < 200)
	{
		if(rectClient.Width() < 150)
		{
			commentsHeight = 0;

			listHeight = rectClient.Height () - cyTlb + 1;
		}
		else
		{
			listWidth = rectClient.Width() - 150; // Doesn't touch the edge - 150
			listHeight = rectClient.Height() - cyTlb;

			commentsX = rectClient.Width() - 149;
			commentsY = cyTlb;
			commentsWidth = 148;
			commentsHeight = rectClient.Height() - cyTlb;
		}
	}	

	m_wndToolBar.SetWindowPos (NULL, 1, rectClient.top, rectClient.Width () - 2, cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);

	m_ListBox.SetWindowPos (NULL, 1, cyTlb, listWidth, listHeight, SWP_NOACTIVATE | SWP_NOZORDER);

	m_BitmapView.SetWindowPos (NULL, commentsX, commentsY, commentsWidth, commentsHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	
}
//************************************************************************************
void MediaBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	AdjustLayout();
}

void MediaBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));

	m_BitmapView.Invalidate();
}

void MediaBar::OnChangeVisualStyle ()
{
	m_wndToolBar.CleanUpLockedImages ();
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	m_wndToolBar.LoadBitmap (bIsHighColor ? IDB_MEDIA : IDR_MEDIA, 0, 0, TRUE /* Locked */);
}
