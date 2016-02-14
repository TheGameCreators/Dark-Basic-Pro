#pragma once
#include "afxwin.h"
#include "afxhtml.h"

class CHtmlViewCustom :
	public CHtmlView
{
public:
	CHtmlViewCustom(void);
	~CHtmlViewCustom(void);

	void OpenALink( CString sLink );
	bool GetDefaultBrowserPath( char *szValue, DWORD dwSize );

protected:
	DECLARE_DYNCREATE(CHtmlViewCustom)

// Operations
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcieDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
	DECLARE_EVENTSINK_MAP()

};
