#include "stdafx.h"
#include "Synergy Editor.h"
#include "WorkSpaceBar.h"
#include "Settings.h"
#include "Utilities.h"
#include "Variables.h"
#include "View.h"

const int nBorderSize = 1;
/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar

BEGIN_MESSAGE_MAP(CWorkSpaceBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(NM_CLICK, 1, OnClick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY( TVN_SELCHANGED, 1, OnSelchangedTreectrl)
	ON_COMMAND(ID_REMOVE_FROM_PROJECT, OnRemove)
	ON_COMMAND(ID_POPUP_ADDNEWFILE, OnAddNewFile)
	ON_COMMAND(ID_POPUP_IMPORTEXISTINGFILE, OnImportFile)
	ON_COMMAND(ID_OPEN_ALL_FILES, OnOpenAllFiles)
	ON_COMMAND(ID_IDR_MAINMAIN, OnSetMain)
	ON_COMMAND(ID_IDR_OPENPROJECTFOLDER, OnOpenFolder)
	ON_COMMAND(ID_IDR_OPENCONTAININGFOLDER, OnOpenFileFolder)
	ON_COMMAND(ID_POPUP_LOCATEFILE, OnLocateFile)
	ON_COMMAND(ID_IDR_PROPERTIES, OnProperties)

	ON_UPDATE_COMMAND_UI(ID_IDR_PROPERTIES, OnPropertiesUpdate)
	ON_UPDATE_COMMAND_UI(ID_IDR_OPENCONTAININGFOLDER, OnOpenFileFolderUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString SolutionTitle = _T("Project");

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar construction/destruction

CWorkSpaceBar::CWorkSpaceBar()
{	
	mode = 0;
	hBold = NULL;
	m_DefaultPath = Utilities::ExtractPath(Utilities::GetApplicationPath());
}

void CWorkSpaceBar::OnImportFile()
{
	TCHAR strFilter[] = { _T("DarkBasic Source Files (*.dba)|*.dba||") };

	CFileDialog FileDlg(TRUE, _T(".*"), NULL, 0, strFilter);
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		m_DefaultPath = Utilities::ExtractPath(FileDlg.GetPathName());
		AfxGetApp()->OpenDocumentFile(FileDlg.GetPathName());
	}
}

void CWorkSpaceBar::Expand()
{
	m_wndTree.Expand(hRoot1, TVE_EXPAND);
}

void CWorkSpaceBar::OnAddNewFile()
{
	GetParentFrame()->SendMessage(WM_COMMAND, ID_FILE_NEWDBA);
}

// Called when the location of the project is changed as the displaying of the files needs to update to reflect their new locations
void CWorkSpaceBar::UpdatePath(CString path)
{
	m_DefaultPath = path;

	HTREEITEM item = m_wndTree.GetNextItem(hRoot1, TVGN_CHILD);
	HTREEITEM delItem;
	CString itemText;
	while(item != NULL)
	{
		itemText = m_wndTree.GetItemText(item);
		if(Utilities::ExtractPath(itemText) == Variables::m_ProjectPath)
		{
			delItem = item;
			item = m_wndTree.GetNextItem(item, TVGN_NEXT);
			m_wndTree.DeleteItem(delItem);
			Variables::UpdateInclude(Variables::IncludeExists(itemText), Utilities::ExtractFilename(itemText)); // Do this first so the file is made bold
			AddNewFile(Utilities::ExtractFilename(itemText));			
		}
		else
		{
			item = m_wndTree.GetNextItem(item, TVGN_NEXT);
		}
	}
}

// The selected item has been made the main file (make bold)
void CWorkSpaceBar::OnSetMain()
{
	HTREEITEM item = m_wndTree.GetSelectedItem();
	CString name = m_wndTree.GetItemText(item);
	if(name == SolutionTitle)
	{
		return;
	}

	m_wndTree.SetItemState(item, TVIS_BOLD, TVIS_BOLD );
	if(hBold != NULL && hBold != item)
	{
		m_wndTree.SetItemState(hBold, 0, TVIS_BOLD );
	}
	hBold = item;

	if(name.Mid(1,1) != _T(":")) // Full path
	{
		name = Variables::m_ProjectPath + name;
	}

	Variables::MakeIncludeMain(name);
}

void CWorkSpaceBar::OnRemove()
{
	HTREEITEM item = m_wndTree.GetSelectedItem();
	CString name = m_wndTree.GetItemText(item);
	if(name == SolutionTitle)
	{
		return;
	}

	if(AfxMessageBox(_T("Are you sure you want to remove the source file named ") + Utilities::ExtractFilename(name),MB_YESNO) == IDNO)
	{
		return;
	}

	App* pApp = (App*)AfxGetApp();
	ASSERT_VALID(pApp);
	CDocTemplate* pTemplate = pApp->pDocTemplate;
	ASSERT_VALID(pTemplate);

	POSITION posDocument = pTemplate->GetFirstDocPosition();

	bool found = false;

	while(posDocument != NULL)
	{
		CDocument* pDoc = pTemplate->GetNextDoc(posDocument);
		ASSERT_VALID(pDoc);
		if(Utilities::RemoveModifiedFlag(pDoc->GetTitle()) == Utilities::ExtractFilename(name))
		{
			pDoc->OnCloseDocument();
		}
	}

	if(name.Mid(1,1) != _T(":")) // Full path
	{
		name = Variables::m_ProjectPath + name;
	}

	Variables::RemoveInclude(name);

	if(item == hBold)
	{
		HighlightNewMain();
	}
	m_wndTree.DeleteItem(item);
}

void CWorkSpaceBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;	
	m_wndTree.ScreenToClient(&pt) ;

	UINT unFlags = 0 ;
	HTREEITEM hItem = m_wndTree.HitTest(pt, &unFlags) ;
	if(!((unFlags & TVHT_ONITEMLABEL) && hItem != NULL))
	{
		return;
	}

	m_wndTree.Select(hItem, TVGN_CARET);

	if(point.x == -1 && point.x == -1)
	{
		CRect rect;
		GetClientRect(&rect);
		point = rect.TopLeft();
		point.Offset(5,5);
		ClientToScreen(&point);
	}

	// Find out if the image is that of a missing file
	int nImage;
	int nSelectedImage;
	m_wndTree.GetItemImage(hItem,nImage,nSelectedImage);
	
	CMenu mnuTop;
	if(m_wndTree.GetItemText(hItem) == SolutionTitle)
	{
		mnuTop.LoadMenu(IDR_SOLUTION_MENU);
	}
	else if(nImage == 3) // Missing file
	{
		mnuTop.LoadMenu(IDR_POPUP_SOLUTION_MISSING);
	}
	else
	{
		mnuTop.LoadMenu(IDR_POPUP_SOLUTION);
	}

	CMenu *pPopup = mnuTop.GetSubMenu(0);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this, NULL);
}

