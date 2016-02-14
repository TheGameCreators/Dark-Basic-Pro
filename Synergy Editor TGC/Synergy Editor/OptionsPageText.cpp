#include "StdAfx.h"
#include "Synergy Editor.h"
#include "OptionsPageText.h"
#include "Settings.h"
#include "View.h"

IMPLEMENT_DYNCREATE(OptionsPageText, CBCGPPropertyPage)

OptionsPageText::OptionsPageText() : CBCGPPropertyPage(OptionsPageText::IDD)
{}

BOOL OptionsPageText::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);
     return CDialog::PreTranslateMessage(pMsg);
}

BOOL OptionsPageText::OnInitDialog()
{
	CWaitCursor wait;
	CBCGPPropertyPage::OnInitDialog();

	CMouseScroll.InsertString(-1,_T("Default"));
	CMouseScroll.InsertString(-1,_T("1"));
	CMouseScroll.InsertString(-1,_T("2"));
	CMouseScroll.InsertString(-1,_T("3"));
	CMouseScroll.InsertString(-1,_T("4"));
	CMouseScroll.InsertString(-1,_T("5"));
	CMouseScroll.InsertString(-1,_T("6"));
	CMouseScroll.InsertString(-1,_T("7"));
	CMouseScroll.InsertString(-1,_T("8"));
	CMouseScroll.InsertString(-1,_T("9"));
	CMouseScroll.InsertString(-1,_T("10"));
	CMouseScroll.InsertString(-1,_T("11"));
	CMouseScroll.InsertString(-1,_T("12"));

	CString MouseScroll(_T("Default"));
	if(Settings::MouseScroll > -1)
	{
		MouseScroll.Empty();
		MouseScroll.Format(_T("%d"), Settings::MouseScroll);
	}
	CMouseScroll.SelectString(-1, MouseScroll);

	cCommandStyle.InsertString(-1, _T("In Uppercase"));
	cCommandStyle.InsertString(-1, _T("In Lowercase"));
	cCommandStyle.InsertString(-1, _T("In Mixedcase"));

	if(Settings::IntellisenseCase == 0)
	{
		cCommandStyle.SelectString(-1, _T("In Uppercase"));
	}
	else if(Settings::IntellisenseCase == 1)
	{
		cCommandStyle.SelectString(-1, _T("In Lowercase"));
	}
	else
	{
		cCommandStyle.SelectString(-1, _T("In Mixedcase"));
	}

	cTabWidth.InsertString(-1,_T("1"));
	cTabWidth.InsertString(-1,_T("2"));
	cTabWidth.InsertString(-1,_T("3"));
	cTabWidth.InsertString(-1,_T("4"));
	cTabWidth.InsertString(-1,_T("5"));
	cTabWidth.InsertString(-1,_T("6"));
	cTabWidth.InsertString(-1,_T("7"));

	CString TabWidth(_T("4"));
	if(Settings::TabWidth != 4)
	{
		TabWidth.Empty();
		TabWidth.Format(_T("%d"), Settings::TabWidth);
	}
	cTabWidth.SelectString(-1, TabWidth);

	cStatusHelp.SetCheck(Settings::AllowStatusBarHelp ? BST_CHECKED : BST_UNCHECKED);
	CToolTips.SetCheck(Settings::AllowToolTips ? BST_CHECKED : BST_UNCHECKED);
	CRedrawCaret.SetCheck(Settings::AllowInvalidateOnCaretMove ? BST_CHECKED : BST_UNCHECKED);
	cBracketBalance.SetCheck(Settings::AllowBracketBalanceCheck ? BST_CHECKED : BST_UNCHECKED);
	cIntellisense.SetCheck(Settings::AllowAutoIntellisense ? BST_CHECKED : BST_UNCHECKED);
	cIndentLine.SetCheck(Settings::AllowAutoIndentBlock ? BST_CHECKED : BST_UNCHECKED);
	cCloseBlock.SetCheck(Settings::AllowAutoCloseBlock ? BST_CHECKED : BST_UNCHECKED);
	cHighlight.SetCheck(Settings::HighlightCurrentLine ? BST_CHECKED : BST_UNCHECKED);
	cReturnLine.SetCheck(Settings::GoToRememberedLine  ? BST_CHECKED : BST_UNCHECKED);
	cConcat.SetCheck(Settings::AutoLineConcat ? BST_CHECKED : BST_UNCHECKED);
	cReplaceTabs.SetCheck(Settings::ReplaceTabs ? BST_CHECKED : BST_UNCHECKED);

	cFunctions.SetCheck(Settings::CollapseFunctions ? BST_CHECKED : BST_UNCHECKED);
	cForLoops.SetCheck(Settings::CollapseForLoops ? BST_CHECKED : BST_UNCHECKED);
	cWhileLoops.SetCheck(Settings::CollapseWhileLoops ? BST_CHECKED : BST_UNCHECKED);
	cRepeatLoops.SetCheck(Settings::CollapseRepeatLoops ? BST_CHECKED : BST_UNCHECKED);
	cCommentBlocks.SetCheck(Settings::CollapseCommentBlocks ? BST_CHECKED : BST_UNCHECKED);
	cTypeDeclarations.SetCheck(Settings::CollapseTypeDeclarations ? BST_CHECKED : BST_UNCHECKED);

	if(m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool( &cStatusHelp, _T("Provide command help in the status bar"));
		m_ToolTip.AddTool( &CToolTips, _T("Provide command help for the command under the cursor as a tooltip"));
		m_ToolTip.AddTool( &CRedrawCaret, _T("Redraw the code window on a caret move, this is needed for the help system to work correctly"));
		m_ToolTip.AddTool( &cBracketBalance, _T("Check and highlight bracket mismatch errors"));
		m_ToolTip.AddTool( &cIntellisense, _T("Allow the intellisense window to popup if there are suggestions available"));
		m_ToolTip.AddTool( &cIndentLine, _T("Allow DarkBASIC Professional Editor increase the code indentation on creation of new code blocks"));
		m_ToolTip.AddTool( &cCloseBlock, _T("Allow DarkBASIC Professional Editor to automatically close newly created code blocks"));
		m_ToolTip.AddTool( &cCommandStyle, _T("The style of the intellisense"));
		m_ToolTip.AddTool( &CMouseScroll, _T("Choose the number of lines horizontal scrolling skips"));
		m_ToolTip.AddTool( &cFunctions, _T("Choose whether functions are collapsable"));
		m_ToolTip.AddTool( &cForLoops, _T("Choose whether for loops are collapsable"));
		m_ToolTip.AddTool( &cWhileLoops, _T("Choose whether while loops are collapsable"));
		m_ToolTip.AddTool( &cRepeatLoops, _T("Choose whether repeat loops are collapsable"));
		m_ToolTip.AddTool( &cCommentBlocks, _T("Choose whether comment blocks are collapsable"));
		m_ToolTip.AddTool( &cTypeDeclarations, _T("Choose whether type declarations are collapsable"));
		m_ToolTip.AddTool( &cHighlight, _T("Choose whether the current line should be highlighted"));
		m_ToolTip.AddTool( &cReturnLine, _T("Choose whether the last edited line of a document should be returned to when the document is reopened"));
		m_ToolTip.AddTool( &cConcat, _T("Choose whether the editor should support the line concatenation feature"));
		m_ToolTip.AddTool( &cReplaceTabs, _T("Choose whether the editor should replace tab characters with (padded) spaces"));

		m_ToolTip.Activate(TRUE);
	}

	OptionsPageText::SetModified(FALSE);

	return TRUE;
}

