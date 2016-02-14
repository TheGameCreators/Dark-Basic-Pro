#include "stdafx.h"
#include "Synergy Editor.h"

#include "Doc.h"
#include "View.h"
#include "GotoLineDlg.h"
#include "Settings.h"
#include "Variables.h"
#include "Utilities.h"
#include "EditorControl.h"

//#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// View
IMPLEMENT_DYNCREATE(View, CBCGPEditView)

static UINT FindReplaceDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(View, CBCGPEditView)
	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_EDIT_FIND, OnExtendedFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnExtendedReplace)
	ON_REGISTERED_MESSAGE(FindReplaceDialogMessage, OnFindReplaceMessage)

	ON_COMMAND(ID_EDIT_GOTOLINE, OnGoLine)

	ON_COMMAND(ID_EDIT_TOGGLEBOOKMARK, OnEditTogglebookmark)
	ON_COMMAND(ID_EDIT_REMOVEALLBREAKPOINTS, OnRemoveAllBreakpoints)
	ON_COMMAND(ID_EDIT_TOGGLEBREAKPOINT, OnToggleBreakpoint)
	ON_COMMAND(ID_EDIT_REMOVEALLBOOKMARKS, OnClearAllBookmarks)
	ON_COMMAND(ID_EDIT_NEXTBOOKMARK, OnEditNextbookmark)
	ON_COMMAND(ID_EDIT_PREVIOUSBOOKMARK, OnEditPreviousbookmark)

	ON_COMMAND(ID__INSERTRGBCOLOUR, OnInsertColour)
	ON_COMMAND_RANGE(666, 700, OnMedia)

	ON_COMMAND(ID_ADVANCED_COMMENTSELECTION, OnComment)
	ON_COMMAND(ID_ADVANCED_UNCOMMENTSELECTION, OnUnComment)

	ON_COMMAND(ID_ADVANCED_INCREASELINEINDENT, OnIndent)
	ON_COMMAND(ID_ADVANCED_DECREASELINEINDENT, OnUnIndent)

	ON_COMMAND(ID_EDIT_HIDESELECTION, OnEditHideselection)
	ON_COMMAND(ID_EDIT_STOPHIDINGCURRENT, OnEditStophidingcurrent)
	ON_COMMAND(ID_EDIT_TOGGLEOUTLINING, OnEditToggleoutlining)
	ON_COMMAND(ID_EDIT_TOGGLEALLOUTLINING, OnEditTogglealloutlining)
	ON_COMMAND(ID_EDIT_COLLAPSETODEFINITIONS, OnEditCollapsetodefinitions)
	ON_COMMAND(ID_EDIT_STOPOUTLINING, OnEditStopoutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_STOPHIDINGCURRENT, OnUpdateEditStophidingcurrent)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HIDESELECTION, OnUpdateEditHideselection)
	ON_COMMAND(ID_EDIT_AUTOOUTLINING, OnEditAutooutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AUTOOUTLINING, OnUpdateEditAutooutlining)
	ON_COMMAND(ID_EDIT_ENABLEOUTLINING, OnEditEnableoutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ENABLEOUTLINING, OnUpdateEditEnableoutlining)
	ON_COMMAND(ID_EDIT_LINENUMBERS, OnEditLinenumbers)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LINENUMBERS, OnUpdateEditLinenumbers)
	ON_UPDATE_COMMAND_UI(ID_EDIT_STOPOUTLINING, OnUpdateEditStopoutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TOGGLEALLOUTLINING, OnUpdateEditTogglealloutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TOGGLEOUTLINING, OnUpdateEditToggleoutlining)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COLLAPSETODEFINITIONS, OnUpdateEditCollapsetodefinitions)

	ON_COMMAND(ID_CONTEXT_GOTODEFINITION, OnDefinition)
	ON_COMMAND(ID_CONTEXT_RETURNTOLASTLINE, OnGoToLastLine)
	ON_COMMAND(ID_ADVANCED_GOTODEFINITION, OnDefinition)
	ON_COMMAND(ID_ADVANCED_GOTOLASTLINE, OnGoToLastLine)
	ON_UPDATE_COMMAND_UI(ID_ADVANCED_GOTODEFINITION, OnUpdateDefinition)
	ON_UPDATE_COMMAND_UI(ID_ADVANCED_GOTOLASTLINE, OnUpdateGoToLastLine)

	// Snippet
	ON_COMMAND(ID_CONTEXT_INSERTSNIPPET, OnInsert)
	ON_COMMAND(ID_CONTEXT_SURROUNDWITHSNIPPET, OnSurround)
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_SURROUNDWITHSNIPPET, OnUpdateSurround)
	ON_MESSAGE(WM_USER, OnInsertData)

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
END_MESSAGE_MAP()