void CWorkSpaceBar::OnPropertiesUpdate(CCmdUI* pCmdUI)
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	int nImage;
	int nSelectedImage;
	m_wndTree.GetItemImage(hItem,nImage,nSelectedImage);
	pCmdUI->Enable(nImage != 3);
}

void CWorkSpaceBar::OnOpenFileFolderUpdate(CCmdUI* pCmdUI)
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	int nImage;
	int nSelectedImage;
	m_wndTree.GetItemImage(hItem,nImage,nSelectedImage);
	pCmdUI->Enable(nImage != 3);
}

void CWorkSpaceBar::Clear()
{
	m_wndTree.DeleteAllItems();
	hRoot1 = m_wndTree.InsertItem (SolutionTitle, 0);
	hBold = NULL;
}

void CWorkSpaceBar::OnClick( NMHDR * pNotifyStruct, LRESULT * result )
{
	CPoint pt ;
	GetCursorPos(&pt) ;
	m_wndTree.ScreenToClient(&pt) ;

	UINT unFlags = 0 ;
	HTREEITEM hItem = m_wndTree.HitTest(pt, &unFlags) ;
	if((unFlags & TVHT_ONITEMLABEL) && hItem != NULL)
	{
		CString name = m_wndTree.GetItemText(hItem);
		if(name == SolutionTitle)
		{
			return;
		}

		// Find out if the image is that of a missing file
		int nImage;
		int nSelectedImage;
		m_wndTree.GetItemImage(hItem,nImage,nSelectedImage);

		if(nImage != 3)
		{
			OpenInclude(name);
		}
	}
}

