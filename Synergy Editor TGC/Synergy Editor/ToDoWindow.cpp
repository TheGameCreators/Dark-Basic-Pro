#include "StdAfx.h"
#include "ToDoWindow.h"
#include "InputDialog.h"
#include "Variables.h"
#include "Resource.h"
#include <vector>

BEGIN_MESSAGE_MAP(ToDoWindow, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(ToDoWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(ID_POPUP_INSERTITEM, AddItem)
	ON_COMMAND(ID_POPUP_REMOVEITEM, RemoveItem)
	ON_NOTIFY(LVN_ITEMCHANGED, 1, OnItemchangedLinksList)
	ON_NOTIFY( LVN_GETINFOTIP, 1, OnInfoTip )
	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_TODO_ADD, OnAdd)
	ON_COMMAND(ID_TODO_REMOVE, OnRemove)
	ON_COMMAND(ID_TODO_UP, OnUp)
	ON_COMMAND(ID_TODO_DOWN, OnDown)
	ON_COMMAND(ID_TODO_EDIT, OnEdit)
	ON_UPDATE_COMMAND_UI(ID_TODO_REMOVE, OnUpdateRemove)
	ON_UPDATE_COMMAND_UI(ID_TODO_UP, OnUpdateUp)
	ON_UPDATE_COMMAND_UI(ID_TODO_DOWN, OnUpdateDown)
	ON_UPDATE_COMMAND_UI(ID_TODO_EDIT, OnUpdateEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const int nBorderSize = 1;

void ToDoWindow::OnContextMenu(CWnd*, CPoint){} // Supress

void ToDoWindow::OnAdd()
{
	AddItem();
}

void ToDoWindow::OnRemove()
{
	RemoveItem();
}

void ToDoWindow::OnUp()
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
			CString temp(szBuffer);
			BOOL checked = ListView_GetCheckState(m_ListBox.GetSafeHwnd(), nItem);
			m_ListBox.DeleteItem(nItem);
			Variables::MoveToDoUp(temp);						
			m_ListBox.InsertItem(nItem - 1, temp);
			m_ListBox.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
			if(checked)
			{
				ListView_SetCheckState(m_ListBox.GetSafeHwnd(), nItem - 1, TRUE);
			}
		}
		m_ListBox.SetFocus();
	}	
}

void ToDoWindow::OnDown()
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
			CString temp(szBuffer);
			BOOL checked = ListView_GetCheckState(m_ListBox.GetSafeHwnd(), nItem);
			m_ListBox.DeleteItem(nItem);
			Variables::MoveToDoDown(temp);						
			m_ListBox.InsertItem(nItem + 1, temp);
			m_ListBox.SetItemState(nItem + 1, LVIS_SELECTED, LVIS_SELECTED);
			if(checked)
			{
				ListView_SetCheckState(m_ListBox.GetSafeHwnd(), nItem + 1, TRUE);
			}
		}
		m_ListBox.SetFocus();
	}	
}

void ToDoWindow::OnUpdateUp(CCmdUI *pCmdUI)
{
	BOOL res = TRUE;	
	UINT i, uSelectedCount = m_ListBox.GetSelectedCount();
	int  nItem = -1;

	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
	   for (i=0;i < uSelectedCount;i++)
	   {
			nItem = m_ListBox.GetNextItem(nItem, LVNI_SELECTED);
			if(nItem < 1)
			{
				res = FALSE;
				break;
			}
	   }
	}
	else
	{
		res = FALSE;
	}

	pCmdUI->Enable(res);
}

void ToDoWindow::OnUpdateDown(CCmdUI *pCmdUI)
{
	BOOL res = TRUE;	
	UINT i, uSelectedCount = m_ListBox.GetSelectedCount();
	int  nItem = -1;

	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
	   for (i=0;i < uSelectedCount;i++)
	   {
			nItem = m_ListBox.GetNextItem(nItem, LVNI_SELECTED);
			if(nItem >= m_ListBox.GetItemCount() - 1)
			{
				res = FALSE;
				break;
			}
	   }
	}
	else
	{
		res = FALSE;
	}

	pCmdUI->Enable(res);
}

void ToDoWindow::OnUpdateRemove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetSelectedCount() > 0);
}

void ToDoWindow::AddItem()
{
	InputDialog dlg(_T("Please enter the to do text"));
	dlg.DoModal();
	CString retText = dlg.GetText();
	if(retText != _T(""))
	{
		m_ListBox.InsertItem(m_ListBox.GetItemCount(), retText);
		Variables::AddToDo(_T("0") + retText);
	}
}

void ToDoWindow::RemoveItem()
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
			CString temp(szBuffer);
			Variables::RemoveToDo(temp);
			m_ListBox.DeleteItem(nItem);
			nItem = -1;
	   }
	}	
}

// Clears the to do list
void ToDoWindow::Clear()
{
	m_ListBox.DeleteAllItems();
}

// Sets the comment text box with preformated text (multiline)
void ToDoWindow::AddComment(CString text)
{
	m_Comments.SetWindowText(text);
}

