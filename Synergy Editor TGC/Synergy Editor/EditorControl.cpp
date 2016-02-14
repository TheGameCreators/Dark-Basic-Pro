#include "StdAfx.h"
#include "EditorControl.h"
#include "MainFrm.h"
#include "Settings.h"
#include "HTMLDocument.h"
#include "SemanticParser.h"

BEGIN_MESSAGE_MAP(EditorControl, CBCGPEditCtrl)
	//{{AFX_MSG_MAP(CMyTreeCtrl)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP     
END_MESSAGE_MAP()

EditorControl::EditorControl(void)
{
	m_HelpURL = Settings::MainHelpURL;
	toolTipPosition = 0;
	m_ToolTipText = _T("");
	//pIntelliSenseWnd = NULL;
	m_IntelliWord = _T("");
	lockWord = false;
	m_SuppressLineUpdate = true;
	m_SourceFile = NULL;
	iDefinitionLine = -1;
	m_bEnableSymSupport = false;

	m_ImageBreak.Load (IDB_BREAK);
	m_ImageBreak.SetSingleImage ();

	m_strWordDelimeters = _T(" \t\n,/<>:\"'`%^&*()-+=!");
	m_bKeepTabs = !Settings::ReplaceTabs;
	
	if(Settings::MouseScroll > -1)
	{
		SetMouseWheelSpeed(Settings::MouseScroll);
	}

	op = new OutlineParser();
}

EditorControl::~EditorControl(void){}

void EditorControl::SetSourceFile(SourceFile* sf)
{
	m_SourceFile = sf;
}

void EditorControl::ReleaseLine()
{
	m_SuppressLineUpdate = false;
}

OutlineParser* EditorControl::GetOutlineParser()
{	
	return op;
}

CBCGPOutlineParser* EditorControl::CreateOutlineParser() const
{	
	op->m_bCaseSensitive = FALSE;
	return op;
}

BOOL EditorControl::OnGetTipText(CString& strTipString)
{
	if(!Settings::AllowToolTips)
	{
		return FALSE;
	}

	POINT coord;
	GetCursorPos(&coord);
	ScreenToClient(&coord);
	toolTipPosition = HitTest((CPoint)coord);

	int line = LineFromChar(toolTipPosition);
	RedrawText(line, line);

	if(m_ToolTipText == _T(""))
	{
		return FALSE;
	}

	strTipString = m_ToolTipText;
	return TRUE;
}

//void EditorControl::Find(CString FindName, BOOL bSearchDown, BOOL bMatchCase, BOOL bMatchWholeWord)
//{
//	FindText(FindName, bSearchDown, bMatchCase, bMatchWholeWord);
//}

void EditorControl::RemoveAllBreakpoints()
{
	DeleteAllMarkers(g_dwBreakPointType | g_dwColorBreakPointType);
}

BOOL EditorControl::ToggleBreakpoint()
{
	int nCurrRow = GetCurRow();
	
	BOOL bMarkerSet = ToggleMarker (nCurrRow, g_dwBreakPointType, NULL, FALSE);

	if (bMarkerSet)
	{
		SetLineColorMarker (nCurrRow, RGB (255, 255, 255), 
			                RGB (127, 0, 0), TRUE, 0, g_dwColorBreakPointType);
	}
	else
	{
		DeleteMarker (nCurrRow, g_dwColorBreakPointType);
	}

	return bMarkerSet;
}

void EditorControl::CommentSelection(bool comment)
{
	// Get selection
	int nStartChar, nEndChar;
	GetSel(nStartChar, nEndChar);
	if(nStartChar ==  -1)
	{
		return;
	}
	int temp = min(nStartChar, nEndChar);
	nEndChar = max(nStartChar, nEndChar);
	nStartChar = temp;
	// Calculate actual line start
	for(int i=nStartChar; i >= 0; i--)
	{
		if(i<0) break;
		if(GetText().GetAt(i) == '\n')
		{
			nStartChar = i + 1;
			break;
		}
		else if(i == 0)
		{
			nStartChar = 0;
		}
	}
	CString text = GetText().Mid(nStartChar, nEndChar - nStartChar);
	// Delete old selection
	DeleteText(nStartChar, nEndChar, FALSE);
	// Process
	bool doIt = false;
	for(int i=0; i<text.GetLength(); i++)
	{
		if(i == 0 || doIt == true)
		{
			if(comment)
			{
				if(text.Mid(i, 2) != _T("//"))
				{
					text.Insert(i, _T("// "));
				}
			}
			else
			{
				if(text.Mid(i, 3) == _T("// "))
				{
					text.Delete(i, 3);
				}
				else if(text.Mid(i, 2) == _T("//"))
				{
					text.Delete(i, 2);
				}
			}
			doIt = false;
		}
		else if(text.GetAt(i) == '\n')
		{
			doIt = true;
		}
	}
	// Insert
	InsertText(text, nStartChar, FALSE);
	// New selection
	SetSel(nStartChar, nStartChar + text.GetLength());
}

void EditorControl::OnSetCaret()
{	
	CBCGPEditCtrl::OnSetCaret();
	// Save line position
	if(m_SourceFile != NULL)
	{
		m_SourceFile->line = GetCurRow();
	}

	if(HighlightCurrentLine())
	{
		return; // This feature depends on a redraw
	}
	if(blockCaretChange)
	{
		blockCaretChange = false;
		return;
	}
	if(!Settings::AllowInvalidateOnCaretMove)
	{
		return;
	}	
	int line = LineFromChar(GetCurOffset());
	RedrawText(line, line);
}

void EditorControl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (point.x < m_nLeftMarginWidth)
	{
		ToggleBreakpoint();
	}
	
	CBCGPEditCtrl::OnLButtonDblClk(nFlags, point);
}

void EditorControl::OnDrawMarker (CDC* pDC, CRect rectMarker, const CBCGPEditMarker* pMarker)
{
	if (pMarker->m_dwMarkerType & g_dwBreakPointType)
	{
		rectMarker.DeflateRect (1, 1);
		rectMarker.left = rectMarker.left + rectMarker.Width () / 2 - rectMarker.Height () / 2;
		rectMarker.right = rectMarker.left + rectMarker.Height ();

		if (m_bGradientMarkers && globalData.m_nBitsPerPixel >= 32)
		{
			m_ImageBreak.DrawEx (pDC, rectMarker, 0,
				CBCGPToolBarImages::ImageAlignHorzCenter,
				CBCGPToolBarImages::ImageAlignVertTop);
		}
		else
		{
			CBrush br (RGB (127, 0, 0));
			CPen pen (PS_SOLID, 1, RGB (127, 0, 0));
			CBrush* pBrOld = pDC->SelectObject (&br);
			CPen* pOldPen = pDC->SelectObject (&pen);

			pDC->Ellipse (rectMarker);
			pDC->SelectObject (pBrOld);
			pDC->SelectObject (pOldPen);
		}
	}	
	else
	{
		CBCGPEditCtrl::OnDrawMarker (pDC, rectMarker, pMarker);
	}
}

