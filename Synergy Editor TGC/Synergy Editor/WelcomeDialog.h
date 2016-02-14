#pragma once

#include "WelcomePages.h"
#include <vector>

class WelcomeDialog : public CBCGPPropertySheet
{
	DECLARE_DYNAMIC(WelcomeDialog)

// Construction
public:
	WelcomeDialog(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	~WelcomeDialog();

// Attributes
public:	
	std::vector<WelcomePages*> m_Pages;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WelcomeDialog)
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();

	// Generated message map functions
protected:
	//{{AFX_MSG(WelcomeDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	CButton m_ButtonAgain, m_Button;

	void OnCheck();

	DECLARE_MESSAGE_MAP()
};
