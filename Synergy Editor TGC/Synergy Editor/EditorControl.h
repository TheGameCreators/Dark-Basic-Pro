#pragma once
#include "bcgpeditctrl.h"
#include <vector>
#include "Variables.h"
#include "Utilities.h"
#include "Synergy Editor.h"
#include "SourceFile.h"
#include "OutlineParser.h"
#include "Keywords.h"

class EditorControl :
	public CBCGPEditCtrl
{
public:
	EditorControl(void);
	~EditorControl(void);
	void RemoveAllBreakpoints();
	BOOL ToggleBreakpoint();
	OutlineParser* GetOutlineParser();
	void SetSourceFile(SourceFile* doc);
	void ReleaseLine(); // The control can now update the source file object
	void CommentSelection(bool comment);

	int iDefinitionLine;

	void SetKeepTabs(bool replace) { m_bKeepTabs = !replace; }

	// Slight changes to support search/replace in selection
	BOOL FindTextE(LPCTSTR lpszFind, BOOL bNext = TRUE, BOOL bCase = TRUE, BOOL bWholeWord = FALSE, BOOL bSelected = FALSE);
	BOOL DoFindTextE(int& nPos, int& nFindLength, LPCTSTR lpszFind, BOOL bNext = TRUE, BOOL bCase = TRUE, BOOL bWholeWord = FALSE, BOOL bSelected = FALSE);
	BOOL ReplaceTextE(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bNext = TRUE, BOOL bCase = TRUE, BOOL bWholeWord = FALSE, BOOL bSelected = FALSE);
	int ReplaceAllE(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bNext = TRUE, BOOL bCase = TRUE, BOOL bWholeWord = FALSE, BOOL bSelected = FALSE);
protected:
	static const DWORD g_dwBreakPointType = g_dwBCGPEdit_FirstUserDefinedMarker;
	static const DWORD g_dwColorBreakPointType = g_dwBCGPEdit_FirstUserDefinedMarker << 1;
	static const DWORD g_dwColorHighlightType = g_dwBCGPEdit_FirstUserDefinedMarker << 2;
	static const TCHAR g_chEOL = _T ('\n');

   virtual void OnDrawText (CDC* pDC);	
	virtual void OnSetCaret();
	virtual BOOL OnGetTipText(CString& strTipString);
	virtual void FindWordStartFinish (int nCaretOffset, const CString& strBuffer,
		                      int& nStartOffset, int& nEndOffset, BOOL bSkipSpaces = TRUE) const;
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnDrawMarker (CDC* pDC, CRect rectMarker, const CBCGPEditMarker* pMarker);
	virtual CBCGPOutlineParser* CreateOutlineParser() const;
private:
	int TestKeyword(TCHAR first, TCHAR second, int i, int &skipTo, int lineNumber, int bufferLength);
	int bracketBalance;
	
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	bool HighlightCurrentLine();

	CString m_HelpURL;
	CString m_StatusText;

	int toolTipPosition;
	CString m_ToolTipText;

	bool blockCaretChange;

	int lastHighlightLinePos;

	// Editor stuff
	int skipTo;
	bool inComment;
	bool inString;
	bool justInString;
	bool inBlockComment;
	int bufferLength;
	CString m_CommentBuffer;

	// Intellisense stuff
	int intellisenseStart;
	void FillList(CObList& lstIntelliSenseData);
	CString m_IntelliWord;
	bool lockWord;
	// Intellisense keyword list
	std::vector <Keyword*>::iterator InIter;
	std::vector <Keyword*> m_InKeywords;

	SourceFile* m_SourceFile; // The source file
	bool m_SuppressLineUpdate;

	CBCGPToolBarImages	m_ImageBreak; // Breakpoint marker

	OutlineParser* op; // Outline parser

	DECLARE_MESSAGE_MAP()
};