void EditorControl::FillList(CObList& lstIntelliSenseData)
{
	m_IntelliWord.MakeUpper();
	TCHAR first = m_IntelliWord.GetAt(0);	
	switch(first)
	{
	case _T('A'):
		m_InKeywords = Keywords::m_AKeywords;
		break;
	case _T('B'):
		m_InKeywords = Keywords::m_BKeywords;
		break;
	case _T('C'):
		m_InKeywords = Keywords::m_CKeywords;
		break;
	case _T('D'):
		m_InKeywords = Keywords::m_DKeywords;
		break;
	case _T('E'):
		m_InKeywords = Keywords::m_EKeywords;
		break;
	case _T('F'):
		m_InKeywords = Keywords::m_FKeywords;
		break;
	case _T('G'):
		m_InKeywords = Keywords::m_GKeywords;
		break;
	case _T('H'):
		m_InKeywords = Keywords::m_HKeywords;
		break;
	case _T('I'):
		m_InKeywords = Keywords::m_IKeywords;
		break;
	case _T('J'):
		m_InKeywords = Keywords::m_JKeywords;
		break;
	case _T('K'):
		m_InKeywords = Keywords::m_KKeywords;
		break;
	case _T('L'):
		m_InKeywords = Keywords::m_LKeywords;
		break;
	case _T('M'):
		m_InKeywords = Keywords::m_MKeywords;
		break;
	case _T('N'):
		m_InKeywords = Keywords::m_NKeywords;
		break;
	case _T('O'):
		m_InKeywords = Keywords::m_OKeywords;
		break;
	case _T('P'):
		m_InKeywords = Keywords::m_PKeywords;
		break;
	case _T('Q'):
		m_InKeywords = Keywords::m_QKeywords;
		break;
	case _T('R'):
		m_InKeywords = Keywords::m_RKeywords;
		break;
	case _T('S'):
		m_InKeywords = Keywords::m_SKeywords;
		break;
	case _T('T'):
		m_InKeywords = Keywords::m_TKeywords;
		break;
	case _T('U'):
		m_InKeywords = Keywords::m_UKeywords;
		break;
	case _T('V'):
		m_InKeywords = Keywords::m_VKeywords;
		break;
	case _T('W'):
		m_InKeywords = Keywords::m_WKeywords;
		break;
	case _T('X'):
		m_InKeywords = Keywords::m_XKeywords;
		break;	
	case _T('Y'):
		m_InKeywords = Keywords::m_YKeywords;
		break;
	case _T('Z'):
		m_InKeywords = Keywords::m_ZKeywords;
		break;
	default:
		m_InKeywords = Keywords::m_MiscKeywords;
		break;
	}

	for (InIter = m_InKeywords.begin(); InIter != m_InKeywords.end(); InIter++ )
	{
		Keyword* keyword = *InIter;
		if(keyword->keyword.Mid(0, m_IntelliWord.GetLength()) == m_IntelliWord)
		{
			if(keyword->keyword.Find(' ') > -1)
			{
				CBCGPIntelliSenseData* pData = new CBCGPIntelliSenseData;
				CString item(keyword->keyword);
				if(Settings::IntellisenseCase == 0)
				{
					 item.MakeUpper();
				}
				else if(Settings::IntellisenseCase == 1)
				{
					item.MakeLower();
				}
				else
				{
					item = Utilities::MixedCase(item);
				}
				pData->m_strItemName = item;
				lstIntelliSenseData.AddTail(pData);
			}
		}
	}
}

void EditorControl::FindWordStartFinish (int nCaretOffset, const CString& strBuffer,
		                      int& nStartOffset, int& nEndOffset, BOOL bSkipSpaces) const
{
	if(m_pIntelliSenseWnd != NULL)
	{
		nStartOffset = intellisenseStart;
		nEndOffset = GetCurOffset();
	}
	else
	{
		CBCGPEditCtrl::FindWordStartFinish (nCaretOffset, strBuffer,
		                      nStartOffset, nEndOffset, bSkipSpaces);
	}
}

void EditorControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar < VK_LEFT && nChar > VK_DOWN)
	{
		blockCaretChange = true;
	}

	bool allowBaseHandle = true;

	int tabLevel = -1;
	CString CloseString;

	if(m_pIntelliSenseWnd == NULL && lockWord)
	{
		lockWord = false;
		m_IntelliWord = _T("");
	}

	if(nChar == VK_F1)
	{
		CString entry = Settings::DBPLocation + _T("Help\\") + m_HelpURL;
		App* app = (App*)AfxGetApp();

		if(Settings::OpenHelpInNewTab)
		{
			app->pHelpTemplate->OpenDocumentFile(entry);
		}
		else
		{
			if(app->pHelpTemplate->GetFirstDocPosition() == NULL)
			{
				app->pHelpTemplate->OpenDocumentFile(entry);
			}
			else
			{
				POSITION pos =  app->pHelpTemplate->GetFirstDocPosition();
				HTMLDocument* doc = DYNAMIC_DOWNCAST (HTMLDocument, app->pHelpTemplate->GetNextDoc(pos));
				doc->OnOpenDocument(entry);
			}
		}		
	}
	else if(nChar == VK_F2)
	{
		Invalidate(TRUE);			
	}
	else if(m_pIntelliSenseWnd == NULL && ((nChar == VK_SPACE && GetKeyState(VK_CONTROL) < 0) || (nChar == VK_SPACE && Settings::AllowAutoIntellisense) || (nChar == 190 && GetKeyState(VK_CONTROL) < 0) || (nChar == 190 && Settings::AllowAutoIntellisense)))
	{
		allowBaseHandle = false;
		CObList lstIntelliSenseData;
		if(nChar == 190)
		{
			int pos = GetCurOffset();
			CString TypeText;
			bool found = false;
			for(int i=pos-1; i > -1; i--) // Try and extract the previous word
			{
				if(!IsCharAlpha(m_strBuffer.GetAt(i)) || i == 0)
				{
					TypeText = m_strBuffer.Mid(i + 1, pos - (i + 1));
					found = true;
					break;
				}
			}
			if(found && SemanticParser::m_Mutex.Lock(1)) // Try and get a lock on the semantic data
			{		
				CString name = TypeText;
				TypeText.MakeUpper();

				// Check to see if the word is a variable that is a type
				std::vector<SemanticVariable*>::iterator mySVVectorIterator;
				for(mySVVectorIterator = SemanticParser::m_vVariables.begin(); mySVVectorIterator != SemanticParser::m_vVariables.end(); mySVVectorIterator++)
				{
					SemanticVariable* variable = *mySVVectorIterator;
					if(variable->match == TypeText)
					{
						// Check the variables type against all of the types
						std::vector<SemanticType*>::iterator mySTVectorIterator;					
						for(mySTVectorIterator = SemanticParser::m_vTypes.begin(); mySTVectorIterator != SemanticParser::m_vTypes.end(); mySTVectorIterator++)
						{
							SemanticType* type = *mySTVectorIterator;
							if(variable->typeMatch == type->match)
							{
								// List the variables held by the type
								std::vector<SemanticVariable*>::iterator typeVariableIt;
								for(typeVariableIt = type->variables.begin(); typeVariableIt != type->variables.end(); typeVariableIt++)
								{
									SemanticVariable* typeVar = *typeVariableIt;
									CBCGPIntelliSenseData* pData = new CBCGPIntelliSenseData;
									pData->m_strItemName = name + _T(".") + typeVar->variable;
									lstIntelliSenseData.AddTail(pData);
								}
								break;
							}
						}
					}
				}
				SemanticParser::m_Mutex.Unlock();
			}
		}
		else if(m_IntelliWord.GetLength() > 0 && m_IntelliWord != _T(" "))
		{
			m_IntelliWord += _T(" ");			
			FillList(lstIntelliSenseData);			
		}
		if(lstIntelliSenseData.GetCount() > 0)
		{
			CPoint ptTopLeft (0, 0);
			OffsetToPoint (GetCurOffset(), ptTopLeft);
			ClientToScreen (&ptTopLeft);

			HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
			CDC* pDC = GetDC();
			CFont* m_pIntelliSenseLBFont = pDC->SelectObject(CFont::FromHandle(hFont));
			::DeleteObject(hFont);

			LOGFONT lf;
			m_pIntelliSenseLBFont->GetLogFont (&lf);

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

			CFont newFont;
			newFont.CreateFontIndirect(&lf);

			m_pIntelliSenseWnd = new CBCGPIntelliSenseWnd;
			m_pIntelliSenseWnd->Create (lstIntelliSenseData, WS_POPUP | WS_VISIBLE | MFS_SYNCACTIVE | MFS_4THICKFRAME, ptTopLeft, this, &newFont);
			lockWord = true;
		}
	}
	else if(nChar == VK_RETURN)
	{
		CString previousLine;
		CString text = m_strBuffer;
		int tabCount = 0;
		int offset = GetCurOffset();
		for(int i=(offset - 1); i >= 0; i--)
		{
			if(i<0) break;
			if(text.GetAt(i) == '\n' || i == 0)
			{
				previousLine = text.Mid(i, GetCurOffset() - i);
				break;
			}
			else if(text.GetAt(i) == '\t')
			{
				tabCount++;
			}
		}

		previousLine = previousLine.Trim();

		// Call semantic parser if allowed
		if(Settings::AllowCheckLine)
		{
			SemanticData* dt = new SemanticData();
			CString toCheck(previousLine);
			toCheck +=  "\n";
			dt->Text = toCheck;
			dt->Filename = _T("Temp");
			dt->mode = 1;
			AfxBeginThread(SemanticParser::ParseThread, dt);
		}

		previousLine.MakeUpper();

		if(previousLine.Left(2) == _T("IF") && previousLine.Find(_T("THEN")) == -1)
		{
			tabLevel = tabCount + 1;
			CloseString = _T("ENDIF");
		}
		else if(previousLine.Left(3) == _T("FOR"))
		{
			tabLevel = tabCount + 1;
			CloseString = _T("NEXT");
		}
		else if(previousLine.Left(8) == _T("FUNCTION"))
		{
			tabLevel = tabCount + 1;
			CloseString = _T("ENDFUNCTION");
		}
		else if(previousLine.Left(2) == _T("DO"))
		{
			tabLevel = tabCount + 1;
			CloseString = _T("LOOP");
		}
		else if(previousLine.Left(6) == _T("REPEAT"))
		{
			tabLevel = tabCount + 1;
			CloseString = _T("UNTIL");
		}
		else if(previousLine.Left(5) == _T("WHILE"))
		{
			tabLevel = tabCount + 1;
			CloseString = _T("ENDWHILE");
		}

		if(Settings::IntellisenseCase == 0)
		{
			CloseString.MakeUpper();
		}
		else if(Settings::IntellisenseCase == 1)
		{
			CloseString.MakeLower();
		}
		else
		{
			CloseString = Utilities::MixedCase(CloseString);
		}
	}

	if(allowBaseHandle)
	{
		CBCGPEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}

	if(tabLevel > -1)
	{		
		int offset = GetCurOffset();

		// Insert the closing block string
		if(Settings::AllowAutoCloseBlock)
		{
			InsertChar('\n', FALSE);
			InsertText(CloseString, GetCurOffset(), FALSE);		
		}
	
		// Insert the extra tab
		if(Settings::AllowAutoIndentBlock)
		{
			InsertText(_T("\t"), offset, TRUE);
			SetCaret(offset + 1);
		}
	}
}

