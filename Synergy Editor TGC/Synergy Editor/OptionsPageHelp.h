#pragma once
#include "afxwin.h"

// OptionsPageHelp dialog
class OptionsPageHelp : public CBCGPPropertyPage
{
	DECLARE_DYNAMIC(OptionsPageHelp)

public:
	OptionsPageHelp(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsPageHelp();

// Dialog Data
	enum { IDD = IDD_OPTIONSPAGES_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl m_ToolTip;
	CEdit cHelpPage;
	CButton cNewTab;
	afx_msg void OnEnChangeHelp();
	afx_msg void OnBnClickedOpenNewTab();
};
