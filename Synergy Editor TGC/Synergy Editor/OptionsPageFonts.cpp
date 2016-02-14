#include "stdafx.h"
#include "Synergy Editor.h"
#include "OptionsPageFonts.h"
#include "Settings.h"
#include "View.h"

// OptionsPageFonts dialog
IMPLEMENT_DYNAMIC(OptionsPageFonts, CBCGPPropertyPage)

OptionsPageFonts::OptionsPageFonts(CWnd* pParent /*=NULL*/)
	: CBCGPPropertyPage(OptionsPageFonts::IDD)
{
	lockColourChange = false;
}

OptionsPageFonts::~OptionsPageFonts()
{}

BOOL OptionsPageFonts::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();
	CWaitCursor wait;

	cFontSize.InsertString(-1,_T("6"));
	cFontSize.InsertString(-1,_T("7"));
	cFontSize.InsertString(-1,_T("8"));
	cFontSize.InsertString(-1,_T("9"));
	cFontSize.InsertString(-1,_T("10"));
	cFontSize.InsertString(-1,_T("11"));
	cFontSize.InsertString(-1,_T("12"));
	cFontSize.InsertString(-1,_T("13"));
	cFontSize.InsertString(-1,_T("14"));
	cFontSize.InsertString(-1,_T("15"));
	cFontSize.InsertString(-1,_T("16"));
	cFontSize.InsertString(-1,_T("17"));
	cFontSize.InsertString(-1,_T("18"));
	cFontSize.InsertString(-1,_T("19"));
	cFontSize.InsertString(-1,_T("20"));
	cFontSize.InsertString(-1,_T("21"));
	cFontSize.InsertString(-1,_T("22"));
	cFontSize.InsertString(-1,_T("23"));
	cFontSize.InsertString(-1, _T("24"));

	cDisplayItem.InsertString(-1, _T("Standard Text"));
	cDisplayItem.InsertString(-1, _T("Keyword"));
	cDisplayItem.InsertString(-1, _T("String"));
	cDisplayItem.InsertString(-1, _T("Comment"));
	cDisplayItem.InsertString(-1, _T("Symbol"));
	cDisplayItem.InsertString(-1, _T("Function"));
	cDisplayItem.InsertString(-1, _T("Type"));
	cDisplayItem.InsertString(-1, _T("Variable"));
	cDisplayItem.SelectString(-1, _T("Standard Text"));
	lockColourChange = true;
	cItemColour.SetColor(Settings::m_ColourStandardText);
	lockColourChange = false;
	cItemColour.EnableOtherButton(_T("Customise"));
	lastColourEntry = _T("Standard Text");

	m_Colors.push_back(Settings::m_ColourStandardText);
	m_Colors.push_back(Settings::m_ColourKeyword);
	m_Colors.push_back(Settings::m_ColourString);
	m_Colors.push_back(Settings::m_ColourComment);
	m_Colors.push_back(Settings::m_ColourSymbol);
	m_Colors.push_back(Settings::m_ColourFunction);
	m_Colors.push_back(Settings::m_ColourType);
	m_Colors.push_back(Settings::m_ColourVariable);

	cEditorBackground.SetColor(Settings::m_ColourBackground);
	cEditorBackground.EnableOtherButton(_T("Customise"));

	CString FontSize;
	FontSize.Format(_T("%d"),Settings::EditorFontSize);
	cFontSize.SelectString(-1, FontSize);

	cFontFace.Setup(DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE);
	cFontFace.SelectString(-1, Settings::EditorFontFace);

	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
		m_ToolTip.AddTool( &cDisplayItem, _T("Choose the display item which you want to customise"));
		m_ToolTip.AddTool( &cItemColour, _T("Choose the colour you want the selected display item to be drawn in"));
		m_ToolTip.AddTool( &cFontFace, _T("Choose the font face you would like to use"));
		m_ToolTip.AddTool( &cFontSize, _T("Choose the font size you would like to use"));

		m_ToolTip.Activate(TRUE);
	}

	OptionsPageFonts::SetModified(FALSE);

	return TRUE;
}

BOOL OptionsPageFonts::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);

     return CBCGPPropertyPage::PreTranslateMessage(pMsg);
}