bool EditorControl::HighlightCurrentLine()
{
	if(!Settings::HighlightCurrentLine)
	{
		if(lastHighlightLinePos > -1) // They may have just turned the option off, so we need to clear old one
		{
			DeleteMarker(lastHighlightLinePos,g_dwColorHighlightType,FALSE);
			lastHighlightLinePos = -1;
		}
		return false;
	}

	int nCurrRow = GetCurRow();

	if(lastHighlightLinePos != nCurrRow)
	{	
		if(lastHighlightLinePos > -1)
		{
			DeleteMarker(lastHighlightLinePos,g_dwColorHighlightType,FALSE);
		}

		SetLineColorMarker (nCurrRow, RGB (255, 255, 255), 
								RGB (211, 211, 211), TRUE, 0, g_dwColorHighlightType);
		
		lastHighlightLinePos = nCurrRow;

		Invalidate();
		return true;
	}
	return false;
}

void EditorControl::OnDrawText (CDC* pDC)
{
	ASSERT_VALID (pDC);

	if (m_strBuffer.IsEmpty ())
	{
		return;
	}

	BOOL bIsPrint = pDC->IsPrinting ();

	CRect rectText;

	if (bIsPrint)
	{
		rectText = m_rectText;
	}
	else
	{
		CRect rectClip; 
		GetClipBox (pDC->m_hDC, rectClip);

		rectText.IntersectRect (rectClip, m_rectText);
		rectText.left = m_rectText.left + 1;
		rectText.right = m_rectText.right;
	}

	if (rectText.IsRectEmpty ())
	{
		return;
	}

	rectText.InflateRect (1, 1);

	CRect rectRow = rectText;

	int nVertOffset = m_nScrollOffsetVert * m_nLineHeight;

	rectRow.OffsetRect (0, -nVertOffset);

	rectRow.DeflateRect (0, 1);

	rectRow.bottom = rectRow.top + m_nLineHeight;

	CString strTabs (_T (' '), m_nTabSize);
	m_nTabLogicalSize = pDC->GetTextExtent (strTabs).cx;

	CString strSlashes (_T ('/'), m_nTabSize);
	int m_nSlashesLogicalSize = pDC->GetTextExtent (strSlashes).cx;

	if (m_nTabLogicalSize < m_nSlashesLogicalSize)
	{
		m_nTabLogicalSize = m_nSlashesLogicalSize;
	}

	//////////////////////////////////////////////////////////////////////////

	// determine start and end offsets of the visible part of buffer

	CPoint ptTopLeft (rectText.left, rectText.top);
	CPoint ptBottomRight (rectText.right - 1, rectText.bottom - 1);

	int nStartOffset = m_nTopVisibleOffset; 
	int nEndOffset = bIsPrint ? m_nBottomVisibleOffset : HitTest (ptBottomRight, FALSE, TRUE);

	if (nStartOffset == -1)
	{
		nStartOffset = 0;
	}

	if (nEndOffset == -1)
	{
		nEndOffset = m_strBuffer.GetLength () - 1;
	}

	nEndOffset = min (nEndOffset, m_strBuffer.GetLength () - 1);

	nStartOffset = GetRowStartByOffset (nStartOffset, TRUE);
	nEndOffset = GetRowEndByOffset (nEndOffset, TRUE);

	nEndOffset = min (nEndOffset, m_strBuffer.GetLength () - 1);

	rectRow.left = rectText.left - m_nScrollOffsetHorz * m_nMaxCharWidth + 1;

	rectRow.top = bIsPrint ? rectText.top : 0;
	rectRow.bottom = rectRow.top + m_nLineHeight;
	int nTabOrigin = rectRow.left;

	CString strWord; 

	int nOldBkMode = pDC->SetBkMode (OPAQUE);

	COLORREF clrDefaultText = GetDefaultTextColor ();
	COLORREF clrDefaultBack = GetDefaultBackColor ();

	COLORREF clrNextWordFore	= clrDefaultText;
	COLORREF clrNextWordBk		= clrDefaultBack;

	CString strOutString;
	COLORREF clrOutText = clrDefaultText;
	COLORREF clrOutBack = clrDefaultBack;

	COLORREF clrBkSel = GetFocus () == this ? m_clrBackSelActive : 
		m_clrBackSelInActive;
	COLORREF clrTextSel = GetFocus () == this ? m_clrTextSelActive : 
		m_clrTextSelInActive;

	CRect rectSel;

	// ----------------------------------------------
	// Build color areas for visible part of a buffer (comments, strings and so on):
	// ----------------------------------------------

	//BCGP_EDIT_COLOR_BLOCK	clrBlock; 
	//BOOL bIsOpenBlock = FindOpenBlock (m_nTopVisibleOffset, &clrBlock);

	//int nCloseBlockOffset = -1;
	//if (bIsOpenBlock)
	//{
	//	nCloseBlockOffset = FindCloseBlock (m_nTopVisibleOffset, &clrBlock);
	//}

	//COLORREF clrBlockFore = (clrBlock.m_clrForeground == -1) ? GetDefaultTextColor () : clrBlock.m_clrForeground;
	//COLORREF clrBlockBack = (clrBlock.m_clrBackground == -1) ? GetDefaultBackColor () : clrBlock.m_clrBackground; 

	//CList <BCGP_EDIT_COLOR_AREA, BCGP_EDIT_COLOR_AREA&> colorAreas;

	//if (bIsOpenBlock) 
	//{
	//	if (nCloseBlockOffset < nEndOffset)
	//	{
	//		BuildColorAreas (colorAreas, nCloseBlockOffset, nEndOffset, NULL, FALSE);
	//	}
	//}
	//else
	//{
	//	BuildColorAreas (colorAreas, nStartOffset, nEndOffset, NULL, FALSE);
	//}

	// ---------------------------------------------------------------
	// Fill background for selected block (for m_bBlockSelectionMode):
	// ---------------------------------------------------------------

	if (!bIsPrint && m_bBlockSelectionMode && m_iStartSel != -1)
	{
		CPoint ptStart;
		CPoint ptEnd;

		ptStart.x = m_ptStartBlockSel.x - m_nScrollOffsetHorz * m_nMaxCharWidth;
		ptStart.y = m_ptStartBlockSel.y - m_nScrollOffsetVert * m_nLineHeight;

		ptEnd.x = m_ptEndBlockSel.x - m_nScrollOffsetHorz * m_nMaxCharWidth;
		ptEnd.y = m_ptEndBlockSel.y - m_nScrollOffsetVert * m_nLineHeight;

		if (ptEnd.y <= ptStart.y)
		{
			int n = ptEnd.y;
			ptEnd.y = ptStart.y;
			ptStart.y = n;

			ptEnd.y += m_nLineHeight; 
			ptStart.y -= m_nLineHeight;
		}

		rectSel.SetRect (ptStart, ptEnd);
		rectSel.NormalizeRect ();

		pDC->FillSolidRect (rectSel, clrBkSel);
	}

	CPoint ptCharOffset (rectRow.TopLeft ());

	TCHAR* lpszOutBuffer = (TCHAR*) alloca (sizeof (TCHAR) * m_nMaxScrollWidth);
	memset (lpszOutBuffer, 0, sizeof (TCHAR) * m_nMaxScrollWidth);
	int iIdx = 0;

	int nNextDelimiter = -1;
	int nCurrRow = m_nScrollOffsetVert;
	int nCurrRowAbsolute = m_nScrollOffsetVert + m_nScrollHiddenVert;
	int nCurrColumn = 0;
	BOOL bColoredLine = FALSE;

	// temporary data for hyperlink support:
	BOOL bHyperlink = FALSE;
	BOOL bHyperlinkOut = FALSE;
	int nHyperlinkStart = -1;
	int nHyperlinkEnd = -1;
	int nCurrHyperlinkHotOffset = (m_bEnableHyperlinkSupport && (m_nCurrHyperlinkHot != -1)) ?
		m_arrHyperlinks [m_nCurrHyperlinkHot].m_nHyperlinkOffset : -1;

	ClearHyperlinks ();
	m_nCurrHyperlink = -1;
	m_nCurrHyperlinkHot = -1;

	// temporary data for outlining support:
	CObList lstCollapsedOutlines;
	if (m_bEnableOutlining)
	{
		m_OutlineNodes.GetBlocksByStateInRange (nStartOffset, nEndOffset, 
			lstCollapsedOutlines, TRUE, FALSE);
	}
	POSITION posCollapsedOutline = lstCollapsedOutlines.GetHeadPosition ();

	// John...
	skipTo = -1;
	inComment = false;
	inString = false;
	justInString = false;
	inBlockComment = false;
	COLORREF currentHighlightColour;
	bracketBalance = 0;
	m_StatusText = _T("");
	m_HelpURL = _T("main.htm");
	m_ToolTipText = _T("");	
	clrDefaultText = Settings::m_ColourStandardText;
	if(!lockWord)
	{
		m_IntelliWord = _T("");
	}
	m_clrBack = Settings::m_ColourBackground;
	iDefinitionLine = -1;
	bufferLength = m_strBuffer.GetLength();
	// Fix comment block problem
	m_CommentBuffer = m_strBuffer.Mid(nStartOffset, 5000);
	m_CommentBuffer.MakeUpper();
	int endC = m_CommentBuffer.Find(_T("REMEND"));
	if(endC > -1)
	{
		int startC = m_CommentBuffer.Find(_T("REMSTART"));
		if(startC == -1 || startC > endC)
		{
			inBlockComment = true;
		}
	}
	//...John

	// ---------------------------------------------
	// Draw the text for the visible part of buffer:
	// ---------------------------------------------
	int i = 0;
	for (i = nStartOffset; i <= nEndOffset; i++)
	{
		COLORREF clrForeground = clrDefaultText;
		COLORREF clrBackground = clrDefaultBack;
		bColoredLine = FALSE;

		TCHAR chNext = m_strBuffer [i];

		BCGP_EDIT_SYM_DEF symDef;
		int nSymExtraLen = 0;
		//BOOL bDrawSymbol = m_bEnableSymSupport ? LookUpSymbol (m_strBuffer, i, nEndOffset + 1, chNext, symDef, nSymExtraLen) : FALSE;
		BOOL bDrawSymbol = FALSE;

		// ------------------
		// Search hyperlinks:
		// ------------------
		bHyperlink = FALSE;
		if (m_bEnableHyperlinkSupport)
		{
			if (i >= nHyperlinkEnd)
			{
				// Find the possible start offset - skip right till the first selectable char:
				nHyperlinkStart = i;
				while (nHyperlinkStart < nEndOffset && 
					m_strNonSelectableChars.Find(m_strBuffer.GetAt (nHyperlinkStart)) != -1)
				{
					nHyperlinkStart++;
				}

				// Find the possible end offset:
				nHyperlinkEnd = nHyperlinkStart;
				int j = 0;

				for (j = nHyperlinkStart; 
					j <= nEndOffset && m_strNonSelectableChars.Find(m_strBuffer.GetAt (j)) == -1;
					j++);
				nHyperlinkEnd = j;

				if (nHyperlinkStart < nHyperlinkEnd)
				{
					CString strHyperlink;
					int nHyperlinkOffset = -1;
					if (FindHyperlinkString (nHyperlinkStart, nHyperlinkEnd, strHyperlink, nHyperlinkOffset))
					{
						ASSERT (nHyperlinkOffset >= nHyperlinkStart);
						ASSERT (nHyperlinkOffset < nHyperlinkEnd);
						ASSERT (strHyperlink.GetLength () <= nHyperlinkEnd - nHyperlinkStart);

						AddHyperlink (strHyperlink, nHyperlinkOffset);
						nHyperlinkStart = nHyperlinkOffset;
						nHyperlinkEnd = nHyperlinkOffset + strHyperlink.GetLength ();
						bHyperlink = (i >= nHyperlinkStart);
					}
					else
					{
						nHyperlinkStart = nHyperlinkEnd;
					}
				}
			}
			else if (i >= nHyperlinkStart)
			{
				bHyperlink = TRUE;
			}
		}
		if (i == nStartOffset)
		{
			bHyperlinkOut = bHyperlink;
		}

		CBCGPOutlineBaseNode* pHiddenText = NULL;
		for (POSITION posNode = posCollapsedOutline; posNode != NULL; 
			lstCollapsedOutlines.GetNext(posNode))
		{
			CBCGPOutlineBaseNode* pNode = (CBCGPOutlineBaseNode*) lstCollapsedOutlines.GetAt (posNode);
			ASSERT_VALID (pNode);

			if (pNode->m_nStart <= i &&
				pNode->m_nEnd >= i)
			{
				pHiddenText = pNode;
				posCollapsedOutline = posNode;
				break;
			}
		}

		// ---------------------
		// Calculate char width:
		// ---------------------

		int nCharWidth = 0;
		if (pHiddenText != NULL)
		{
			ASSERT_VALID (pHiddenText);
			OnCalcOutlineSymbol (pDC, ptCharOffset, pHiddenText);
			nCharWidth = pHiddenText->m_rectTool.Width ();
		}
		else if (bDrawSymbol)
		{
			IMAGEINFO imgInfo;
			m_pSymbolsImgList->GetImageInfo (symDef.m_nIndex, &imgInfo);
			nCharWidth = imgInfo.rcImage.right - imgInfo.rcImage.left;
		} 
		else if (!m_mapCharWidth.Lookup (chNext, nCharWidth))
		{
			nCharWidth = GetStringExtent (pDC, CString (chNext), 1).cx;
			if (nCharWidth == 0 && chNext == g_chEOL)
			{
				nCharWidth = m_nMaxCharWidth;
			}

			m_mapCharWidth.SetAt (chNext, nCharWidth);
		}

		if (chNext == _T ('\t') && pHiddenText == NULL && !bDrawSymbol)
		{
			int nRestOfTab = m_nTabSize - nCurrColumn % m_nTabSize;

			nCurrColumn += nRestOfTab; 
			nCharWidth = nRestOfTab * nCharWidth / m_nTabSize;
		}
		else
		{
			nCurrColumn++;
		}

		// --------------
		// Define colors:
		// --------------

		BOOL bColorFound = FALSE;

		// 1) selection has the highest priority, check the character for selection first:
		if (!bIsPrint && IsInsideSelection (i, ptCharOffset, rectSel, nCharWidth))
		{
			clrForeground = clrTextSel; 
			clrBackground = clrBkSel; 
			bColorFound = TRUE;
		}
		// 2) check for colored line marker:
		else if (!bIsPrint && IsColoredLine (nCurrRowAbsolute, clrForeground, clrBackground))
		{
			bColorFound = TRUE;
			bColoredLine = TRUE;
		}

		// 5) Check all color words (keywords and words):
		if (chNext != g_chEOL && !bDrawSymbol && pHiddenText == NULL)
		{
			if (nNextDelimiter == -1) 
			{		
				CString strNextWord;

				for (int iIdx = i; iIdx <= nEndOffset; iIdx++)
				{
					TCHAR ch = m_strBuffer [iIdx];
					if (m_strWordDelimeters.Find (ch) != -1)
					{
						nNextDelimiter = iIdx;
						break;
					}
				}

				if (nNextDelimiter == -1)
				{
					nNextDelimiter = nEndOffset + 1;
				}

				if (nNextDelimiter != -1)
				{
					strNextWord = m_strBuffer.Mid(i, nNextDelimiter - i);
					if(i < GetCurOffset() && i > skipTo && !lockWord && !inComment && !inBlockComment && !inString)
					{
						m_IntelliWord.Format(_T("%s"), strNextWord); // John		
						intellisenseStart = i;
					}
				}

				// John...				
				if(i > skipTo && !inComment && !inString && strNextWord != _T(""))
				{			
					CString upNext = strNextWord;
					upNext.MakeUpper();
					if(inBlockComment)
					{
						if(upNext == _T("REMEND"))
						{
							inBlockComment = false;		
							inComment = true;
							clrNextWordFore = Settings::m_ColourComment;
						}
					}
					else
					{
						if(upNext == _T("REM"))
						{
							inComment = true;
						}
						else if(upNext == _T("REMSTART"))
						{
							inBlockComment = true;
						}
						else if(Utilities::isNumber(upNext))
						{
							currentHighlightColour = Settings::m_ColourSymbol;
							skipTo = i + upNext.GetLength();
						}
						else
						{
							TCHAR first = upNext.GetAt(0);
							TCHAR second = 0;
							if(upNext.GetLength() > 1)
							{
								second = upNext.GetAt(1);
							}
							int res = TestKeyword(first, second, i, skipTo, nCurrRow, bufferLength);								
							if(res == 1) // Keyword
							{
								currentHighlightColour = Settings::m_ColourKeyword;
							} 
							else if(res == 2) // Function
							{
								currentHighlightColour = Settings::m_ColourFunction;
							}
							else if(res == 3) // Type
							{
								currentHighlightColour = Settings::m_ColourType;
							}
							else if(res == 4) // Variable
							{
								currentHighlightColour = Settings::m_ColourVariable;
							}
							else
							{
								currentHighlightColour = Settings::m_ColourStandardText;
							}
						}
					}
				}
				//...John
			}

			if (i >= nNextDelimiter - 1)
			{
				nNextDelimiter = -1;
			}

			if (!bColorFound)
			{
				clrForeground = clrNextWordFore;
				clrBackground = clrNextWordBk;
			}

			ptCharOffset.x += nCharWidth;
		}

		// 6) check for hilited text:
		if (!bIsPrint)
		{
			IsHilitedText (i, clrForeground, clrBackground);
		}

		// John...
		if(chNext == '\"')
		{
			inString = !inString;
			if(!inString)
			{
				justInString = true;
			}
		}
		// ...John

		// 7) User can define color of current char:
		if (!bColorFound && !bDrawSymbol && pHiddenText == NULL && !inComment && i > skipTo && !inBlockComment && !inString) // John
		{
			//John...
			if(chNext == '`')
			{
				inComment = true;
			}
			else if(chNext == '/' && i+1 < m_strBuffer.GetLength() && m_strBuffer.GetAt(i+1) == '/')
			{
				inComment = true;
			}
			else if(chNext == '<' || chNext == '>' || chNext == ':' || chNext == '+' || chNext == '-' || chNext == '*' || chNext == '/' || chNext == ',' || chNext == '(' || chNext == ')' || chNext == '=')
			{
				clrForeground = Settings::m_ColourSymbol;
				bColorFound = TRUE;
			}
			else if(chNext == '{' || chNext == '}' || chNext == '@') // Invalid characters
			{
				clrBackground = 0x000000FF;
			}
			if(Settings::AllowBracketBalanceCheck)
			{
				if(chNext == '(')
				{
					bracketBalance++;
				}
				else if(chNext == ')')
				{
					bracketBalance--;
					if(bracketBalance < 0)
					{
						clrBackground = 0x000000FF;
					}
				}
			}
			//...John
		}

		// John...
		if(i <= skipTo)
		{
			clrForeground = currentHighlightColour;
		}
		else if(inComment || inBlockComment)
		{
			clrForeground = Settings::m_ColourComment;
		}
		else if(inString || justInString)
		{
			clrForeground = Settings::m_ColourString;
			justInString = false;
		}	
		else if(!bColorFound)
		{
			clrForeground = Settings::m_ColourStandardText;
		}
		//...John

		// 8) check for hyperlink:
		else if (m_bEnableHyperlinkSupport && bHyperlink)
		{
			if (clrForeground != clrTextSel && m_bColorHyperlink)
			{
				clrForeground = m_clrHyperlink;
			}
		}

		// --------------------
		// Proceed end of line:
		// --------------------

		if (chNext == g_chEOL && pHiddenText == NULL)
		{
			// John...
			bracketBalance = 0;
			inComment = false;
			inString = false;
			// ...John
			pDC->SetTextColor (clrOutText);
			pDC->SetBkColor   (clrOutBack);

			CFont* pOldFont = NULL;
			if (bHyperlinkOut)
			{
				pOldFont = SelectFont (pDC, bHyperlinkOut);
			}

			if (iIdx != 0)
			{
				if (m_bKeepTabs)
				{
					rectRow.left += DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
				}
				else
				{
					DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
					rectRow.left += pDC->GetTextExtent (lpszOutBuffer).cx;
				}

				iIdx = 0;
				memset (lpszOutBuffer, 0, sizeof (TCHAR) * m_nMaxScrollWidth);
				rectRow.TopLeft ();
			}

			if (bHyperlinkOut)
			{
				pDC->SelectObject (pOldFont);
			}

			if (i < max (m_iStartSel, m_iEndSel) && !bColoredLine)
			{
				if (clrForeground != clrOutText || clrBackground != clrOutBack)
				{
					pDC->SetTextColor (clrForeground);
					pDC->SetBkColor   (clrBackground);
				}

				DrawString (pDC, _T (" "), rectRow, nTabOrigin, clrDefaultBack);
			}

			clrOutText = clrForeground;
			clrOutBack = clrBackground;
			bHyperlinkOut = bHyperlink;

			if (!bIsPrint)
			{
				DrawColorLine (pDC, nCurrRowAbsolute, rectRow);
			}

			rectRow.OffsetRect (0, m_nLineHeight);
			rectRow.left = rectText.left - m_nScrollOffsetHorz * m_nMaxCharWidth + 1;
			ptCharOffset = rectRow.TopLeft ();

			nCurrRow++;
			nCurrRowAbsolute++;
			nCurrColumn = 0;

			continue;
		} 

		// -----------------------------------
		// Proceed end of same color fragment:
		// -----------------------------------

		if (clrForeground != clrOutText || clrBackground != clrOutBack || 
			bHyperlink != bHyperlinkOut ||
			bDrawSymbol || pHiddenText != NULL)
		{
			pDC->SetTextColor (clrOutText);
			pDC->SetBkColor   (clrOutBack);

			if (iIdx != 0)
			{
				CFont* pOldFont = NULL;
				if (bHyperlinkOut)
				{
					pOldFont = SelectFont (pDC, bHyperlinkOut);
				}

				if (m_bKeepTabs)
				{
					rectRow.left += DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
				}
				else
				{
					DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
					rectRow.left += pDC->GetTextExtent (lpszOutBuffer).cx;
				}

				if (bHyperlinkOut)
				{
					pDC->SelectObject (pOldFont);
				}

				iIdx = 0;
				memset (lpszOutBuffer, 0, sizeof (TCHAR) * m_nMaxScrollWidth);
				rectRow.TopLeft ();
			}

			clrOutText = clrForeground;
			clrOutBack = clrBackground;
			bHyperlinkOut = bHyperlink;
		}

		// ---------------------------------------------------
		// Proceed special symbols (SymSupport and Outlining):
		// ---------------------------------------------------
		if (pHiddenText != NULL)
		{
			ASSERT_VALID (pHiddenText);

			if (clrBackground != clrDefaultBack)
			{
				CRect rect = rectRow;
				rect.right = rect.left + nCharWidth;

				pDC->FillSolidRect (rect, clrBackground);
			}
			OnDrawOutlineSymbol (pDC, pHiddenText, m_clrLineOutline, clrBackground);

			rectRow.left += nCharWidth;
			i += pHiddenText->m_nEnd - pHiddenText->m_nStart;

			ptCharOffset = rectRow.TopLeft ();

			nCurrRowAbsolute += GetNumOfCharsInText (pHiddenText->m_nStart, pHiddenText->m_nEnd, g_chEOL);

			continue;
		}
		else if (bDrawSymbol)
		{
			if (clrBackground != clrDefaultBack)
			{
				CRect rect = rectRow;
				rect.right = rect.left + nCharWidth;

				pDC->FillSolidRect (rect, clrBackground);
			}
			if (!OnDrawSymbol (pDC, rectRow, symDef))
			{
				m_pSymbolsImgList->Draw (pDC, symDef.m_nIndex, rectRow.TopLeft (), ILD_TRANSPARENT);
			}

			rectRow.left += nCharWidth;
			i += nSymExtraLen;

			ptCharOffset = rectRow.TopLeft ();

			continue;
		}

		lpszOutBuffer [iIdx] = chNext;

		// ---------------------------
		// Trancate very long strings:
		// ---------------------------
		if (iIdx + 1 > m_nMaxScrollWidth - 1)
		{
			lpszOutBuffer [iIdx + 1] = _T ('\0');
			continue;
		}

		lpszOutBuffer [iIdx + 1] = _T ('\0');
		iIdx++;
	}

	// --------------------------
	// Draw the last of the text:
	// --------------------------

	if (iIdx != 0)
	{
		pDC->SetTextColor (clrOutText);
		pDC->SetBkColor   (clrOutBack);

		CFont* pOldFont = NULL;
		if (bHyperlinkOut)
		{
			pOldFont = SelectFont (pDC, bHyperlinkOut);
		}

		if (m_bKeepTabs)
		{
			rectRow.left += DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
		}
		else
		{
			DrawString (pDC, lpszOutBuffer, rectRow, nTabOrigin, clrDefaultBack);
			rectRow.left += pDC->GetTextExtent (lpszOutBuffer).cx;
		}

		if (bHyperlinkOut)
		{
			pDC->SelectObject (pOldFont);
		}

		if (i < m_strBuffer.GetLength () && m_strBuffer.GetAt (i) == g_chEOL && 
			i >= min (m_iStartSel, m_iEndSel) && i <= max (m_iStartSel, m_iEndSel))
		{
			pDC->SetTextColor (clrTextSel);
			pDC->SetBkColor   (clrBkSel);

			rectRow.left += pDC->GetTextExtent (lpszOutBuffer).cx;
			DrawString (pDC, _T (" "), rectRow, nTabOrigin, clrDefaultBack);
		}

		if (!bIsPrint)
		{
			DrawColorLine (pDC, nCurrRowAbsolute, rectRow);
		}
	}

	pDC->SetBkMode (nOldBkMode);

	// restore indexes for hyperlink support:
	if (m_bEnableHyperlinkSupport)
	{
		m_nCurrHyperlink = LookupHyperlinkByOffset(m_nCurrOffset);
		m_nCurrHyperlinkHot = LookupHyperlinkByOffset(nCurrHyperlinkHotOffset);
	}

	if(Settings::AllowStatusBarHelp && m_StatusText != _T(""))
	{
		MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
		pMainWnd->SetStatusText(m_StatusText);
	}
}

