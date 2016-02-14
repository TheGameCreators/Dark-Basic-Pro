// ChildFrm.h : interface of the ChildFrame class
//
#pragma once

class ChildFrame : public CBCGPMDIChildWnd
{
	DECLARE_DYNCREATE(ChildFrame)
public:
	ChildFrame();

// Attributes
protected:
	//CSplitterWnd m_wndSplitter;
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow);

// Implementation
public:
	virtual ~ChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