void OptionsPageText::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK3, cStatusHelp);
	DDX_Control(pDX, IDC_CHECK1, CToolTips);
	DDX_Control(pDX, IDC_CHECK2, CRedrawCaret);
	DDX_Control(pDX, IDC_CHECK4, cBracketBalance);
	DDX_Control(pDX, IDC_INTELLISENSE, cIntellisense);
	DDX_Control(pDX, IDC_INDENT_BLOCK, cIndentLine);
	DDX_Control(pDX, IDC_CLOSE_BLOCK, cCloseBlock);
	DDX_Control(pDX, IDC_COMMAND_STYLE, cCommandStyle);
	DDX_Control(pDX, IDC_MOUSE_SCROLL, CMouseScroll);
	DDX_Control(pDX, IDC_FOLD_FUNCTION, cFunctions);
	DDX_Control(pDX, IDC_FOLD_TYPES, cTypeDeclarations);
	DDX_Control(pDX, IDC_FOLD_FOR, cForLoops);
	DDX_Control(pDX, IDC_FOLD_WHILE, cWhileLoops);
	DDX_Control(pDX, IDC_FOLD_REPEAT, cRepeatLoops);
	DDX_Control(pDX, IDC_FOLD_COMMENTS, cCommentBlocks);
	DDX_Control(pDX, IDC_HIGHLIGHT_LINE, cHighlight);
	DDX_Control(pDX, IDC_RETURN_LINE, cReturnLine);
	DDX_Control(pDX, IDC_LINE_CONCAT, cConcat);
	DDX_Control(pDX, IDC_TAB_WIDTH, cTabWidth);
	DDX_Control(pDX, IDC_REPLACE_TABS, cReplaceTabs);
}

void OptionsPageText::OnChange()
{
	OptionsPageText::SetModified(TRUE);
}

