#pragma once

class CSolutionToolBar : public CBCGPToolBar
{
public:	
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CBCGPToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};

class CWorkSpaceBar : public CBCGPDockingControlBar
{
public:
	CWorkSpaceBar();

// Attributes
protected:
	CSolutionToolBar	m_wndToolBar1, m_wndToolBar2;
	CTreeCtrl			m_wndTree;
	CImageList			m_TreeImages;
	HTREEITEM			hRoot1;
	HTREEITEM			hBold;

	CString m_DefaultPath;
	int mode;

	void OnSetMain();
	void OnRemove();
	void OnImportFile();
	void OnAddNewFile();

	void OnOpenFolder(); // Open project folder
	void OnOpenFileFolder(); // Open file folder
	void OnProperties(); // File properties

	void HighlightNewMain(); // Called when an entry is deleted

	void OnLocateFile(); // Used to locate missing files

	afx_msg void OnClick( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	void OnPropertiesUpdate(CCmdUI* pCmdUI);
	void OnOpenFileFolderUpdate(CCmdUI* pCmdUI);

// Operations
public:
	void Expand();
	void AddNewFile(CString file);
	void DeleteOldFile(CString file);
	void Clear();
	void OnOpenAllFiles();
	void OpenInclude(CString name);
	void UpdatePath(CString path);

	void OnChangeVisualStyle ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceBar)
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	void OnSelchangedTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
