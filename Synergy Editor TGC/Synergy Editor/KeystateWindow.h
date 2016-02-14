#pragma once

#include "KeystateChild.h"

class KeystateWindow :
	public CFrameWnd
{
	
public:
	KeystateWindow();
protected: 
	DECLARE_DYNAMIC(KeystateWindow)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void PostNcDestroy();

// Implementation
public:
	virtual ~KeystateWindow();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	KeystateChild    m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
};