// Clears the comments box
void ToDoWindow::ClearComments()
{
	m_Comments.SetWindowTextW(_T(""));
}

// Writes the contents of the comments box to the vectors
void ToDoWindow::DumpComments()
{
	Variables::ClearComment();
	int count = m_Comments.GetLineCount();
	for(int i=0;i<count;i++)
	{
		int len = m_Comments.LineLength(m_Comments.LineIndex(i));
		CString buffer;
		m_Comments.GetLine(i, buffer.GetBuffer(len), len);
		buffer.ReleaseBuffer(len);
		Variables::AddComment(buffer);
	}
}

void ToDoWindow::AddItem(CString item, BOOL checked)
{
	m_ListBox.InsertItem(m_ListBox.GetItemCount(), item);
	ListView_SetCheckState(m_ListBox.GetSafeHwnd(), m_ListBox.GetItemCount(), checked);
	m_ListBox.EnsureVisible(m_ListBox.GetItemCount()-1, TRUE);
	CString newItem = checked ? _T("1") : _T("0");
	Variables::AddToDo(newItem + item);
}

void ToDoWindow::OnEdit()
{
	UINT i, uSelectedCount = m_ListBox.GetSelectedCount();
	int  nItem = -1;
	BOOL checked = false;

	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
	   for (i=0;i < uSelectedCount;i++)
	   {
			nItem = m_ListBox.GetNextItem(nItem, LVNI_SELECTED);
			CString oldText(m_ListBox.GetItemText(nItem, 0));
			checked = ListView_GetCheckState(m_ListBox.GetSafeHwnd(), nItem);
			InputDialog dlg(_T("Please enter the to do text"), oldText);
			dlg.DoModal();
			CString retText = dlg.GetText();
			if(retText != _T(""))
			{
				m_ListBox.DeleteItem(nItem);
				m_ListBox.InsertItem(nItem, retText);
				ListView_SetCheckState(m_ListBox.GetSafeHwnd(), nItem, checked);
				Variables::RenameToDo(oldText, retText);
			}
		}
	}
}

void ToDoWindow::OnUpdateEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListBox.GetSelectedCount() > 0);
}

int ToDoWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create listbox
	const DWORD dwViewStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_NOCOLUMNHEADER;
	
	if (!m_ListBox.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create list view\n");
		return -1;      // fail to create
	}

	m_ListBox.SetView(LV_VIEW_LIST);

	m_ListBox.SetExtendedStyle(m_ListBox.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_INFOTIP);

	m_Comments.Create(ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER, rectDummy, this, 2);

	LOGFONT logfont;
	memset (&logfont, 0, sizeof (LOGFONT));
	logfont.lfHeight = 8;
	lstrcpy (logfont.lfFaceName, _T ("MS Sans Serif"));

	m_lbFont.CreateFontIndirect (&logfont);
	m_Comments.SetFont(&m_lbFont);

	m_wndToolBar.Create (this, dwDefaultToolbarStyle, IDR_TODO);
	m_wndToolBar.LoadToolBar (IDR_TODO, 0, 0, TRUE /* Is locked */);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		
	m_wndToolBar.SetBarStyle (m_wndToolBar.GetBarStyle () & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner (this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);

	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
		m_ToolTip.AddTool( &m_Comments, _T("Enter project comments"));
		m_ToolTip.Activate(TRUE);
	}
	
	OnChangeVisualStyle ();
	AdjustLayout ();
	return 0;
}

void  ToDoWindow::OnInfoTip( NMHDR * pNMHDR, LRESULT * pResult )
{
	NMLVGETINFOTIP* pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip);

	wcscpy_s(pInfoTip->pszText, MAX_PATH, m_ListBox.GetItemText(pInfoTip->iItem, 0));
}

void ToDoWindow::OnItemchangedLinksList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
		return;	// No change

	BOOL bPrevState = (BOOL)(((pNMListView->uOldState &
				LVIS_STATEIMAGEMASK)>>12)-1);   // Old check box state
	if (bPrevState < 0)	// On startup there's no previous state 
		bPrevState = 0; // so assign as false (unchecked)

	// New check box state
	BOOL bChecked=(BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12)-1);
	if (bChecked < 0) // On non-checkbox notifications assume false
		bChecked = 0;

	if (bPrevState == bChecked) // No change in check box
		return;

	CString text = m_ListBox.GetItemText(pNMListView->iItem, 0);
	Variables::ChangeToDo(text, bChecked);
}

void ToDoWindow::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	AdjustLayout ();
}

void ToDoWindow::AdjustLayout ()
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

	m_Comments.SetWindowPos (NULL, commentsX, commentsY, commentsWidth, commentsHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}

void ToDoWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_ListBox.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void ToDoWindow::OnChangeVisualStyle ()
{
	m_wndToolBar.CleanUpLockedImages ();
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	m_wndToolBar.LoadBitmap (bIsHighColor ? IDB_TODO_24 : IDB_TODO, 0, 0, TRUE /* Locked */);
}
