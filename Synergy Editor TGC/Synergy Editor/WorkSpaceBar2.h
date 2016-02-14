#pragma once

class CPropertiesToolBar : public CBCGPToolBar
{
public:	
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CBCGPToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};

class CWorkSpaceBar2 : public CBCGPDockingControlBar
{
public:
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

// Attributes
protected:
	CBCGPPropList 		m_wndList;
	CPropertiesToolBar	m_wndToolBar;

	void OnContextMenu(CWnd* pWnd, CPoint point); // Supress

// Operations
public:
	void UpdateFromLibrary();
	void SetBuildTypeToMedia();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceBar2)
	//}}AFX_VIRTUAL

// Implementation
private:
	void OnChangeVisualStyle ();
	void OnSortingprop();
	void OnUpdateSortingprop(CCmdUI* pCmdUI);
	void OnExpand();
	void OnUpdateExpand(CCmdUI* pCmdUI);

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkSpaceBar2)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
