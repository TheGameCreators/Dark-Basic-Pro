// TGCOnline.h : main header file for the TGCONLINE application
//

#if !defined(AFX_TGCONLINE_H__330E7515_7DBA_4CAD_ADAB_BE809818753D__INCLUDED_)
#define AFX_TGCONLINE_H__330E7515_7DBA_4CAD_ADAB_BE809818753D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineApp:
// See TGCOnline.cpp for the implementation of this class
//

class CTGCOnlineApp : public CWinApp
{
public:
	CTGCOnlineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGCOnlineApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTGCOnlineApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TGCONLINE_H__330E7515_7DBA_4CAD_ADAB_BE809818753D__INCLUDED_)
