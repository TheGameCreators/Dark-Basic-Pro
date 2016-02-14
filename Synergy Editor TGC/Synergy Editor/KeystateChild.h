#pragma once

class KeystateChild :
	public CWnd
{
// Construction
public:
	KeystateChild();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	CButton m_Button;

	void OnClick();

// Implementation
public:
	virtual ~KeystateChild();

	// Generated message map functions
protected:
	UINT key;
	UINT ascii;

	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};
