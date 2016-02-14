#pragma once

#include "OptionsPageEnvironment.h"
#include "OptionsPageDefaults.h"
#include "OptionsPageStartup.h"
#include "OptionsPageFonts.h"
#include "OptionsPageHelp.h"
#include "OptionsPageText.h"
#include "OptionsPageSemantic.h"

class COptionsDlg : public CBCGPPropertySheet
{
	DECLARE_DYNAMIC(COptionsDlg)

// Construction
public:
	COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	~COptionsDlg();

// Attributes
public:
	OptionsPageEnvironment	m_Environment;
	OptionsPageFonts		m_Fonts;
	OptionsPageText			m_Text;	
	OptionsPageDefaults		m_Defaults;	
	OptionsPageSemantic		m_Semantic;
	OptionsPageStartup		m_Startup;
	OptionsPageHelp			m_Help;	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog();

	// Generated message map functions
protected:
	// virtual void OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader);

	//{{AFX_MSG(COptionsDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	CStatic m_cRestartRequired;
	DECLARE_MESSAGE_MAP()
};