// View construction/destruction
View::View()
{
	m_pExtendedFind = NULL;
	// m_pExtendedReplace = NULL;
}

/*
* Turns off the semantic parsing system for this view
*/
View::~View()
{
	SemanticParserManager::CloseView(this);
}

/*
* Cleans up resources
*/
void View::OnDestroy() 
{
	if (m_pExtendedFind != NULL)
	{
		m_pExtendedFind->PostMessage(WM_CLOSE);
	}

	//if (m_pExtendedReplace != NULL)
	//{
	//	m_pExtendedReplace->PostMessage(WM_CLOSE);
	//}

	CBCGPEditView::OnDestroy();
}

/*
* Turns on the semantic parsing system for this view
*/
void View::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{	
	if(bActivate)
	{
		SemanticParserManager::SetView(this);	
	}
	CBCGPEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/*
* Custom Find
*/
void View::OnExtendedFind()
{
	DoFindReplace(TRUE);
}

void View::OnExtendedReplace()
{
	DoFindReplace(FALSE);
}

void View::DoFindReplace(BOOL bFindOnly)
{
	if (m_pExtendedFind != NULL)
	{
		m_pExtendedFind->SetFocus ();
		return;
	}

	ASSERT_VALID (m_pEdit);
	int iStartSel1, iEndSel1;
	m_pEdit->GetSel(iStartSel1, iEndSel1);
	int iStartSel = min (iStartSel1, iEndSel1);
	int iEndSel = max (iStartSel1, iEndSel1);

	m_pExtendedFind = new ExtendedFind();
	m_pExtendedFind->m_pParent = this;
	m_pExtendedFind->m_fr.lpTemplateName = bFindOnly ? MAKEINTRESOURCE(IDD_FIND) : MAKEINTRESOURCE(IDD_REPLACE);

    if(m_pExtendedFind->Create(bFindOnly, NULL, NULL, FR_ENABLETEMPLATE | FR_DOWN, this))
	{
		if (IsDisableMainframeForFindDlg ())
		{
			AfxGetMainWnd()->ModifyStyle(0,WS_DISABLED);
		}
		m_pExtendedFind->cSelected.SetCheck(iStartSel >= 0 && iEndSel > iStartSel);
		m_pExtendedFind->SetActiveWindow();
		m_pExtendedFind->ShowWindow(TRUE);
	}
	else
	{
		delete m_pExtendedFind;
	}
}

LRESULT View::OnFindReplaceMessage(WPARAM wParam, LPARAM lParam)
{
    CFindReplaceDialog* pFindReplace = CFindReplaceDialog::GetNotifier(lParam);
    ASSERT(pFindReplace != NULL);

    if (pFindReplace->IsTerminating())
    {
        pFindReplace = NULL;

    }
	else if (pFindReplace->FindNext())
    {
		DWORD m_dwFindMask = m_pExtendedFind->m_fr.Flags;
	    if(!FindText(pFindReplace->GetFindString(),m_dwFindMask, ((ExtendedFind*)pFindReplace)->cSelected.GetCheck() == TRUE))
		{
            OnTextNotFound(pFindReplace->GetFindString());
		}
    }
	else if(pFindReplace->ReplaceCurrent())
	{
		DWORD m_dwFindMask = m_pExtendedFind->m_fr.Flags;
		m_strReplaceText = pFindReplace->GetReplaceString();
		if(!FindText(m_pExtendedFind->GetFindString(),m_dwFindMask, ((ExtendedFind*)m_pExtendedFind)->cSelected.GetCheck() == TRUE))
		{
            OnTextNotFound(pFindReplace->GetFindString());
		}
	}
	else if(pFindReplace->ReplaceAll())
	{
		DWORD m_dwFindMask = m_pExtendedFind->m_fr.Flags;
		m_strReplaceText = pFindReplace->GetReplaceString();
		if(!FindText(m_pExtendedFind->GetFindString(),m_dwFindMask, ((ExtendedFind*)m_pExtendedFind)->cSelected.GetCheck() == TRUE))
		{
            OnTextNotFound(pFindReplace->GetFindString());
		}
	}
    return 0;

}

BOOL View::FindText (const CString& strFind, DWORD dwFindMask, bool bInSelection)
{
	if (m_pEdit == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (m_pEdit);

	if ((dwFindMask & FR_REPLACE) != 0)
	{		
		return m_pEdit->ReplaceTextE(strFind, m_strReplaceText,
							(dwFindMask & FR_DOWN) != 0,
							(dwFindMask & FR_MATCHCASE) != 0,
							(dwFindMask & FR_WHOLEWORD) != 0,
							bInSelection);
	}
	else if ((dwFindMask & FR_REPLACEALL) != 0)
	{
		return m_pEdit->ReplaceAllE(strFind, m_strReplaceText,
							(dwFindMask & FR_DOWN) != 0,
							(dwFindMask & FR_MATCHCASE) != 0,
							(dwFindMask & FR_WHOLEWORD) != 0,
							bInSelection);
	}
	else
	{
		return m_pEdit->FindTextE(strFind,
							(dwFindMask & FR_DOWN) != 0,
							(dwFindMask & FR_MATCHCASE) != 0,
							(dwFindMask & FR_WHOLEWORD) != 0,
							bInSelection);
	}
}

/*
* Navigates to the line containing the variable/function/type definition
*/
void View::OnDefinition()
{
	ASSERT_VALID(m_pEdit);
	m_ReturnLines.push(m_pEdit->GetCurRow());
	if(m_pEdit->iDefinitionLine > -1)
	{
		HighlightLine(m_pEdit->iDefinitionLine);
	}
}

/*
* Returns to the line at the top of the stack
*/
void View::OnGoToLastLine()
{
	ASSERT_VALID(m_pEdit);
	if(m_ReturnLines.size() > 0)
	{		
		HighlightLine(m_ReturnLines.top());
		m_ReturnLines.pop();
	}
}

/*
* Highlights the line specified
*/
void View::HighlightLine(int line)
{
	ASSERT_VALID(m_pEdit);
	if(line > -1)
	{
		m_pEdit->SelectLine(line);
	}
}

/*
* Navigates to the line specified
*/
void View::GoToLine(int line)
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->GoToLine(line + 1);
}

/*
* Sets the source file object for this document
*/
void View::SetSourceFile(SourceFile* sf)
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->SetSourceFile(sf);
}

