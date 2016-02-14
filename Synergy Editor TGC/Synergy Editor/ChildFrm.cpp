// ChildFrm.cpp : implementation of the ChildFrame class
//
#include "stdafx.h"
#include "Synergy Editor.h"

#include "ChildFrm.h"

#include "Variables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChildFrame

IMPLEMENT_DYNCREATE(ChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(ChildFrame, CBCGPMDIChildWnd)
END_MESSAGE_MAP()

// ChildFrame construction/destruction

ChildFrame::ChildFrame()
{}

ChildFrame::~ChildFrame()
{}

BOOL ChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//return m_wndSplitter.Create(this,
	//	2, 2,			// TODO: adjust the number of rows, columns
	//	CSize(10, 10),	// TODO: adjust the minimum pane size
	//	pContext);
	return CBCGPMDIChildWnd::OnCreateClient(lpcs, pContext);
}

BOOL ChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_SYSMENU;

	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;	

	return TRUE;
}

// ChildFrame diagnostics

#ifdef _DEBUG
void ChildFrame::AssertValid() const
{
	CBCGPMDIChildWnd::AssertValid();
}

void ChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// ChildFrame message handlers

void ChildFrame::ActivateFrame(int nCmdShow)
{
	CBCGPMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED);
}
