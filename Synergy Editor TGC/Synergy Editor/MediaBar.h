#pragma once

#include "ImagePreviewStatic.h"

class CMediaToolBar : public CBCGPToolBar
{
public:	
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CBCGPToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};

class MediaBar : public CBCGPDockingControlBar
{
public:
	MediaBar();

// Attributes
protected:
	CListCtrl		m_ListBox;
	CMediaToolBar	m_wndToolBar;

	CImagePreviewStatic m_BitmapView;

	CString m_Filename;
	CString m_DefaultPath;

	void OnRemove();
	void OnImport();
	void OnClearAll();

	void OnUpdateRemove(CCmdUI *pCmdUI);
	void OnUpdatePreview(CCmdUI *pCmdUI);
	void OnPreview();
	void OnItemchangedLinksList(NMHDR* pNMHDR, LRESULT* pResult);

	void AdjustLayout ();

// Operations
public:
	void AddNewFile(CString file);
	void Clear();
	void UpdatePath(CString path)
	{
		m_DefaultPath = path;
	}

	void OnChangeVisualStyle ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceBar)
	//}}AFX_VIRTUAL

// Implementation
public:

private:
	void OnContextMenu(CWnd*, CPoint); // Supress

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