void View::OnEditHideselection() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->HideSelection ();
}

void View::OnUpdateDefinition(CCmdUI* pCmdUI)
{
	ASSERT_VALID(m_pEdit);
	pCmdUI->Enable(m_pEdit->iDefinitionLine > -1);
}

void View::OnUpdateGoToLastLine(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ReturnLines.size() > 0);
}

void View::OnUpdateEditHideselection(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);

	int nStart = 0;
	int nEnd = 0;
	m_pEdit->GetSel (nStart, nEnd);

	pCmdUI->Enable (
		nStart >= 0 && nEnd >= 0 && (nStart != nEnd) &&
		m_pEdit->IsOutliningEnabled () &&
		!m_pEdit->IsAutoOutliningEnabled ());
}

void View::OnEditStophidingcurrent() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->StopHidingCurrent ();
}

void View::OnUpdateEditStophidingcurrent(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable (
		m_pEdit->IsOutliningEnabled () &&
		!m_pEdit->IsAutoOutliningEnabled ());
}

void View::OnEditToggleoutlining() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->ToggleOutlining ();
}

void View::OnUpdateEditToggleoutlining(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable (m_pEdit->IsOutliningEnabled ());
}

void View::OnEditTogglealloutlining() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->ToggleAllOutlining ();
}

