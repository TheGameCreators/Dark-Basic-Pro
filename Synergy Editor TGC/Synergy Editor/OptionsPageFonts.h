#pragma once
#include "afxwin.h"
#include <vector>

// OptionsPageFonts dialog
class OptionsPageFonts : public CBCGPPropertyPage
{
	DECLARE_DYNAMIC(OptionsPageFonts)

public:
	OptionsPageFonts(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsPageFonts();

// Dialog Data
	enum { IDD = IDD_OPTIONSPAGE_FONTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString lastColourEntry;
	std::vector <COLORREF> m_Colors;
	bool lockColourChange;

	void DoOldColour();

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl m_ToolTip;
	CBCGPFontComboBox cFontFace;
	CComboBox cFontSize;
	CComboBox cDisplayItem;
	CBCGPColorButton cItemColour;
	CBCGPColorButton cEditorBackground;
	afx_msg void OnCbnSelchangeFontCombo();
	afx_msg void OnCbnSelchangeFontSize();
	afx_msg void OnCbnSelchangeFontCombo2();
	afx_msg void OnBnClickedColor();
	afx_msg void OnBnClickedBackColor();
};