void OptionsPageFonts::OnOK()
{
	cFontFace.GetWindowText(Settings::EditorFontFace);

	CString FontSize;
	cFontSize.GetWindowText(FontSize);
	FontSize.LockBuffer();
	Settings::EditorFontSize = _ttoi( FontSize.GetBuffer() );
	FontSize.ReleaseBuffer();

	// Colours
	Settings::m_ColourStandardText = m_Colors.at(0);
	Settings::m_ColourKeyword = m_Colors.at(1);
	Settings::m_ColourString = m_Colors.at(2);
	Settings::m_ColourComment = m_Colors.at(3);
	Settings::m_ColourSymbol = m_Colors.at(4);
	Settings::m_ColourFunction = m_Colors.at(5);
	Settings::m_ColourType = m_Colors.at(6);
	Settings::m_ColourVariable = m_Colors.at(7);
	Settings::m_ColourBackground = cEditorBackground.GetColor();

	CWinApp* pApp = AfxGetApp();

	POSITION posTemplate = pApp->GetFirstDocTemplatePosition();

	while(posTemplate != NULL)
	{
		CDocTemplate* pTemplate = pApp->GetNextDocTemplate(posTemplate);

		POSITION posDocument = pTemplate->GetFirstDocPosition();

		while(posDocument != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc(posDocument);
			View* pView;
			for (POSITION pos = pDoc->GetFirstViewPosition (); pos != NULL;)
			{
				pView = DYNAMIC_DOWNCAST (View,	pDoc->GetNextView (pos));

				if (pView != NULL)
				{
					pView->Initialize();
				}
			}
		}
	}

	CBCGPPropertyPage::OnOK();	
}

void OptionsPageFonts::DoOldColour()
{
	if(lastColourEntry == _T("Standard Text"))
	{
		m_Colors.at(0) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Keyword"))
	{
		m_Colors.at(1) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("String"))
	{
		m_Colors.at(2) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Comment"))
	{
		m_Colors.at(3) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Symbol"))
	{
		m_Colors.at(4) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Function"))
	{
		m_Colors.at(5) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Type"))
	{
		m_Colors.at(6) = cItemColour.GetColor();
	}
	else if(lastColourEntry == _T("Variable"))
	{
		m_Colors.at(7) = cItemColour.GetColor();
	}
}

void OptionsPageFonts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONT_COMBO, cFontFace);
	DDX_Control(pDX, IDC_FONT_SIZE, cFontSize);
	DDX_Control(pDX, IDC_FONT_COMBO2, cDisplayItem);
	DDX_Control(pDX, IDC_COLOR, cItemColour);
	DDX_Control(pDX, IDC_BACK_COLOR, cEditorBackground);
}

BEGIN_MESSAGE_MAP(OptionsPageFonts, CDialog)
	ON_CBN_SELCHANGE(IDC_FONT_COMBO, &OptionsPageFonts::OnCbnSelchangeFontCombo)
	ON_CBN_SELCHANGE(IDC_FONT_SIZE, &OptionsPageFonts::OnCbnSelchangeFontSize)
	ON_CBN_SELCHANGE(IDC_FONT_COMBO2, &OptionsPageFonts::OnCbnSelchangeFontCombo2)
	ON_BN_CLICKED(IDC_COLOR, &OptionsPageFonts::OnBnClickedColor)
	ON_BN_CLICKED(IDC_BACK_COLOR, &OptionsPageFonts::OnBnClickedBackColor)
END_MESSAGE_MAP()

// OptionsPageFonts message handlers
void OptionsPageFonts::OnCbnSelchangeFontCombo()
{
	OptionsPageFonts::SetModified(TRUE);
}

void OptionsPageFonts::OnCbnSelchangeFontSize()
{
	OptionsPageFonts::SetModified(TRUE);
}

void OptionsPageFonts::OnCbnSelchangeFontCombo2()
{
	lockColourChange = true;
	cDisplayItem.GetWindowTextW(lastColourEntry);

	if(lastColourEntry == _T("Standard Text"))
	{
		cItemColour.SetColor(m_Colors.at(0));
	}
	else if(lastColourEntry == _T("Keyword"))
	{
		cItemColour.SetColor(m_Colors.at(1));
	}
	else if(lastColourEntry == _T("String"))
	{
		cItemColour.SetColor(m_Colors.at(2));
	}
	else if(lastColourEntry == _T("Comment"))
	{
		cItemColour.SetColor(m_Colors.at(3));
	}
	else if(lastColourEntry == _T("Symbol"))
	{
		cItemColour.SetColor(m_Colors.at(4));
	}
	else if(lastColourEntry == _T("Function"))
	{
		cItemColour.SetColor(m_Colors.at(5));
	}
	else if(lastColourEntry == _T("Type"))
	{
		cItemColour.SetColor(m_Colors.at(6));
	}
	else if(lastColourEntry == _T("Variable"))
	{
		cItemColour.SetColor(m_Colors.at(7));
	}
	cItemColour.UpdateWindow();
	lockColourChange = false;
}

void OptionsPageFonts::OnBnClickedColor()
{
	if(!lockColourChange)
	{
		DoOldColour(); // Update since the last change
		OptionsPageFonts::SetModified(TRUE);
	}
}

void OptionsPageFonts::OnBnClickedBackColor()
{
	OptionsPageFonts::SetModified(TRUE);
}
