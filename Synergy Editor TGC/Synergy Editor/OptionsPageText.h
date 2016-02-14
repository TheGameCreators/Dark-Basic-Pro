#pragma once
#include "afxwin.h"

class OptionsPageText : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(OptionsPageText)

// Construction
public:
	OptionsPageText();

// Dialog Data
	//{{AFX_DATA(OptionsPageText)
	enum { IDD = IDD_OPTIONSPAGE_TEXT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsPageText)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnOK();

	void OnChange();
	void DoOldColour();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsPageText)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CToolTipCtrl m_ToolTip;
	CButton cStatusHelp;
	CButton CToolTips;
	CButton CRedrawCaret;
	CButton cBracketBalance;
	CButton cIntellisense;
	CButton cIndentLine;
	CButton cCloseBlock;
	CComboBox cCommandStyle;
	CComboBox CMouseScroll;
	CButton cFunctions;
	CButton cForLoops;
	CButton cWhileLoops;
	CButton cRepeatLoops;
	CButton cCommentBlocks;
	CButton cTypeDeclarations;
	afx_msg void OnCbnSelchangeFontCombo2();
	CButton cHighlight;
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedIntellisense();
	afx_msg void OnCbnSelchangeCommandStyle();
	afx_msg void OnBnClickedHighlightLine();
	afx_msg void OnBnClickedIndentBlock();
	afx_msg void OnBnClickedCloseBlock();
	afx_msg void OnCbnSelchangeMouseScroll();
	afx_msg void OnBnClickedFoldFunction();
	afx_msg void OnBnClickedFoldWhile();
	afx_msg void OnBnClickedFoldComments();
	afx_msg void OnBnClickedFoldFor();
	afx_msg void OnBnClickedFoldRepeat();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck1();
	CButton cReturnLine;
	CButton cConcat;
	CComboBox cTabWidth;
	CButton cReplaceTabs;
};