void View::OnUpdateEditTogglealloutlining(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable (m_pEdit->IsOutliningEnabled ());
}

void View::OnEditCollapsetodefinitions() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->CollapseToDefinitions ();
}

void View::OnUpdateEditCollapsetodefinitions(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable (m_pEdit->IsOutliningEnabled ());
}

void View::OnEditStopoutlining() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->StopOutlining ();
}

void View::OnUpdateEditStopoutlining(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable (
		m_pEdit->IsOutliningEnabled () &&
		m_pEdit->IsAutoOutliningEnabled ());
}

void View::OnEditAutooutlining() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->EnableAutoOutlining (!m_pEdit->IsAutoOutliningEnabled ());
}

void View::OnUpdateEditAutooutlining(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->SetCheck (m_pEdit->IsAutoOutliningEnabled () ? 1 : 0);
	pCmdUI->Enable (m_pEdit->IsOutliningEnabled ());
}

/*
* Modifies the outlining mode of the editor control
*/
void View::OnEditEnableoutlining() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->EnableOutlining (!m_pEdit->IsOutliningEnabled ());
	m_pEdit->SetOutlineMargin (m_pEdit->IsOutliningEnabled ());
}

/*
* Modifies the visibility of the selected flag of the 'enable outlining' option
*/
void View::OnUpdateEditEnableoutlining(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->SetCheck (m_pEdit->IsOutliningEnabled () ? 1 : 0);
}

/*
* Modifies the visibility of the line numbers margin
*/
void View::OnEditLinenumbers() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->SetLineNumbersMargin (!m_pEdit->IsLineNumbersMarginVisible ());
}

/*
* Controls the visibility of the selected flag of the 'show line numbers' menu item
*/
void View::OnUpdateEditLinenumbers(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->SetCheck (m_pEdit->IsLineNumbersMarginVisible () ? 1 : 0);
}

/*
* Comments the selected code
*/
void View::OnComment()
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->CommentSelection(TRUE);
}

/*
* Un-Comments the selected code
*/
void View::OnUnComment()
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->CommentSelection(FALSE);
}

/*
* Indents the selected code
*/
void View::OnIndent()
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->IndentSelection(TRUE);
}

/*
* Un-Indents the selected code
*/
void View::OnUnIndent()
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->IndentSelection(FALSE);
}

/*
* Inserts text at the specified position
* Used by the keystate window
*/
void View::InsertText(CString text)
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->InsertText(text);
}

/*
* Shows a color dialog box and then handles the insertion of the DarkBASIC Professional source code
*/
void View::OnInsertColour()
{
	CBCGPColorDialog clrDlg;
	if(clrDlg.DoModal() == IDOK)
	{
		COLORREF color = clrDlg.GetColor();
		int red = GetRValue(color);
		int green = GetGValue(color);
		int blue = GetBValue(color);

		CString colorS;
		if(Settings::IntellisenseCase == 0)
		{
			colorS = _T("RGB(");
		}
		else if(Settings::IntellisenseCase == 1)
		{
			colorS = _T("rgb(");
		}
		else
		{
			colorS = _T("Rgb(");
		}	

		char pszNumRed [4] = {0};
		_itoa_s(red,pszNumRed, 4, 10);	
		colorS += pszNumRed;
		colorS += _T(",");
		char pszNumGreen [4] = {0};
		_itoa_s(green,pszNumGreen, 4, 10);
		colorS += pszNumGreen;
		colorS += _T(",");
		char pszNumblue [4] = {0};
		_itoa_s(blue,pszNumblue, 4, 10);
		colorS += pszNumblue;
		colorS += _T(")");
		m_pEdit->InsertText(colorS);
	}
}

