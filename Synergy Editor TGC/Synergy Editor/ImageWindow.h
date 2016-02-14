#pragma once

#include "ImageChild.h"

class ImageWindow :
	public CFrameWnd
{
	
public:
	ImageWindow();
protected: 
	DECLARE_DYNAMIC(ImageWindow)

// Attributes
public:

// Operations
public:

// Overrides
public:
	void SetPicture(CString m_Filename);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~ImageWindow();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	ImageChild    m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
};
