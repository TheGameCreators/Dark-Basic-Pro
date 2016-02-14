// DarkBASIC Professional Editor.h : main header file for the DarkBASIC Professional Editor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "MainFrm.h"
#include "AdvertThread.h"


// App:
// See DarkBASIC Professional Editor.cpp for the implementation of this class

class App : public CWinApp,	public CBCGPWorkspace
{
public:
	App();
	void OnHelp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void AddToRecentFileList(LPCTSTR lpszPathName);
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	DECLARE_MESSAGE_MAP()

public:
	CMultiDocTemplate* pHelpTemplate;
	CMultiDocTemplate* pDocTemplate;
	CAdvertThread* pAdvertThread;
	CFrameWnd* pAdvertWnd;
	CHtmlView* pAdvertView;

	bool stopNewDialog;
	bool createdSettings;

private:
	MainFrame* pMainFrame;

	void DoSelectedAction();

	unsigned long m_gdiplusToken;
};

extern App theApp;
