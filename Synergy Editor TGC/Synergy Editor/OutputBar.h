// outputbar.h : interface of the COutputBar class
//
/////////////////////////////////////////////////////////////////////////////

#include "afxhtml.h"
#include "thread.h"

#pragma once

// OUTPUT BAR

class COutputBar : public CBCGPDockingControlBar
{
public:
	COutputBar();

// Attributes
protected:

	CListCtrl	m_wndList;
	CProgressCtrl m_wndProg;

	int lastnumber;
	int base;

	void OnCopy();
	void OnWrite();


// Operations
public:

	void RefreshAnyAdvert ();
	void AdjustLayout ();
	void AddOutputText(CString text);
	void SetStatus(int status);
	void SetStatusMax(int max);
	
	void OnClear();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputBar();

// Generated message map functions
protected:
	//{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy ( void );
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
