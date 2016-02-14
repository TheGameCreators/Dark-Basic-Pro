#pragma once
#include "afxwin.h"
#include "Utilities.h"

// NewProjectDialog dialog
class NewProjectDialog : public CDialog
{
	DECLARE_DYNAMIC(NewProjectDialog)

public:
	NewProjectDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~NewProjectDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void CheckCanOK();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
private:
	CListBox cTemplates;
	CEdit cName;
	CEdit cLocation;
	CButton cBrowseFolder;
	CButton cCreateDirectory;
	CButton cStickyIncludes;
	CButton cSkip;	
	CButton cOK;
	CButton cCancel;

	CString templates;

	CToolTipCtrl m_ToolTip;
public:
	CString projectName;
	CString projectPath;
	CString templateName;
	bool stickyIncludes;

	afx_msg void OnEnChangeProjectName();
	afx_msg void OnEnChangeProjectLocation();
	afx_msg void OnLbnSelchangeListTemplates();
	afx_msg void OnBnClickedBtnSkip();	
	afx_msg void OnBnClickedBtnBrowse();
};