/*
* Handles selection of the dynamic media menu
*/
void View::OnMedia(UINT nid)
{
	int pos = 666;
	CString name;
	for(UINT i = 0; i < Variables::m_DBPropertyMedia.size(); i++)
	{		
		TCHAR str[MAX_PATH];
		CString source = Variables::m_ProjectPath;
		CString to = Variables::m_DBPropertyMedia.at(i);
		PathRelativePathTo(str, source, FILE_ATTRIBUTE_DIRECTORY, to, FILE_ATTRIBUTE_NORMAL);
		CString path(str);
		name = path;
		if(nid == pos) break;
		pos++;
		name = Variables::m_DBPropertyMedia.at(i);
		if(nid == pos) break;
		pos++;
		name = Utilities::ExtractFilename(Variables::m_DBPropertyMedia.at(i));
		if(nid == pos) break;
		pos++;
	}

	m_pEdit->InsertText(name);
}

/*
* Handles the context menu for the editor
*/
void View::OnContextMenu(CWnd* pWnd, CPoint point)
{
	ASSERT_VALID(m_pEdit);
	CMenu mnuTop;
	CMenu* pPopup;

	CPoint pt(point);
	m_SnippetPos = pt;

	ScreenToClient(&pt);

	if (pt.x < m_pEdit->m_nLeftMarginWidth)
	{
		mnuTop.LoadMenu(IDR_POPUP_MARGIN);
		pPopup = mnuTop.GetSubMenu(0);
	}
	else
	{
		mnuTop.LoadMenu(IDR_CODE_CONTEXT);

		pPopup = mnuTop.GetSubMenu(0);

		CMenu* mediaMenu = pPopup->GetSubMenu(13);
		ASSERT(mediaMenu);
		mediaMenu->RemoveMenu(0, MF_BYPOSITION);

		pPopup->EnableMenuItem(ID_CONTEXT_GOTODEFINITION, m_pEdit->iDefinitionLine > -1 ? MF_ENABLED : MF_DISABLED);
		pPopup->EnableMenuItem(ID_CONTEXT_RETURNTOLASTLINE, m_ReturnLines.size() > 0 ? MF_ENABLED : MF_DISABLED);

		int pos = 666;
		for(UINT i = 0; i < Variables::m_DBPropertyMedia.size(); i++)
		{		
			TCHAR str[MAX_PATH];
			CString source = Variables::m_ProjectPath;
			CString to = Variables::m_DBPropertyMedia.at(i);
			PathRelativePathTo(str, source, FILE_ATTRIBUTE_DIRECTORY, to, FILE_ATTRIBUTE_NORMAL);
			CString path(str);
			mediaMenu->AppendMenu(MF_STRING, pos, str);
			pos++;
			mediaMenu->AppendMenu(MF_STRING, pos, Variables::m_DBPropertyMedia.at(i));
			pos++;
			mediaMenu->AppendMenu(MF_STRING, pos,Utilities::ExtractFilename(Variables::m_DBPropertyMedia.at(i)));
			pos++;
		}
	}

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this, NULL);
}

/*
* Returns the text from the editor component
*/
CString View::GetText() const
{
	ASSERT_VALID (m_pEdit);
	return m_pEdit->GetText();
}

/*
* Serializes the file to the harddisk
*/
void View::SaveFile(CArchive& ar)
{
	ASSERT_VALID(m_pEdit);
	m_pEdit->Serialize(ar);
}

/*
* Asks the user which line they want to navigate to
*/
void View::OnGoLine()
{
	CGotoLineDlg dlg (this);
	if (dlg.DoModal () != IDOK)
	{
		return;
	}

	ASSERT_VALID(m_pEdit);
	m_pEdit->GoToLine (dlg.m_nLineNumber);
}

/*
* Performs standard initialization routines
*/
void View::OnInitialUpdate() 
{
	CBCGPEditView::OnInitialUpdate();

	TCHAR  sResName[5]  = _T("#188");
	TCHAR sRestype[13] = _T("XML");
	HRSRC hres = FindResource(NULL, sResName, sRestype);
	int size = SizeofResource(NULL, hres);
	HGLOBAL    hbytes = LoadResource(NULL, hres);
        
	char const* pData = reinterpret_cast<char const*>(LockResource(hbytes)); 

	CString xml(pData, size);

	UnlockResource(hbytes);
	FreeResource(hbytes);

	m_pEdit->LoadXMLSettingsFromBuffer(xml);

	m_pEdit->EnableToolTips ();

	m_pEdit->SetLineNumbersMargin (TRUE, 30);
	m_pEdit->SetOutlineMargin(TRUE, 20);

	m_pEdit->EnableOutlining (TRUE);
	m_pEdit->EnableOutlineParser (TRUE);
	m_pEdit->EnableAutoOutlining(TRUE);

	Initialize();		
}

