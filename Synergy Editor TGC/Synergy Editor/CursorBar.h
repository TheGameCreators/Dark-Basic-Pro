#pragma once

class CCursorToolBar : public CBCGPToolBar
{
public:	
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CBCGPToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};

class CursorBar : public CBCGPDockingControlBar
{
// Operations
public:
	void Clear();
	void UpdatePath(CString path)
	{
		m_DefaultPath = path;
	}
	void AddNewFile(CString file);

protected:
	CListCtrl		m_ListBox;
	CCursorToolBar	m_wndToolBar;

	CString m_DefaultPath;

	void AdjustLayout ();
	void OnChangeVisualStyle ();

	void OnImport();
	void OnDelete();
	void OnDefault();
	void OnUpdateDelete(CCmdUI *pCmdUI);
	void OnUpdateDefault(CCmdUI *pCmdUI);

	HRESULT SaveIconToFile(HICON hIcon, LPCTSTR lpFileName);

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	void OnContextMenu(CWnd*, CPoint); // Supress
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