void OptionsPageText::OnOK()
{
	CBCGPPropertyPage::OnOK();

	Settings::AllowStatusBarHelp = (cStatusHelp.GetCheck() == BST_CHECKED);
	Settings::AllowToolTips = (CToolTips.GetCheck() == BST_CHECKED);
	Settings::AllowInvalidateOnCaretMove = (CRedrawCaret.GetCheck() == BST_CHECKED);
	Settings::AllowBracketBalanceCheck = (cBracketBalance.GetCheck() == BST_CHECKED);
	Settings::AllowAutoIntellisense = (cIntellisense.GetCheck() == BST_CHECKED);
	Settings::AllowAutoIndentBlock = (cIndentLine.GetCheck() == BST_CHECKED);
	Settings::AllowAutoCloseBlock = (cCloseBlock.GetCheck() == BST_CHECKED);

	Settings::CollapseFunctions = (cFunctions.GetCheck() == BST_CHECKED);
	Settings::CollapseForLoops = (cForLoops.GetCheck() == BST_CHECKED);
	Settings::CollapseWhileLoops =(cWhileLoops.GetCheck() == BST_CHECKED);
	Settings::CollapseRepeatLoops = (cRepeatLoops.GetCheck() == BST_CHECKED);
	Settings::CollapseCommentBlocks = (cCommentBlocks.GetCheck() == BST_CHECKED);
	Settings::CollapseTypeDeclarations = (cTypeDeclarations.GetCheck() == BST_CHECKED);
	
	Settings::HighlightCurrentLine = (cHighlight.GetCheck() == BST_CHECKED);
	Settings::GoToRememberedLine = (cReturnLine.GetCheck() == BST_CHECKED);
	Settings::AutoLineConcat = (cConcat.GetCheck() == BST_CHECKED);
	Settings::ReplaceTabs = (cReplaceTabs.GetCheck() == BST_CHECKED);
	
	CString Case;
	cCommandStyle.GetWindowText(Case);
	if(Case == _T("In Uppercase"))
	{
		Settings::IntellisenseCase = 0;
	}
	else if(Case == _T("In Lowercase"))
	{
		Settings::IntellisenseCase = 1;
	}
	else
	{
		Settings::IntellisenseCase = 2;
	}

	CString MouseScroll;
	CMouseScroll.GetWindowText(MouseScroll);
	if(MouseScroll == _T("Default"))
	{
		Settings::MouseScroll = -1;
	}
	else
	{	
		Settings::MouseScroll = _ttoi(MouseScroll);
	}	

	CString TabWidth;
	cTabWidth.GetWindowText(TabWidth);
	Settings::TabWidth = _ttoi(TabWidth);

	App* app = (App*)AfxGetApp();
	POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();
	while(posDocument != NULL)
	{
		Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
		POSITION posView = pDoc->GetFirstViewPosition ();
		View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
		if (pView != NULL)
		{
			pView->UpdateEditorFromSettings();
		}
	}
}

// The default MFC implementation of OnApply() would call OnOK().
BOOL OptionsPageText::OnApply()
{
	return CBCGPPropertyPage::OnApply();
}

BEGIN_MESSAGE_MAP(OptionsPageText, CBCGPPropertyPage)
	ON_CONTROL(BN_CLICKED, IDC_CHECK1, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_CHECK2, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_CHECK3, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_CHECK4, OnChange)
	ON_BN_CLICKED(IDC_CHECK3, OnChange)
	ON_BN_CLICKED(IDC_INTELLISENSE, OnChange)
	ON_CBN_SELCHANGE(IDC_COMMAND_STYLE, OnChange)
	ON_BN_CLICKED(IDC_HIGHLIGHT_LINE, OnChange)
	ON_BN_CLICKED(IDC_RETURN_LINE, OnChange)
	ON_BN_CLICKED(IDC_INDENT_BLOCK, OnChange)
	ON_BN_CLICKED(IDC_CLOSE_BLOCK, OnChange)
	ON_CBN_SELCHANGE(IDC_MOUSE_SCROLL, OnChange)
	ON_BN_CLICKED(IDC_FOLD_FUNCTION, OnChange)
	ON_BN_CLICKED(IDC_FOLD_WHILE, OnChange)
	ON_BN_CLICKED(IDC_FOLD_COMMENTS, OnChange)
	ON_BN_CLICKED(IDC_FOLD_TYPES, OnChange)
	ON_BN_CLICKED(IDC_FOLD_FOR, OnChange)
	ON_BN_CLICKED(IDC_FOLD_REPEAT, OnChange)
	ON_BN_CLICKED(IDC_CHECK2, OnChange)
	ON_BN_CLICKED(IDC_CHECK4, OnChange)
	ON_BN_CLICKED(IDC_CHECK1, OnChange)
	ON_BN_CLICKED(IDC_LINE_CONCAT, OnChange)
END_MESSAGE_MAP()
