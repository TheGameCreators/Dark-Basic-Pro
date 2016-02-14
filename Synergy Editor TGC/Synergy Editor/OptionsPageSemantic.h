#pragma once
#include "resource.h"

class OptionsPageSemantic : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(OptionsPageSemantic)

// Construction
public:
	OptionsPageSemantic();

// Dialog Data
	//{{AFX_DATA(OptionsPageSemantic)
	enum { IDD = IDD_OPTIONSPAGE_SEMANTIC };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsPageSemantic)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsPageSemantic)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CToolTipCtrl m_ToolTip;

	CButton cEnableCodeView;
	CComboBox cListOrder;
	CButton cPromptError;
	afx_msg void OnBnClickedEnableCodeView();
	afx_msg void OnCbnSelchangeListOrder();
	afx_msg void OnBnClickedPromptError();
	CButton cChangeCase;
	afx_msg void OnChange();
};
