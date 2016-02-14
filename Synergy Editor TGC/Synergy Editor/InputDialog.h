#pragma once

#include "resource.h"
#include "afxwin.h"

// InputDialog dialog

class InputDialog : public CDialog
{
	DECLARE_DYNAMIC(InputDialog)

public:
	InputDialog(CString prompt, CString def = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~InputDialog();

	CString GetText();

	void SetText(CString text);

// Dialog Data
	enum { IDD = IDD_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	CString retText, promptText;

	DECLARE_MESSAGE_MAP()
public:
	CEdit cText;
	CStatic cPromptText;
};
