#if !defined(AFX_GOTOLINEDLG_H__7BC09FDD_9392_4DFD_A141_FD599212DCED__INCLUDED_)
#define AFX_GOTOLINEDLG_H__7BC09FDD_9392_4DFD_A141_FD599212DCED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoLineDlg.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGotoLineDlg dialog

class CGotoLineDlg : public CDialog
{
// Construction
public:
	CGotoLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoLineDlg)
	enum { IDD = IDD_GOTO_LINE };
	int		m_nLineNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoLineDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOLINEDLG_H__7BC09FDD_9392_4DFD_A141_FD599212DCED__INCLUDED_)
