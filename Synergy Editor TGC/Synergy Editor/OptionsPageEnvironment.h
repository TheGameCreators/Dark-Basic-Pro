#pragma once
#include "resource.h"
#include "afxwin.h"

class OptionsPageEnvironment : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(OptionsPageEnvironment)

// Construction
public:
	OptionsPageEnvironment();

// Dialog Data
	//{{AFX_DATA(OptionsPageEnvironment)
	enum { IDD = IDD_OPTIONSPAGE_ENVIRONMENT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsPageEnvironment)
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnOK();

	void OnChange();
	void OnChangeStyle();
	void OnBrowse();
	void OnBrowseTemp();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsPageEnvironment)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CToolTipCtrl m_ToolTip;
	CEdit cCompiler;
	CButton cStatus;
	CButton cAccelerate;
	CEdit cHelp;
	CButton cNewHelpTab;	
	CButton cBrowse;
	CButton cBrowseTemp;
	CEdit cTempPath;
	CButton cShowOutput;
	afx_msg void OnEnChangeTempPath();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedShowOutput();
	afx_msg void OnEnChangeProjectLoad();
	afx_msg void OnBnClickedOpenNewTab();
	CComboBox cAppearance;
	CButton cClearOutput;
	CButton cOpenAllIncludes;
	afx_msg void OnBnClickedOpenIncludes();
	CBCGPColorButton cStyleHue;
};