// Menu item to open all includes
void CWorkSpaceBar::OnOpenAllFiles()
{
	std::vector <SourceFile *>::iterator SFIter;
	for ( SFIter = Variables::m_IncludeFiles.begin( ) ; SFIter != Variables::m_IncludeFiles.end( ) ; SFIter++ )
	{
		OpenInclude((*SFIter)->filename);
	}
}

// Opens an include if it's not already open
void CWorkSpaceBar::OpenInclude(CString name)
{
	App* app = (App*)AfxGetApp();
	ASSERT_VALID(app);

	POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

	bool found = false;

	while(posDocument != NULL)
	{
		CDocument* pDoc = app->pDocTemplate->GetNextDoc(posDocument);
		ASSERT_VALID(pDoc);

		CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
		CString titleUpper = title;
		titleUpper.MakeUpper();
		if(title.Right(3) != _T("DBA"))
		{
			title.Append(_T(".dba"));
		}
		if(title == Utilities::ExtractFilename(name))
		{
			found = true;
			POSITION pos = pDoc->GetFirstViewPosition();
			View* view = (View*)pDoc->GetNextView(pos);	
			ASSERT_VALID(view);

			MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
			ASSERT_VALID(pMainWnd);
			pMainWnd->MDIActivate(view->GetParent());
			break;
		}
	}

	if(!found)
	{
		CString path(name);

		if(name.Mid(1,1) != _T(":")) // Full path
		{
			path = Variables::m_ProjectPath + name;
		}

		CFileStatus file;
		if(CFile::GetStatus(path,file))
		{
			AfxGetApp()->OpenDocumentFile(path);
			if(Settings::GoToRememberedLine) Utilities::GoToLine(path);
		}
		else
		{
			if (AfxMessageBox(_T("The file ") + path + _T(" does not exist, do you wish to remove it from your project?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{				
				Variables::RemoveInclude(name);
				DeleteOldFile(name);
			}
		}
	}
}

// Called when the user is trying to locate a missing file
void CWorkSpaceBar::OnLocateFile()
{
	TCHAR strFilter[] = { _T("DarkBasic Source Files (*.dba)|*.dba||") };

	CFileDialog FileDlg(TRUE, _T(".*"), NULL, 0, strFilter);
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		CString path = FileDlg.GetPathName();
		m_wndTree.DeleteItem(m_wndTree.GetSelectedItem());
		m_DefaultPath = Utilities::ExtractPath(path);
		AfxGetApp()->OpenDocumentFile(path);
		// Files part of the project will not automatically be readded to the solution explorer, so do it now
		if(Variables::IncludeExists(Utilities::GetRelativePath(path)) > -1)
		{
			AddNewFile(Utilities::GetRelativePath(path));
		}
	}
}

// Adds a new file to the tree, if it doesn't exist on the hardrive it gets a warning icon
void CWorkSpaceBar::AddNewFile(CString file)
{
	CString path(Utilities::GetRelativePath(file));

	// Make sure it doesn't already exist
	HTREEITEM findItem = m_wndTree.GetNextItem(hRoot1, TVGN_CHILD);
	while(findItem != NULL)
	{
		CString itemText = Utilities::GetRelativePath(m_wndTree.GetItemText(findItem));
		if(itemText == path)
		{
			return;
		}
		findItem = m_wndTree.GetNextItem(findItem, TVGN_NEXT);
	}

	HTREEITEM item;
	CFileStatus status;
	if(CFile::GetStatus(file,status))
	{
		item = m_wndTree.InsertItem (path, 2, 2, hRoot1);
	}
	else
	{
		item = m_wndTree.InsertItem (path, 3, 3, hRoot1);		
	}
	if(Variables::IncludeExists(file) == 0)
	{
		if(hBold != NULL)
		{			
			m_wndTree.SetItemState(hBold, 0, TVIS_BOLD);
		}
		m_wndTree.SetItemState(item, TVIS_BOLD, TVIS_BOLD);
		hBold = item;
	}
}

// Removes the specified file from the tree, automatically handles relative path conversion
void CWorkSpaceBar::DeleteOldFile(CString file)
{
	file = Utilities::GetRelativePath(file); // Convert full name into one that is stored

	HTREEITEM item = m_wndTree.GetNextItem(hRoot1, TVGN_CHILD);
	CString itemText = Utilities::GetRelativePath(m_wndTree.GetItemText(item));
	while(item != NULL && itemText != file)
	{
		item = m_wndTree.GetNextItem(item, TVGN_NEXT);
		if(item != NULL)
		{
			itemText = Utilities::GetRelativePath(m_wndTree.GetItemText(item));
		}
	}

	if(item != NULL)
	{
		if(item == hBold)
		{
			HighlightNewMain();
		}
		m_wndTree.DeleteItem(item);
	}
}

// Called when the main file needs to be highlighted
void CWorkSpaceBar::HighlightNewMain()
{
	if(Variables::m_IncludeFiles.size() == 0)
	{
		hBold = NULL;
		return;
	}

	CString file = Utilities::GetRelativePath(Variables::m_IncludeFiles.at(0)->filename); // Convert full name into one that is stored

	HTREEITEM item = m_wndTree.GetNextItem(hRoot1, TVGN_CHILD);
	CString itemText = Utilities::GetRelativePath(m_wndTree.GetItemText(item));
	while(item != NULL && itemText != file)
	{
		item = m_wndTree.GetNextItem(item, TVGN_NEXT);
		if(item != NULL)
		{
			itemText = Utilities::GetRelativePath(m_wndTree.GetItemText(item));
		}
	}

	if(item != NULL)
	{
		m_wndTree.SetItemState(item, TVIS_BOLD, TVIS_BOLD );
		hBold = item;
	}
}

// Open project folder
void CWorkSpaceBar::OnOpenFolder()
{
	::ShellExecute (NULL, NULL, Variables::m_ProjectPath, NULL, NULL, SW_SHOWNORMAL);
}

// Open file folder
void CWorkSpaceBar::OnOpenFileFolder()
{
	HTREEITEM item = m_wndTree.GetSelectedItem();
	CString name = m_wndTree.GetItemText(item);
	if(name.Mid(1,1) == _T(":")) // Full path
	{
		::ShellExecute (NULL, NULL, Utilities::ExtractPath(name), NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		::ShellExecute (NULL, NULL, Utilities::ExtractPath(Variables::m_ProjectPath + name), NULL, NULL, SW_SHOWNORMAL);
	}	
}

// Opens the file properties window
void CWorkSpaceBar::OnProperties()
{
	HTREEITEM item = m_wndTree.GetSelectedItem();
	CString name = m_wndTree.GetItemText(item);
	
	SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_INVOKEIDLIST;
	sei.hwnd = this->GetSafeHwnd();
    sei.lpVerb = _T("properties");
     
	TCHAR* text;

	if(name.Mid(1,1) == _T(":")) // Full path
	{
		int size = name.GetLength() + 1;
		text = new TCHAR[size];
		wcscpy_s(text, size, name);
	}
	else
	{
		CString name2 = Variables::m_ProjectPath + name;		
		int size = name2.GetLength() + 1;
		text = new TCHAR[size];
		wcscpy_s(text, size, name2);
	}
	sei.lpFile = text;
	ShellExecuteEx(&sei);
	delete text;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar message handlers

int CWorkSpaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TreeImages.Create (IDB_TREE, 16, 0, RGB (255, 0, 255));

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tool bar
	m_wndToolBar1.Create (this, dwDefaultToolbarStyle, IDR_SOLUTION);
	m_wndToolBar1.LoadToolBar (IDR_SOLUTION, 0, 0, TRUE /* Is locked */);
	m_wndToolBar1.SetBarStyle(m_wndToolBar1.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);		
	m_wndToolBar1.SetBarStyle (m_wndToolBar1.GetBarStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar1.SetOwner (this);	
	m_wndToolBar1.SetRouteCommandsViaFrame (FALSE); // All commands will be routed via this control , not via the parent frame:

	// Create tool bar
	m_wndToolBar2.Create (this, dwDefaultToolbarStyle, IDR_SOURCEFILE);
	m_wndToolBar2.LoadToolBar (IDR_SOURCEFILE, 0, 0, TRUE /* Is locked */);
	m_wndToolBar2.SetBarStyle(m_wndToolBar2.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);		
	m_wndToolBar2.SetBarStyle (m_wndToolBar2.GetBarStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar2.SetOwner (this);	
	m_wndToolBar2.SetRouteCommandsViaFrame (FALSE); // All commands will be routed via this control , not via the parent frame:
	m_wndToolBar2.ShowWindow(SW_HIDE);

	// Create tree windows.
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	
	if (!m_wndTree.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	m_wndTree.SetImageList(&m_TreeImages, TVSIL_NORMAL);

	OnChangeVisualStyle ();

	Clear();

	return 0;
}

void CWorkSpaceBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect (rectClient);

	int cyTlb = m_wndToolBar1.CalcFixedLayout (FALSE, TRUE).cy;

	m_wndToolBar1.SetWindowPos (NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndToolBar2.SetWindowPos (NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndTree.SetWindowPos (NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height () - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CWorkSpaceBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	GetClientRect(rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void CWorkSpaceBar::OnSelchangedTreectrl(NMHDR* pNMHDR, LRESULT* pResult)
{
     NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

     HTREEITEM hItem = m_wndTree.GetSelectedItem();
     CString strItemText = m_wndTree.GetItemText(hItem);

	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	 if(strItemText == SolutionTitle)
	 {
		if(mode != 0)
		{
			m_wndToolBar2.ShowWindow(SW_HIDE);
			m_wndToolBar1.ShowWindow(SW_SHOW);
			mode = 0;
		}	
	 }
	 else
	 {
		if(mode != 1)
		{
			m_wndToolBar1.ShowWindow(SW_HIDE);
			m_wndToolBar2.ShowWindow(SW_SHOW);
			mode = 1;
		}
	 }

    *pResult = 0;
}

void CWorkSpaceBar::OnChangeVisualStyle ()
{
	m_TreeImages.DeleteImageList ();

	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiBmpId = bIsHighColor ? IDB_TREE24 : IDB_TREE;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT (FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (bIsHighColor) ? ILC_COLOR24 : ILC_COLOR4;

	m_TreeImages.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
	m_TreeImages.Add (&bmp, RGB (255, 0, 255));

	m_wndTree.SetImageList (&m_TreeImages, TVSIL_NORMAL);

	m_wndToolBar1.CleanUpLockedImages ();
	m_wndToolBar1.LoadBitmap (bIsHighColor ? IDB_SOLUTION : IDR_SOLUTION, 0, 0, TRUE /* Locked */);

	m_wndToolBar2.CleanUpLockedImages ();
	m_wndToolBar2.LoadBitmap (bIsHighColor ? IDB_SOURCEFILE : IDR_SOURCEFILE, 0, 0, TRUE /* Locked */);
}