/*
* Creates the new editor component
*/
CBCGPEditCtrl* View::CreateEdit ()
{
	m_pEdit = new EditorControl;
	return m_pEdit;
}

/*
* Toggles a bookmark at the specified row
*/
void View::OnEditTogglebookmark() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->ToggleMarker (m_pEdit->GetCurRow (), g_dwBCGPEdit_BookMark);
}

/*
* Removes all of the breakpoints from the document
*/
void View::OnRemoveAllBreakpoints()
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->RemoveAllBreakpoints();
	m_Breakpoints.clear();
}

/*
* Toggles the breakpoint at the current row
* Removes the breakpoint from the vector if a breakpoint is being removed
*/
void View::OnToggleBreakpoint() 
{
	ASSERT_VALID (m_pEdit);
	int line = m_pEdit->GetCurRow();
	if(m_pEdit->ToggleBreakpoint())
	{
		m_Breakpoints.push_back(line);
	}
	else
	{
		std::vector <int>::iterator Iter;
		for ( Iter = m_Breakpoints.begin( ) ; Iter != m_Breakpoints.end( ) ; Iter++ )
		{
			int item = *Iter;
			if(item == line)
			{
				m_Breakpoints.erase(Iter);
				break;
			}			
		}
	}
}

/*
* Clears all of the bookmarks
*/
void View::OnClearAllBookmarks()
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->DeleteAllMarkers(g_dwBCGPEdit_BookMark);
}

/*
* Controls the visibility of the clear all button.
* It is only visible when there are markers in the document
*/
void View::OnUpdateClearAllBookmarks(CCmdUI* pCmdUI) 
{
	ASSERT_VALID (m_pEdit);
	pCmdUI->Enable(m_pEdit->HasMarkers(g_dwBCGPEdit_BookMark));	
}

/*
* Goes to the next marker
*/
void View::OnEditNextbookmark() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->GoToNextMarker(g_dwBCGPEdit_BookMark, TRUE);
}

/*
* Goes to the previous marker
*/
void View::OnEditPreviousbookmark() 
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->GoToNextMarker(g_dwBCGPEdit_BookMark,FALSE);
}

/**
* Initializes the control with the font and outlining styles
*/
void View::Initialize()
{
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	CDC* pDC = GetDC();

	CFont* pFont = pDC->SelectObject(CFont::FromHandle(hFont));
	pDC->SelectObject(pFont);
	::DeleteObject(hFont);

	LOGFONT lf;
	pFont->GetLogFont (&lf);

	CString strFontName(Settings::EditorFontFace);
	CopyMemory(lf.lfFaceName,(LPCTSTR)strFontName,(strFontName.GetLength() + 1) * sizeof(TCHAR));
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfHeight = -MulDiv (Settings::EditorFontSize, pDC->GetDeviceCaps(LOGPIXELSY), 72); 

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&lf);

	ASSERT_VALID (m_pEdit);
	m_pEdit->SetFont (&m_Font);
	m_pEdit->SetPrinterFont (&m_Font);

	// Also handle the outlining
	m_pEdit->GetOutlineParser()->RemoveAllBlockTypes();
	if(Settings::CollapseFunctions)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("FUNCTION"), _T("ENDFUNCTION"), _T("Function..."));
	}
	if(Settings::CollapseForLoops)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("FOR"), _T("NEXT"), _T("For..."));
	}
	if(Settings::CollapseCommentBlocks)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("REMSTART"), _T("REMEND"), _T("Remark..."));
	}
	if(Settings::CollapseWhileLoops)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("WHILE"), _T("ENDWHILE"), _T("While..."));
	}
	if(Settings::CollapseRepeatLoops)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("REPEAT"), _T("UNTIL"), _T("Repeat..."));
	}
	if(Settings::CollapseTypeDeclarations)
	{
		m_pEdit->GetOutlineParser()->AddBlockType(_T("TYPE"), _T("ENDTYPE"), _T("Type..."));
	}
	m_pEdit->UpdateAutoOutlining(0, m_pEdit->GetText().GetLength() - 1);
}

