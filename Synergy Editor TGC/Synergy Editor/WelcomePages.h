#pragma once
#include "afxwin.h"
#include "Resource.h"

// WelcomePages dialog
class WelcomePages : public CBCGPPropertyPage
{
	DECLARE_DYNAMIC(WelcomePages)

public:
	WelcomePages(CWnd* pParent = NULL);   // standard constructor
	virtual ~WelcomePages();
	void SetPath(CString path);

// Dialog Data
	enum { IDD = IDD_welcome };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL WelcomePages::OnInitDialog();

	void CreateButton(CString title, CString filename, CString bitmap);
	void Execute(int id);

	wchar_t* string;

	CString m_Path, title;
	CString avis[10];
	int buttonNum;

	DECLARE_MESSAGE_MAP()
public:
	CBCGPButton cButton1;
	CBCGPButton cButton2;
	CBCGPButton cButton3;
	CBCGPButton cButton4;
	CBCGPButton cButton5;
	CBCGPButton cButton6;
	CBCGPButton cButton7;
	CBCGPButton cButton8;
	CBCGPButton cButton9;
	CBCGPButton cButton10;
	CStatic cText;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
};
