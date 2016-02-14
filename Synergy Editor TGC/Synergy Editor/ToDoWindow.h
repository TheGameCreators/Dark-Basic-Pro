#pragma once
#include "bcgpdockingcontrolbar.h"

class CToDoToolBar : public CBCGPToolBar
{
public:	
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CBCGPToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};

class ToDoWindow :
	public CBCGPDockingControlBar
{
public:
	void Clear();
	void AddItem(CString item, BOOL checked);

	void AddComment(CString line);
	void ClearComments();
	void DumpComments();

private:
	void AddItem();
	void RemoveItem();
	void AdjustLayout ();

	void OnAdd();
	void OnRemove();
	void OnUp();
	void OnDown();
	void OnEdit();

	void OnUpdateRemove(CCmdUI *pCmdUI);
	void OnUpdateUp(CCmdUI *pCmdUI);
	void OnUpdateDown(CCmdUI *pCmdUI);
	void OnUpdateEdit(CCmdUI *pCmdUI);

	void OnContextMenu(CWnd*, CPoint); // Supress

	// Generated message map functions
protected:
	CToolTipCtrl	m_ToolTip;
	CToDoToolBar	m_wndToolBar;
	CListCtrl		m_ListBox;
	CEdit			m_Comments;
	CFont			m_lbFont;

	void OnItemchangedLinksList(NMHDR* pNMHDR, LRESULT* pResult);
	void OnChangeVisualStyle ();

	//{{AFX_MSG(CWorkSpaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnInfoTip( NMHDR * pNMHDR, LRESULT * pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
