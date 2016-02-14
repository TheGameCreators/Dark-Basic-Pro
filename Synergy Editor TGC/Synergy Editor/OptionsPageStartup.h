#pragma once
#include "afxwin.h"


// OptionsPageStartup dialog

class OptionsPageStartup : public CBCGPPropertyPage
{
	DECLARE_DYNAMIC(OptionsPageStartup)

public:
	OptionsPageStartup(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsPageStartup();

// Dialog Data
	enum { IDD = IDD_OPTIONSPAGE_STARTUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl m_ToolTip;
	CComboBox cAction;
	CEdit cFile;
	CButton cBrowse;
	afx_msg void OnBnClickedBrowseProject();
	afx_msg void OnCbnSelchangeStartup();
	afx_msg void OnEnChangeProjectLoad();
};
