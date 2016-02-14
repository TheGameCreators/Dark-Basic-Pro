#pragma once
#include "afxwin.h"
#include "Resource.h"


// KeywordDialog dialog

class KeywordDialog : public CDialog
{
	DECLARE_DYNAMIC(KeywordDialog)

public:
	KeywordDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KeywordDialog();

// Dialog Data
	enum { IDD = IDD_KEYWORDS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();	

	LRESULT SetProcessMessage(WPARAM a, LPARAM b);
	LRESULT SetFile(WPARAM a, LPARAM b);
	LRESULT SetLine(WPARAM a, LPARAM b);
	LRESULT CloseDialog(WPARAM a, LPARAM b);

	DECLARE_MESSAGE_MAP()
public:
	CStatic cProcess;
	CStatic cFile;
	CStatic cLine;
};