/**
* Opens the file specified
*/ 
void View::OpenFile(CString szFile)
{
	ASSERT_VALID (m_pEdit);
	m_pEdit->OpenFile(szFile);
}

/**
* Used to control the visibility of the surround menu item
* It can't be visible when no text is selected
*/
void View::OnUpdateSurround(CCmdUI* pCmdUI)
{
	ASSERT_VALID (m_pEdit);
	int start, end;
	m_pEdit->GetSel(start, end);
	pCmdUI->Enable(start != end);
}

/**
* Called by the insert snippet dialog to insert text into the control
* a = The text to insert
* b = The type of insertion (insert or surround)
*/
LRESULT View::OnInsertData(WPARAM a, LPARAM b)
{
	ASSERT_VALID (m_pEdit);
	TCHAR* aa = (TCHAR*)a;
	if(b == 0) // Insert snippet
	{
		m_pEdit->InsertText(aa, GetEditCtrl()->GetCurOffset());
	}
	else // Surround
	{
		TCHAR* bb = (TCHAR*)b;
		int start, end;
		m_pEdit->GetSel(start, end);
		if(start > end)
		{
			int temp = start;
			start = end;
			end = temp;
		}	
		m_pEdit->SetSel(-1, -1);
		m_pEdit->InsertText(bb, end);
		m_pEdit->InsertText(aa, start);
		delete bb;
	}
	delete aa;
	return TRUE;
}

/**
* Creates a new non-modal 'Insert...' code snippet window
*/
void View::OnInsert()
{
	sWnd = new SnippetWnd(false);
	CRect rec(m_SnippetPos.x,m_SnippetPos.y,m_SnippetPos.x+100,m_SnippetPos.y+100);
	sWnd->Create(NULL, NULL, WS_POPUP | WS_VISIBLE | MFS_SYNCACTIVE | MFS_4THICKFRAME, rec, this);
}

/**
* Creates a new non-modal 'Surround with...' code snippet window
*/
void View::OnSurround()
{
	sWnd = new SnippetWnd(true);
	CRect rec(m_SnippetPos.x,m_SnippetPos.y,m_SnippetPos.x+100,m_SnippetPos.y+100);
	sWnd->Create(NULL, NULL, WS_POPUP | WS_VISIBLE | MFS_SYNCACTIVE | MFS_4THICKFRAME, rec, this);
}

/**
* Updates the editor control from the settings file for the settings that need to be applied explicitly
*/
void View::UpdateEditorFromSettings()
{
	ASSERT_VALID (m_pEdit);
	if(Settings::MouseScroll > -1)
	{
		m_pEdit->SetMouseWheelSpeed(Settings::MouseScroll);
	}
	else
	{
		m_pEdit->SetMouseWheelSpeed(1);
	}
	// Tab size
	m_pEdit->SetTabSize(Settings::TabWidth);
	m_pEdit->m_nIndentSize = Settings::TabWidth;
	// Replace Tabs
	m_pEdit->SetKeepTabs(!Settings::ReplaceTabs);
}

BOOL View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// View drawing
void View::OnDraw(CDC* /*pDC*/)
{
	Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

// View printing
void View::OnFilePrintPreview()
{
	BCGPPrintPreview (this);
}

BOOL View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) { /* TODO: add extra initialization before printing */ }

void View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) { /* TODO: add cleanup after printing */ }

// View diagnostics
#ifdef _DEBUG
void View::AssertValid() const
{
	CView::AssertValid();
}

void View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Doc* View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Doc)));
	return (Doc*)m_pDocument;
}
#endif //_DEBUG
