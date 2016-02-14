#pragma once
#include "afxwin.h"


// FindInProjectDialog dialog

class FindInProjectDialog : public CDialog
{
	DECLARE_DYNAMIC(FindInProjectDialog)

public:
	FindInProjectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_FIND_IN_FILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl m_ToolTip;

	CComboBox cmbLookIn;
	CButton chkMatchCase;
	CListBox lstResults;
	CButton btnFind;
	CButton rdOpenFile;
	CButton rdListFiles;
	CEdit txtFind;

	afx_msg void OnBnClickedFindButton();

private:
	void FindTextIn(CString filename, bool tryView);
	CString GetNextFolderFile(CString oldfilename);
	CString GetNextProjectFile(CString oldfilename);
	void FindTextInFolder();
	bool GoToTextIn(CString filename, bool tryView);

	CString firstFile;
	CString currentFile;
	int currentLine;
public:
	afx_msg void OnBnClickedHighlightText();
	afx_msg void OnEnChangeFindWhat();
	afx_msg void OnLbnDblclkResultsList();
};
