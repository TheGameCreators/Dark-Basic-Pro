// TGCOnlineDlg.h : header file
//

#if !defined(AFX_TGCONLINEDLG_H__60D022E5_63FE_4F5E_9CBD_E6821D2D59BC__INCLUDED_)
#define AFX_TGCONLINEDLG_H__60D022E5_63FE_4F5E_9CBD_E6821D2D59BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineDlg dialog

class CTGCOnlineDlg : public CDialog
{
// Construction
public:
//	CTGCOnlineDlg(CWnd* pParent = NULL);	// standard constructor
	CTGCOnlineDlg(UINT uDialog);	// standard constructor

	bool BuildTheCertificateFile ( LPSTR pAnySerial, LPSTR* ppResponseCertificate, int* piIndexToInternalProduct );

	bool m_bFirstPage;
	char m_pProductName [ 256 ];

// Dialog Data
	//{{AFX_DATA(CTGCOnlineDlg)
	enum { IDD = IDD_TGCONLINE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGCOnlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTGCOnlineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TGCONLINEDLG_H__60D022E5_63FE_4F5E_9CBD_E6821D2D59BC__INCLUDED_)
