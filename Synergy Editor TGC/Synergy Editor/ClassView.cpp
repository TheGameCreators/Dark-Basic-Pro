#include "StdAfx.h"
#include "ClassView.h"
#include "Resource.h"
#include "MainFrm.h"
#include "View.h"

#include <algorithm>
#include "Settings.h"

const int nBorderSize = 1;

ClassView::ClassView(void)
{
	treeTitle = _T("Project");
	functionsTitle = _T("Functions");
	typesTitle = _T("Types");
	labelsTitle = _T("Labels");
	variablesTitle = _T("Variables");
	errorsTitle = _T("Errors");
	constantsTitle = _T("Constants");
}

int SortByValFunction(const SemanticFunction* t1, const SemanticFunction* t2 )
{
	return (t1->function < t2->function);
}

int SortByValType(const SemanticType* t1, const SemanticType* t2 )
{
	return (t1->type < t2->type);
}

int SortByValVariable(const SemanticVariable* t1, const SemanticVariable* t2 )
{
	return (t1->variable < t2->variable);
}

int SortByValLabel(const SemanticLabel* t1, const SemanticLabel* t2 )
{
	return (t1->label < t2->label);
}

BEGIN_MESSAGE_MAP(ClassView, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()

	ON_NOTIFY(NM_DBLCLK, 1, OnClick)
	ON_MESSAGE(WM_USER, OnAddText)
	ON_MESSAGE(WM_USER+1, OnClear)
	ON_MESSAGE(WM_USER+2, OnErrorMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ClassView::OnContextMenu(CWnd* pWnd, CPoint point){} // Supress

void ClassView::ClearExisting()
{
	HTREEITEM hNextItem;

	// Errors
	HTREEITEM hChildItem = m_wndTree.GetChildItem(hErrors);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }

   // Functions
   hChildItem = m_wndTree.GetChildItem(hFunctions);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }

   // Types
    hChildItem = m_wndTree.GetChildItem(hTypes);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }

   // Labels
    hChildItem = m_wndTree.GetChildItem(hLabels);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }

   // Variables
   hChildItem = m_wndTree.GetChildItem(hVariables);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }

    // Constants
   hChildItem = m_wndTree.GetChildItem(hConstants);

   while (hChildItem != NULL)
   {
      hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
      m_wndTree.DeleteItem(hChildItem);
      hChildItem = hNextItem;
   }
}

LRESULT ClassView::OnClear(WPARAM a, LPARAM b)
{
	ClearExisting();
	return TRUE;
}

LRESULT ClassView::OnErrorMessage(WPARAM a, LPARAM b)
{
	TCHAR* message = (TCHAR*)b;
	AfxMessageBox(message, MB_OK | MB_ICONINFORMATION);
	delete message;
	return TRUE;
}

LRESULT ClassView::OnAddText(WPARAM a, LPARAM b)
{	
	HTREEITEM selectedTree = m_wndTree.GetSelectedItem();
	HTREEITEM newItem;

	CString selected = m_wndTree.GetItemText(selectedTree);
	ClearExisting();

	if(!SemanticParser::m_Mutex.Lock(10))
	{
		return TRUE;
	}

	if(Settings::CodeViewOrder == 1)
	{
		std::sort(SemanticParser::m_vFunctions.begin(), SemanticParser::m_vFunctions.end(), SortByValFunction);
		std::sort(SemanticParser::m_vTypes.begin(), SemanticParser::m_vTypes.end(), SortByValType);
		std::sort(SemanticParser::m_vLabels.begin(), SemanticParser::m_vLabels.end(), SortByValLabel);
		std::sort(SemanticParser::m_vVariables.begin(), SemanticParser::m_vVariables.end(), SortByValVariable);
		std::sort(SemanticParser::m_vConstants.begin(), SemanticParser::m_vConstants.end(), SortByValVariable);
	}

	for(mySEVectorIterator = SemanticParser::m_vMessages.begin(); mySEVectorIterator != SemanticParser::m_vMessages.end(); mySEVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySEVectorIterator)->message, 3, 3, hErrors);
		if((*mySEVectorIterator)->message == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}
	for(mySFVectorIterator = SemanticParser::m_vFunctions.begin(); mySFVectorIterator != SemanticParser::m_vFunctions.end(); mySFVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySFVectorIterator)->definition, 5, 5, hFunctions);
		if((*mySFVectorIterator)->definition == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}
	for(mySTVectorIterator = SemanticParser::m_vTypes.begin(); mySTVectorIterator != SemanticParser::m_vTypes.end(); mySTVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySTVectorIterator)->type, 5, 5, hTypes);
		if((*mySTVectorIterator)->type == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}
	for(mySLVectorIterator = SemanticParser::m_vLabels.begin(); mySLVectorIterator != SemanticParser::m_vLabels.end(); mySLVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySLVectorIterator)->label, 5, 5, hLabels);
		if((*mySLVectorIterator)->label == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}
	for(mySVVectorIterator = SemanticParser::m_vVariables.begin(); mySVVectorIterator != SemanticParser::m_vVariables.end(); mySVVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySVVectorIterator)->variable, 5, 5, hVariables);
		if((*mySVVectorIterator)->variable == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}
	for(mySCVectorIterator = SemanticParser::m_vConstants.begin(); mySCVectorIterator != SemanticParser::m_vConstants.end(); mySCVectorIterator++)
	{
		newItem = m_wndTree.InsertItem ((*mySCVectorIterator)->variable, 5, 5, hConstants);
		if((*mySCVectorIterator)->variable == selected)
		{
			m_wndTree.SelectItem(newItem);
		}
	}

	SemanticParser::m_Mutex.Unlock();
	return TRUE;
}