int inline EditorControl::TestKeyword(TCHAR first, TCHAR second, int i, int &skipTo, int lineNumber, int length)
{
	std::vector <Keyword*> m_Keywords;
	int startC = -1, endC = -1;
	switch(first)
	{
	case _T('A'):
		m_Keywords = Keywords::m_AKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'N')
		{
			startC = Keywords::a;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::a;
		}
		break;
	case _T('B'):
		m_Keywords = Keywords::m_BKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'I')
		{
			startC = Keywords::b;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::b;
		}
		break;
	case _T('C'):
		m_Keywords = Keywords::m_CKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'L')
		{
			startC = Keywords::c;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::c;
		}
		break;
	case _T('D'):
		m_Keywords = Keywords::m_DKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::d;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::d;
		}
		break;
	case _T('E'):
		m_Keywords = Keywords::m_EKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'N')
		{
			startC = Keywords::e;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::e;
		}
		break;
	case _T('F'):
		m_Keywords = Keywords::m_FKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'I')
		{
			startC = Keywords::f;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::f;
		}
		break;
	case _T('G'):
		m_Keywords = Keywords::m_GKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::g;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::g;
		}
		break;
	case _T('H'):
		m_Keywords = Keywords::m_HKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'I')
		{
			startC = Keywords::h;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::h;
		}
		break;
	case _T('I'):
		m_Keywords = Keywords::m_IKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'N')
		{
			startC = Keywords::i;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::i;
		}
		break;
	case _T('J'):
		m_Keywords = Keywords::m_JKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'O')
		{
			startC = Keywords::j;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::j;
		}
		break;
	case _T('K'):
		m_Keywords = Keywords::m_KKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::k;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::k;
		}
		break;
	case _T('L'):
		m_Keywords = Keywords::m_LKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'M')
		{
			startC = Keywords::l;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::l;
		}
		break;
	case _T('M'):
		m_Keywords = Keywords::m_MKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'A')
		{
			startC = Keywords::m;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::m;
		}
		break;
	case _T('N'):
		m_Keywords = Keywords::m_NKeywords;
		if(second <= 'E')
		{
			startC = Keywords::n;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::n;
		}
		break;
	case _T('O'):
		m_Keywords = Keywords::m_OKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'B')
		{
			startC = Keywords::o;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::o;
		}
		break;
	case _T('P'):
		m_Keywords = Keywords::m_PKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'I')
		{
			startC = Keywords::p;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::p;
		}
		break;
	case _T('Q'):
		m_Keywords = Keywords::m_QKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::q;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::q;
		}
		break;
	case _T('R'):
		m_Keywords = Keywords::m_RKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::r;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::r;
		}
		break;
	case _T('S'):
		m_Keywords = Keywords::m_SKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::s;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::s;
		}
		break;
	case _T('T'):
		m_Keywords = Keywords::m_TKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'I')
		{
			startC = Keywords::t;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::t;
		}
		break;
	case _T('U'):
		m_Keywords = Keywords::m_UKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'N')
		{
			startC = Keywords::u;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::u;
		}
		break;
	case _T('V'):
		m_Keywords = Keywords::m_VKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'E')
		{
			startC = Keywords::v;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::v;
		}
		break;
	case _T('W'):
		m_Keywords = Keywords::m_WKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'R')
		{
			startC = Keywords::w;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::w;
		}
		break;
	case _T('X'):
		m_Keywords = Keywords::m_XKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= ' ')
		{
			startC = Keywords::x;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::x;
		}
		break;	
	case _T('Y'):
		m_Keywords = Keywords::m_YKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= ' ')
		{
			startC = Keywords::y;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::y;
		}
		break;
	case _T('Z'):
		m_Keywords = Keywords::m_ZKeywords;
		if(second == 0)
		{
			startC = 0;
			endC = (int)m_Keywords.size();
		}
		else if(second <= 'O')
		{
			startC = Keywords::z;
			endC = (int)m_Keywords.size();
		}
		else
		{
			startC = 0;
			endC = Keywords::z;
		}
		break;
	default:
		m_Keywords = Keywords::m_MiscKeywords;
		startC = 0;
		endC = (int)m_Keywords.size();
		break;
	}

	TCHAR test;
	int val; // int version of test
	register int keylength;
	register int curOffset = GetCurOffset();
	Keyword* keyword;
	CString newBuffer = _T("");
	int copiedLength = 0;
	int toVal = (i+Keywords::LongestKeyword) > length ? length : (i+Keywords::LongestKeyword);
	for(int match=i; match < toVal; match++)
	{
		test = (TCHAR)_totupper(m_strBuffer.GetAt(match));	
		val = test;
		newBuffer.AppendChar(test);
		copiedLength++;
		//if(test == '(' || test == '\n' || test == '\t' || test == ':' || test == '\"' || test == ',' || test == '+' || test == '*' || test  == '-' || test == '/' || test == '=')
		if(!((val > 64 && val < 91) /* A-Z */ || (val > 47 && val < 58) /* 0-9 */ || val == 32 /* */ || val == 35 /* # */|| val == 36 /* $ */ || val == 95 /* _ */))
		{
			break;
		}		
	}
	if(copiedLength < 2)
	{
		return 0;
	}

	if(startC > -1 && endC > -1)
	{
		int keyi;
		std::vector <Keyword*>::iterator myIntVectorIterator; // New
		myIntVectorIterator = m_Keywords.begin() + startC; // New
		for(keyi=startC; keyi<endC; keyi++)
		{
			keyword = *myIntVectorIterator; // New
			myIntVectorIterator ++; // New
			keylength = keyword->length;
			if(keylength > copiedLength)
			{
				continue;
			}

			if(wcsncmp(newBuffer, keyword->keyword, keylength) == 0)
			{
				test = newBuffer.GetAt(keylength);
                // IRM 20091002 - Add colon as a delimiter
                if(length == (keylength + i) || test == ' ' || test == '(' || test == '\n' || test == ':')
				{
					skipTo = i + keylength - 1;
					if(curOffset >= i && curOffset <= skipTo + 1)
					{
						m_StatusText = keyword->keyword + _T(" ") + keyword->help;
						m_HelpURL = keyword->url;
					}
					if(toolTipPosition >= i && toolTipPosition <= skipTo)
					{
						m_ToolTipText = keyword->keyword +_T(" ") + keyword->help;
					}
					return 1;
				}
			}			
		}
	}

	// Variables, Functions and Types
	if(!SemanticParser::m_Mutex.Lock(1))
	{
		return 0;
	}

	SemanticVariable* variable;

	std::vector<SemanticVariable*>::iterator mySVVectorIterator; // New

	for(mySVVectorIterator = SemanticParser::m_vVariables.begin(); 
        mySVVectorIterator != SemanticParser::m_vVariables.end();
        mySVVectorIterator++)
	{
		variable = *mySVVectorIterator;

		if(variable->endline != -1 && (lineNumber < variable->line || lineNumber > variable->endline)) // out of scope
		{
			continue;
		}

		keylength = variable->match.GetLength();
		if(keylength > copiedLength)
		{
			continue;
		}

		if(wcsncmp(newBuffer,variable->match,keylength) == 0)
		{
			test = newBuffer.GetAt(keylength);
			val = (int)test;			
			if(length == (keylength + i) || val == 32 || test == '\n' || val == 47 || (val > 39 && val < 46) || (val > 59 && val < 63))
			{
				if(Settings::AllowVariableFix && m_strBuffer.Mid(i, keylength) != variable->variable) // Fix up
				{
					m_strBuffer.Delete(i, keylength);
					m_strBuffer.Insert(i, variable->variable);
				}
				skipTo = i + keylength - 1;
				if(curOffset >= i && curOffset <= skipTo + 1)
				{
					if(!lockWord)
					{
						m_IntelliWord = _T("");
					}
					m_StatusText = variable->help;
					iDefinitionLine = variable->line;
					m_HelpURL = _T("main.htm");
				}
				if(toolTipPosition >= i && toolTipPosition <= skipTo)
				{
					m_ToolTipText = variable->help;
				}
				SemanticParser::m_Mutex.Unlock();
				return 4;
			}
		}
	}

	SemanticFunction* function;

	std::vector<SemanticFunction*>::iterator mySFVectorIterator; // New

	for(mySFVectorIterator = SemanticParser::m_vFunctions.begin(); 
        mySFVectorIterator != SemanticParser::m_vFunctions.end();
        mySFVectorIterator++)
	{
		function = *mySFVectorIterator;

		keylength = function->function.GetLength();
		if(keylength > length)
		{
			continue;
		}

		if(wcsncmp(newBuffer, function->function, keylength) == 0)
		{
			test = newBuffer.GetAt(keylength);						
			if(length == (keylength + i) || test == ' ' || test == '\n' || test == '(')
			{
				skipTo = i + keylength - 1;
				if(curOffset >= i && curOffset <= skipTo + 1)
				{
					if(!lockWord)
					{
						m_IntelliWord = _T("");
					}
					m_StatusText = function->definition;
					iDefinitionLine = function->line;
					m_HelpURL = _T("main.htm");
				}
				if(toolTipPosition >= i && toolTipPosition <= skipTo)
				{
					m_ToolTipText = function->definition;
				}
				SemanticParser::m_Mutex.Unlock();
				return 2;
			}
		}
	}

	SemanticType* type;

	std::vector<SemanticType*>::iterator mySTVectorIterator; // New

	for(mySTVectorIterator = SemanticParser::m_vTypes.begin(); 
        mySTVectorIterator != SemanticParser::m_vTypes.end();
        mySTVectorIterator++)
	{
		type = *mySTVectorIterator;

		keylength = type->type.GetLength();
		if(keylength > copiedLength)
		{
			continue;
		}

		if(wcsncmp(newBuffer, type->match, keylength) == 0)
		{
			test = newBuffer.GetAt(keylength);
			if(length == (keylength + i) || test == ' ' || test == '\n' || test == '(' || test == '.')
			{
				skipTo = i + keylength - 1;
				if(curOffset >= i && curOffset <= skipTo + 1)
				{
					if(!lockWord)
					{
						m_IntelliWord = _T("");
					}
					m_StatusText = _T("TYPE ") + type->type;
					iDefinitionLine = type->line;
					m_HelpURL = _T("main.htm");
				}
				if(toolTipPosition >= i && toolTipPosition <= skipTo)
				{
					m_ToolTipText = type->type;
				}
				SemanticParser::m_Mutex.Unlock();
				return 3;
			}
		}
	}

	SemanticLabel* label;

	std::vector<SemanticLabel*>::iterator mySLVectorIterator; // New

	for(mySLVectorIterator = SemanticParser::m_vLabels.begin(); 
        mySLVectorIterator != SemanticParser::m_vLabels.end();
        mySLVectorIterator++)
	{		
		label = *mySLVectorIterator;

		keylength = label->label.GetLength();
		if(keylength > copiedLength)
		{
			continue;
		}

		if(wcsncmp(newBuffer, label->match, keylength) == 0)
		{
			test = newBuffer.GetAt(keylength);			
			if(length == (keylength + i) || test == ':')
			{
				skipTo = i + keylength - 1;
				if(curOffset >= i && curOffset <= skipTo + 1)
				{
					if(!lockWord)
					{
						m_IntelliWord = _T("");
					}
					m_StatusText = _T("LABEL ") + label->label;
					iDefinitionLine = label->line;
					m_HelpURL = _T("main.htm");
				}
				if(toolTipPosition >= i && toolTipPosition <= skipTo)
				{
					m_ToolTipText = _T("LABEL ") + label->label;
				}
				SemanticParser::m_Mutex.Unlock();
				return -1; // Don't highlight
			}
		}
	}
	
	SemanticParser::m_Mutex.Unlock();

	return 0;
}
//****************************************************************************************
BOOL EditorControl::FindTextE(LPCTSTR lpszFind, BOOL bNext /* = TRUE */, BOOL bCase /* = TRUE */, BOOL bWholeWord /* = FALSE */, BOOL bSelectedText /* = FALSE */)
{
	int nPos = -1;
	int nFindLen = 0;

	if (!DoFindTextE(nPos, nFindLen, lpszFind, bNext, bCase, bWholeWord, bSelectedText))
	{
		return FALSE;
	}
	
	ASSERT (nPos >= 0);
	SetSel (nPos, nPos + nFindLen);

	return TRUE;
}
//****************************************************************************************
BOOL EditorControl::DoFindTextE(int& nPos, int& nFindLength, LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWholeWord, BOOL bSelectedText)
{
	if (lpszFind == NULL || lpszFind [0] == 0)
	{
		return FALSE;
	}

	int nCurrOffset;

	int iStartSel = min (m_iStartSel, m_iEndSel);
	int iEndSel = max (m_iStartSel, m_iEndSel);
	
	BOOL bCanSelect = bSelectedText && !(iStartSel < 0 || iEndSel <= iStartSel); // John
	int iOriginalOffset = iStartSel;

	if (iStartSel < 0 || iEndSel <= iStartSel)
	{
		// no selection - use caret offset
		nCurrOffset = m_nCurrOffset;
	}
	else
	{
		if (bNext)
		{
			// selection - use the end of the selection as an offset
			nCurrOffset = iEndSel;
		}
		else
		{
			// selection - use the start of the selection as an offset
			nCurrOffset = iStartSel;
		}
	}

	if (nCurrOffset >= m_strBuffer.GetLength())
	{
		nCurrOffset = m_strBuffer.GetLength() - 1;
	}
	else if (nCurrOffset < 0)
	{
		nCurrOffset = 0;
	}

	CString strTmpBuffer;

	CString strFind(lpszFind);
	CString *pStrBuffer = NULL;

	// John... - Limit the buffer if we are only doing selection
	CString m_strSearchBuffer = m_strBuffer;
	if(bCanSelect)
	{
		m_strSearchBuffer = m_strSearchBuffer.Mid(iStartSel, iEndSel - iStartSel);
		iStartSel = 0;
		iEndSel = iEndSel - iStartSel;
	}
	
	if (!bCase)
	{
		strTmpBuffer = m_strSearchBuffer;
		strTmpBuffer.MakeLower();
		strFind.MakeLower();

		pStrBuffer = &strTmpBuffer;
	}

	if (!bNext)
	{
		if (!pStrBuffer)
		{
			strTmpBuffer = m_strSearchBuffer;
			pStrBuffer = &strTmpBuffer;
		}

		pStrBuffer->MakeReverse();
		strFind.MakeReverse();

		nCurrOffset = m_strBuffer.GetLength() - nCurrOffset;
	}

	const int nFindLen = strFind.GetLength();
	lpszFind = strFind.GetBuffer(0);

	if (pStrBuffer == NULL)
	{
		pStrBuffer = &m_strSearchBuffer;
	}

	nPos = 0;
	int nLastOffset = nCurrOffset;
	
	// Find the string from nCurrOffset towards end of the buffer:
	do
	{
		nPos = pStrBuffer->Find(lpszFind, nCurrOffset);

		// for backword search:
		if (!bNext)
		{
			if (nPos == nLastOffset)
			{
				if (m_iStartSel < m_iEndSel && nPos == m_strBuffer.GetLength() - m_iEndSel)
				{
					nPos = pStrBuffer->Find(lpszFind, ++nCurrOffset);

					if (nPos == -1)
					{
						nCurrOffset += (nFindLen - 1);
					}

					nLastOffset = nCurrOffset;
				}
			}
		}

		if (!bWholeWord || nPos == -1)
		{
			break;
		}
		
		// skip all substrings which are not embaced with delimiters
		int nPos1 = (nPos > 0)? m_strWordDelimeters.Find(pStrBuffer->GetAt(nPos - 1)): 0;
		int nPos2 = (nPos + nFindLen < m_strBuffer.GetLength() - 1)? m_strWordDelimeters.Find(pStrBuffer->GetAt(nPos + nFindLen)): 0;

		if (nPos1 == -1 || nPos2 == -1)
		{
			nCurrOffset = nPos + nFindLen;
			nPos = -1;
			
			if (nCurrOffset == m_strBuffer.GetLength())
			{
				break;
			}
		}
	}
	while (nPos == -1);

	// Find the string from the begin of the buffer towards nCurrOffset:
	if (nPos == -1)
	{
		nLastOffset += nFindLen - 1;
		if (nLastOffset > pStrBuffer->GetLength())
		{
			nLastOffset = pStrBuffer->GetLength();
		}

		if (nLastOffset > 0)
		{
			LPTSTR pszBuffer = pStrBuffer->GetBuffer(nLastOffset + 1);
			TCHAR tCh = pszBuffer[nLastOffset];

			pszBuffer[nLastOffset] = 0;

			if (!bWholeWord)
			{
				nPos = pStrBuffer->Find(lpszFind);
			}
			else
			{
				nCurrOffset = 0;

				do
				{
					nPos = pStrBuffer->Find(lpszFind, nCurrOffset);

					// skip all substrings which are not embaced with delimiters
					if (nPos == -1)
					{
						break;
					}
					else
					{
						int nPos1 = (nPos > 0)? m_strWordDelimeters.Find(pszBuffer[nPos - 1]): 0;
						int nPos2 = (nPos + nFindLen < nLastOffset - 1)? m_strWordDelimeters.Find(pszBuffer[nPos + nFindLen]): 0;

						if (nPos1 == -1 ||
							nPos2 == -1)
						{
							nCurrOffset = nPos + nFindLen;
							nPos = -1;

							if (nCurrOffset == nLastOffset)
							{
								break;
							}
						}
					}
				}
				while (nPos == -1);
			}
			
			pszBuffer[nLastOffset] = tCh;
			pStrBuffer->ReleaseBuffer();
		}
	}

	if (nPos == -1)
	{
		return FALSE;
	}

	// John.. A few changes here, see base implementation
	if (!bNext)
	{
		if(bCanSelect)
		{
			nPos = iOriginalOffset + (m_strSearchBuffer.GetLength() - (nPos + nFindLen));
		}
		else
		{
			nPos = m_strBuffer.GetLength() - (nPos + nFindLen);
		}
	}
	else if(bCanSelect)
	{
		nPos += iOriginalOffset;
	}

	nFindLength = nFindLen;
	return TRUE;
}
//***********************************************************************************
BOOL EditorControl::ReplaceTextE(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bNext, BOOL bCase, BOOL bWholeWord, BOOL bSelected)
{
	if (lpszReplace == NULL)
	{
		return FALSE;
	}

	if (m_bReadOnly)
	{
		return FALSE;
	}

	BOOL bModified = FALSE;

	// -----------------------------------
	// Compare the selection with lpszFind
	// -----------------------------------
	int iStartSel = min (m_iStartSel, m_iEndSel);
	int iEndSel = max (m_iStartSel, m_iEndSel);

	if (iStartSel >= 0 && iEndSel > iStartSel)
	{
		CString strSelectedText = m_strBuffer.Mid (iStartSel, iEndSel - iStartSel);
		BOOL bIdentical = (bCase) ? (strSelectedText.Compare (lpszFind) == 0) : (strSelectedText.CompareNoCase (lpszFind) == 0);

		if (bWholeWord)
		{
			// skip all substrings which are not embraced with delimiters
			int nPos1 = (iStartSel > 0)? m_strWordDelimeters.Find(m_strBuffer.GetAt(iStartSel - 1)): 0;
			int nPos2 = (iEndSel < m_strBuffer.GetLength() - 1)? m_strWordDelimeters.Find(m_strBuffer.GetAt(iEndSel)): 0;
			if (nPos1 == -1 || nPos2 == -1)
			{
				// skip
				bIdentical = FALSE;
			}
		}

		// --------------------------------------
		// If selected text is same as lpszFind,
		// replace selected text with lpszReplace
		// --------------------------------------
		if (bIdentical)
		{
			BOOL bForceNextUndo = TRUE;

			DeleteSelectedText (FALSE, FALSE, bForceNextUndo);
			SetLastUndoReason (g_dwUATReplace);
			int nSaveOffset = m_nCurrOffset;
			InsertText (lpszReplace, -1, FALSE, FALSE, TRUE, FALSE);

			SetSel2 (nSaveOffset, m_nCurrOffset, FALSE, FALSE);

			m_bIsModified = TRUE;
			bModified = TRUE;
			
			RedrawWindow ();
		}
	}

	// ----------------------------------------
	// Search for the next match of a substring
	// ----------------------------------------
	return FindTextE (lpszFind, bNext, bCase, bWholeWord, bSelected) || bModified;
}
//***********************************************************************************
int EditorControl::ReplaceAllE(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bNext, BOOL bCase, BOOL bWholeWord, BOOL bSelected)
{
	if (lpszReplace == NULL)
	{
		return 0;
	}

	if (m_bReadOnly)
	{
		return 0;
	}

	CWaitCursor wait;
	SetRedraw (FALSE);

	int nFirstOffset = -1; // save offset to prevent loop
	int nPos = -1;
	int nFindLen = 0;
	int nReplaceLen = (int) _tcslen (lpszReplace);

	// John.. Keep track of original selection
	int iStartSel = min (m_iStartSel, m_iEndSel);
	int iEndSel = max (m_iStartSel, m_iEndSel);
	// Calculate lengths of strings to handle variance
	UINT lengthReplace = _tcslen(lpszReplace);
	UINT lengthFind = _tcslen(lpszFind);
	int length;

	int nCount = 0;
	while (DoFindTextE(nPos, nFindLen, lpszFind, bNext, bCase, bWholeWord, bSelected))
	{
		if (nPos < 0)
		{
			ASSERT (FALSE);
			break;
		}

		if (nFirstOffset == -1)
		{
			nFirstOffset = nPos; 
		}
		else if (nFirstOffset == nPos)
		{
			// passed to the end of the file
			break;
		}
		else if ((nFirstOffset < nPos + nFindLen) && (nFirstOffset + nReplaceLen > nPos))
		{
			// found entry is a part of the replace string
			break;
		}

		if (nFirstOffset > nPos)
		{
			nFirstOffset += nReplaceLen - nFindLen;
		}

		SetSel2 (nPos, nPos + nFindLen, FALSE, FALSE);

		// replace selected text with lpszReplace:
		BOOL bForceNextUndo = TRUE;

		DeleteSelectedText (FALSE, FALSE, bForceNextUndo);
		SetLastUndoReason (g_dwUATReplace);
		int nSaveOffset = m_nCurrOffset;
		InsertText (lpszReplace, -1, FALSE, FALSE, TRUE, FALSE);

		SetSel2 (nSaveOffset, m_nCurrOffset, FALSE, FALSE);

		m_bIsModified = TRUE;

		nCount++;

        // IRM 20090910 - Skip past the replaced text as we don't want to 'find' it again.
        nPos += nReplaceLen;

		length = (lengthReplace - lengthFind) * nCount; // John... The size of the selected text may have changed, so calculate difference
		SetSel2 (nPos, iEndSel + length, FALSE, FALSE); // John... Reset the original selection so that the find text method works
	}

	// John.. Set the new selection to the original start to the original end +/- the difference between the find and replace text
	SetSel2(iStartSel, iEndSel + length);
	// ..John

	SetRedraw ();
	RedrawWindow ();

	return nCount;
}
