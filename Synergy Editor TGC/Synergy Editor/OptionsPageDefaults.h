#pragma once
#include "resource.h"

class OptionsPageDefaults : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(OptionsPageDefaults)

// Construction
public:
	OptionsPageDefaults();

// Dialog Data
	//{{AFX_DATA(OptionsPageDefaults)
	enum { IDD = IDD_OPTIONSPAGE_DEFAULT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(OptionsPageDefaults)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnOK();

	void OnChange();
	LRESULT OnPropertyChanged (WPARAM,LPARAM lParam);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(OptionsPageDefaults)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CToolTipCtrl m_ToolTip;

	CBCGPProp* m_Title;
	CBCGPProp* pType;

	CBCGPFileProp* pIcon;
	CBCGPFileProp* pFilename;

	CBCGPProp* pScreenType;
	CBCGPProp* pFullResolution;
	CBCGPProp* pWindowResolution;
	CBCGPProp* pCompress;
	CBCGPProp* pEncrypt;

	CBCGPProp* pComments;
	CBCGPProp* pCompany;
	CBCGPProp* pCopyright;
	CBCGPProp* pDescription;
	CBCGPProp* pVersion;

	CStatic			m_wndPropListLocation;
	CBCGPPropList 	m_wndList;

	bool updatingProperties;
};