void ClassView::Clear()
{
	m_wndTree.DeleteAllItems();
	hRoot1 = m_wndTree.InsertItem (treeTitle, 0);
	hFunctions = m_wndTree.InsertItem (functionsTitle, 2, 2, hRoot1);
	hTypes = m_wndTree.InsertItem (typesTitle, 2, 2, hRoot1);
	hLabels = m_wndTree.InsertItem (labelsTitle, 2, 2, hRoot1);
	hVariables = m_wndTree.InsertItem (variablesTitle, 2, 2, hRoot1);
	hErrors = m_wndTree.InsertItem (errorsTitle, 2, 2, hRoot1);
	hConstants = m_wndTree.InsertItem (constantsTitle, 2, 2, hRoot1);
	
	m_wndTree.Expand(hRoot1,TVE_EXPAND);
	m_wndTree.Expand(hFunctions,TVE_EXPAND);
	m_wndTree.Expand(hTypes,TVE_EXPAND);
	m_wndTree.Expand(hLabels,TVE_EXPAND);
	m_wndTree.Expand(hVariables,TVE_EXPAND);
	m_wndTree.Expand(hErrors,TVE_EXPAND);
	m_wndTree.Expand(hConstants,TVE_EXPAND);
}

void ClassView::OnClick( NMHDR * pNotifyStruct, LRESULT * result )
{
	if(!SemanticParser::m_Mutex.Lock(1))
	{
		return;
	}

	CPoint pt ;
	GetCursorPos(&pt) ;
	m_wndTree.ScreenToClient(&pt) ;

	UINT unFlags = 0 ;
	HTREEITEM hItem = m_wndTree.HitTest(pt, &unFlags) ;
	if((unFlags & TVHT_ONITEMLABEL) && hItem != NULL)
	{
		CString name = m_wndTree.GetItemText(hItem);
		HTREEITEM hParent = m_wndTree.GetParentItem(hItem);
		MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
		int line = 0;
		if(hParent == hRoot1)
		{
			SemanticParser::m_Mutex.Unlock();
			return;
		}
		else if(hParent == hErrors)
		{
			for(mySEVectorIterator = SemanticParser::m_vMessages.begin(); mySEVectorIterator != SemanticParser::m_vMessages.end(); mySEVectorIterator++)
			{
				if((*mySEVectorIterator)->message == name)
				{
					line = (*mySEVectorIterator)->line;
					break;
				}
			}
		}
		else if(hParent == hFunctions)
		{
			for(mySFVectorIterator = SemanticParser::m_vFunctions.begin(); mySFVectorIterator != SemanticParser::m_vFunctions.end(); mySFVectorIterator++)
			{
				if((*mySFVectorIterator)->definition == name)
				{
					line = (*mySFVectorIterator)->line;
					break;
				}
			}
		}
		else if(hParent == hTypes)
		{
			for(mySTVectorIterator = SemanticParser::m_vTypes.begin(); mySTVectorIterator != SemanticParser::m_vTypes.end(); mySTVectorIterator++)
			{
				if((*mySTVectorIterator)->type == name)
				{
					line = (*mySTVectorIterator)->line;
					break;
				}
			}
		}
		else if(hParent == hVariables)
		{
			for(mySVVectorIterator = SemanticParser::m_vVariables.begin(); mySVVectorIterator != SemanticParser::m_vVariables.end(); mySVVectorIterator++)
			{
				if((*mySVVectorIterator)->variable == name)
				{
					line = (*mySVVectorIterator)->line;
					break;
				}
			}
		}
		else if(hParent == hLabels)
		{			
			for(mySLVectorIterator = SemanticParser::m_vLabels.begin(); mySLVectorIterator != SemanticParser::m_vLabels.end(); mySLVectorIterator++)
			{
				if((*mySLVectorIterator)->label == name)
				{
					line = (*mySLVectorIterator)->line;
					break;
				}
			}
		}
		else if(hParent == hConstants)
		{
			for(mySCVectorIterator = SemanticParser::m_vConstants.begin(); mySCVectorIterator != SemanticParser::m_vConstants.end(); mySCVectorIterator++)
			{
				if((*mySCVectorIterator)->variable == name)
				{
					line = (*mySCVectorIterator)->line;
					break;
				}
			}
		}
		CMDIChildWnd *pChild = (CMDIChildWnd *) pMainWnd->GetActiveFrame();
		ASSERT_VALID(pChild);
		View* pView = DYNAMIC_DOWNCAST (View, pChild->GetActiveView());
		ASSERT_VALID(pView);
		if(pView != NULL)
		{
			pView->HighlightLine(line);
		}
	}

	SemanticParser::m_Mutex.Unlock();
}

int ClassView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_TreeImages.Create (IDB_CLASS_VIEW, 16, 0, RGB (255, 0, 0));

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tree windows.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	
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

void ClassView::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, cx - 2 * nBorderSize, cy - 2 * nBorderSize, SWP_NOACTIVATE | SWP_NOZORDER);
}

void ClassView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void ClassView::OnChangeVisualStyle ()
{
	m_TreeImages.DeleteImageList ();

	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiBmpId = bIsHighColor ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap (uiBmpId))
	{
		TRACE(_T ("Can't load bitmap: %x\n"), uiBmpId);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap (&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (bIsHighColor) ? ILC_COLOR24 : ILC_COLOR4;

	m_TreeImages.Create (16, bmpObj.bmHeight, nFlags, 0, 0);
	m_TreeImages.Add (&bmp, RGB (255, 0, 0));

	m_wndTree.SetImageList (&m_TreeImages, TVSIL_NORMAL);
}
