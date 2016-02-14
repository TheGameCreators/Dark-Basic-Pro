#pragma once

#include "afxhtml.h"
#include "HTMLDocument.h"

class CHelp :
	public CHtmlView
{
public:
	CHelp();
	~CHelp();

protected: // create from serialization only
	DECLARE_DYNCREATE(CHelp)

	// Attributes
public:
#ifdef _DEBUG
		HTMLDocument* GetDocument();
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Overrides
// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CMfcieView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_VIRTUAL
	void OnTitleChange(LPCTSTR lpszText);
	void OnDocumentComplete(LPCTSTR lpszUrl);
	void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMfcieView)
	afx_msg void OnGoBack();
	afx_msg void OnGoForward();
	afx_msg void OnGoStartPage();
	afx_msg void OnViewStop();
	afx_msg void OnViewRefresh();
	afx_msg void OnViewFontsLargest();
	afx_msg void OnViewFontsLarge();
	afx_msg void OnViewFontsMedium();
	afx_msg void OnViewFontsSmall();
	afx_msg void OnViewFontsSmallest();
	afx_msg void OnFileOpen();
	afx_msg void OnEditFind();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString url;
	bool m_bHelpShown;
};
