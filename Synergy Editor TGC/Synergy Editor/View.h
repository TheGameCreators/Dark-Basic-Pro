// View.h : interface of the View class
#pragma once

#include "SourceFile.h"
#include <stack>
#include <vector>
#include "SnippetWnd.h"
#include "ExtendedFind.h"

class EditorControl;
class Doc;

class View : public CBCGPEditView
{
protected: // create from serialization only
	View();
	DECLARE_DYNCREATE(View)

// Attributes
public:
	Doc* GetDocument() const;

// Operations
public:
	EditorControl*	m_pEdit;
	
	void SetText(CString text);
	CString GetText() const;
	void SaveFile(CArchive& ar);

	void Initialize();
	void OpenFile(CString szFile);

	void InsertText(CString text);

	void SetSourceFile(SourceFile* sf);
	void GoToLine(int line);
	void HighlightLine(int line);
	void UpdateEditorFromSettings();

	std::vector<int> m_Breakpoints;

	ExtendedFind *m_pExtendedFind;
	BOOL FindText (const CString& strFind, DWORD dwFindMask, bool bInSelection);

protected:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual CBCGPEditCtrl* CreateEdit ();
	virtual void OnInitialUpdate();
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

	void OnGoLine();

	CFont m_Font;	

	void OnInsertColour();
	void OnMedia(UINT nid);	

	void OnEditTogglebookmark();
	void OnRemoveAllBreakpoints();
	void OnToggleBreakpoint();
	void OnClearAllBookmarks();
	void OnUpdateClearAllBookmarks(CCmdUI* pCmdUI);
	void OnEditNextbookmark();
	void OnEditPreviousbookmark();

	void OnComment();
	void OnUnComment();
	
	void OnIndent();
	void OnUnIndent();

	void OnDefinition();
	void OnGoToLastLine();
	std::stack<int> m_ReturnLines;

	// Snippet
	void OnInsert();
	void OnSurround();
	LRESULT OnInsertData(WPARAM a, LPARAM b);
	void OnUpdateSurround(CCmdUI* pCmdUI);
	SnippetWnd *sWnd;
	CPoint m_SnippetPos;

	// Find
	void OnExtendedFind();
	void OnExtendedReplace();
	void DoFindReplace(BOOL bFindOnly);
	LRESULT OnFindReplaceMessage(WPARAM wParam, LPARAM lParam);	

// Implementation
public:
	virtual ~View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnDestroy();

	afx_msg void OnFilePrintPreview();

	afx_msg void OnEditHideselection();
	afx_msg void OnEditStophidingcurrent();
	afx_msg void OnEditToggleoutlining();
	afx_msg void OnEditTogglealloutlining();
	afx_msg void OnEditCollapsetodefinitions();
	afx_msg void OnEditStopoutlining();
	afx_msg void OnUpdateEditStophidingcurrent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditHideselection(CCmdUI* pCmdUI);
	afx_msg void OnEditAutooutlining();
	afx_msg void OnUpdateEditAutooutlining(CCmdUI* pCmdUI);
	afx_msg void OnEditEnableoutlining();
	afx_msg void OnUpdateEditEnableoutlining(CCmdUI* pCmdUI);
	afx_msg void OnEditLinenumbers();
	afx_msg void OnUpdateEditLinenumbers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditStopoutlining(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTogglealloutlining(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditToggleoutlining(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCollapsetodefinitions(CCmdUI* pCmdUI);
	
	afx_msg void OnUpdateDefinition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGoToLastLine(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in View.cpp
inline Doc* View::GetDocument() const
   { return reinterpret_cast<Doc*>(m_pDocument); }
#endif

